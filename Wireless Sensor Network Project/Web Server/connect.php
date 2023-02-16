<?php
$MyUsername = "iot1";  // enter your username for mysql
$MyPassword = "xxxxxxxx";  // enter your password for mysql
$MyHostname = "127.0.0.1";      // this is usually "localhost" unless your database resides on a different server

$dbh = mysql_pconnect($MyHostname , $MyUsername, $MyPassword);
$selected = mysql_select_db("iot",$dbh); //Enter your database name here

/*Parametri accesso al db mysql*/
$_CONFIG["mysql_host"]="localhost";
$_CONFIG["mysql_username"]="iot1";
$_CONFIG["mysql_pwd"]="xxxxxxxx";
$_CONFIG["mysql_db_name"]="iot";

/*Credenziali di autenticazione alla pagina web*/
$_CONFIG["auth_username"]="admin";
$_CONFIG["auth_password"]="password";

/*Lasso di tempo da mostrare sul grafico, in ore. Mantenere il numero pari*/
$_CONFIG["chart_timespan"]=24;
?>