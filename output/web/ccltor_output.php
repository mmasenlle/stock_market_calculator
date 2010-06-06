<html>
 <head>
  <title>CALCULINATOR OUTPUT RESULTS</title>
  <meta HTTP-EQUIV="PRAGMA" CONTENT="NO-CACHE">
  <meta HTTP-EQUIV="CACHE-CONTROL" CONTENT="NO-CACHE,NO-STORE,MUST-REVALIDATE">
  <style type="text/css">
body {
        max-width: 600px;
}
tr {
        background: #FCFDFC;
        padding: 3px;
        font-size: 14px;
}
tr th {
        background: #93AFCF;
        color: #FFFFFF;
        font-weight: bold;
}
table {
        width: 90%;
        max-width: 100%;
        padding: 2px;
        text-align: center;
}
</style>
 </head>
 <body>
  <div align="center">
<?php
$chart_file = "tmp/chart-" . time() . ".png";
$cmd = "../ccltor_output -l tmp/output_web.log -P tmp -o " . $_POST['mode'];
if ($_POST['mode'] == 'png')
	$cmd = $cmd . " -O " . $chart_file;
$cmd = $cmd . " -V " . $_POST['value'] . " -i " . $_POST['item'] . " -t " . $_POST['type'];
$cmd = $cmd . " -s " . $_POST['day_start'] . " -S " . $_POST['time_start'];
$cmd = $cmd . " -e " . $_POST['day_end'] . " -E " . $_POST['time_end'];

//echo $cmd;
system($cmd);

if ($_POST['mode'] == 'png')
	echo '<img src="' . $chart_file . '" />';

?>
</div>
 </body>
</html>
