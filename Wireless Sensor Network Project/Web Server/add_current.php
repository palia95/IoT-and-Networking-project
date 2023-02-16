<?php
    // Connect to MySQL
    include("connect.php");

    // Prepare the SQL statement
    date_default_timezone_set('Europe/Rome');
    $id = mysql_query("SELECT `ID` FROM `current` ORDER BY date DESC LIMIT 1");
    $dateS = date('Y/m/d h:i:s', time());
    $SQL = "INSERT INTO `current` (`id`, `date`, `irms`, `power`) VALUES ('$id', '$dateS','".$_GET["irms"]."','".$_GET["pwr"]."')";     

    // Execute SQL statement
    mysql_query($SQL);
?>