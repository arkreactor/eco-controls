<?php

	$temp = file_get_contents("wired_data.txt");
	
	$temp_cp = split("</br>", $temp);	
	
	$myLastElement = end($temp_cp);
	reset($temp_cp);
	
	$myLastElement = str_replace("\n", "", $myLastElement);
	

?>
<!DOCTYPE html>
<html lang="en">
	<head>
		<link rel="stylesheet" href="./css/bootstrap.css" type="text/css"/>
		<link rel="stylesheet" href="./css/style.css" type="text/css"/>
		<script type="text/javascript" src="js/jquery-1.9.1.min.js"></script>
		<script src="js/bootstrap.js" language="javascript" type="text/javascript" ></script>
		<script language="javascript"> 
			function toggleData(){
				var data = document.getElementById("dataDiv");
				var btn = document.getElementById("buttonReadings");


				if (data){
					var vis = data.style.visibility
					if (vis == "hidden"){
						data.style.visibility = "visible";
						btn.innerHTML = "Hide Readings";
					} else {
						data.style.visibility = "hidden";
						btn.innerHTML = "Show Readings";
					}
				}
			}
			
			function togglePump(state){
					if (state == 1){
						$.ajax({
							url: "pumpControl.php?state=on",
							type: "POST",
							timeout: onInterval,
							data: "",
							error: function(){ alert("An error occurred") },
							//success: function(response){ alert(response.Time+"::"+response.Sensa); }
							success: function(response){ displaySensa(response); }
							}); 
					} else {
						$.ajax({
							url: "pumpControl.php?state=off",
							type: "POST",
							data: "",
							error: function(){ alert("An error occurred") },
							//success: function(response){ alert(response.Time+"::"+response.Sensa); }
							success: function(response){ displaySensa(response); }
							}); 
					}		
			}
			
			
		
			
			function displaySensa(currentSensa){
				$("#currentSensaDiv").html(currentSensa.Sensa);	
			}
			
			function updateSensa(){
				$.ajax({
					url: "currentSensa.php",
					dataType: "json",
					type: "GET",
					data: "",
					error: function(){ alert("An error occurred") },
					//success: function(response){ alert(response.Time+"::"+response.Sensa); }
					success: function(response){ displaySensa(response); }
					}); 
			}
			
			function setupTimer(){
				setInterval(updateSensa, 10000);
			}
			
		</script>
	</head>

	<body onload="setupTimer()"> 
		<center><img src="img/sensabio.jpg" ></center>

		<div id="showData">	

<?php
		echo "*** CURRENT SENSA ***"."<br/>";
?>

		<div id="currentSensaDiv">	
<?php
		echo $myLastElement."<br>";
?>
		</div>
		
		<br>

		<button id="buttonReadings" class="btn btn-primary" type="button" onclick="toggleData()">Show Readings</button>

		<div id="buttonPmp">
			<button id="buttonPumpOn" class="btn btn-primary" type="button" onclick="togglePump(1)">Turn Pump On</button>
			<select>
			  <option>10 seconds</option>
			  <option>20 seconds</option>
			  <option>30 seconds</option>
			  <option>1 minute</option>
			  <option>2 minutes</option>
			</select>
			<button id="buttonPumpOff" class="btn btn-primary" type="button" onclick="togglePump(0)">Turn Pump Off</button>
			
		</div>

			<div id="dataDiv" style="visibility:hidden">
<?php
		echo $temp;
?>
			</div>

		</div>

	</body>
</html>
