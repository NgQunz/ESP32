import requests, json, datetime, time

def backup_firebase():
    url = "https://nckh-clkk-default-rtdb.asia-southeast1.firebasedatabase.app/.json"
    try:
        r = requests.get(url)
        if r.status_code == 200:
            data = r.json()

            # Thêm thời gian backup vào dữ liệu (tùy chọn)
            data["_last_backup"] = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")

            # Ghi đè vào cùng 1 file
            filename = "backup.json"
            with open(filename, "w", encoding="utf-8") as f:
                json.dump(data, f, ensure_ascii=False, indent=2)

            print(f"✅ Sao lưu thành công: {filename} ({data['_last_backup']})")
        else:
            print(f"❌ Lỗi tải Firebase: {r.status_code}")
    except Exception as e:
        print("⚠️ Lỗi kết nối:", e)

# 🔁 Lặp lại mỗi 10 phút
while True:
    backup_firebase()
    print("⏳ Chờ 10 phút...")
    time.sleep(600)  # 600 giây = 10 phút
