<?php
$hostname = "localhost";
$username = "root";
$password = "";
$database = "sensor-temp";
$conn = mysqli_connect($hostname, $username, $password, $database);  

if (!$conn) {
    die("Connection failed: " . mysqli_connect_error());
}

echo "Database connection is OK";
print_r($_POST);

if (isset($_POST["tempc"]) && isset($_POST["humi"]) && isset($_POST["ppm"])) {
    $c = $_POST["tempc"];
    $h = $_POST["humi"];
    $p = $_POST["ppm"];

    $sql = "INSERT INTO dht22_mq2 (tempc, humi, ppm) VALUES ($c, $h, $p)";

    if (mysqli_query($conn, $sql)) {
        echo "New record created successfully";
    } else {
        echo "Error: " . $sql . "<br>" . mysqli_error($conn);
    }
}
?>