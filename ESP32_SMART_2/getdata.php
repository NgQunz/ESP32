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

// Đọc dữ liệu từ bảng dht22_mq2
$sql = "SELECT id, tempc, humi, ppm, datetime FROM dht22_mq2 ORDER BY datetime ASC";
$result = mysqli_query($conn, $sql);

if (!$result) {
    die("Error executing query: " . mysqli_error($conn));
}

$data = array();
while ($row = mysqli_fetch_assoc($result)) {
    $data[] = $row;
}

echo json_encode($data);
mysqli_close($conn);
?>
