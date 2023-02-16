<?php 
    // Start MySQL Connection
    include('connect.php'); 

    date_default_timezone_set('Europe/Rome');
       
    $endDate=new DateTime();
    $startDate=new DateTime($endDate->format("Y-m-d\TH:00:00P"));

    $db = new PDO('mysql:host='.$_CONFIG["mysql_host"].';dbname='.$_CONFIG["mysql_db_name"].';charset=utf8mb4', $_CONFIG["mysql_username"], $_CONFIG["mysql_pwd"]);
	$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
    
    $startDate->modify("-".($_CONFIG["chart_timespan"]/2)." hour");
    $endDate=clone $startDate;
    $endDate->modify("+".$_CONFIG["chart_timespan"]." hour");

    $curpage = $_SERVER['PHP_SELF'];
    header('Refresh: 15; url=' . $curpage);
?>

<html>
<head>
    <link href="css/style.css" rel="stylesheet" type="text/css">

    <div class="navbar">
        <a href="index.php">Dati Meteo</a>
        <a href="indoor.php">Dati indoor</a>
        <a href="current.php">Consumi</a>
    </div>

    <style type="text/css">

    </style>

	<script src="js/Chart.bundle.js"></script>
	<script src="js/utils.js"></script>
    <script src="js/moment.min.js"></script>
	<script src="js/jquery.min.js"></script>
	<script src="js/Chart.min.js"></script>

	<style type="chart">
	canvas{
		-moz-user-select: none;
		-webkit-user-select: none;
		-ms-user-select: none;
	}
	</style>

</head>

    <body>
    <br>
    <br>
    <h1>Ultima lettura dati</h1>

    <table border="0" cellspacing="0" cellpadding="4">
      <tr>
            <td class="table_titles">ID</td>
            <td class="table_titles">Date and Time</td>
            <td class="table_titles">Temperature</td>
            <td class="table_titles">Humidity</td>
            <td class="table_titles">Light</td>
          </tr>
<?php

    // Retrieve all records and display them
    $result = mysql_query("SELECT * FROM `indoor` ORDER BY ID DESC LIMIT 1");

    // Used for row color toggle
    $oddrow = true;

    // process every record
    while( $row = mysql_fetch_array($result) )
    {
        if ($oddrow) 
        { 
            $css_class=' class="table_cells_odd"'; 
        }
        else
        { 
            $css_class=' class="table_cells_even"'; 
        }

        $oddrow = !$oddrow;

        echo '<tr>';
        echo '   <td'.$css_class.'>'.$row["ID"].'</td>';
        echo '   <td'.$css_class.'>'.$row["date"].'</td>';
        echo '   <td'.$css_class.'>'.$row["temperature"].'</td>';
        echo '   <td'.$css_class.'>'.$row["humidity"].'</td>';
        echo '   <td'.$css_class.'>'.$row["light"].'</td>';
        echo '</tr>';
    }
?>
    </table>

    <?php
    //SELECT `temperature` FROM `data` ORDER BY date DESC LIMIT 2
    //SELECT `temperature` FROM `data` ORDER BY date DESC LIMIT 1, 2

    //recupero le informazioni salvate
    $startd=$startDate->format("Y-m-d H:i:s");
    $endd=$endDate->format("Y-m-d H:i:s");
    
    //$stmt = $db->prepare("select date,ID,temperature from letture where date>'$startd' and date<'$endd' order by date asc");
    $stmt = $db->prepare("select date,ID,temperature from indoor order by id desc limit 265");
    $stmt->execute();
    $data=array();
    $time=array();

    while($row = $stmt->fetch(PDO::FETCH_ASSOC)) {
        $d=new DateTime($row["date"]);
        $jsDate=$d->format('d-m H:i');
        
        if ((!isset($data["singleDataset"])) && (!isset($time["Dataset"]))){
            $data["singleDataset"]="";
            $time["Dataset"]="";
        }
        else{
            $data["singleDataset"].=",";
            $time["Dataset"].=",";
        }
        $data["singleDataset"].=$row["temperature"];
        $time["Dataset"].="'".$jsDate."'";
    }
    ?>

    <table style="width:100%;" >
        <tr>
            <td style="width:50%;" >
                <canvas id="canvas1"></canvas>
            </td>
            <td style="width:50%;" >
                <canvas id="canvas2"></canvas>
            </td>
        </tr>
        <tr>
            <td style="width:50%;" >
                <canvas id="canvas3"></canvas>
            </td>
            <td style="width:50%;" >
                <!-- <canvas id="canvas4"></canvas> -->
            </td>
        </tr>
    </table>

	<script>
		var config1 = {
			type: 'line',
			data: {
                <?php $i=0;       
                foreach ($time as $key => $jsDate) {
                    ?>
                    labels: [<?php echo $time ["Dataset"]?>],
                <?php
                    $i++;
                    if ($i<count($time)){
                        echo ",";
                    }
                }
                ?>
                datasets: [
					    {
					    label: 'Temperatura Taverna',
                        backgroundColor: window.chartColors.red,
                        borderColor: window.chartColors.red,
                        fill: false,
                    <?php $c=0;
			
                    foreach ($data as $key => $temperature) {
                        ?>
					    data:[<?php  echo $temperature ?>]
				    <?php
					    $c++;
					    if ($c<count($data)){
						    echo ",";
					    }
				    }
                    ?>
                    }
				]
            },
			options: {
				responsive: true,
				elements: {
					line: {
						tension: 0, // disables bezier curves
					}
				},
				tooltips: {
					mode: 'index',
					intersect: false,
				},
				hover: {
					mode: 'nearest',
					intersect: true
				},
				scales: {
					xAxes: [{
						display: true,
					}],
					yAxes: [{
						display: true,
						scaleLabel: {
							display: true,
							labelString: 'Temperatura C'
						},
						ticks: {
							

							// forces step size to be 5 units
							stepSize: 3
						}
					}]
				}
			}
		};

    <?php
    $stmt1 = $db->prepare("select date,ID,humidity from indoor order by id desc limit 265");
    $stmt1->execute();
    $data1=array();
    while($row = $stmt1->fetch(PDO::FETCH_ASSOC)) {
        if (!isset($data1["singleDataset"])){
            $data1["singleDataset"]="";
        }else{
            $data1["singleDataset"].=",";
        }
        $data1["singleDataset"].=$row["humidity"];
    }
    ?>


		var config2 = {
			type: 'line',
			data: {
                <?php $i=0;       
                foreach ($time as $key => $jsDate) {
                    ?>
                    labels: [<?php echo $time ["Dataset"]?>],
                <?php
                    $i++;
                    if ($i<count($time)){
                        echo ",";
                    }
                }
                ?>
                datasets: [
					    {
					    label: 'Umidita Indoor',
                        backgroundColor: window.chartColors.green,
                        borderColor: window.chartColors.green,
                        fill: false,
                    <?php $c=0;
			
                    foreach ($data1 as $key => $humidity) {
                        ?>
					    data:[<?php  echo $humidity ?>]
				    <?php
					    $c++;
					    if ($c<count($data1)){
						    echo ",";
					    }
				    }
                    ?>
                    }
				]
            },
			options: {
				responsive: true,
				elements: {
					line: {
						tension: 0, // disables bezier curves
					}
				},
				tooltips: {
					mode: 'index',
					intersect: false,
				},
				hover: {
					mode: 'nearest',
					intersect: true
				},
				scales: {
					xAxes: [{
						display: true,
					}],
					yAxes: [{
						display: true,
						scaleLabel: {
							display: true,
							labelString: 'Umidita %'
						},
						ticks: {
							

							// forces step size to be 5 units
							stepSize: 2
						}
					}]
				}
			}
        };

    <?php
    $stmt2 = $db->prepare("select date,ID,light from indoor order by id desc limit 265");
    $stmt2->execute();
    $data2=array();
    while($row = $stmt2->fetch(PDO::FETCH_ASSOC)) {
        if (!isset($data2["singleDataset"])){
            $data2["singleDataset"]="";
        }else{
            $data2["singleDataset"].=",";
        }
        $data2["singleDataset"].=$row["light"];
    }
    ?>


		var config3 = {
			type: 'line',
			data: {
                <?php $i=0;       
                foreach ($time as $key => $jsDate) {
                    ?>
                    labels: [<?php echo $time ["Dataset"]?>],
                <?php
                    $i++;
                    if ($i<count($time)){
                        echo ",";
                    }
                }
                ?>
                datasets: [
					    {
					    label: 'Luce ON/OFF',
                        backgroundColor: window.chartColors.green,
                        borderColor: window.chartColors.green,
                        fill: false,
                    <?php $c=0;
			
                    foreach ($data2 as $key => $light) {
                        ?>
					    data:[<?php  echo $light ?>]
				    <?php
					    $c++;
					    if ($c<count($data2)){
						    echo ",";
					    }
				    }
                    ?>
                    }
				]
            },
			options: {
				responsive: true,
				elements: {
					line: {
						tension: 0, // disables bezier curves
					}
				},
				tooltips: {
					mode: 'index',
					intersect: false,
				},
				hover: {
					mode: 'nearest',
					intersect: true
				},
				scales: {
					xAxes: [{
						display: true,
					}],
					yAxes: [{
						display: true,
						scaleLabel: {
							display: true,
							labelString: 'Light'
						},
						ticks: {
							

							// forces step size to be 5 units
							stepSize: 1
						}
					}]
				}
			}
        };

		window.onload = function() {
            var ctx1 = document.getElementById('canvas1').getContext('2d');
            var ctx2 = document.getElementById('canvas2').getContext('2d');
            var ctx3 = document.getElementById('canvas3').getContext('2d');
            window.myLine = new Chart(ctx1, config1);
            window.myLine = new Chart(ctx2, config2);
            window.myLine = new Chart(ctx3, config3);
		};
	</script>

    </body>
</html>