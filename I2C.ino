#include <Wire.h>
#include <RTClib.h>
#include <MutichannelGasSensor.h>


#define partikkel_address 0x69 
#define rtc_address  0x68
#define SENSOR_ADDR_OLD     0x04        // default to 0x04
#define SENSOR_ADDR_NEW     0x19        // change i2c address to 0x19

typedef struct multigasreadings {
  float co;     //ppm
  float nh3;    //ppm
  float no2;    //ppm
  float c3h8;   //ppm
  float c4h10;  //ppm
  float ch4;    //ppm
  float h2;     //ppm
  float c2h5oh; //ppm
};

multigasreadings gasread;

void setup() {  
  //endring av multigas i2c-addresse kan droppes
  gas.begin(SENSOR_ADDR_OLD);     //
  gas.change_i2c_address(SENSOR_ADDR_NEW);
  gas.powerOn();
}

void loop() {
  multigas();
}



void multigas(){
  gasread.co = gas.measure_CO();
  gasread.nh3 = gas.measure_NH3();
  gasread.no2 = gas.measure_NO2();
  gasread.c3h8 = gas.measure_C3H8();
  gasread.c4h10 = gas.measure_C4H10();
  gasread.ch4 = gas.measure_CH4();
  gasread.h2 = gas.measure_H2();
  gasread.c2h5oh = gas.measure_C2H5OH();
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
