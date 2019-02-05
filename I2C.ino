#include <Wire.h>
#include <RTClib.h>
#include <MutichannelGasSensor.h>

int partikkel_address = 105;

uint16_t rtc_year;
uint8_t rtc_month;
uint8_t rtc_day;
uint8_t rtc_hour;
uint8_t rtc_minute;
uint8_t rtc_second;

void setup() {


}

void loop() {
  

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
 

