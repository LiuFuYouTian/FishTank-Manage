#include"relate.h"

ControlType Control;
#define FeedCheckDelay (uint32_t) 1000*35
#define FeedMotorDelay (uint32_t) 1000*1

//String FeedTimeStr = "06:00,10:00,14:00,18:00,22:15,02.00";
String FeedTimeStr = "";
String PumpTimeMaintainStr = "07:00,19:00";

void PostSensorData(void)
{
    Serial.printf("\r\nPostSensorData Start = %d\r\n",millis());

    String  Str  = PosDataLinkVal("Water_Temperature"     ,SensorData.Temp         ,false);
            Str += PosDataLinkVal("Lingth_Intensity"      ,SensorData.Ligth        ,false);
            Str += PosDataLinkVal("Power_Voltage"         ,SensorData.PowerValue   ,false);
            Str += PosDataLinkVal("Water_level"           ,SensorData.WaterLevel   ,false);
            Str += PosDataLinkVal("Water_Pump_Speed"      ,SensorData.PumpSpeed    ,false);
            Str += PosDataLinkVal("millis"                ,millis()/1000/60        ,false);
            Str += PosDataLinkVal("Feed_Count"            ,SensorData.FeedCount    ,false);
            Str += PosDataLinkStr("FeedTimeStr"          ,FeedTimeStr              ,false);
            Str += PosDataLinkStr("PumpTimeMaintainStr"  ,PumpTimeMaintainStr      ,true);

    PosServerData(Str);
    Serial.printf("\r\nPostSensorData End = %d\r\n",millis());
}

void PostClossFeed(void)
{
    Serial.printf("\r\nPostSensorData Start = %d\r\n",millis());

    String  Str = PosDataLinkVal("Feed_Switch",0,false);
            Str += PosDataLinkVal("Feed_Count",SensorData.FeedCount    ,true);
            
    PosServerData(Str);
    Serial.printf("\r\nPostSensorData End = %d\r\n",millis());
}

void GetServerOrder(void)
{
    Serial.printf("\r\nGetServerOrder Start = %d\r\n",millis());

    String ServerData = GetServerData("Water_Pump_Power,Air_Pump_Power,LED_Power,Feed_Switch,Auto_StarAndStop,FeedTimePoint,PumpTimeMaintainPoint");
    //String ServerData = GetServerData("Water_Pump_Power,Air_Pump_Power,LED_Power,Feed_Switch,Auto_StarAndStop");

    //Serial.println("ServerData = " + ServerData);

    if(String(ERROR) != ServerData)
    {
      Control.Water_Pump_Power  = GetServerDataValue("Water_Pump_Power",ServerData)/100*255;
      Control.Air_Pump_Power    = GetServerDataValue("Air_Pump_Power",ServerData)/100*255;
      Control.LED_Power         = GetServerDataValue("LED_Power",ServerData)/100*255;
      Control.Auto_StarAndStop  = GetServerDataValue("Auto_StarAndStop",ServerData);
      Control.Feed_Switch       = GetServerDataValue("Feed_Switch",ServerData);

      FeedTimeStr           = GetServerDataString("FeedTimePoint",ServerData);
      PumpTimeMaintainStr   = GetServerDataString("PumpTimeMaintainPoint",ServerData);
    }

    Serial.printf("Control.Water_Pump_Power = %d\r\n" ,Control.Water_Pump_Power);
    Serial.printf("Control.Air_Pump_Power   = %d\r\n" ,Control.Air_Pump_Power);
    Serial.printf("Control.LED_Power        = %d\r\n" ,Control.LED_Power);
    Serial.printf("Control.Feed_Switch      = %d\r\n" ,Control.Feed_Switch);
    Serial.printf("Control.Auto_StarAndStop = %d\r\n" ,Control.Auto_StarAndStop);

    Serial.println(FeedTimeStr);
    Serial.println(PumpTimeMaintainStr);

    Serial.printf("\r\nGetServerOrder End = %d\r\n",millis());
}

void NetConnect(void *pt)
{
  while(1)
  {

    Serial.println("NetConnect");
    //Serial.println("Control.Feeding_Sign = flase");
    if(WiFi.status() == WL_CONNECTED)
    {
      PostSensorData();
      GetServerOrder();
    }
    else
    {
      WIFInit();
    }
    vTaskDelay(20);
  }
}

void DeviceInit(void)
{
  Control.Water_Pump_Power = 255;
  Control.Air_Pump_Power = 255;
  Control.LED_Power = 50;
  Control.Feed_Switch = 0;

  pinMode(PumpPWMIO,OUTPUT);//指示LED控制GPIO
  ledcSetup(PumpPWM_CH, 5000, 8); // 设置通道
  ledcAttachPin(PumpPWMIO, PumpPWM_CH);  // 将通道与对应的引脚连接,LED_Power
  ledcWrite(PumpPWM_CH,Control.Water_Pump_Power);

  pinMode(LEDPWMIO,OUTPUT);//指示LED控制GPIO
  ledcSetup(LEDPWM_CH, 200, 8); // 设置通道
  ledcAttachPin(LEDPWMIO, LEDPWM_CH);  // 将通道与对应的引脚连接,LED_Power
  ledcWrite(LEDPWM_CH,Control.LED_Power);


  pinMode(AirPumpPWMIO,OUTPUT);//指示LED控制GPIO
  ledcSetup(AirPumpPWM_CH, 200, 8); // 设置通道
  ledcAttachPin(AirPumpPWMIO, AirPumpPWM_CH);  // 将通道与对应的引脚连接,LED_Power
  ledcWrite(AirPumpPWM_CH,Control.Air_Pump_Power);


  pinMode(FeedControlIO,OUTPUT);//指示LED控制GPIO
  digitalWrite(FeedControlIO, LOW);

  pinMode(StatLEDIO,OUTPUT);//指示LED控制GPIO
  digitalWrite(StatLEDIO, LOW);

  pinMode(BuzzIO,OUTPUT);//指示LED控制GPIO
  digitalWrite(BuzzIO, LOW);
  
}

void FeedDeviceConnect(void)
{
        Serial.printf("\r\nFeedDeviceConnect Start = %d\r\n",millis());
        Control.Feed_Switch = 0;
        SensorData.FeedCount++;

        PostClossFeed();
        vTaskSuspend(_GetSensorData);
        vTaskSuspend(_NetConnect);

        digitalWrite(FeedControlIO, HIGH);
        vTaskDelay(FeedMotorDelay);

        uint16_t delay_1ms = 0;

        while(digitalRead(FeedResetIO) == HIGH)
        {
          vTaskDelay(1);
          if(delay_1ms++ >= 1000*6) break;
        }

        digitalWrite(FeedControlIO, LOW);
        Control.Feed_Switch = 0;

        vTaskResume(_GetSensorData);
        vTaskResume(_NetConnect);

        Serial.printf("\r\nFeedDeviceConnect End = %d\r\n",millis());
}

int TimePointCheck(String str ,uint8_t IsHour)
{
  String current = "";

  if(timeinfo.tm_hour < 10)
  {
    current = "0" + String(timeinfo.tm_hour) + ":";
  }
  else 
  {
    current = String(timeinfo.tm_hour) + ":";
  }


  if(IsHour == false)
  {
    current += String(timeinfo.tm_min);
  }

  //Serial.println(str);
  //Serial.println(&timeinfo, "%F %T %A"); // 格式化输出:2021-10-24 23:00:44 Sunday

  return str.indexOf(current);
}

void DeviceConnect(void *pt)
{
  while(1)
  {

      if(Control.Water_Pump_Power <= PWM_Offset) Control.Water_Pump_Power = 0;
      if(Control.LED_Power <= PWM_Offset) Control.LED_Power = 0;
      if(Control.Air_Pump_Power <= PWM_Offset) Control.Air_Pump_Power = 0;
/*    
      if(Control.LED_Power == 0 && SensorData.Ligth <= Ligth_Offset)
      {
        Control.LED_Power = 255;
      }

      if(SensorData.WaterLevel <= Water_Offset && Control.Water_Pump_Power != 0)
      {
        Control.Water_Pump_Power = 0;
        ledcWrite(PumpPWM_CH,0);
      }
*/
      if(TimePointCheck(PumpTimeMaintainStr,true) == -1 && Control.Auto_StarAndStop == 0)
      {
        ledcWrite(PumpPWM_CH,Control.Water_Pump_Power);
      }


      ledcWrite(LEDPWM_CH,Control.LED_Power);
      ledcWrite(AirPumpPWM_CH,Control.Air_Pump_Power);

      if(Control.Feed_Switch != 0)
      {
        FeedDeviceConnect();
      }

      int Sum = Control.Water_Pump_Power + Control.LED_Power + Control.Air_Pump_Power + Control.Feed_Switch + Control.Auto_StarAndStop + SensorData.FeedCount;
      if(Sum != Control.Order_Sum)
      {
        Control.Order_Sum = Sum;
        for(uint8_t i = 0;i <= 6;i++)
        {
          digitalWrite(BuzzIO, !digitalRead(BuzzIO));
          vTaskDelay(50);
        }
        digitalWrite(BuzzIO, LOW);
      }
      else
      {
        vTaskDelay(50);
      }
      digitalWrite(StatLEDIO, !digitalRead(StatLEDIO));
  }
}

uint8_t Auto_StarAndStopFlag = 0;

void FeedConnect(void *pt)
{
  while(1)
  {
    if(TimePointCheck(FeedTimeStr,false) != -1 || Control.Feed_Switch != 0)
    {
        FeedDeviceConnect();
    }

    if(TimePointCheck(PumpTimeMaintainStr,true) != -1 || Control.Auto_StarAndStop == 1)
    {
        Auto_StarAndStopFlag = !Auto_StarAndStopFlag;
        if(Auto_StarAndStopFlag == 0)
          ledcWrite(PumpPWM_CH,0);
        else
          ledcWrite(PumpPWM_CH,255);
    }

    //vTaskResume(_GetSensorData);
    //vTaskResume(_NetConnect);

    vTaskDelay(FeedCheckDelay);
  }
}

