
// (Based on Ethernet's WebClient Example)
#include <OneWire.h>
#include <DallasTemperature.h>
#include "WiFly.h"
#include "Credentials.h"
#include <SoftwareSerial.h>                                                    //add the soft serial libray



#define SERIAL_RATE 38400

#define rxpin 2                                                                //set the RX pin to pin 2
#define txpin 3                                                                //set the TX pin to pin 3
#define EPOCH_DELAY 666666

//int pump = 6;


// Data wire is plugged into pin 7 on the Arduino
#define ONE_WIRE_BUS 7

DeviceAddress thermometer = { 0x28, 0xCD, 0xCE, 0x36, 0x04, 0x00, 0x00, 0x94 };


String user_command = "";				// a string to hold incoming data from the PC

String data_parameters_string = "";


// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

SoftwareSerial oxy_serial(rxpin, txpin);                                         //enable the soft serial port



// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

String sensorstring = "";				// a string to hold the data from the Atlas Scientific product



/// TCP network
/*    KNONW ADDRESS VERSION
byte server[] = {64, 207, 139, 75}; // Taste Farm
//Client client(server, 80);
*/

/// DNS VERSION
Client client("grapethinking.com", 80);




/// STATE MACHINE

#define MAX_OXYGEN_READ_ATTEMPTS 20

boolean user_command_stringcomplete = false;		// have we received all the data from the PC

boolean did_read_temp = false;  			// control program state.
boolean did_read_oxygen_density = false;		// have we received all the data from the Atlas Scientific product

boolean standalone_state = false;
boolean oxygen_command = false;
int oxygen_read_attempts = MAX_OXYGEN_READ_ATTEMPTS;


typedef enum {
	TEMPERATURE,
	TEMPERATURE_C,
	TEMPERATURE_F,
	OXYGEN_DENSITY,
	ALL_SENSORS
} SensorTypes;


String sensor_state[ALL_SENSORS] = {
  "TEMPERATURE1",
  "TEMPERATURE1",
  "TEMPERATURE1",
  "TEMPERATURE1"  
};
boolean sensor_state_read[ALL_SENSORS] = { false, false };





void serialEvent() {                                             // if the hardware serial port receives a char
	char inchar = (char)Serial.read();                           // get the char we just received
	user_command += inchar;                                       // add it to the inputString
	if ( inchar == '\r' ) { user_command_stringcomplete = true; }            // if the incoming character is a <CR>, set the flag
}




void setup() {

	Serial.begin(SERIAL_RATE);

	WiFly.begin();

	if (!WiFly.join(ssid, passphrase)) {  // get onto the secure wifi.
		Serial.println("Association failed.");
		while (1) {
			// Hang on failure.
		}
	}

        //pinMode(pump, OUTPUT);

	// Start up the library
	sensors.begin();
	// set the resolution to 10 bit (good enough?)
	sensors.setResolution(thermometer, 10);

        oxy_serial.begin(SERIAL_RATE);                                                    //set baud rate for software serial port to 38400
   
        sensorstring.reserve(30);
        data_parameters_string.reserve(256);

	oxy_serial.print("E\r");
	delay(700);
	oxy_serial.print("L1\r");
	delay(700);

}




void
process_user_command(String user_command) {
	if ( user_command == "detach" ) {
		standalone_state = true;
	} else {
		// other commands...
	}
}





///  TEMPERATURE READ & TRANSMIT

// READ
// Read temperature and format a string

String read_temperature(DeviceAddress device_address) {
  String temp_report = "-0.0";
  
  sensors.requestTemperatures();              /// tell the device to measure.
  
  float tempC = sensors.getTempC(device_address);    /// read the output port
    
  if (tempC == -127.00) {
    Serial.print("Error getting temperature");
  } else {

	  /// url parameters
    
    unsigned long int tC = (unsigned long int)(tempC*100);
    String tempC_str =  String(tC, DEC); 
    
    temp_report = "thermoC=";
    temp_report += tempC_str;

    sensor_state[TEMPERATURE_C] = tempC_str;
   
    tC =  (unsigned long int)(DallasTemperature::toFahrenheit(tempC)*100);
    String tempF_str =  String(tC, DEC); 
    
    sensor_state[TEMPERATURE_F] = tempF_str;

    temp_report += "&thermoF=";
    temp_report += tempF_str;

    Serial.println(temp_report);
  }
  
  return(temp_report);
}



// TRANSMIT


void
clear_transmit_parameters(void) {
	data_parameters_string = "";
}


void
add_transmit_parameter(String parameter,String value) {
	String pfront = "?";
	if ( data_parameters_string.length() > 0 ) {
		pfront = "&";
	}
	data_parameters_string += pfront;
        data_parameters_string += parameter;
        data_parameters_string += "=";
        data_parameters_string += value;
 }



void
transmit_data()
{
	Serial.println("connecting...");

	if (client.connect()) {
		Serial.println("connected");
	} else {
		Serial.println("connection failed");
	}

	String get_line = "GET /taste/sensors_report.php";   /// url to handle recpetion of measurement, this client..
	get_line += data_parameters_string;
	get_line += " HTTP/1.1";

	Serial.println(get_line);

	if ( client.connected() ) {
		client.println(get_line);
		client.println("Host: s149546.gridserver.com");
		client.println();
	}

}



void
transmit(void) {
  
 Serial.println("TRANSMIT " + data_parameters_string);
  
	transmit_data();
}





void loop() {
  
	String temp = "";
/*
	if ( !standalone_state ) {

		if ( user_command_stringcomplete ) {					// if a string from the PC has been recived in its entierty
			oxy_serial.print(user_command_stringcomplete);		// send that string to the Atlas Scientific product
			process_user_command(user_command);
			user_command = "";									// clear the string:
			user_command_stringcomplete = false;				// reset the flage used to tell if we have recived a completed string from the PC
		}

	} else {
*/


			/// reads a character at a time each time through the loop.   // perhaps there can be a nested loop in a subroutine. (Arduino context has to be understood.)
		if ( did_read_temp && did_read_oxygen_density ) { // Program state, wait for HTTP response.

			if ( client.available() ) {
				char c = client.read();
				Serial.print(c);
			}

			if ( !client.connected() ) {
				Serial.println();
				Serial.println("disconnecting.");
				client.stop();
									/// Got done with the communication... get ready for the next reading.
				did_read_temp = false;
				did_read_oxygen_density = false;
				oxygen_command = false;
                                oxygen_read_attempts = MAX_OXYGEN_READ_ATTEMPTS;
                                sensorstring = "";

Serial.println("one internet message sent");
				delay(EPOCH_DELAY); // wait some time between readings for business process, etc.  Check on wheather a failed attempt at reading happened.
                                //digitalWrite(pump, HIGH);   // turn the LED on (HIGH is the voltage level)
                                //delay(15000);               // wait for a second
                                //digitalWrite(pump, LOW);    // turn the LED off by making the voltage LOW
  			/*    for(;;)
				;
			*/
			}

		}


		// read serial line ( oxygen density ) until line end
		if ( !did_read_oxygen_density ) {

			if ( !oxygen_command ) {  /// did not start reading => SO ISSUE READ COMMAND
				oxy_serial.print("R\r");
				delay(1000);			// delay time for chip
				oxygen_command = true;  // don't process until completely collected..
				oxygen_read_attempts = MAX_OXYGEN_READ_ATTEMPTS;
			}

                        if ( oxygen_read_attempts >= 0 ) {
         			oxygen_read_attempts--;
                        }

			while ( oxy_serial.available() ) {							// while a char is holding in the serial buffer
				char inchar = (char)oxy_serial.read();					// get the new char
				if (inchar == '\r') {
					
					did_read_oxygen_density = true;

					sensor_state[OXYGEN_DENSITY] = sensorstring;
					sensor_state_read[OXYGEN_DENSITY] = true;
 Serial.println(sensorstring);
					break;

				} 	// if the incoming character is a <CR>, set the flag

				sensorstring += inchar;									// add it to the sensorString
			}


			if ( oxygen_read_attempts <= 0 ) {
                                did_read_oxygen_density = true;
				sensor_state_read[OXYGEN_DENSITY] = true;
			}

		}


		if ( sensor_state_read[OXYGEN_DENSITY] && !did_read_temp ) {    // don't read until the oxygen is collected. And, read just once.

			sensor_state[TEMPERATURE] = read_temperature(thermometer);
			sensor_state_read[TEMPERATURE] = true;
			did_read_temp = true;

		}


		if ( sensor_state_read[TEMPERATURE] && sensor_state_read[OXYGEN_DENSITY] ) {   ///  When both are read... tell the host.
Serial.println("READY");

			clear_transmit_parameters();

Serial.println("TEMPERATURE_C " + sensor_state[TEMPERATURE_C]);
Serial.println("TEMPERATURE_F " + sensor_state[TEMPERATURE_F]);
Serial.println("OXYGEN_DENSITY " + sensor_state[OXYGEN_DENSITY]);

			add_transmit_parameter("thermoC",sensor_state[TEMPERATURE_C]);
			add_transmit_parameter("thermoF",sensor_state[TEMPERATURE_F]);
			sensor_state_read[TEMPERATURE] = false;

			if ( oxygen_read_attempts > 0 ) {
				add_transmit_parameter("oxygen_density",sensor_state[OXYGEN_DENSITY]);
				sensor_state_read[OXYGEN_DENSITY] = false;
			}


			transmit_data();
		}

/*
	}
*/


}


/// temperatur=45&oxygen_density=0.7




