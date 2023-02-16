<?php
    // Connect to MySQL
    include("connect.php");

    // Prepare the SQL statement
    date_default_timezone_set('Europe/Rome');
    $id = mysql_query("SELECT `ID` FROM `indoor` ORDER BY date DESC LIMIT 1");
    $dateS = date('Y/m/d h:i:s', time());
    $SQL = "INSERT INTO `indoor` (`id`, `date`, `temperature`, `humidity`, `light`) VALUES ('$id', '$dateS','".$_GET["temp"]."','".$_GET["hum"]."','".$_GET["light"]."')";     
    
    // Execute SQL statement
    mysql_query($SQL);
?>