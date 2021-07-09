#include <sequencer2.h>
#include <Ezo_i2c_util.h>
#include <sequencer1.h>
#include <Ezo_i2c.h>
#include <sequencer4.h>
#include <sequencer3.h>
#include <iot_cmd.h>

#include <Wire.h>
#include <EEPROM.h>

// EZO/Atlas stuff (not sure iot_cmd is used; TODO: remove and test)
#include <Ezo_i2c.h>
#include <Ezo_i2c_util.h>
#include <iot_cmd.h>
#include <sequencer4.h>                  //imports a 4 function sequencer 

// Set EZO addresses
Ezo_board ph = Ezo_board(99, "PH");      //create a PH circuit object, who's address is 99 and name is "PH"
Ezo_board rtd = Ezo_board(102, "TEMP"); //create an RTD Temperature circuit object who's address is 102 and name is "TEMP"
Ezo_board DO = Ezo_board(97, "DO"); //create an DO dissolved Oxygen circuit object who's address is 97 and name is "DO"
Ezo_board ec = Ezo_board(100, "EC");    //create an EC Salinity circuit object who's address is 100 and name is "EC"

//SD stuff
#include <SPI.h>
#include <SD.h>
const int chipSelect = 10;

//forward declarations of functions to use them in the sequencer before defining them
void step1 (); //read RTD cicuit
void step2 (); //temperature compensation 
void step3 (); //send a read command
void step4 (); //print data to serial monitor or save to SD

Sequencer4 Seq(&step1, 1000, &step2, 1000, &step3, 1000, &step4, 1000); //calls the steps in sequence with time in between them

void setup() {
  Wire.begin();                           //start the I2C
  Serial.begin(9600);                   //start the serial communication to the computer

  // Initialize SD card
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  
}

void loop() {
 Seq.run();                              //run the sequncer to do the polling
}
 void step1() {   //get the reading from the RTD circuit
 rtd.send_read_cmd();
 }
 void step2() {   //assign temperature for temperature compensation
   if ((rtd.get_error() == Ezo_board::SUCCESS) && (rtd.get_last_received_reading() > -1000.0)) { //if the temperature reading has been received and it is valid
    ph.send_cmd_with_num("T,", rtd.get_last_received_reading());
    ec.send_cmd_with_num("T,", rtd.get_last_received_reading());
    DO.send_cmd_with_num("T,", rtd.get_last_received_reading());
    } 
  else {                                                                                      //if the temperature reading is invalid
    ph.send_cmd_with_num("T,", 25.0);
    ec.send_cmd_with_num("T,", 25.0);                                                          //send default temp = 25 deg C to EC sensor
    DO.send_cmd_with_num("T,", 25.0);
   }
 }
 void step3() { //send a read command. we use this command instead of PH.send_cmd("R");
                //to let the library know to parse the reading
  ph.send_read_cmd();
  ec.send_read_cmd();
  DO.send_read_cmd();
 }
 void step4() { //print data to serial monitor and to SD card

  // make a string for assembling the data to log:
  String dataString = "";
  
  //get the reading from the RTD circuit
  receive_and_print_reading(rtd);
  if (rtd.get_error() == Ezo_board::SUCCESS);   //if the RTD reading was successful (back in step 1)
  {
    dataString += String(rtd.get_last_received_reading());
    dataString += ",";
    Serial.print("  ");
  }
    
  Serial.println();
  
  //get the reading from the PH circuit
  receive_and_print_reading(ph);
  if (ph.get_error() == Ezo_board::SUCCESS); //if the pH reading was successful (back in step 2)
  {
    dataString += String(ph.get_last_received_reading());
    dataString += ",";
    Serial.print("  ");
  }
  Serial.println();
  
  //get the reading from the EC circuit
  receive_and_print_reading(ec);
  if (ec.get_error() == Ezo_board::SUCCESS); //if the EC reading was successful (back in step 2)
  {
    dataString += String(ec.get_last_received_reading());
    dataString += ",";
    Serial.print("  ");
  }
  Serial.println();
  
  //get the reading from the DO circuit
  receive_and_print_reading(DO);   
  dataString += String(DO.get_last_received_reading(), 2);
  if (DO.get_error() == Ezo_board::SUCCESS); //if the DO reading was successful (back in step 2)
  {
    dataString += String(do.get_last_received_reading());
    dataString += ",";
    Serial.print("  ");
  }
  Serial.println();  

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    
    // print to the serial port, too:
    Serial.println(dataString);
  }
  
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
  
}

// based on an example by Atlas Scientific
