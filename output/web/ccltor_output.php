<html>
 <head>
  <title>CCLTOR OUTPUT</title>
 </head>
 <body>
  <h1>CCLTOR OUTPUT</h1>
<?php
$cmd = "../ccltor_output -l tmp/output_web.log -P tmp -o " . $_POST['mode'];
if ($_POST['mode'] == 'png')
	$cmd = $cmd . " -O tmp/chart.png";
$cmd = $cmd . " -V " . $_POST['value'] . " -i " . $_POST['item'] . " -t " . $_POST['type'];
$cmd = $cmd . " -s " . $_POST['day_start'] . " -S " . $_POST['time_start'];
$cmd = $cmd . " -e " . $_POST['day_end'] . " -E " . $_POST['time_end'];

//echo $cmd;
system($cmd);

if ($_POST['mode'] == 'png')
	echo '<img src="tmp/chart.png" />';

?>
 </body>
</html>
