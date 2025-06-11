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


if (isset($_POST["tempc"]) && isset($_POST["heartrate"])) {  
   
	$c = $_POST["tempc"];
	$f = $_POST["heartrate"];
	$sql = "INSERT INTO mlx90614 (tempc, heartrate) VALUES (".$c.", ".$f.")"; 

	if (mysqli_query($conn, $sql)) {  
		echo "New record created successfully";  
	} else {  
		echo "Error: " . $sql . "<br>" . mysqli_error($conn);  
	} 
} 
?>