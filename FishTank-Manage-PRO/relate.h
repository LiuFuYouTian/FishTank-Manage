#ifndef RELATE_H
#define RELATE_H

  #include <WiFi.h>
  #include <DS18B20.h>
  #include <BH1750.h>
  #include "Wire.h"

  #define BuzzIO        (uint8_t) 9
  #define FeedControlIO (uint8_t) 39
  #define FeedResetIO   (uint8_t) 37
  #define StatLEDIO      (uint8_t) 36

  #define HumidityControlIO (uint8_t) 17
  #define HumidityResetIO   (uint8_t) 21

  #define PumpPWMIO     (uint8_t) 5
  #define LEDPWMIO      (uint8_t) 40
  #define AirPumpPWMIO  (uint8_t) 35

  #define PWM_Offset  (uint8_t) 50
  #define Ligth_Offset  (float) 300.00

  #define Water_Offset  (float) 60.00
  #define Power_Offset  (float) 9.50

  #define Pump_Offset  (float) 10.00
  #define Temp_Up_Offset  (float) 35.00
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

enum{
   HumidityOFF = 0,
   HumidityOn1,
   HumidityOn2,
};


  #define PumpSpeedIO   (uint8_t) 3

  #define WaterLevelADC (uint8_t) 1
  #define PowerIn12VADC   (uint8_t) 4
  #define PowerUp12VADC   (uint8_t) 2
  #define PowerDown5VADC  (uint8_t) 6

  #define PowerVolCal   (float) 5.7
  #define WaterLevelHigh     (float) 2300
  #define WaterLevelLow      (float) 200

  #define DS18B20IO   (uint8_t) 7
  #define IIC_SDA     (uint8_t) 12
  #define IIC_SCL     (uint8_t) 11

  #define ERROR "error" 

  #define PWM_Gain 0.045

  typedef struct
  {
    int16_t Water_Pump_Power;
    int16_t Air_Pump_Power;
    int16_t LED_Power;
    int16_t Feed_Switch;
    int16_t Auto_StarAndStop;
    int16_t Auto_LED;
    int16_t Order_Sum;
    int16_t LightDownCount;
    int16_t HumidityState;
    int16_t Humidity_Switch;
    int16_t Auto_Humidity;
    int16_t TempUpCount;
    float Water_Pump_Power_Gain;
  }ControlType;

  typedef struct
  {
    float Temp;
    float PowerInValue;
    float PowerUpValue;
    float PowerDownValue;
    float WaterLevel;
    float Ligth;
    float PumpSpeed;
    float PumpSpeedError;
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