from flask import Flask, request, jsonify
import mysql.connector
from datetime import datetime

app = Flask(__name__)

# ==== KẾT NỐI MYSQL ====
db_config = {
    "host": "127.0.0.1",
    "user": "root",
    "password": "304123",
    "database": "AirQualityDB"
}

def insert_data(data):
    try:
        conn = mysql.connector.connect(**db_config)
        cursor = conn.cursor()

        sql = """
        INSERT INTO DATA_AIR (Time, City, Country, PM25, CO, CO2, NH3, Temperature, Humidity)
        VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s)
        """
        values = (
            datetime.now(),
            data.get("city", "Unknown"),
            data.get("country", "VN"),
            data.get("pm25", 0),
            data.get("co", 0),
            data.get("co2", 0),
            data.get("nh3", 0),
            data.get("temp", 0),
            data.get("hum", 0)
        )

        cursor.execute(sql, values)
        conn.commit()
        cursor.close()
        conn.close()
        return True
    except Exception as e:
        print("❌ Lỗi khi ghi MySQL:", e)
        return False

# ==== API NHẬN DỮ LIỆU TỪ ESP32 ====
@app.route('/upload', methods=['POST'])
def upload_data():
    data = request.get_json()
    if not data:
        return jsonify({"status": "error", "message": "No JSON received"}), 400

    print("📩 Dữ liệu nhận:", data)

    if insert_data(data):
        return jsonify({"status": "ok", "message": "Data saved"}), 200
    else:
        return jsonify({"status": "error", "message": "Database insert failed"}), 500

if __name__ == '__main__':
    print("🚀 Server chạy tại http://127.0.0.1:5000/upload")
    app.run(host='0.0.0.0', port=5000)
