#include"relate.h"

#define BH1750address  (uint8_t)0x23

#define LPFParameter 0.3

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


    SensorData.Temp       = 25;
    SensorData.PowerValue = 12;
    SensorData.WaterLevel = 90;
    SensorData.PumpSpeed  = 300;
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

    SensorData.PowerValue = SensorData.PowerValue*PowerVolCal/1000;                      
    SensorData.WaterLevel = map(SensorData.WaterLevel,WaterLevelLow,WaterLevelHigh,0,100);

    SensorData.PumpSpeed  = pulseIn(PumpSpeedIO,HIGH);
    if(SensorData.PumpSpeed != 0)
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

    Serial.printf("\r\nGetSensorData End = %d\r\n",millis());
    vTaskDelay(1000);
  }
}
