#include "SPI.h"

#define     CS_Pin   PE3                                    // Pin PE3 as the chip select
#define     DELTA_T     25                                  // 1000/40 = 25 hz, 1000/20 = 50 hz

SPIClass SPI_1(1);                                          // Create an instance of the SPI Class called SPI_1 that uses the 2nd SPI Port
SPISettings settingsA(2000000, MSBFIRST, SPI_MODE3);        // Set up the speed, data order and data mode

int counter;
byte w;
short x,y,z;
float K=0.061;                                              // (4000/65535) milli-g per digit for +/-2g full scale using 16 bit digital output
float x_fl,y_fl,z_fl;
unsigned long time_now,time_,time_old;
float G_Dt=0.02;

void setup() {
  // put your setup code here, to run once:

  //Initialize USB port and speed
  Serial.begin(9600);

  //SPI channel was in SPI_1
  pinMode (CS_Pin, OUTPUT);                                 //Chip Select pin to control SPI
  digitalWrite(CS_Pin, HIGH);                               //Disable SPI
  SPI_1.begin();
  SPI_1.beginTransaction(settingsA);

  // read who am i data from acc LIS3DSH
  digitalWrite(CS_Pin, LOW);                                //Enable SPI
  SPI_1.transfer(0x8F);
  w = SPI_1.transfer(0);                                    //Read 'WHO_AM_I' register
  digitalWrite(CS_Pin, HIGH);                               //Disable SPI  
  Serial.println("WAI = " + String (w));
    
  // setting CTRL_REG4
  digitalWrite(CS_Pin, LOW);                                //Enable SPI
  SPI_1.transfer(0x20);                                     //Send address of 'Control register 4' to write configuration
  SPI_1.transfer(0x7F);                                     //Write a value that enables x,y,z accelerometers
  digitalWrite(CS_Pin, HIGH);                               //Disable SPI

  // setting CNTRL_REG5
  digitalWrite(CS_Pin, LOW);                                //Enable SPI
  SPI_1.transfer(0x24);                                     //Send address of 'Control register 4' to write configuration
  SPI_1.transfer(0x00);                                     //Write a value that enables x,y,z accelerometers, 0x00 2g, 0x08 4g, 0x10 6g, 0x18 8G, etc
  digitalWrite(CS_Pin, HIGH);                               //Disable SPI

  counter = 0;
  time_ = millis();
  delayMicroseconds(DELTA_T);
  
}

void loop() {
  // put your main code here, to run repeatedly:

   time_now = millis();                              // delta t

   if((time_now-time_)>=DELTA_T){
  
    time_old = time_;
    time_ = time_now;   
    G_Dt = (time_-time_old)/1000.0;

    //read data accelerometer
    digitalWrite(CS_Pin, LOW);                      //Enable SPI
    
    SPI_1.transfer(0xA8);                           //Send address of LSB of x. Address is auto-increased after each reading.
    x = SPI_1.transfer(0) | SPI_1.transfer(0)<<8;   //x acceleration 
    y = SPI_1.transfer(0) | SPI_1.transfer(0)<<8;   //y acceleration
    z = SPI_1.transfer(0) | SPI_1.transfer(0)<<8;   //z acceleration
    digitalWrite(CS_Pin, HIGH);                     //Disable SPI

    x_fl=x*K;
    y_fl=y*K;
    z_fl=z*K;
    
    Serial.println(String(counter)+","+String(time_now)+","+String(x_fl)+","+String(y_fl)+","+String(z_fl));
    
    counter++;    
   }
  
    // The code in the "loop()" statement will automatically repeat.
}
