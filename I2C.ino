#include <sps30.h>
#include <Adafruit_BME280.h>
#include <MQ131.h>  //Heater consumes at least 150 mA. So, don't connect it directly on a pin of the Arduino
#include <Wire.h>
#include <RTClib.h>
#include <MutichannelGasSensor.h>



#define partikkel_address 0x69 
#define rtc_address  0x68
#define MULTIGAS_ADDR_OLD     0x04        // default to 0x04
#define MULTIGAS_ADDR_NEW     0x19        // change i2c address to 0x19
#define humidity_pressure_address 0x77
#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C

MQ131 sensor(2,A0, LOW_CONCENTRATION, 10000); 

int rtc_year;
int rtc_month;
int rtc_day;
int rtc_hour;
int rtc_minute;
int rtc_second;


float Ozoneppm ; //ppm

typedef struct multigasreadings {
  float co;     //ppm
  float nh3;    //ppm
  float no2;    //ppm
};

float temp;
float pressure;
float altitude;
float humidity;

multigasreadings gasread;
 RTC_DS3231 rtc;
void setup() {  
  Serial.begin(9600);
  
  //endring av multigas i2c-addresse kan droppes
  //gas.begin(MULTIGAS_ADDR_OLD);     //
  //gas.change_i2c_address(MULTIGAS_ADDR_NEW);
  gas.begin(MULTIGAS_ADDR_NEW);//the default I2C address of the slave is 0x04
  gas.powerOn();
  
rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
//Ozonesetup must be edited
 // ozonesetup();

/*-------------bme sensor---------------*/
  bool status;  
  status = bme.begin();  
  if (!status) {
      Serial.println("Could not find a valid BME280 sensor, check wiring!");
      //while (1);
  }
/*-------------------------------------------------------------------*/

}

void loop() {
  multigas();
  rtc_read();
  delay(1000);
  //bme_sens_read();
  //ozoneread();
}



void multigas(){
  gasread.co = gas.measure_CO();
  gasread.nh3 = gas.measure_NH3();
  gasread.no2 = gas.measure_NO2();
 
  Serial.println(gas.measure_CO());
  Serial.println(gas.measure_NH3());
  Serial.println(gas.measure_NO2());
  Serial.println();
  
}

void rtc_read() {
  DateTime now = rtc.now();
  rtc_year = now.year();
  rtc_month = now.month();
  rtc_day = now.day();
  rtc_hour = now.hour();
  rtc_minute = now.minute();
  rtc_second = now.second();

  Serial.println(rtc_year);
  Serial.println(rtc_month);
  Serial.println(rtc_day);
  Serial.println(rtc_hour);
  Serial.println(rtc_minute);
  Serial.println(rtc_second);
  Serial.println();
}


void bme_sens_read() {
  temp = bme.readTemperature();
  pressure = bme.readPressure() / 100.0F;
  altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  humidity = bme.readHumidity();
}

// - Ozonedetector gasread;
// - Heater control on chosen pin (Pin 2)
// - Sensor analog read on chosen pin (Pin A0)
// - Model LOW_CONCENTRATION
// - Load resistance RL of 10KOhms (10000 Ohms)

//Defines MQ131 sensorvalues

void ozonesetup() {
  
  sensor.begin();
  Serial.println("Calibration in progress...");
  
  sensor.calibrate(); //Setup for the MQ131
                      //Example: R0 = 10kOhm
                      //Tells time for heating. Needed for accurate reading - Usually powered for more than 24h.
  
  Serial.println("Calibration done!");
  Serial.print("R0 = ");
  Serial.print(sensor.getR0());
  Serial.println(" Ohms");
  Serial.print("Time to heat = ");
  Serial.print(sensor.getTimeToRead());
  Serial.println(" s");

}
void ozoneread() {
  Ozoneppm = sensor.getO3(PPM); //This gives a value in parts per million, change to PPB if a more accurate value is needed
             //sensor.getO3(PPB);
}

