#include <Wire.h>
#include <EEPROM.h>

#include <Ezo_i2c.h>
#include <Ezo_i2c_util.h>
#include <iot_cmd.h>
#include <sequencer4.h>                  //imports a 4 function sequencer 

Ezo_board ph = Ezo_board(99, "PH");      //create a PH circuit object, who's address is 99 and name is "PH"
Ezo_board rtd = Ezo_board(102, "TEMP"); //create an RTD Temperature circuit object who's address is 102 and name is "TEMP"
Ezo_board DO = Ezo_board(97, "DO"); //create an DO dissolved Oxygen circuit object who's address is 97 and name is "DO"
Ezo_board ec = Ezo_board(100, "EC");    //create an EC Salinity circuit object who's address is 100 and name is "EC"

//forward declarations of functions to use them in the sequencer before defining them
void step1 (); //read RTD cicuit
void step2 (); //temperature compensation 
void step3 (); //send a read command
void step4 (); //print data to serial monitor or save to SD

Sequencer4 Seq(&step1, 1000, &step2, 1000, &step3, 1000, &step4, 1000); //calls the steps in sequence with time in between them

void setup() {
  Wire.begin();                           //start the I2C
  Serial.begin(9600);                   //start the serial communication to the computer
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
    DO.send_cmd_with_num("T,", 20.0);
   }
 }
 void step3() { //send a read command. we use this command instead of PH.send_cmd("R");
                //to let the library know to parse the reading
  ph.send_read_cmd();
  ec.send_read_cmd();
  DO.send_read_cmd();
 }
 void step4() { //print data so serial monitor
    //get the reading from the RTD circuit
    receive_and_print_reading(rtd);  
      if (rtd.get_error() == Ezo_board::SUCCESS);   //if the RTD reading was successful (back in step 1)
      Serial.print("  ");
    Serial.println();  
   //get the reading from the PH circuit
    receive_and_print_reading(ph);  
      if (ph.get_error() == Ezo_board::SUCCESS);   //if the PH reading was successful (back in step 2)
      Serial.print("  ");
   Serial.println();  
   //get the reading from the EC circuit
    receive_and_print_reading(ec);            
       if (ec.get_error() == Ezo_board::SUCCESS); //if the EC reading was successful (back in step 2)
      Serial.print("  ");      
   Serial.println();  
   //get the reading from the DO circuit
receive_and_print_reading(DO);            
       if (DO.get_error() == Ezo_board::SUCCESS); //if the DO reading was successful (back in step 2)
     Serial.print("  ");
 Serial.println();  
 }
 //void start_datalogging(){
  //polling = true;                                                 //set poll to true to start the polling loop


// function to decode the reading after the read command was issued
void receive_reading(Ezo_board &Sensor) {

  Serial.print(Sensor.get_name()); Serial.print(": ");  // print the name of the circuit getting the reading
  Sensor.receive_read_cmd();                            // get the response data

  switch (Sensor.get_error()) {                         // switch case based on what the response code is.
    case Ezo_board::SUCCESS:
      Serial.print(Sensor.get_last_received_reading()); //the command was successful, print the reading
      break;

    case Ezo_board::FAIL:
      Serial.print("Failed ");                          //means the command has failed.
      break;

    case Ezo_board::NOT_READY:
      Serial.print("Pending ");                         //the command has not yet been finished calculating.
      break;

    case Ezo_board::NO_DATA:
      Serial.print("No Data ");                         //the sensor has no data to send.
      break;
  }
}

// based on an example by Atlas Scientific
