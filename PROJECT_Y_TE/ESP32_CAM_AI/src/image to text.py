import cv2
import numpy as np
import pyrealsense2 as rs

# ====== Camera RealSense ======
pipeline = rs.pipeline()
config = rs.config()
config.enable_stream(rs.stream.color, 640, 480, rs.format.bgr8, 30)
profile = pipeline.start(config)

import time
time.sleep(2)
for _ in range(5):
    pipeline.wait_for_frames()

frames = pipeline.wait_for_frames(timeout_ms=5000)
color_frame = frames.get_color_frame()
if not color_frame:
    pipeline.stop()
    raise RuntimeError("Không đọc được camera")

frame = np.asanyarray(color_frame.get_data())
pipeline.stop()

# ====== Crop vùng LED cố định trên Omron ======
x, y, w, h = 100, 100, 50, 80  # chỉnh theo vị trí LED
led_roi = frame[y:y+h, x:x+w]

# ====== Chuyển sang grayscale và threshold ======
gray = cv2.cvtColor(led_roi, cv2.COLOR_BGR2GRAY)
_, thresh = cv2.threshold(gray, 150, 255, cv2.THRESH_BINARY_INV)

# ====== Template matching ======
templates = {}
for i in range(10):
    t = cv2.imread(f"templates/{i}.png", cv2.IMREAD_GRAYSCALE)
    _, t_thresh = cv2.threshold(t, 150, 255, cv2.THRESH_BINARY_INV)
    templates[str(i)] = t_thresh

best_match = None
max_val = 0
for digit, t_img in templates.items():
    res = cv2.matchTemplate(thresh, t_img, cv2.TM_CCOEFF_NORMED)
    _, val, _, _ = cv2.minMaxLoc(res)
    if val > max_val:
        max_val = val
        best_match = digit

print("Số LED trên Omron:", best_match)
