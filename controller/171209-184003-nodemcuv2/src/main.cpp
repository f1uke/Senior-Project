#include <Arduino.h>
#include <DHT.h>
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>
#include "myprotocol.h"

#define BLYNK_PRINT terminal
#define DHTPIN D2     // what digital pin the DHT22 is conected to
#define DHTTYPE DHT22 // there are multiple kinds of DHT sensors
#define PIR_MOTION_SENSOR D8
#define D7s D7

DHT dht(DHTPIN, DHTTYPE);
Servo myservo;
SimpleTimer timer;
// WidgetTerminal terminal(V10);
myprotocol myiot;

const char *ssid = "F1uke";
const char *passw = "lingfluke";


// char ssid[] = "F1uke";
char pass[] = "lingfluke";
char auth[] = "bd4cab06a86b4bfeaeac926c5ab7ce88";
char server[] = "blynk-cloud.com";
unsigned int port = 8442;

//delayTime
int timemerSingnalToRelay = 0;
double timemerMoveServo = 258600;
int timemerShowHumidityAndTemp = 0;
int timmerPIR = 0;
int timmerLEMP = 0;
int timmerFAN = 0;
float Offset = 0.0;

void sendUptime()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  t = ((int)(t * 10) / 10.0);
  h = ((int)(h * 10) / 10.0);

  if (isnan(h) || isnan(t) || h >= 200 || t >= 200)
  {
    // loop();
    return;
  }
  else
  {
    Blynk.virtualWrite(8, t); // virtual pin
    Blynk.virtualWrite(9, h); // virtual pin
  }
}

void CheckConnection()
{ // check every 11s if connected to Blynk server
  if (!Blynk.connected())
  {
    Serial.println("Not connected to Blynk server");
    Blynk.connect(); // try to connect to server with default timeout
  }
  else
  {
    Serial.println("Connected to Blynk server");
  }
}

void setup()
{
  Serial.begin(9600);

  // Blynk
  Serial.println("Blynk Started");
  Serial.println("-------------------------------------");
  // Blynk.begin(auth, ssid, pass);

  Blynk.connectWiFi(ssid, pass);
  Blynk.config(auth, server, port);
  Blynk.connect();
  Blynk.notify("เครื่องฟักไข่เริ่มทำงาน!");

  dht.begin();
  timer.setInterval(10000, sendUptime); //1000=1s
  timer.setInterval(60000, CheckConnection);

  Serial.println("Device Started");
  Serial.println("-------------------------------------");
  Serial.println("Running DHT!");
  Serial.println("-------------------------------------");

  Serial.println("TEST SERVO");
  Serial.println("-------------------------------------");
  myservo.attach(D7);
  // myservo.write(80);
  // delay(5000);
  // myservo.write(50);
  // delay(2500);
  // myservo.write(110);
  // delay(2500);
  myservo.write(80);
  delay(2500);
  myservo.detach();

  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D5, OUTPUT);
  digitalWrite(D1, LOW);
  digitalWrite(D5, HIGH);

  pinMode(PIR_MOTION_SENSOR, INPUT);
  Serial.println("here");

}

boolean isPeopleDetected()
{
  int sensorValue = digitalRead(PIR_MOTION_SENSOR);
  Serial.println("Start PIR!");
  Serial.println("-------------------------------------");
  if (sensorValue == HIGH) //if the sensor value is HIGH?
  {
    return true; //yes,return true
  }
  else
  {
    return false; //no,return false
  }
}

void loop()
{

  if (Blynk.connected())
  {
    Blynk.run();
  }

  timer.run();

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h2 = dht.readHumidity();
  float t2 = dht.readTemperature();
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h2) || isnan(t2) || isnan(f))
  {
    Serial.println("Failed to read from DHT sensor!");
    // ESP.restart();
    return;
  }

// write data to server
  myiot.WriteDHT(t2,h2);

  //Servo Control

  if (timemerMoveServo == 259200)
  {
    myservo.attach(D7);
    Serial.println("servo: 0");
    myservo.write(50);
    delay(2500);
    myservo.detach();
  }
  if (timemerMoveServo == 259800)
  {
    myservo.attach(D7);
    myservo.write(110);
    Serial.println("servo: 180");
    delay(2500);
    myservo.detach();
    // setTo 0
    timemerMoveServo = 258600;
  }

  // notify alert detect
  // if (h2 > 90)
  // {
  //   Blynk.notify("มีความชื้นสูงเกิดไป!");
  // }
  if (t2 < 30)
  {
    Blynk.notify("อุณหภูมิต่ำเปิดไป!");
  }

  // RESTART if have some problem.
  // if (h2 > 100 && t2 < 30 && timemerMoveServo == 300800)
  // {
  //   ESP.restart();
  // }

  //Fan and Lamp Control
  if (timemerSingnalToRelay >= 1)
  {
    //Fan
    // if (h2 >= 60)
    // {
    //   digitalWrite(D5, HIGH);
    //   Blynk.virtualWrite(1, 1023);
    // }
    // if (h2 < 55)
    // {
    //   digitalWrite(D5, LOW);
    //   Blynk.virtualWrite(1, 0);
    // }

    //Lamp recomment37.5

    if (timemerMoveServo >= 258600)
    {
      if (t2 < (37.5 + Offset))
      {
        digitalWrite(D1, HIGH);
        Blynk.virtualWrite(2, 1023);
      }
      else if (t2 >= (37.5 + Offset))
      {
        digitalWrite(D1, LOW);
        Blynk.virtualWrite(2, 0);
      }

      timemerSingnalToRelay = 0;
    }
    else
    {
      if (t2 < (37.5 + Offset))
      {
        digitalWrite(D1, HIGH);
        Blynk.virtualWrite(2, 1023);
      }
      else if (t2 >= (37.5 + Offset))
      {
        digitalWrite(D1, LOW);
        Blynk.virtualWrite(2, 0);
      }

      timemerSingnalToRelay = 0;
    }
  }

  //Show on terminalMonitor
  if (timemerShowHumidityAndTemp = 1)
  {
    Serial.print("Humidity: ");
    Serial.print(h2);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t2);
    Serial.print(" *C ");
    Serial.print(f);
    Serial.print(" *F\t\n");

    timemerShowHumidityAndTemp = 0;
  }

  // delay after LEMP = HIGH
  if (digitalRead(D1) == LOW)
  {
    timmerLEMP += 1;
  }
  else
  {
    timmerLEMP = 0;
  }
  if (timmerLEMP > 100)
  {
    timmerLEMP = 61;
  }
  // delay after FAN = LOW
  if (digitalRead(D5) == LOW)
  {
    timmerFAN += 1;
  }
  else
  {
    timmerFAN = 0;
  }
  if (timmerFAN > 100)
  {
    timmerFAN = 61;
  }

// check pir
  // if (timmerPIR >= 30 && timemerMoveServo != 0 && timmerLEMP > 5 && timemerMoveServo != 259200 && timemerMoveServo != 259800)
  if (timmerPIR >= 10 && timemerMoveServo != 259200 && timemerMoveServo != 259800)
  {
    if (digitalRead(D5) == HIGH)
    {
      digitalWrite(D5, LOW);
      delay(2000);
      if (isPeopleDetected()) //if it detects the moving people?
      {
        Serial.println("People is Detected!");
        digitalWrite(BUILTIN_LED, HIGH);
        Blynk.virtualWrite(3, 1023);
        Blynk.notify("ตรวจพบการเคลื่อนไหว อาจเกินการฟักของไข่!");
        timmerPIR = 0;
      }
      else
      {
        digitalWrite(BUILTIN_LED, LOW);
        Blynk.virtualWrite(3, 0);
        timmerPIR = 0;
      }
      digitalWrite(D5, HIGH);
    }
    else if (timmerFAN >= 2)
    {
      if (isPeopleDetected()) //if it detects the moving people?
      {
        Serial.println("People is Detected!");
        digitalWrite(BUILTIN_LED, HIGH);
        Blynk.virtualWrite(3, 1023);
        Blynk.notify("ตรวจพบการเคลื่อนไหว อาจเกินการฟักของไข่!");
        timmerPIR = 0;
      }
      else
      {
        digitalWrite(BUILTIN_LED, LOW);
        Blynk.virtualWrite(3, 0);
        timmerPIR = 0;
      }
    }
  }
  else if (timmerPIR >= 100)
  {
    timmerPIR = 0;
  }

  //delay
  Serial.print("timmerPIR\t\t");
  Serial.println(timmerPIR);
  Serial.print("timemerMoveServo\t");
  Serial.println(timemerMoveServo);
  Serial.print("timmerLEMP\t\t");
  Serial.println(timmerLEMP);
  Serial.print("timmerFAN\t\t");
  Serial.println(timmerFAN);
  Serial.print("this is day ==\t");
  Serial.println((((timemerMoveServo / 60) / 60) / 24));

  timemerSingnalToRelay += 1;
  timemerShowHumidityAndTemp += 1;
  timemerMoveServo += 1;
  timmerPIR += 1;

  digitalWrite(BUILTIN_LED, HIGH);
  delay(1000);
  digitalWrite(BUILTIN_LED, LOW);
}
