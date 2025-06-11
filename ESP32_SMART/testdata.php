<?php
$hostname = "localhost";
$username = "root";
$password = "";
$database = "temp_humi_mq2";

// Kết nối MySQL
$conn = mysqli_connect($hostname, $username, $password, $database);
if (!$conn) {
    die("Connection failed: " . mysqli_connect_error());
}

// Hiển thị dữ liệu POST để debug
print_r($_POST);

// Kiểm tra dữ liệu được gửi lên
if (isset($_POST["Temp"]) && isset($_POST["Humi"]) && isset($_POST["Mq2"])) {
    $temp = floatval($_POST["Temp"]);
    $humi = floatval($_POST["Humi"]);
    $mq2 = intval($_POST["Mq2"]);

    // Thực hiện câu lệnh SQL
    $sql = "INSERT INTO dht22_mq2 (Temp, Humi, Mq2) VALUES ($temp, $humi, $mq2)";

    if (mysqli_query($conn, $sql)) {
        echo "New record created successfully";
    } else {
        echo "Error: " . $sql . "<br>" . mysqli_error($conn);
    }
} else {
    echo "Thiếu dữ liệu POST: cần Temp, Humi, Mq2";
}
?>
