<html>
 <head>
  <title>CALCULINATOR DAY</title>
  <meta HTTP-EQUIV="PRAGMA" CONTENT="NO-CACHE">
  <meta HTTP-EQUIV="CACHE-CONTROL" CONTENT="NO-CACHE,NO-STORE,MUST-REVALIDATE">
 </head>
 <body>
  <div align="center">
<?php
$chart_file = "tmp/chart-" . time();
$cmd = "../ccltor_output -l tmp/output_web.log -P tmp -o png -s -5 -t all -i price -m :,P,R1,S1 -O " . $chart_file;
$cmdv = "../ccltor_output -l tmp/output_web.log -P tmp -o png -s -5 -t all -i volume -O " . $chart_file;
$values = array("M.SAN","M.BBVA","M.TEF","M.IBE","M.ITX","M.BVA","M.PAS","M.POP","M.REP");
for ($i = 0; $i < count($values); $i++) {
    system($cmd . "-$i.png -V $values[$i]");
    system($cmdv . "-v$i.png -V $values[$i]");
}
echo "<table><tr>";
for ($j = 0; $j < count($values); ) {
	for ($i = $j; $i < ($j + 3) && $i < count($values); $i++) {
    	echo '<td><img src="' . $chart_file . '-' . $i . '.png" width="400"/></td>';
	}
	echo "</tr><tr>";
	for ($i = $j; $i < ($j + 3) && $i < count($values); $i++) {
    	echo '<td><img src="' . $chart_file . '-v' . $i . '.png" height="120" width="400"/></td>';
	}
	$j = $i;
	echo "</tr><tr>";
}
echo "</tr></table>";

?>
</div>
 </body>
</html>
