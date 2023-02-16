<?php
    // Connect to MySQL
    include("connect.php");

    // Prepare the SQL statement
    date_default_timezone_set('Europe/Rome');
    $id = mysql_query("SELECT `ID` FROM `data` ORDER BY date DESC LIMIT 1");
    $dateS = date('Y/m/d h:i:s', time());
    //put data into the query catched by the GETs method
    $SQL = "INSERT INTO `letture` (`id`, `date`, `temperature`, `humidity`, `pressure`, `dewpoint`) VALUES ('$id', '$dateS','".$_GET["temp"]."','".$_GET["hum"]."','".$_GET["pr"]."','".$_GET["dp"]."')";     
    
    // Execute SQL statement
    mysql_query($SQL);
?>