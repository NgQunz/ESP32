<?php
header('Content-Type: application/json');

$hostname = "localhost";
$username = "root";
$password = "";
$database = "sensor-temp";
$conn = mysqli_connect($hostname, $username, $password, $database);  

if (!$conn) {  
    die("Connection failed: " . mysqli_connect_error());  
}  

// Lấy dữ liệu từ bảng mlx90614
$sql = "SELECT id, tempc, heartrate, datetime FROM mlx90614 ORDER BY datetime ASC";
$result = mysqli_query($conn, $sql);

if (!$result) {
    // Nếu câu lệnh SQL có lỗi, hiển thị lỗi
    die("Error executing query: " . mysqli_error($conn));
}
$data = array();
while ($row = mysqli_fetch_assoc($result)) {
    $data[] = $row; // Lưu mỗi dòng kết quả vào mảng
}

echo json_encode($data); // Trả về dữ liệu dạng JSON

mysqli_close($conn);
?>
