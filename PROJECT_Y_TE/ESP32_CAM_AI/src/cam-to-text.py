import cv2
import numpy as np
from collections import OrderedDict

# Định nghĩa các đoạn LED (7 segments)
SEGMENTS = {
    (1, 1, 1, 0, 1, 1, 1): 0,
    (0, 0, 1, 0, 0, 1, 0): 1,
    (1, 0, 1, 1, 1, 0, 1): 2,
    (1, 0, 1, 1, 0, 1, 1): 3,
    (0, 1, 1, 1, 0, 1, 0): 4,
    (1, 1, 0, 1, 0, 1, 1): 5,
    (1, 1, 0, 1, 1, 1, 1): 6,
    (1, 1, 1, 0, 0, 1, 0): 7,
    (1, 1, 1, 1, 1, 1, 1): 8,
    (1, 1, 1, 1, 0, 1, 1): 9
}
def resize_image(img, target_width=93, target_height=124):
    """Resize ảnh về kích thước mục tiêu"""
    height, width = img.shape[:2]
    print(f"Kích thước ảnh gốc: {width}x{height} pixels")
    
    img_resized = cv2.resize(img, (target_width, target_height))
    print(f"Kích thước ảnh sau resize: {target_width}x{target_height} pixels")
    
    return img_resized
def preprocess_image(image_path):
    """Tiền xử lý ảnh"""
    img = cv2.imread(image_path)
    if img is None:
        raise ValueError("Không thể đọc ảnh")
        # In kích thước ảnh
    height, width = img.shape[:2]
    print(f"Kích thước ảnh: {width}x{height} pixels")
        # Resize ảnh về kích thước 93x124
    img = resize_image(img, target_width=93, target_height=124)
    # Chuyển sang grayscale
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    
    # Làm mờ để giảm nhiễu
    blurred = cv2.GaussianBlur(gray, (5, 5), 0)
    
    # Threshold để tách số ra khỏi nền
    _, thresh = cv2.threshold(blurred, 0, 255, cv2.THRESH_BINARY_INV + cv2.THRESH_OTSU)
    
    return img, thresh

def find_digit_contours(thresh):
    """Tìm các contour của chữ số"""
    contours, _ = cv2.findContours(thresh, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    
    # Lọc contour theo kích thước
    digit_contours = []
    for contour in contours:
        x, y, w, h = cv2.boundingRect(contour)
        area = cv2.contourArea(contour)
        
        # Lọc các vùng quá nhỏ hoặc quá lớn
        # Giảm ngưỡng width để bắt được số 1 (rất mỏng)
        # Kiểm tra cả diện tích để loại bỏ nhiễu
        if 15 < h < 200 and 5 < w < 100 and area > 100:
            digit_contours.append(contour)
    
    return digit_contours

def group_contours_by_row(contours, row_threshold=30):
    """
    Nhóm các contour theo hàng dựa trên tọa độ Y
    
    Args:
        contours: Danh sách contours
        row_threshold: Ngưỡng khoảng cách Y để coi là cùng hàng
    
    Returns:
        List of lists, mỗi list chứa các contour của một hàng
    """
    if not contours:
        return []
    
    # Lấy tọa độ y của mỗi contour
    contour_data = []
    for contour in contours:
        x, y, w, h = cv2.boundingRect(contour)
        contour_data.append({
            'contour': contour,
            'x': x,
            'y': y,
            'w': w,
            'h': h,
            'center_y': y + h // 2
        })
    
    # Sắp xếp theo Y
    contour_data.sort(key=lambda c: c['center_y'])
    
    # Nhóm thành các hàng
    rows = []
    current_row = [contour_data[0]]
    
    for i in range(1, len(contour_data)):
        # Nếu khoảng cách Y nhỏ hơn ngưỡng, cùng hàng
        if abs(contour_data[i]['center_y'] - current_row[0]['center_y']) < row_threshold:
            current_row.append(contour_data[i])
        else:
            # Sắp xếp các chữ số trong hàng từ trái sang phải
            current_row.sort(key=lambda c: c['x'])
            rows.append(current_row)
            current_row = [contour_data[i]]
    
    # Thêm hàng cuối cùng
    if current_row:
        current_row.sort(key=lambda c: c['x'])
        rows.append(current_row)
    
    return rows

def extract_segments(roi):
    """Trích xuất trạng thái 7 đoạn LED"""
    h, w = roi.shape
    
    # Định nghĩa vị trí các đoạn (tỷ lệ %)
    segments = OrderedDict([
        ("top", [(0.25, 0.05), (0.75, 0.2)]),
        ("top_left", [(0.05, 0.2), (0.25, 0.5)]),
        ("top_right", [(0.75, 0.2), (0.95, 0.5)]),
        ("middle", [(0.25, 0.45), (0.75, 0.55)]),
        ("bottom_left", [(0.05, 0.5), (0.25, 0.8)]),
        ("bottom_right", [(0.75, 0.5), (0.95, 0.8)]),
        ("bottom", [(0.25, 0.8), (0.75, 0.95)])
    ])
    
    segment_states = []
    
    for seg_name, ((x1_pct, y1_pct), (x2_pct, y2_pct)) in segments.items():
        # Chuyển % sang pixel
        x1 = int(w * x1_pct)
        y1 = int(h * y1_pct)
        x2 = int(w * x2_pct)
        y2 = int(h * y2_pct)
        
        # Lấy vùng đoạn
        segment_roi = roi[y1:y2, x1:x2]
        
        # Đếm số pixel trắng (sáng)
        white_pixels = cv2.countNonZero(segment_roi)
        total_pixels = segment_roi.size
        
        # Ngưỡng khác nhau cho từng segment
        # Với số 1 rất mỏng, cần ngưỡng cao hơn cho các segment ngang
        if seg_name in ["top", "middle", "bottom"]:
            threshold = 0.6  # Ngưỡng cao hơn cho segment ngang
        else:
            threshold = 0.5  # Ngưỡng thấp hơn cho segment dọc
        
        is_on = (white_pixels / float(total_pixels)) > threshold
        segment_states.append(1 if is_on else 0)
    
    return tuple(segment_states)

def recognize_digit(segments, roi=None):
    """Nhận diện chữ số từ trạng thái 7 đoạn + rule đặc biệt cho số 1"""

    # ---- RULE ĐẶC BIỆT CHO SỐ 1 (cách 3) ----
    # Nếu ROI rất hẹp → gần như chắc chắn là số 1
    if roi is not None:
        h, w = roi.shape
        if h / float(w) > 3.5:     # tỷ lệ cao / rộng rất lớn là số 1
            return 1
    # -----------------------------------------

    # Nếu pattern khớp 100% thì dùng trực tiếp
    if segments in SEGMENTS:
        return SEGMENTS[segments]
    
    # # Tìm pattern gần nhất nếu không khớp
    # min_diff = 8
    # best_match = None
    
    # for pattern, digit in SEGMENTS.items():
    #     diff = sum(abs(a - b) for a, b in zip(segments, pattern))
    #     if diff < min_diff:
    #         min_diff = diff
    #         best_match = digit
    
    # return best_match if min_diff <= 2 else None
    return None

def read_seven_segment_display(image_path, debug=False):
    """
    Đọc số từ màn hình LED 7 đoạn theo từng dòng
    
    Args:
        image_path: Đường dẫn đến ảnh
        debug: Hiển thị ảnh debug hay không
    
    Returns:
        List chứa các số từng dòng
    """
    # Tiền xử lý
    img, thresh = preprocess_image(image_path)
    
    # Tìm các chữ số
    digit_contours = find_digit_contours(thresh)
    
    # Nhóm theo hàng
    rows = group_contours_by_row(digit_contours)
    
    result = []
    
    for row_idx, row in enumerate(rows):
        digits = []
        
        for contour_data in row:
            x = contour_data['x']
            y = contour_data['y']
            w = contour_data['w']
            h = contour_data['h']
            
            # Lấy vùng chứa chữ số
            roi = thresh[y:y+h, x:x+w]

            # --- FIX SỐ 1 BỊ THÀNH SỐ 8 ---
            # Nếu chữ số quá mỏng (w nhỏ so với h) → chắc chắn là số 1
            ratio = h / float(w)
            if ratio > 3.2:  # số 1 rất mỏng, tỉ lệ cao
                # Cắt sát hơn 2 bên để không ăn vào segment ngang khi resize
                pad = int(w * 0.25)   # lấy 25% mỗi bên
                x1 = pad
                x2 = w - pad
                roi = roi[:, x1:x2]   # chỉ cắt chiều ngang
            # --------------------------------

            # Resize để chuẩn hóa
            roi = cv2.resize(roi, (57, 88))

            
            # Trích xuất trạng thái các đoạn
            segments = extract_segments(roi)
            
            # Nhận diện chữ số
            digit = recognize_digit(segments, roi=roi)
            
            if digit is not None:
                digits.append(str(digit))
                
                if debug:
                    # Vẽ khung và số nhận dạng được
                    cv2.rectangle(img, (x, y), (x+w, y+h), (0, 255, 0), 2)
                    cv2.putText(img, str(digit), (x-20, y+20), 
                               cv2.FONT_HERSHEY_SIMPLEX, 0.9, (0, 255, 0), 2)
                    # # Vẽ số thứ tự hàng
                    # cv2.putText(img, f"Row {row_idx+1}", (x-50, y+h//2), 
                    #            cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 0, 0), 2)
        
        if digits:
            result.append(''.join(digits))
    
    if debug:
        cv2.imshow("Original", img)
        cv2.imshow("Threshold", thresh)
        cv2.waitKey(0)
        cv2.destroyAllWindows()
    
    return result

# Sử dụng
if __name__ == "__main__":
    image_path = "anh7doan2.jpg"
    
    # Đọc số theo từng dòng
    numbers = read_seven_segment_display(image_path, debug=True)
    
    print("Các số đọc được:")
    for i, number in enumerate(numbers, 1):
        print(f"Dòng {i}: {number}")
    
    # Hoặc in theo format khác
    print("\nKết quả:", numbers)
