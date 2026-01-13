from pydrive.auth import GoogleAuth
from pydrive.drive import GoogleDrive
import os, re, cv2, numpy as np, pyrealsense2 as rs, time

# ====== 1. Khởi tạo camera RealSense ======
pipeline = rs.pipeline()
config = rs.config()
config.enable_stream(rs.stream.color, 640, 480, rs.format.bgr8, 30)
profile = pipeline.start(config)

time.sleep(2)
for _ in range(5):
    pipeline.wait_for_frames()

frames = pipeline.wait_for_frames(timeout_ms=5000)
color_frame = frames.get_color_frame()
if not color_frame:
    pipeline.stop()
    raise RuntimeError("❌ Không đọc được camera RealSense")

frame = np.asanyarray(color_frame.get_data())
pipeline.stop()

# ====== 2. Lưu ảnh chụp lại ======
image_path = r"d:\ESP32\PROJECT_Y_TE\ESP32_CAM_AI\capture.jpg"
cv2.imwrite(image_path, frame)
print("📸 Ảnh đã được chụp và lưu:", image_path)

# ====== 3. Xác thực Google Drive ======
gauth = GoogleAuth()
if os.path.exists("mycreds.txt"):
    gauth.LoadCredentialsFile("mycreds.txt")

if gauth.credentials is None:
    gauth.LocalWebserverAuth()
elif gauth.access_token_expired:
    gauth.Refresh()
else:
    gauth.Authorize()

gauth.SaveCredentialsFile("mycreds.txt")
drive = GoogleDrive(gauth)

# ====== 4. Upload ảnh ======
upload_file = drive.CreateFile({
    'title': os.path.basename(image_path),
    'mimeType': 'image/jpeg'
})
upload_file.SetContentFile(image_path)
upload_file.Upload()
print(f"✅ Ảnh đã upload: {upload_file['title']}")
print(f"🔗 Link ảnh: https://drive.google.com/file/d/{upload_file['id']}/view")

# ====== 5. Tạo bản sao OCR (Google Docs) ======
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

# ====== 6. Lấy nội dung văn bản OCR ======
file_obj = drive.CreateFile({'id': ocr_file['id']})
text_content = file_obj.GetContentString(mimetype='text/plain')

print("\n📜 Văn bản trích xuất:")
print("----------------------------------")
print(text_content)
print("----------------------------------")

# ====== 7. Lọc ra 3 giá trị số cuối ======
numbers = re.findall(r'\d+', text_content)
if len(numbers) >= 3:
    sys_val, dia_val, pulse_val = numbers[-3:]
    print("\n🩺 Kết quả đo được:")
    print(f"SYS (mmHg): {sys_val}")
    print(f"DIA (mmHg): {dia_val}")
    print(f"PULSE (/min): {pulse_val}")
else:
    print("⚠️ Không đủ dữ liệu số.")

# ====== 8. Xóa file OCR và ảnh trên Drive ======
try:
    drive.auth.service.files().delete(fileId=upload_file['id']).execute()
    drive.auth.service.files().delete(fileId=ocr_file['id']).execute()
    print("🧹 Đã xóa file OCR và ảnh gốc khỏi Drive.")
except Exception as e:
    print(f"⚠️ Lỗi khi xóa file: {e}")
