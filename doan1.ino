#include <SimpleDHT.h>
#include "MQ135.h"

#define pin_analog 33 // độ ẩm đất
#define pin_relay 13 // máy bơm
#define pin_relay1 15 // quạt

//#define  cambien  35 // khai báo chân digital  cho cảm biến ánh sáng

//2 chan set chieu quay, hoat dong dung hay quay cua dong co ( trang 342 sach arduino), chan co xung PWM(chan dau ~)
#define IN1 4
#define IN2 5

// cảm biến mưa
#define rainSensor  12
int TT_maiche = 2;
int TT_tudong ;
int TT_quat;
int TT_maybom ;
int TT_maiche_closed = 0;
int TT_maiche_opened = 1;
int incomingByte = 0;   // dùng để lưu giá trị được gửi
// DHT11


int pinDHT11 = 14;
SimpleDHT11 dht11;
byte temperature = 0;
byte humidity = 0;
int time_out = 0; 
 
// MQ-135
#define PIN_MQ135 A0   //Khai báo pin nối với chân AO
MQ135 mq135_sensor = MQ135(PIN_MQ135);   //Khai báo đối tượng thư viện
float ppm;
int do_am_dat;

//app blynk
// Fill-in information from your Blynk Template here
#define BLYNK_TEMPLATE_ID "TMPLq6sn1FOO"
#define BLYNK_DEVICE_NAME "Nhakinhthongminh"

#define BLYNK_FIRMWARE_VERSION        "0.1.0"

#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG

#define APP_DEBUG

// Uncomment your board, or configure a custom board in Settings.h
//#define USE_SPARKFUN_BLYNK_BOARD
#define USE_NODE_MCU_BOARD
//#define USE_WITTY_CLOUD_BOARD
//#define USE_WEMOS_D1_MINI

#include "BlynkEdgent.h"
//#define BLYNK_PRINT Serial
//#include <ESP8266WiFi.h>
//#include <WiFiClient.h>
//#include <BlynkSimpleEsp8266.h>
//
//char auth[] = "AjlCx55SsyHRLzQsKQCcX27AO0e77h5F";
char ssid[] = "Hai";
char pass[] = "10101994";
WidgetLED led1(V1);

BlynkTimer timer;
// V1 LED Widget is blinking
// day du lieu len app blynk
void blinkLedWidget() // kiem tra xem co ket noi voi blynk chua
{
  if (led1.getValue()) {
    led1.off();
    Serial.println("LED on V1: off");
  } else {
    led1.on();
    Serial.println("LED on V1: on");
  }
}

BLYNK_CONNECTED()
{
  Blynk.syncVirtual(V0);
  Blynk.syncVirtual(V1);
  Blynk.syncVirtual(V2);
  Blynk.syncVirtual(V3);
  Blynk.syncVirtual(V4);
  Blynk.syncVirtual(V5);
  Blynk.syncVirtual(V7);
}

// máy bơm
BLYNK_WRITE(V0)
{
  int pinvalue0 = param.asInt();
  // String i = param.asStr();
  // double d = param.asDouble();
  if (pinvalue0 == 1)
  {
    TT_maybom = 1;
  }
  else
  {
    TT_maybom = 0;
  }
  // gui ve app blynk
  Blynk.virtualWrite(V0, pinvalue0);
}

// quạt
BLYNK_WRITE(V2)
{
  int pinvalue1 = param.asInt();
  // String i = param.asStr();
  // double d = param.asDouble();
  if ( pinvalue1 == 1)
  {
    TT_quat = 1;
  }
  else
  {
    TT_quat = 0;
  }
  // gui ve app blynk
  Blynk.virtualWrite(V2, pinvalue1);
}

// mái che
BLYNK_WRITE(V3)
{
  int pinvalue2 = param.asInt();
  Serial.println(pinvalue2);
  // String i = param.asStr();
  // double d = param.asDouble();
  if ( pinvalue2 == 0)
  {
    TT_maiche = 0;
  }
  else
  {
    TT_maiche = 1;
  }
  // gui ve app blynk
  Blynk.virtualWrite(V3, pinvalue2);
}

// chế độ tự động
BLYNK_WRITE(V7)
{
  int pinvalue7 = param.asInt();
  // String i = param.asStr();
  // double d = param.asDouble();
  if ( pinvalue7 == 1)
  {
    TT_tudong = 1;
  }
  else
  {
    TT_tudong = 0;
  }
  // gui ve app blynk
  Blynk.virtualWrite(V7, pinvalue7);
}

// DHT- nhiet do do am
void sendSensor()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V5, humidity);
  Blynk.virtualWrite(V4, temperature);
}

void moMaiChe()
{
  if(TT_maiche_closed == 1 && TT_maiche_opened == 0)
  {
    Serial.println("TT_maiche: on");
    digitalWrite(IN2,LOW); // Set Dir high  - Set chiều quay mức cao
    for(int x = 0; x < 1024; x++) // lặp 4096 lần
  
    {
      digitalWrite(IN1,HIGH); // Output high - xung mức cao
      delay(1);         // Trễ 1ms
      digitalWrite(IN1,LOW); // Output low - xung mức thấp
      delay(3);         // Trễ 3ms
    }
    
     TT_maiche_closed = 0;
     TT_maiche_opened = 1;
  }
  else
  {
   TT_maiche_closed = 0;
   TT_maiche_opened = 1;
  }
}

void dongMaiChe()
{
  if(TT_maiche_closed == 0 && TT_maiche_opened == 1)
  {
    Serial.println("TT_maiche: off");
     digitalWrite(IN2,HIGH); // Set Dir low - Set chiều quay mức thấp
    for(int x = 0; x < 1024; x++) // lặp 4096 lần
  
    {
      digitalWrite(IN1,HIGH); // Output high - xung mức cao
      delay(1);         // Trễ 1ms
      digitalWrite(IN1,LOW); // Output low - xung mức thấp
      delay(3);         // Trễ 3ms
    }
    
     TT_maiche_closed = 1;
     TT_maiche_opened = 0;
  }
  else
  {
     TT_maiche_closed = 1;
     TT_maiche_opened = 0;
  }
}


void sendSensor1()
{
  float ppm = mq135_sensor.getPPM() / 100;
  Blynk.virtualWrite(V6, ppm);
}

void setup()
{
 TT_maiche_closed = 1;
 TT_maiche_opened = 0;
  // MQ135
  pinMode (PIN_MQ135, INPUT);
  // máy bơm
  pinMode (pin_analog, INPUT);
  pinMode (pin_relay, OUTPUT);

  // quạt
  pinMode (pin_relay1, OUTPUT);

  // cảm biến mưa
  pinMode(rainSensor, INPUT);
  // mái che
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  
  Serial.begin(9600);//Mở cổng Serial ở mức 960
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("dang ket noi....");
  }

  Serial.println("WiFi da ket noi");
  BlynkEdgent.begin();
  timer.setInterval(1000L, blinkLedWidget);
  timer.setInterval(1000L, sendSensor);
  timer.setInterval(1000L, sendSensor1);
}

void loop()
{

  
        // nếu còn có thể đọc được 
//        if (Serial.available() > 0) {
//                // đọc chữ liệu
//                incomingByte = Serial.read();
//
//                // trả về những gì nhận được
//                Serial.print("Toi nhan duoc: ");
//                if (incomingByte == -1) {
//                  Serial.println("Toi khong nhan duoc gi ca");
//                }
//                else
//                {
////                  Serial.println(char(incomingByte));
//                  if (char(incomingByte) == '0') {
//                  TT_tudong = 0;
//                  Serial.println("TT_tudong OFF");
//                } else if (char(incomingByte) == '1') {
//                  TT_tudong = 1;
//                  Serial.println("TT_tudong ON");
//                } else if (char(incomingByte) == '2') {
//                  TT_maybom = 0;
//                  Serial.println("TT_maybom OFF");
//                } else if (char(incomingByte) == '3') {
//                  TT_maybom = 1;
//                  Serial.println("TT_maybom ON");
//                } else if (char(incomingByte) == '4') {
//                  TT_quat = 0;
//                  Serial.println("TT_quat OFF");
//                } else if (char(incomingByte) == '5') {
//                  TT_quat = 1;
//                  Serial.println("TT_quat ON");
//                } else if (char(incomingByte) == '6') {
//                  TT_maiche = 0;
//                  Serial.println("TT_maiche DONG");
//                } else if (char(incomingByte) == '7') {
//                  TT_maiche = 1;
//                  Serial.println("TT_maiche MO");
//                }
//                }
//        }
  // DHT11
  readDHT11();
  //MQ-135
  readMQ135();
  // quạt
  if (TT_tudong == 1)
  {
    float ppm = mq135_sensor.getPPM() / 100;
    if (int(ppm) >= 120)
    {
      digitalWrite(pin_relay1, HIGH ); // kích hoạt relay
      delay(1000);
    }
    else
    {
      digitalWrite(pin_relay1, LOW);
    }
  }
  else
  {
    if (TT_quat == 1)
    {
      Serial.println("TT_quat 1 : on");
      digitalWrite(pin_relay1, HIGH ); // kích hoạt relay
    }
    else
    {
      Serial.println("TT_quat 1: off");
      digitalWrite(pin_relay1, LOW);
    }
  }

  // cảm biên mưa
  int value = analogRead(rainSensor);//Đọc tín hiệu cảm biến mưa
  Serial.println(value);
  if ( TT_tudong == 1)
  {
    if (value == 1023)
    {
      Serial.println("Dang khong mua"); // mở
      dongMaiChe();
    }
    else if(value == 0)
    {
      Serial.println("Dang mua");
      moMaiChe();
    }
  }
  else
  {
    if (TT_maiche == 1)
    {
      moMaiChe();
    }
    else if (TT_maiche == 0)
    {
       dongMaiChe();
    }
  }
  // máy bơm
  if (TT_tudong == 1)
  {
      if (temperature > 32)
      {
        Serial.println("TT_quat 1 : on");
        digitalWrite(pin_relay1, HIGH ); // kích hoạt relay
        delay(1000);
      }
      else
      {
        Serial.println("TT_quat 1 : off");
        digitalWrite(pin_relay1, LOW ); // kích hoạt relay
      }
  }
  else
  {
    if (TT_maybom == 1)
    {
      Serial.println("May bom 1: on");
      digitalWrite(pin_relay, HIGH); // kích hoạt rờ le
      time_out++;
      if(time_out >10000)
      {
        time_out = 0;
        Serial.println("May bom 1: off");
        digitalWrite(pin_relay, LOW );
      }
    }
    else
    {
      Serial.println("May bom 1: off");
      digitalWrite(pin_relay, LOW );
    }
  } 
  TT_maiche = 2;
  // BLYNK
  BlynkEdgent.run();
  timer.run();
}

void readDHT11()
{
  // start working...
  Serial.println("=================================");
  Serial.println("Sample DHT11...");
//  lcd.setCursor(0, 1);
//  lcd.print("T=");
//  lcd.setCursor(2, 1);
//  lcd.print(temperature);
//  lcd.setCursor(4, 1);
//  lcd.print("o");
//  lcd.setCursor(5, 1);
//  lcd.print("C");
//  lcd.setCursor(6, 1);
//  lcd.print("    ");
//  lcd.setCursor(10, 1);
//  lcd.print("H=");
//  lcd.setCursor(12, 1);
//  lcd.print(humidity);
//  lcd.setCursor(14, 1);
//  lcd.print("%");
//  lcd.setCursor(15, 1);
//  lcd.print(" ");

  // read without samples.
  if (dht11.read(pinDHT11, &temperature, &humidity, NULL)) {
    Serial.print("Read DHT11 failed.");
  }
}
void readMQ135() {
  float ppm = mq135_sensor.getPPM() / 100; //Đọc giá trị ppm
  Serial.print("PPM: ");
  Serial.print(ppm);
  Serial.println("ppm");
//  lcd.setCursor(0, 0);
//  lcd.print("p=");
//  lcd.setCursor(2, 0);
//  lcd.print(ppm);
//  lcd.setCursor(6, 0);
//  lcd.print("    ");
}
