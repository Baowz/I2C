//----------------------------------------------------------------
//These are libraries needed for the different code functions
#include <Adafruit_BME280.h> //Pressure and humidity sensor
//#include <MQ131.h>  // Ozone sensor - The heater consumes at least 150 mA. So, don't connect it directly on a pin of the Arduino
#include <Wire.h>   //Standard arduino library
#include <RTClib.h> //Real rime clock 
#include <MutichannelGasSensor.h> //Multigas sensor - Grove P2502 
#include <SPI.h>    //bibliotek for SPI-kommuniskajon 
#include <SD.h>     //bibliotek for SD-kort
//----------------------------------------------------------------
//I2C adresses - Crucial for multisensor communication. Change if needed.
#define rtc_address  0x68                 // Default I2C adress
//#define MULTIGAS_ADDR_OLD     0x04        // default to 0x04
#define MULTIGAS_ADDR_NEW     0x19        // change I2C address to 0x19
#define humidity_pressure_address 0x77    // Defrault I2C adress
#define iaqaddress 0x5A                   // VOC I2C adress
#define SEALEVELPRESSURE_HPA (1013.25)    // Defines normalized pressure

//----------------------------------------------------------------
//Integers adding - Gives sign bits
int delay_time = 1000;
int rtc_year;
int rtc_month;
int rtc_day;
int rtc_hour;
int rtc_minute;
int rtc_second;
uint8_t statu;       // VOC
uint16_t tvoc;       // VOC
uint16_t predict;    // VOC
int32_t resistance;  // VOC
//float Ozoneppm ; //ppm
float temperature;     //Celsius
float pressure; //Bar
float altitude; //Meters
float humidity; //Percentage

typedef struct multigasreadings {
  float co;     //ppm
  float nh3;    //ppm
  float no2;    //ppm
};
multigasreadings gasread; //Defines Multigrasreading

//----------------------------------------------------------------
//Spesific variables from sensor libraries
Adafruit_BME280 bme; // I2C
RTC_DS3231 rtc; //Defines RTC
//MQ131 sensor(2,A0, LOW_CONCENTRATION, 10000); // - Ozonedetector gasread;
//                                              // - Heater control on chosen pin (Pin 2)
//                                              // - Sensor analog read on chosen pin (Pin A0)
//                                              // - Model LOW_CONCENTRATION
//                                              // - Load resistance RL of 10KOhms (10000 Ohms)

//----------------------------------------------------------------
//Setup - This initialize variables, pin modes and libraries

/*
  MOSI - pin 51
  MISO - pin 50
  SCK - pin 52
  sS - pin 53
*/

const int chipSelect = 53;  //pin for SPI-kommunikasjon

void setup() {
  Serial.begin(9600); //This is the datadrate - 9600 bit/s
  Wire.begin();  //starts I2C
  gas.begin(MULTIGAS_ADDR_NEW); //The default I2C address of the slave is 0x04
  gas.powerOn(); //Function fur turning the multigas sensor on
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //Adds the value of todays date and time from the connected computer to the rtc clock
  rtc.begin();
  bool status;
  status = bme.begin();
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
  }

  //  ozonesetup();
  if (!SD.begin(chipSelect)) {        
    Serial.println("Could not find SD-Card");
    return;
  }
  Serial.println("SD-card ready");
  //Write a header in txt file
  String dataString = "";
  dataString += "Time"; 
  dataString += ", Preasure";
  dataString += ", Temperature";
  dataString += ", Humidity";
  dataString += ", Altitude";
  dataString += ", tvoc";
  dataString += ", Predict"; 
    
  File dataFil = SD.open("weatherData.txt", FILE_WRITE);
  if (dataFil) {
    dataFil.println(dataString);   //write string to SD-card
    dataFil.close();               //clode file
    Serial.println(dataString);     //Print string to seriell monitor
  }
  else {
    Serial.println("error opening weatherData.txt");
  }
}

//--------------------------------------------------------------------
//Declare what functions that the program is going to run
void loop() {
  multigas(); //Calls for multigas function
  delay(delay_time);
  rtc_read(); //Calls for clock function
  delay(delay_time);
  bme_sens_read();
  delay(delay_time);
  voc();      //Calls for the VOC (CO2) sensor function
  delay(1000);
  //  ozoneread();
  //  delay(delay_time);
  lagreSD();
  }


//---------------------------------------------------------------------
//Under are the functions called for in void loop

void multigas() {                   //Multigass function
  Serial.println("Multigas_sens");
  gasread.co = gas.measure_CO();    //Reads CO value from sensor
  gasread.nh3 = gas.measure_NH3();  //Reads NH3 value from sensor
  gasread.no2 = gas.measure_NO2();  //Reads NO2 value from sensor

  Serial.println(gas.measure_CO()); //Prints to scope
  Serial.println(gas.measure_NH3());
  Serial.println(gas.measure_NO2());
  Serial.println();

}

void rtc_read() {                   //Real time clock function
  Serial.println("RTC");
  DateTime now = rtc.now();         //Calls for the microcontrollers value
  rtc_year = now.year();            //Values rtc_year with value from clock
  rtc_month = now.month();          //Values rtc_month with value from clock
  rtc_day = now.day();              //Values rtc_day with value from clock
  rtc_hour = now.hour();            //Values rtc_hour with value frmo clock
  rtc_minute = now.minute();        //Values rtc_minutes with value from clock
  rtc_second = now.second();        //Vaøies rtc_second with value from clock

  Serial.println(rtc_year);         //Prints to scope
  Serial.println(rtc_month);
  Serial.println(rtc_day);
  Serial.println(rtc_hour);
  Serial.println(rtc_minute);
  Serial.println(rtc_second);
  Serial.println();                 //Adds a spacing at the end
}


void voc() {                        //VOC-sensor function

  readAllBytes_voc();                   //Function for reading sensor bytes
  checkStatus_voc();                    //Checks status (Pollutionvalue of CO2)
  Serial.println("VOC");
  Serial.print("CO2:");             //Prints to scope
  Serial.print(predict);
  Serial.print(", Status:");
  Serial.print(statu, HEX);
  Serial.print(", Resistance:");
  Serial.print(resistance);
  Serial.print(", TVoC:");
  Serial.println(tvoc);
  Serial.println();                 //Adds a spacing at the end
  //delay(2000);
}

void readAllBytes_voc() {              //readAllBytes function used in the VOC function

  Wire.requestFrom(iaqaddress, 9); //Requests data from the I2C adress
  //Under values are read from the I2C of the sensor
  predict = (Wire.read() << 8 | Wire.read());
  statu = Wire.read();
  resistance = (Wire.read() & 0x00) | (Wire.read() << 16) | (Wire.read() << 8 | Wire.read());
  tvoc = (Wire.read() << 8 | Wire.read());
}

void checkStatus_voc() {

  //VOC printing sequence

  if (statu == 0x10)    // Status = Ready to read. If not = No module
  {
    Serial.println("Warming up..."); //Prints to scope
  }
  else if (statu == 0x00)
  {
    Serial.println("Ready");
  }
  else if (statu == 0x01)
  {
    Serial.println("Busy");
  }
  else if (statu == 0x80)
  {
    Serial.println("Error");
  }
  else
    Serial.println("No Status, check module");
}

void bme_sens_read() {
  Serial.println("BME_SENSOR");
  temperature = bme.readTemperature();            //Reads temperature value from sensor
  Serial.println(temperature);
  pressure = bme.readPressure() / 100.0F;  //Calculates pressure
  Serial.println(pressure);
  altitude = bme.readAltitude(SEALEVELPRESSURE_HPA); //Reads altitude and uses the SEALEVELPRESSURE numeral to calculate difference
  Serial.println(altitude);
  humidity = bme.readHumidity();           //Reads humidity
  Serial.println(humidity);
  Serial.println();
}

//Defines MQ131 sensorvalues
//void ozonesetup() {
//  sensor.begin();
//  Serial.println("Calibration in progress...");
//
//  sensor.calibrate(); //Setup for the MQ131
//                      //Example: R0 = 10kOhm
//                      //Tells time for heating. Needed for accurate reading - Usually powered for more than 24h.
//
//  Serial.println("Calibration done!"); //Prints to scope
//  Serial.print("R0 = ");
//  Serial.print(sensor.getR0());        //Fetches the R0 resistor value
//  Serial.println(" Ohms");
//  Serial.print("Time to heat = ");
//  Serial.print(sensor.getTimeToRead());
//  Serial.println(" s");
//}

//void ozoneread() {
//  Ozoneppm = sensor.getO3(PPM);        //This gives a value in parts per million, change to PPB if a more accurate value is needed.
//                                       //sensor.getO3(PPB);
//}


void lagreSD(){//Print global variables to SD-card
  String dataString = "";
  dataString += String(rtc_second);
  dataString += String(rtc_minute);
  dataString += String(rtc_hour);  
  dataString += ", ";
  dataString += String(pressure);
  dataString += ", ";
  dataString += String(temperature);
  dataString += ", ";
  dataString += String(humidity);
  dataString += ", ";
  dataString += String(altitude);
  dataString += ", ";
  dataString += String(tvoc);
  dataString += ", ";
  dataString += String(predict);         

  
  File dataFil = SD.open("weatherData.txt", FILE_WRITE);
  if (dataFil) {
    dataFil.println(dataString);   //write string to SD-card
    dataFil.close();               //clode file
    Serial.println(dataString);     //Print string to seriell monitor
  }
  else {
    Serial.println("error opening weatherData.txt");
  }
}
