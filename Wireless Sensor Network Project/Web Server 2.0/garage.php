<?php 
    // Connect to MySQL
    $MyUsername = "xxx";   // enter your username for mysql
    $MyPassword = "xxx";   // enter your password for mysql
    $MyHostname = "xxx"; // this is usually "localhost" unless your database resides on a different server
    $MyDBname = "xxx";

    // Create connection
    $conn = new mysqli($MyHostname , $MyUsername, $MyPassword, $MyDBname);
    // Check connection
    if ($conn->connect_error) {
        die("Connection failed: " . $conn->connect_error);
    } 

    //initial day time
    $dateI = date('Y/m/d H:i:s', mktime(0,0,1));
    //actual day time
    $dateF = date('Y/m/d H:i:s', time());
    
    //query first and second day readings
    $sql = "SELECT id, date, temperature, humidity, pressure, dewpoint FROM garage order by id desc limit 285";
    $sql2 = "SELECT id, date, temperature, humidity, pressure, dewpoint FROM garage order by id desc limit 286, 285";

    //fetch queries
    $result = $conn->query($sql);
    $result2 = $conn->query($sql2);

    //first day time readings
    while ($data = $result->fetch_assoc()){
        $sensor_data[] = $data;
    }
    $readings_time = array_column($sensor_data, 'date');

    //second day time readings
    while ($data2 = $result2->fetch_assoc()){
        $sensor_data2[] = $data2;
    }
    $readings_time2 = array_column($sensor_data2, 'date');

    //first day readings
    $temp = json_encode(array_reverse(array_column($sensor_data, 'temperature')), JSON_NUMERIC_CHECK);
    $hum = json_encode(array_reverse(array_column($sensor_data, 'humidity')), JSON_NUMERIC_CHECK);
    $pr = json_encode(array_reverse(array_column($sensor_data, 'pressure')), JSON_NUMERIC_CHECK);
    $dp = json_encode(array_reverse(array_column($sensor_data, 'dewpoint')), JSON_NUMERIC_CHECK);
    $reading_time = json_encode(array_reverse($readings_time), JSON_NUMERIC_CHECK);

    //second day readings
    $temp2 = json_encode(array_reverse(array_column($sensor_data2, 'temperature')), JSON_NUMERIC_CHECK);
    $hum2 = json_encode(array_reverse(array_column($sensor_data2, 'humidity')), JSON_NUMERIC_CHECK);
    $pr2 = json_encode(array_reverse(array_column($sensor_data2, 'pressure')), JSON_NUMERIC_CHECK);
    $dp2 = json_encode(array_reverse(array_column($sensor_data2, 'dewpoint')), JSON_NUMERIC_CHECK);
    $reading_time2 = json_encode(array_reverse($readings_time2), JSON_NUMERIC_CHECK);

    //get max, min e last values
    $lasttemp = mysqli_fetch_array($conn->query("SELECT temperature FROM garage order by id desc limit 1"));
    $lasthum = mysqli_fetch_array($conn->query("SELECT humidity FROM garage order by id desc limit 1"));
    $lastdp = mysqli_fetch_array($conn->query("SELECT dewpoint FROM garage order by id desc limit 1"));
    $lastpr = mysqli_fetch_array($conn->query("SELECT pressure FROM garage order by id desc limit 1"));
    $maxtemp = mysqli_fetch_array($conn->query("SELECT MAX(temperature) FROM garage WHERE date > '$dateI' and date < '$dateF'"));
    $maxhum = mysqli_fetch_array($conn->query("SELECT MAX(humidity) FROM garage WHERE date > '$dateI' and date < '$dateF'"));
    $maxdp = mysqli_fetch_array($conn->query("SELECT MAX(dewpoint) FROM garage WHERE date > '$dateI' and date < '$dateF'"));
    $maxpr = mysqli_fetch_array($conn->query("SELECT MAX(pressure) FROM garage WHERE date > '$dateI' and date < '$dateF'"));
    $mintemp = mysqli_fetch_array($conn->query("SELECT MIN(temperature) FROM garage WHERE date > '$dateI' and date < '$dateF'"));
    $minhum = mysqli_fetch_array($conn->query("SELECT MIN(humidity) FROM garage WHERE date > '$dateI' and date < '$dateF'"));
    $mindp = mysqli_fetch_array($conn->query("SELECT MIN(dewpoint) FROM garage WHERE date > '$dateI' and date < '$dateF'"));
    $minpr = mysqli_fetch_array($conn->query("SELECT MIN(pressure) FROM garage WHERE date > '$dateI' and date < '$dateF'"));

    //clean array
    $result->free();
    $result2->free();
    $conn->close();

    //refresh page every 120 seconds, next step would be dynamic graphs
    $curpage = $_SERVER['PHP_SELF'];
    header('Refresh: 120; url=' . $curpage);
?>

<html>
<head>
    <link href="css/style.css" rel="stylesheet" type="text/css">

    <div class="navbar">
        <a href="index.php" >Dati Meteo</a>
        <a href="indoor.php">Dati indoor</a>
        <a href="garage.php">Garage</a>
    </div>

    <style type="text/css">

    </style>
</head>

    <body>
    <br>
    <br>
    <br>
        
    <div style="width:99%;">
    <script src="https://code.highcharts.com/highcharts.js"></script>
    <style>
        h2 {
        font-family: Arial;
        font-size: 2.5rem;
        text-align: center;
        }
    </style>
<div id="chart-temperature" class="container"></div>
    <div>
        <table class="table_titles" style="width:100%">
            <tbody>
                <tr>
                    <td>Actual Temperature: <?php echo $lasttemp[0]; ?></td>
                    <td>Max Temperature: <?php echo round($maxtemp[0], 2); ?></td>
                    <td>Min Temperature: <?php echo round($mintemp[0], 2); ?></td>                                    
                <tr>
            </tbody>
        </table>
    </div>
    <div id="chart-humidity" class="container"></div>
    <div>
        <table class="table_titles" style="width:100%">
            <tbody>
                <tr>
                    <td>Actual Humidity: <?php echo $lasthum[0]; ?></td>
                    <td>Max Humidity: <?php echo round($maxhum[0], 2); ?></td>
                    <td>Min Humidity: <?php echo round($minhum[0], 2); ?></td>                                    
                <tr>
            </tbody>
        </table>
    </div>
    <div id="chart-pressure" class="container"></div>
    <div>
        <table class="table_titles" style="width:100%">
            <tbody>
                <tr>
                    <td>Actual Pressure: <?php echo $lastpr[0]; ?></td>
                    <td>Max Pressure: <?php echo round($maxpr[0], 2); ?></td>
                    <td>Min Pressure: <?php echo round($minpr[0], 2); ?></td>                                    
                <tr>
            </tbody>
        </table>
    </div>
    <div id="chart-dewpoint" class="container"></div>
    <div>
        <table class="table_titles" style="width:100%">
            <tbody>
                <tr>
                    <td>Actual Dew Point: <?php echo $lastdp[0]; ?></td>
                    <td>Max Dew Point: <?php echo round($maxdp[0], 2); ?></td>
                    <td>Min Dew Point: <?php echo round($mindp[0], 2); ?></td>                                    
                <tr>
            </tbody>
        </table>
    </div>
    <script>
    //put first day data into arrays
    var temp = <?php echo $temp; ?>;
    var hum = <?php echo $hum; ?>;
    var pr = <?php echo $pr; ?>;
    var dp = <?php echo $dp; ?>;
    var reading_time = <?php echo $reading_time; ?>;

    //put second day data into arrays
    var temp2 = <?php echo $temp2; ?>;
    var hum2 = <?php echo $hum2; ?>;
    var pr2 = <?php echo $pr2; ?>;
    var dp2 = <?php echo $dp2; ?>;
    var reading_time2 = <?php echo $reading_time2; ?>;

    console.time('line');
    Highcharts.theme = {
        colors: ['#2b908f', '#90ee7e', '#f45b5b', '#7798BF', '#aaeeee', '#ff0066',
        '#eeaaee', '#55BF3B', '#DF5353', '#7798BF', '#aaeeee'],
        chart: {
        backgroundColor: {
            linearGradient: { x1: 0, y1: 0, x2: 1, y2: 1 },
            stops: [
                [0, '#2a2a2b'],
                [1, '#3e3e40']
            ]
        },
        style: {
            fontFamily: '\'Unica One\', sans-serif'
        },
        marginLeft: 80,
        marginRight: 30,
        plotBorderColor: '#606063'
        },
        title: {
        style: {
            color: '#E0E0E3',
            textTransform: 'uppercase',
            fontSize: '20px'
        }
        },
        subtitle: {
        style: {
            color: '#E0E0E3',
            textTransform: 'uppercase'
        }
        },
        xAxis: {
        gridLineColor: '#707073',
        labels: {
            style: {
                color: '#E0E0E3'
            }
        },
        lineColor: '#707073',
        minorGridLineColor: '#505053',
        tickColor: '#707073',
        title: {
            style: {
                color: '#A0A0A3'

            }
        }
        },
        yAxis: {
        gridLineColor: '#707073',
        labels: {
            style: {
                color: '#E0E0E3'
            }
        },
        lineColor: '#707073',
        minorGridLineColor: '#505053',
        tickColor: '#707073',
        tickWidth: 1,
        title: {
            style: {
                color: '#A0A0A3'
            }
        }
        },
        tooltip: {
        backgroundColor: 'rgba(0, 0, 0, 0.85)',
        style: {
            color: '#F0F0F0'
        }
        },
        plotOptions: {
        series: {
            dataLabels: {
                color: '#B0B0B3'
            },
            marker: {
                lineColor: '#333'
            }
        },
        boxplot: {
            fillColor: '#505053'
        },
        candlestick: {
            lineColor: 'white'
        },
        errorbar: {
            color: 'white'
        }
        },
        legend: {
        itemStyle: {
            color: '#E0E0E3'
        },
        itemHoverStyle: {
            color: '#FFF'
        },
        itemHiddenStyle: {
            color: '#606063'
        }
        },
        credits: {
        style: {
            color: '#666'
        }
        },
        labels: {
        style: {
            color: '#707073'
        }
        },

        drilldown: {
        activeAxisLabelStyle: {
            color: '#F0F0F3'
        },
        activeDataLabelStyle: {
            color: '#F0F0F3'
        }
        },

        navigation: {
        buttonOptions: {
            symbolStroke: '#DDDDDD',
            theme: {
                fill: '#505053'
            }
        }
        },

        // scroll charts
        rangeSelector: {
        buttonTheme: {
            fill: '#505053',
            stroke: '#000000',
            style: {
                color: '#CCC'
            },
            states: {
                hover: {
                fill: '#707073',
                stroke: '#000000',
                style: {
                    color: 'white'
                }
                },
                select: {
                fill: '#000003',
                stroke: '#000000',
                style: {
                    color: 'white'
                }
                }
            }
        },
        inputBoxBorderColor: '#505053',
        inputStyle: {
            backgroundColor: '#333',
            color: 'silver'
        },
        labelStyle: {
            color: 'silver'
        }
        },

        navigator: {
        handles: {
            backgroundColor: '#666',
            borderColor: '#AAA'
        },
        outlineColor: '#CCC',
        maskFill: 'rgba(255,255,255,0.1)',
        series: {
            color: '#7798BF',
            lineColor: '#A6C7ED'
        },
        xAxis: {
            gridLineColor: '#505053'
        }
        },

        scrollbar: {
            barBackgroundColor: '#808083',
            barBorderColor: '#808083',
            buttonArrowColor: '#CCC',
            buttonBackgroundColor: '#606063',
            buttonBorderColor: '#606063',
            rifleColor: '#FFF',
            trackBackgroundColor: '#404043',
            trackBorderColor: '#404043'
        },

        // special colors for some of the
        legendBackgroundColor: 'rgba(0, 0, 0, 0.5)',
        background2: '#505053',
        dataLabelsColor: '#B0B0B3',
        textColor: '#C0C0C0',
        contrastTextColor: '#F0F0F3',
        maskColor: 'rgba(255,255,255,0.3)'
    };

    // Apply the theme
    Highcharts.setOptions(Highcharts.theme);

    // Chart tipe and definition
    var chartT = new Highcharts.Chart({
    chart:{ renderTo : 'chart-temperature', zoomType: 'x' },
    title: { text: 'Garage Temperature' },
    subtitle: { text: 'Select area to enlarge' },
    series: [{
        showInLegend: true,
        data: temp,
        lineWidth: 1,
        name: 'Last 24h measurement'
    },
    {
        showInLegend: true,
        data: temp2,
        lineWidth: 1,
        name: 'Last 48h measurement'
    }],
    plotOptions: {
        line: { animation: true,
        dataLabels: { enabled: true }
        },
        series: [{ color: '#e01616' },{ color: '#88b576'}]
    },
    xAxis: { 
        type: 'datetime',
        tickInterval: 11,
        categories: reading_time
    },
    yAxis: {
        title: { text: 'Temperature (Celsius)' }
    },
    credits: { enabled: false }
    });
    console.timeEnd('line');

    var chartH = new Highcharts.Chart({
    chart:{ renderTo:'chart-humidity', zoomType: 'x'  },
    title: { text: 'Garage Humidity' },
    subtitle: { text: 'Select area to enlarge' },
    series: [{
        showInLegend: true,
        data: hum,
        lineWidth: 1,
        name: 'Last 24h measurement'
    },
    {
        showInLegend: true,
        data: hum2,
        lineWidth: 1,
        name: 'Last 48h measurement'
    }],
    plotOptions: {
        line: { animation: true,
        dataLabels: { enabled: true }
        },
        series: [{ color: '#e01616' },{ color: '#88b576'}]
    },
    xAxis: {
        type: 'datetime',
        tickInterval: 11,
        categories: reading_time
    },
    yAxis: {
        title: { text: 'Humidity (%)' }
    },
    credits: { enabled: false }
    });

    var chartP = new Highcharts.Chart({
    chart:{ renderTo : 'chart-pressure', zoomType: 'x' },
    title: { text: 'Garage Pressure' },
    subtitle: { text: 'Select area to enlarge' },
    series: [{
        showInLegend: true,
        data: pr,
        lineWidth: 1,
        name: 'Last 24h measurement'
    },
    {
        showInLegend: true,
        data: pr2,
        lineWidth: 1,
        name: 'Last 48h measurement'
    }],
    plotOptions: {
        line: { animation: true,
        dataLabels: { enabled: true }
        },
        series: [{ color: '#e01616' },{ color: '#88b576'}]
    },
    xAxis: { 
        type: 'datetime',
        tickInterval: 11,
        categories: reading_time
    },
    yAxis: {
        title: { text: 'Pressure (hPa)' }
    },
    credits: { enabled: false }
    });

    var chartD = new Highcharts.Chart({
    chart:{ renderTo:'chart-dewpoint', zoomType: 'x'  },
    title: { text: 'Garage Dew point' },
    subtitle: { text: 'Select area to enlarge' },
    series: [{
        showInLegend: true,
        data: dp,
        lineWidth: 1,
        name: 'Last 24h measurement'
    },
    {
        showInLegend: true,
        data: dp2,
        lineWidth: 1,
        name: 'Last 48h measurement'
    }],
    plotOptions: {
        line: { animation: true,
        dataLabels: { enabled: true }
        },
        series: [{ color: '#e01616' },{ color: '#88b576'}]
    },
    xAxis: {
        type: 'datetime',
        tickInterval: 11,
        categories: reading_time
    },
    yAxis: {
        title: { text: 'Temperature (Celsius)' }
    },
    credits: { enabled: false }
    });

    </script>
    </div>
</body>
</html>
