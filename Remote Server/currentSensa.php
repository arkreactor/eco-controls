<?php

	$temp = file_get_contents("wired_data.txt");

	$temp_cp = split("</br>", $temp);	

	$myLastElement = end($temp_cp);
	reset($temp_cp);

	$myLastElement = str_replace("\n", "", $myLastElement);
	
	$jsonOutput =<<<THISISJSON
	{
		"Time": "{$currentTime}",
		"Sensa": "{$myLastElement}"
	}
THISISJSON;

 echo $jsonOutput;
?>

