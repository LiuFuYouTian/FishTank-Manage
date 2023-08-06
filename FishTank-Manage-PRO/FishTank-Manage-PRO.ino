#include"relate.h"
#define NTP1  "ntp1.aliyun.com"
#define NTP2  "ntp2.aliyun.com"
#define NTP3  "ntp3.aliyun.com"

TaskHandle_t _GetSensorData;
TaskHandle_t _NetConnect;
TaskHandle_t _DeviceConnect;

void setup() {
  // put your setup code here, to run once:
  DeviceInit();           //控制设备初始化
  Serial.begin(115200);   //调试串口

  WIFInit();
  SensorInit();           //外部传感器初始化

  xTaskCreate(GetSensorData,"GetSensorData",2048,NULL,3,&_GetSensorData);
  xTaskCreate(NetConnect,"NetConnect",2048,NULL,5,&_NetConnect);

  xTaskCreate(DeviceConnect,"DeviceConnect",2048,NULL,3,&_DeviceConnect);
  xTaskCreate(FeedConnect,"FeedConnect",2048,NULL,20,NULL);
}

void loop() {
  // put your main code here, to run repeatedly:
  //Serial.printf("%s\r\n",GetServerDataValue("Water_Temperature"));

  vTaskDelay(1000);
  if(!getLocalTime(&timeinfo))
  {
    configTime(8 * 3600, 0, NTP1, NTP2,NTP3);
    Serial.println("configTime");
  }
  //Serial.println(&timeinfo, "%F %T %A"); // 格式化输出:2021-10-24 23:00:44 Sunday
}

