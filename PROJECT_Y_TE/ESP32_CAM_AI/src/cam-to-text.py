from pydrive.auth import GoogleAuth
from pydrive.drive import GoogleDrive
import os, re

# ===== 1. Xác thực Google Drive (chỉ 1 lần) =====
gauth = GoogleAuth()

# Nếu đã có token thì dùng lại, khỏi xác thực lại
if os.path.exists("mycreds.txt"):
    gauth.LoadCredentialsFile("mycreds.txt")

if gauth.credentials is None:
    gauth.LocalWebserverAuth()  # Lần đầu chạy → mở trình duyệt xác thực
elif gauth.access_token_expired:
    gauth.Refresh()  # Token hết hạn → tự làm mới
else:
    gauth.Authorize()  # Đã hợp lệ → dùng luôn

# Lưu lại token để lần sau chạy tự động
gauth.SaveCredentialsFile("mycreds.txt")

drive = GoogleDrive(gauth)

# ===== 2. Đường dẫn ảnh cần OCR =====
image_path = r"d:\ESP32\PROJECT_Y_TE\ESP32_CAM_AI\capture.jpg"

# ===== 3. Upload ảnh gốc =====
upload_file = drive.CreateFile({
    'title': os.path.basename(image_path),
    'mimeType': 'image/jpeg'
})
upload_file.SetContentFile(image_path)
upload_file.Upload()
print(f"✅ Ảnh đã upload: {upload_file['title']}")
print(f"🔗 Link ảnh: https://drive.google.com/file/d/{upload_file['id']}/view")

# ===== 4. Tạo bản sao OCR (Docs) =====
ocr_file = drive.auth.service.files().copy(
    fileId=upload_file['id'],
    body={
        'title': 'OCR_Result',
        'mimeType': 'application/vnd.google-apps.document'
    },
    ocr=True
).execute()

print("✅ OCR file đã được tạo trên Google Docs.")
print(f"🔗 Link Docs: https://docs.google.com/document/d/{ocr_file['id']}/edit")

# ===== 5. Lấy nội dung text từ Google Docs =====
file_obj = drive.CreateFile({'id': ocr_file['id']})
text_content = file_obj.GetContentString(mimetype='text/plain')

print("\n📜 Văn bản trích xuất:")
print("----------------------------------")
print(text_content)
print("----------------------------------")

# ===== 6. Lọc 3 giá trị số =====
# text_content là nội dung OCR
# Tìm tất cả số trong văn bản
numbers = re.findall(r'\d+', text_content)

# Lấy 3 số cuối cùng (thường là SYS, DIA, PULSE)
if len(numbers) >= 3:
    sys_val, dia_val, pulse_val = numbers[-3:]
    print("\n🩺 Kết quả đo được:")
    print(f"SYS (mmHg): {sys_val}")
    print(f"DIA (mmHg): {dia_val}")
    print(f"PULSE (/min): {pulse_val}")
else:
    print("⚠️ Không đủ dữ liệu số.")
# ===== 7. XÓA FILE TRÊN DRIVE SAU KHI HOÀN TẤT =====
try:
    drive.auth.service.files().delete(fileId=upload_file['id']).execute()
    drive.auth.service.files().delete(fileId=ocr_file['id']).execute()
    print("🧹 Đã xóa file OCR và ảnh gốc khỏi Drive.")
except Exception as e:
    print(f"⚠️ Lỗi khi xóa file: {e}")