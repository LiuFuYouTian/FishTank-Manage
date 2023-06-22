#include"relate.h"
#define GetServerDataDelay 40 

const char* ssid     = "TP_LINK_DEV";
const char* password = "t12345678g";

struct 
{
  const char* ip      = "183.230.40.33"; //欲访问的地址
  const uint8_t port  = 80;         //服务器端口号
  const String url    = "http://api.heclouds.com/devices/599859039/datapoints?type=3";        //网址 设备ID
  const String ur2    = "http://api.heclouds.com/devices/599859039/datapoints?datastream_id=";//网址 设备ID
  const String apikey = "B88LcmiEOhetXDsS2OcrI6lYouc=";                                       //api-key
}Server;

uint8_t ClientState = true;//服务器连接状态

WiFiClient client; //声明一个客户端对象，用于与服务器进行连接
struct tm timeinfo;

void WIFInit(void)
{ 
  WiFi.begin(ssid, password); //

  while (WiFi.status() != WL_CONNECTED) {
      vTaskDelay(500);
      Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

float GetServerDataValue(String datatype,String data)
{
  String DatavalueStrReverse = "";
  String DatavalueStr = "";
  float datavalue = -1;
  int length = 0;
  if(data != ERROR)
  {
    int place = data.indexOf(datatype) - 10;//数值为type向后偏移10个字符

    if(place == -1) return 0;

    do   
    {
      length ++;
      DatavalueStrReverse += data[place--];
    }while(data[place] != ':' || data[place] == '-' && place >= 0);

    //Serial.println(DatavalueStrReverse);

    do   
    {
      length --;
      DatavalueStr += DatavalueStrReverse[length];
    }while(length);

    //Serial.println(DatavalueStr);

    datavalue = DatavalueStr.toFloat();
  } 

  //Serial.print("\r\n");
  //Serial.print(datavaluestr);
  //Serial.print("\r\n");
  //Serial.print(datatype + " = ");//串口打印HTTP请求 
  //Serial.println(datavalue);

  return datavalue;    
}

String GetServerDataString(String datatype,String data)
{
  String DatavalueStrReverse = "";
  String DatavalueStr = "";
  float datavalue = -1;
  int length = 0;
  if(data != ERROR)
  {
    int place = data.indexOf(datatype) - 10;//为type向后偏移1个字符

    if(place == -1) return DatavalueStr;

    do   
    {
      length ++;
      DatavalueStrReverse += data[place--];
      //Serial.println("data=" + data[place]);
    }while(data[place] != ':' && place >= 0);

    //Serial.println(DatavalueStrReverse);
    DatavalueStr += '0';
    do   
    {
        length --;
        DatavalueStr += DatavalueStrReverse[length];

        if(length%4 == 0 && length)
        {
          DatavalueStr += ',';
        }
        else if(length%2 == 0 && length)
        {
          DatavalueStr += ':';
        }
    }while(length);

    //Serial.println("DatavalueStrReverse="+DatavalueStrReverse[place]);
    Serial.println(datatype +  " = " + DatavalueStr);
  }

  //Serial.print("\r\n");
  //Serial.print(datavaluestr);
  //Serial.print("\r\n");
  //Serial.print(datatype + " = ");//串口打印HTTP请求 
  //Serial.println(datavalue);

  return DatavalueStr;    
}

uint8_t SerialLock = false;

String GetServerData(String datatype)
{
  while (SerialLock == true) {
      vTaskDelay(10);
      Serial.println("GetServerData SerialLock!!!");
  }

  SerialLock = true;

  Serial.println("At GetServerData SerialLock = true");

  String RETURN = ERROR;  
  String post;

  if (client.connect(Server.ip, Server.port))//判断是否连接服务器
  {//连接成功
    ClientState = true;
    post = String("GET ") + Server.ur2 + datatype + " HTTP/1.1\r\n" + "api-key: " + Server.apikey+ "\r\n" + "Host: api.heclouds.com\r\n\r\n";

    Serial.print(post);//串口打印HTTP请求
    client.print(post);//向服务器发送HTTP请求
    
    uint16_t delaycount = 0;
    //Serial.print("vTaskDelaystar\r\n");
    while(!client.available())
    {//判断WIFI是否有数据    
      if(delaycount++ >= GetServerDataDelay)
      {
        break;
      }
      vTaskDelay(100); 
    }
    //Serial.print("vTaskDelayend\r\n");
    while(client.available())
    {//判断WIFI是否有数据
      RETURN = client.readStringUntil('\r');//读取WIFI数据赋给line
      vTaskDelay(200); 
    }
    Serial.println(RETURN);                 //打印order_buff     
  }
  else 
  {
    ClientState = false;
    Serial.printf("Server no connect!\r\n");
  }

  SerialLock = false;
  Serial.println("At GetServerData SerialLock = false");
  vTaskDelay(20);
  return RETURN;
}

String PosDataLinkVal(String datatype,float datavalul,uint8_t EndFlag)
{
  String data = "\"" + datatype + "\":" + String(datavalul);

  if(EndFlag == false)
  {
    data += ",";
  }
  return data;
}


String PosDataLinkStr(String datatype,String datastr,uint8_t EndFlag)
{
  String data = "\"" + datatype + "\":" + "\"" + datastr + "\"";

  if(EndFlag == false)
  {
    data += ",";
  }
  return data;
}

String PosServerData(String datatype)
{
  while (SerialLock == true) {
      vTaskDelay(10);
      Serial.println("PosServerData SerialLock!!!");
  }

  SerialLock = true;
  Serial.println("At PosServerData SerialLock = true");

  String RETURN = ERROR;  
      String data = "{" + datatype + "}\r\n\r\n";
      //String data = "{\"Water_Pump_Speed\":" + String(datavalul)+"}\r\n\r\n";
      uint16_t  Length = data.length();

      String post = String("POST ") + 
                    Server.url + 
                    " HTTP/1.1\r\n" +
                    "api-key: "+ Server.apikey + 
                    "\r\n" + 
                    "Host:api.heclouds.com\r\n" + 
                    "Connection:close\r\n" + 
                    "Content-Length:" + 
                    String(Length) + 
                    "\r\n\r\n" + data;
      if (client.connect(Server.ip, Server.port))//判断是否连接服务器
      {//连接成功
          ClientState = true;
          Serial.println(Length);
          Serial.print(post);//串口打印HTTP请求
          client.print(post);//向服务器发送HTTP请求

          //Serial.print("vTaskDelaystar\r\n");
          uint16_t delaycount = 0;

          while(!client.available())
          {//判断WIFI是否有数据    
            if(delaycount ++ >= GetServerDataDelay)
            {
              break;
            }
            vTaskDelay(100); 
          }

          uint8_t overtime = 0;
          while(client.available())
          {//判断WIFI是否有数据
            RETURN = client.readStringUntil('\r');//读取WIFI数据赋给line 
            vTaskDelay(200);
            if(overtime++ >= 100) break;
          }
          Serial.print(RETURN);//打印line  
      }
      else
      {
        ClientState = false;
        Serial.printf("Server no connect!\r\n");
      }
  SerialLock = false;
  Serial.println("At PosServerData SerialLock = false");
  vTaskDelay(20);
  return RETURN;
}