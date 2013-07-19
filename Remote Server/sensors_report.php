<?php

	$tempF = $_GET["thermoF"];
	$tempC = $_GET["thermoC"];
	$d_oxy = $_GET["oxygen_density"];

	$tempF  = $tempF / 100;
	$tempC  = $tempC / 100;

	$tempF = sprintf("%.2f",$tempF);
	$tempC = sprintf("%.2f",$tempC);

	$record = file_get_contents("wired_data.txt");

	$time = date("M:d:Y H:i:s");

	$record .= "</br>\n";
	$record .= "$time :  <b>Temp: $tempF DO: $d_oxy </b>  ";

	file_put_contents("wired_data.txt",$record);


?>
