#include <Adafruit_BME280.h>
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

uint16_t rtc_year;
uint8_t rtc_month;
uint8_t rtc_day;
uint8_t rtc_hour;
uint8_t rtc_minute;
uint8_t rtc_second;


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

void setup() {  
  //endring av multigas i2c-addresse kan droppes
  gas.begin(MULTIGAS_ADDR_OLD);     //
  gas.change_i2c_address(MULTIGAS_ADDR_NEW);
  gas.powerOn();

/*-------------bme sensor---------------*/
  bool status;  
  status = bme.begin();  
  if (!status) {
      Serial.println("Could not find a valid BME280 sensor, check wiring!");
      while (1);
  }
/*-------------------------------------------------------------------*/
}

void loop() {
  multigas();
  rtc();
  bme_sens_read();
}



void multigas(){
  gasread.co = gas.measure_CO();
  gasread.nh3 = gas.measure_NH3();
  gasread.no2 = gas.measure_NO2();
}

void rtc() {
  RTC_DS3231 rtc;

  DateTime now = rtc.now();

  rtc_year = now.year();
  rtc_month = now.month();
  rtc_day = now.day();
  rtc_hour = now.hour();
  rtc_minute = now.minute();
  rtc_second = now.second();
  
  //return rtc_year, rtc_month, rtc_day, rtc_hour, rtc_minute, rtc_second;
}

void bme_sens_read() {
  temp = bme.readTemperature();
  pressure = bme.readPressure() / 100.0F;
  altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  humidity = bme.readHumidity();
}
/*
 * 
 * 
 * PSEUDOKODE
 * Definerer variabler og I2C-addresser
 * 
 * 
 * void setup(){
 * wire.begin();
 * 
 * }
 * 
 * void loop (){
 *  partikkelsensor();
 *  trykkogfuktighet();
 *  ozone();
 *  voc();
 *  rtc();
 *  multigas();
 * }
 * 
 * 
 * byte partikkelsensor(){
 * 
 * }
 * 
 * byte trykkogfuktighet(){
 * 
 * }
 * 
 * byte ozone(){
 * 
 * }
 * 
 * byte voc(){
 * 
 * }
 * 
 * byte rtc(){
 * 
 * }
 * 
 * byte mutligas(){
 * 
 * }
 */



