#include"relate.h"

ControlType Control;
#define FeedCheckDelay (uint32_t) 1000*31
#define FeedMotorDelay (uint32_t) 1000*1

String FeedTimePoint = "06:00,10:00,14:00,18:00,22:15,02.00";
String PumpTimeMaintainPoint = "07:00,19:00";


void PostSensorData(void)
{
    Serial.printf("\r\nPostSensorData Start = %d\r\n",millis());

    String  Str  = PosDataLink("Water_Temperature" ,SensorData.Temp         ,false);
            Str += PosDataLink("Lingth_Intensity"  ,SensorData.Ligth        ,false);
            Str += PosDataLink("Power_Voltage"     ,SensorData.PowerValue   ,false);
            Str += PosDataLink("Water_level"       ,SensorData.WaterLevel   ,false);
            Str += PosDataLink("Water_Pump_Speed"  ,SensorData.PumpSpeed    ,false);
            Str += PosDataLink("millis"            ,millis()                ,false);
            Str += PosDataLink("Feed_Count"         ,SensorData.FeedCount    ,true);

    PosServerData(Str);
    Serial.printf("\r\nPostSensorData End = %d\r\n",millis());
}

void PostClossFeed(void)
{
    Serial.printf("\r\nPostSensorData Start = %d\r\n",millis());

    String  Str = PosDataLink("Feed_Switch",0,true);
    PosServerData(Str);
    Serial.printf("\r\nPostSensorData End = %d\r\n",millis());
}

void GetServerOrder(void)
{
    Serial.printf("\r\nGetServerOrder Start = %d\r\n",millis());

    String ServerData = GetServerData("Water_Pump_Power,Air_Pump_Power,LED_Power,Feed_Switch,Auto_StarAndStop");

    if(String(ERROR) != ServerData)
    {
      Control.Water_Pump_Power  = GetServerDataValue("Water_Pump_Power",ServerData)/100*255;
      Control.Air_Pump_Power    = GetServerDataValue("Air_Pump_Power",ServerData)/100*255;
      Control.LED_Power         = GetServerDataValue("LED_Power",ServerData)/100*255;
      Control.Auto_StarAndStop  = GetServerDataValue("Auto_StarAndStop",ServerData);
      Control.Feed_Switch  = GetServerDataValue("Feed_Switch",ServerData);
    }

    Serial.printf("Control.Water_Pump_Power = %d\r\n" ,Control.Water_Pump_Power);
    Serial.printf("Control.Air_Pump_Power   = %d\r\n" ,Control.Air_Pump_Power);
    Serial.printf("Control.LED_Power        = %d\r\n" ,Control.LED_Power);
    Serial.printf("Control.Feed_Switch      = %d\r\n" ,Control.Feed_Switch);
    Serial.printf("Control.Auto_StarAndStop = %d\r\n" ,Control.Auto_StarAndStop);

    Serial.printf("\r\nGetServerOrder End = %d\r\n",millis());
}

void NetConnect(void *pt)
{
  while(1)
  {
    if(WiFi.status() == WL_CONNECTED)
    {
      PostSensorData();
      GetServerOrder();
    }
    else
    {
      WIFInit();
    }
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
  
}

void FeedDeviceConnect(void)
{
        Serial.printf("\r\nFeedDeviceConnect Start = %d\r\n",millis());
        Control.Feed_Switch = 0;

        vTaskDelete(_GetSensorData);
        vTaskDelete(_NetConnect);

        vTaskDelay(3000);

        PostClossFeed();

        digitalWrite(FeedControlIO, HIGH);
        vTaskDelay(FeedMotorDelay);

        uint16_t delay_1ms = 0;

        while(digitalRead(FeedResetIO) == HIGH)
        {
          vTaskDelay(1);
          if(delay_1ms++ >= 1000*6) break;
        }

        digitalWrite(FeedControlIO, LOW);
        SensorData.FeedCount++;
        Control.Feed_Switch = 0;

        xTaskCreate(GetSensorData,"GetSensorData",2048,NULL,3,&_GetSensorData);
        xTaskCreate(NetConnect,"NetConnect",2048,NULL,4,&_NetConnect);

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

  Serial.println(str);
  Serial.println(&timeinfo, "%F %T %A"); // 格式化输出:2021-10-24 23:00:44 Sunday

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

      if(SensorData.WaterLevel <= Water_Offset)
      {
        Control.Water_Pump_Power = 0;
        ledcWrite(PumpPWM_CH,0);
      }
*/
      if(TimePointCheck(PumpTimeMaintainPoint,true) == -1 && Control.Auto_StarAndStop == 0)
      {
        ledcWrite(PumpPWM_CH,Control.Water_Pump_Power);
      }


      ledcWrite(LEDPWM_CH,Control.LED_Power);
      ledcWrite(AirPumpPWM_CH,Control.Air_Pump_Power);

      if(Control.Feed_Switch != 0)
      {
        FeedDeviceConnect();
      }

      int Sum = Control.Water_Pump_Power + Control.LED_Power + Control.Air_Pump_Power + Control.Feed_Switch + Control.Auto_StarAndStop;
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
    if(TimePointCheck(FeedTimePoint,false) != -1)
    {
        FeedDeviceConnect();
    }

    if(TimePointCheck(PumpTimeMaintainPoint,true) != -1 || Control.Auto_StarAndStop == 1)
    {
        Auto_StarAndStopFlag = !Auto_StarAndStopFlag;
        if(Auto_StarAndStopFlag == 0)
          ledcWrite(PumpPWM_CH,0);
        else
          ledcWrite(PumpPWM_CH,255);
    }

    vTaskDelay(FeedCheckDelay);
  }
}

