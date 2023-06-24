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
  #define Ligth_Offset  (float) 300.00

  #define Water_Offset  (float) 70.00
  #define Power_Offset  (float) 11.00

  #define Pump_Offset  (float) 10.00
  #define Temp_Up_Offset  (float) 32.00
  #define Temp_Dow_Offset  (float) 10.00


enum{
   PumpPWM_CH =  1,
   AirPumpPWM_CH,
   LEDPWM_CH,
};

enum{
   NoWarn = 0,
   WarnWaterPumpUnusual,
   WarnPowerDown,
   WarnWaterLevelDown,
   WarnWaterTempUnusual,
};


  #define PumpSpeedIO   (uint8_t) 10

  #define WaterLevelADC (uint8_t) 0
  #define Power12VADC   (uint8_t) 3
  #define PowerVolCal   (float) 7.8
  #define WaterLevelHigh     (float) 2200
  #define WaterLevelLow      (float) 400

  #define DS18B20IO   (uint8_t) 1
  #define IIC_SDA     (uint8_t) 4
  #define IIC_SCL     (uint8_t) 5

  #define ERROR "error" 

  typedef struct
  {
    int16_t Water_Pump_Power;
    int16_t Air_Pump_Power;
    int16_t LED_Power;
    int16_t Feed_Switch;
    int16_t Auto_StarAndStop;
    int16_t Auto_LED;
    int16_t Order_Sum;
  }ControlType;

  typedef struct
  {
    float Temp;
    float PowerValue;
    float WaterLevel;
    float Ligth;
    float PumpSpeed;
    int FeedCount;
    int WarnFlag;
  }SensorDataType;

  extern SensorDataType SensorData;
  extern ControlType Control;
  extern struct tm timeinfo;
  extern String FeedTimePoint;

  extern uint8_t ClientState;

  void PostSensorData(void);
#endif