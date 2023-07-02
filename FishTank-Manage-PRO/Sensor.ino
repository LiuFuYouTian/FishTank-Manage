#include"relate.h"

#define BH1750address  (uint8_t)0x23

#define LPFParameter 0.1

DS18B20 ds(DS18B20IO);
BH1750 lightMeter;

SensorDataType SensorData;
SensorDataType LastSensorData;


void SensorInit(void)
{
  SensorData.FeedCount = 0;

  analogReadResolution(12);     //ADC设置为12bit
  pinMode(WaterLevelADC,INPUT); //水位电压检测
  pinMode(Power12VADC,INPUT);   //12V电压检测  

  pinMode(PumpSpeedIO,INPUT);  //水泵转速检测
  pinMode(FeedResetIO,INPUT);  //水泵转速检测


  Wire.begin(IIC_SDA, IIC_SCL);//光线传感器IIC初始化
  lightMeter.begin();

  Serial.print("Devices: ");
  Serial.println(ds.getNumberOfDevices());
  Serial.println();


  LastSensorData.Temp       = 25;
  LastSensorData.PowerValue = 12;
  LastSensorData.WaterLevel = 90;
  LastSensorData.PumpSpeed  = 300;
}

float LowPsaa(float newdata,float lastdata)
{
  return newdata*LPFParameter + (1 - LPFParameter)*lastdata;
}


void GetSensorData(void *pt)
{
  while(1)
  {
    Serial.printf("\r\nGetSensorData Start = %d\r\n",millis());
    //digitalWrite(BuzzIO,!digitalRead(BuzzIO));
    
    SensorData.Temp       = ds.getTempC();
    SensorData.Ligth      = lightMeter.readLightLevel();

    SensorData.PowerValue = analogReadMilliVolts(Power12VADC);     //读取12V电源电压
    SensorData.WaterLevel = analogReadMilliVolts(WaterLevelADC);        //读取水位监测ADC电压


    //Serial.printf("SensorData.PowerValueADC  = %fmV\r\n",SensorData.PowerValue);
    //Serial.printf("SensorData.WaterLevelADC  = %fmV\r\n",SensorData.WaterLevel);

    SensorData.PowerValue = (float)(SensorData.PowerValue/1000)*PowerVolCal;                 
    SensorData.WaterLevel = map(SensorData.WaterLevel,WaterLevelLow,WaterLevelHigh,0,100);

    SensorData.PumpSpeed  = pulseIn(PumpSpeedIO,HIGH);
    if(SensorData.PumpSpeed >= 2500)
    {
      SensorData.PumpSpeed = (float)1000000/SensorData.PumpSpeed;
    }

    /*
    Serial.printf("SensorData.Temp        = %f\r\n",SensorData.Temp);
    Serial.printf("SensorData.Ligth       = %f\r\n",SensorData.Ligth);
    Serial.printf("SensorData.PowerValue  = %f\r\n",SensorData.PowerValue);
    Serial.printf("SensorData.WaterLevel  = %f\r\n",SensorData.WaterLevel);
    Serial.printf("SensorData.PumpSpeed   = %f\r\n",SensorData.PumpSpeed);
    */

    SensorData.Temp = LowPsaa(SensorData.Temp,LastSensorData.Temp);
    LastSensorData.Temp = SensorData.Temp;
  
    SensorData.Ligth = LowPsaa(SensorData.Ligth,LastSensorData.Ligth);
    LastSensorData.Ligth = SensorData.Ligth;

    SensorData.PowerValue = LowPsaa(SensorData.PowerValue,LastSensorData.PowerValue);
    LastSensorData.PowerValue = SensorData.PowerValue;

    SensorData.WaterLevel = LowPsaa(SensorData.WaterLevel,LastSensorData.WaterLevel);
    LastSensorData.WaterLevel = SensorData.WaterLevel;

    SensorData.PumpSpeed = LowPsaa(SensorData.PumpSpeed,LastSensorData.PumpSpeed);
    LastSensorData.PumpSpeed = SensorData.PumpSpeed;

    /*
    Serial.printf("SensorData.TempLP        = %f\r\n",SensorData.Temp);
    Serial.printf("SensorData.LigthLP       = %f\r\n",SensorData.Ligth);
    Serial.printf("SensorData.PowerValueLP  = %f\r\n",SensorData.PowerValue);
    Serial.printf("SensorData.WaterLevelLP  = %f\r\n",SensorData.WaterLevel);
    Serial.printf("SensorData.PumpSpeed   = %f\r\n",SensorData.PumpSpeed);
    */
    SensorData.WarnFlag = NoWarn;

    if(SensorData.PumpSpeed <= Pump_Offset && Control.Water_Pump_Power != 0 && TimePointCheck(PumpTimeMaintainStr,true) == -1 && Control.Auto_StarAndStop == 0)//泵转速异常检测
    {
      SensorData.WarnFlag = WarnWaterPumpUnusual;
    }

    if(SensorData.PowerValue <= Power_Offset)//电源电压异常检测
    {
      SensorData.WarnFlag = WarnPowerDown;
    }

    if(SensorData.WaterLevel <= Water_Offset)//水位异常检测
    {
      SensorData.WarnFlag = WarnWaterLevelDown;
    }

    if(SensorData.Temp >= Temp_Up_Offset || SensorData.Temp <= Temp_Dow_Offset)//水温异常检测
    {
      SensorData.WarnFlag = WarnWaterTempUnusual;
    }

    if(SensorData.WarnFlag != 0)//蜂鸣器报警
    {
        for(uint8_t i = 0;i <= 10;i++)
        {
          digitalWrite(BuzzIO, !digitalRead(BuzzIO));
          vTaskDelay(20);
        }
        digitalWrite(BuzzIO, LOW);
    }

    Serial.printf("\r\nGetSensorData End = %d\r\n",millis());
    vTaskDelay(1000);
  }
}
