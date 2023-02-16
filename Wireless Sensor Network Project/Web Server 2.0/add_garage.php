<?php
    // Connect to MySQL
    $MyUsername = "xxx";   // enter your username for mysql
    $MyPassword = "xxx";   // enter your password for mysql
    $MyHostname = "xxx"; // this is usually "localhost" unless your database resides on a different server
    $MyDBname = "xxx"; 
    $api_key_value = "xxx";

    date_default_timezone_set('Europe/Rome');

    $api_key = test_input($_GET["api_key"]);
    if($api_key == $api_key_value) {
        $dateS = date('Y/m/d H:i:s', time());
        $temp = test_input($_GET["temp"]);
        $hum = test_input($_GET["hum"]);
        $pr = test_input($_GET["pr"]);
        $dp = test_input($_GET["dp"]);
        // Create connection
        $conn = new mysqli($MyHostname , $MyUsername, $MyPassword, $MyDBname);
        // Check connection
        if ($conn->connect_error) {
            die("Connection failed: " . $conn->connect_error);
        } 
        
        $sql = "INSERT INTO `garage` (`date`, `temperature`, `humidity`, `pressure`, `dewpoint`)
        VALUES ('$dateS','" . $temp . "', '" . $hum . "', '" . $pr . "', '" . $dp . "')";
        
        if ($conn->query($sql) === TRUE) {
            echo "New record created successfully";
        } 
        else {
            echo "Error: " . $sql . "<br>" . $conn->error;
        }
    
        $conn->close();
    }
    else {
        echo "Wrong API Key provided.";
    }
    
    function test_input($data) {
        $data = trim($data);
        $data = stripslashes($data);
        $data = htmlspecialchars($data);
        return $data;  
    }
?>