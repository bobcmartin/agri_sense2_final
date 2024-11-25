
/*

    AVR IoT / AVNET Soil Moisture Sensor 
    
    Bob Martin / Microchip
    Oct 2024

*/
/*
    (c) 2022 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#include <string.h>
#include <stdlib.h>
#include "log.h"

#include <Wire.h>
#include <Adafruit_MS8607.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_seesaw.h"
#include "agri-sense2.h"


void iotconnect_init(void);
void iotconnect_publish(void);    


void init_sensor_array(void);
void connect_lte(void);
void read_sensor_array(void);

// I2C addresses
#define T6793_ADDRESS      0x15
#define I2C_MUX 0x70

#define PTH_CH_ID          3
#define CO2_CH_ID          3
#define SS3_CH_ID          4
#define SS4_CH_ID          5
#define SS2_CH_ID          6
#define SS1_CH_ID          7




// create sensor objects

Adafruit_MS8607 pth_ms8607;
Adafruit_seesaw ss1(&Wire1);
Adafruit_seesaw ss2(&Wire1);
Adafruit_seesaw ss3(&Wire1);
Adafruit_seesaw ss4(&Wire1);

#define VBAT_CH A2
#define SAMPLE_TIME 5000UL     

// globals 
struct _data_frame data_frame,*frame_ptr;
unsigned long int sample_time = 0UL;

void setup() 
{
 
  Log.begin(115200);
  Log.raw("====== Agri-sense 2 Core ======");

  frame_ptr = &data_frame;
  frame_ptr->frame_count = 0UL;
  
  sensor_array_init();  
  iotconnect_init();

}

void spin_lock(void)
{

   while(1)
    {

      Log.raw("beep\r\n");
      delay(1000);

    }

}


void loop()
{
   unsigned long now;

   while(1)    // true executive loop
   {
      now = millis();
      if (now - sample_time > SAMPLE_TIME)
      {
          sample_time = now;
          frame_ptr->frame_count++;
          scan_ms8607();
          scan_T6793();
          scan_bat_voltage();
          scan_ss1();
          scan_ss2();
          scan_ss3();
          scan_ss4();
          print_scan();               
          iotconnect_publish();    
      }
   }

}

void print_scan(void)
{
    Log.rawf("\r\n====== frame : %lu ========= \r\n",frame_ptr->frame_count);
    Log.rawf("air temp : %5.2f  humidity : %3.1f atm pressure : %5.1f\r\n", frame_ptr->pth_temp,frame_ptr->pth_humidity,frame_ptr->pth_pressure);
    Log.rawf("ss1 temp : %5.2f ss1 moisture lvl %d\r\n",frame_ptr->ss1_temp,frame_ptr->ss1_moisture);
    Log.rawf("ss2 temp : %5.2f ss2 moisture lvl %d\r\n",frame_ptr->ss2_temp,frame_ptr->ss2_moisture);
    Log.rawf("ss3 temp : %5.2f ss3 moisture lvl %d\r\n",frame_ptr->ss3_temp,frame_ptr->ss3_moisture);
    Log.rawf("ss4 temp : %5.2f ss4 moisture lvl %d\r\n",frame_ptr->ss4_temp,frame_ptr->ss4_moisture);
    Log.rawf("CO2 Level %d  bat voltage %3.1f\r\n\n",frame_ptr->CO2_level,frame_ptr->bat_voltage);

}

void scan_bat_voltage(void)
{

  unsigned int  vbat_raw;
  float vbat;
  
  analogReference(0x05);
  vbat_raw = analogRead(PIN_PD3);
  vbat = (float)(vbat_raw)/1024.0 * 4.02463 * 3.3;
  frame_ptr->bat_voltage = vbat;          

}


void i2c_channel_select(uint8_t chan_id)
{

  Wire1.begin();
  Wire1.beginTransmission(I2C_MUX);
  Wire1.write(1 << chan_id);
  Wire1.endTransmission(true);  
  // Wire1.end();

}

void scan_ms8607(void)
{

   sensors_event_t temp, pressure, humidity;
   pth_ms8607.begin(&Wire1);
   pth_ms8607.getEvent(&pressure, &temp, &humidity);

   frame_ptr->pth_temp= temp.temperature; 
   frame_ptr->pth_pressure = pressure.pressure;
   frame_ptr->pth_humidity = humidity.relative_humidity;    

} 

void sensor_array_init(void)
{

    
  // ms8607 PTH sensor
  // select ms8607 I2C channel
  
  i2c_channel_select(SS1_CH_ID);
  if(init_ss1())
    Log.raw("soil sensor 1 detected\r\n");
  else Log.raw("soil sensor 1 init fail\r\n");
  
  i2c_channel_select(SS2_CH_ID);
  if(init_ss2())
    Log.raw("soil sensor 2 detected\r\n");
  else Log.raw("soil sensor 2 init fail\r\n");


  i2c_channel_select(SS3_CH_ID);
  if(init_ss3())
    Log.raw("soil sensor 3 detected\r\n");
  else Log.raw("soil sensor 3 init fail\r\n");

  i2c_channel_select(SS4_CH_ID);
  if(init_ss4())
    Log.raw("soil sensor 4 detected\r\n");
  else Log.raw("soil sensor 4 init fail\r\n");

 
  // MS607
  i2c_channel_select(CO2_CH_ID);
  if(pth_ms8607.begin(&Wire1) == 0)
    Log.raw("PTH sensor init fail\r\n");
  else Log.raw("PTH sensor detected\r\n");

  i2c_channel_select(CO2_CH_ID);

  //  CO2 Sensor
   if(init_T6793())
    Log.raw("T6793 CO2 sensor detected\r\n");
  else Log.raw("no CO2 sensor\r\n");
  
}


uint8_t init_ss1(void)
{
    uint8_t flag = false;
        
    if(!ss1.begin(0x37))
        return(false);
    else return(true);

}

uint8_t init_ss2(void)
{
    uint8_t flag = false;
        
    if(!ss2.begin(0x39))
        return(false);
    else return(true);

}

uint8_t init_ss3(void)
{
    uint8_t flag = false;
    
    
    if(!ss3.begin(0x38))    
        return(false);
    else return(true);
}



uint8_t init_ss4(void)
{
    uint8_t flag = false;
    
    // Adafruit_seesaw ss(&Wire1);
    // i2c_channel_select(SS3_CH_ID); 
    if(!ss4.begin(0x36))
    {
        
       return(false);

    }
      
    else return(true);

}

uint8_t init_T6793(void)
{

  uint8_t i2c_data[4],flag = false;
  
  
    // start I2C
    Wire1.begin();
    Wire1.beginTransmission(T6793_ADDRESS);
    Wire1.write(0x04); 
    Wire1.write(0x13); 
    Wire1.write(0x8A); 
    Wire1.write(0x00); 
    Wire1.endTransmission(false);

    delay(1);
    Wire1.requestFrom(T6793_ADDRESS, 4);    // request 4 bytes from slave device
    if(Wire1.available())
    {
      i2c_data[0] = Wire1.read();
      i2c_data[1] = Wire1.read();
      i2c_data[2] = Wire1.read();
      i2c_data[3] = Wire1.read();
    }  
    // Wire1.end();

    if((i2c_data[2] & 0x04) == 0x04)     // bit 2 is set if in I2C mode
      flag = true;
    else flag = false;
    return(flag);

}


void scan_T6793(void)
{
  
   uint8_t i2c_data[4];

  
  i2c_channel_select(PTH_CH_ID);

  Wire1.begin();
  Wire1.beginTransmission(T6793_ADDRESS);
  Wire1.write(0x04); 
  Wire1.write(0x13); 
  Wire1.write(0x8B); 
  Wire1.write(0x00); 
  Wire1.write(0x01);
  Wire1.endTransmission(false);

  delay(1);
  Wire1.requestFrom(T6793_ADDRESS, 4);    // request 4 bytes from slave device
  if(Wire1.available())
  {
    i2c_data[0] = Wire1.read();
    i2c_data[1] = Wire1.read();
    i2c_data[2] = Wire1.read();
    i2c_data[3] = Wire1.read();
  }
  
  Wire1.end();
  frame_ptr->CO2_level= ((i2c_data[2] & 0x3F ) << 8) | i2c_data[3];
}


void scan_ss1(void)
{

    float soil_temp;
    uint16_t soil_moisture;

    i2c_channel_select(SS1_CH_ID);
    soil_temp = ss1.getTemp();
    soil_moisture = ss1.touchRead(0);
    frame_ptr->ss1_temp = soil_temp;
    frame_ptr->ss1_moisture = scale_moisture(soil_moisture);

}

void scan_ss2(void)
{

    float soil_temp;
    uint16_t soil_moisture;

    i2c_channel_select(SS2_CH_ID);
    soil_temp = ss2.getTemp();
    soil_moisture = ss2.touchRead(0);
    frame_ptr->ss2_temp = soil_temp;
    frame_ptr->ss2_moisture = scale_moisture(soil_moisture);

}

void scan_ss3(void)
{

    float soil_temp;
    uint16_t soil_moisture;

    i2c_channel_select(SS3_CH_ID);
    soil_temp = ss3.getTemp();
    soil_moisture = ss3.touchRead(0);
    frame_ptr->ss3_temp = soil_temp;
    frame_ptr->ss3_moisture = scale_moisture(soil_moisture);

}

void scan_ss4(void)
{

    float soil_temp;
    uint16_t soil_moisture;

    i2c_channel_select(SS4_CH_ID);
    soil_temp = ss4.getTemp();
    soil_moisture = ss4.touchRead(0);
    frame_ptr->ss4_temp = soil_temp;
    frame_ptr->ss4_moisture = scale_moisture(soil_moisture);



}


unsigned int scale_moisture(unsigned int raw_value)
{

    float new_moisture;

    new_moisture = (float)(raw_value) * .1367 - 40.0;
    if(new_moisture < 1.0)
      new_moisture = 0.0;

    return((unsigned int)(new_moisture));
}     









