#ifndef RELATE_H
#define RELATE_H

  #include <WiFi.h>
  #include <DS18B20.h>
  #include <BH1750.h>
  #include "Wire.h"

  #define BuzzIO        (uint8_t) 12
  #define FeedControlIO (uint8_t) 18
  #define FeedResetIO   (uint8_t) 19
  #define StatLEDIO      (uint8_t) 13

  #define PumpPWMIO     (uint8_t) 6
  #define LEDPWMIO      (uint8_t) 7
  #define AirPumpPWMIO  (uint8_t) 2

  #define PWM_Offset  (uint8_t) 50
  #define Ligth_Offset  (uint8_t) 50
  #define Water_Offset  (uint8_t) 50


enum{
   PumpPWM_CH =  1,
   AirPumpPWM_CH,
   LEDPWM_CH,
};

  #define PumpSpeedIO   (uint8_t) 10

  #define WaterLevelADC (uint8_t) 0
  #define Power12VADC   (uint8_t) 3
  #define PowerVolCal   (float) 6.722
  #define WaterLevelCal     (float) 48
  #define WaterLevelOffset  (float) 0.9

  #define DS18B20IO   (uint8_t) 1
  #define IIC_SDA     (uint8_t) 4
  #define IIC_SCL     (uint8_t) 5

  #define ERROR "error" 

  typedef struct
  {
    uint8_t Water_Pump_Power;
    uint8_t Air_Pump_Power;
    uint8_t LED_Power;
    uint8_t Feed_Switch;
    uint8_t Auto_StarAndStop;
    uint16_t Order_Sum;
  }ControlType;

  typedef struct
  {
    float Temp;
    float PowerValue;
    float WaterLevel;
    float Ligth;
    float PumpSpeed;
    int FeedCount;
  }SensorDataType;

  extern SensorDataType SensorData;
  extern ControlType Control;
  extern struct tm timeinfo;
  extern String FeedTimePoint;

  void PostSensorData(void);
#endif