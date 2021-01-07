/* ESP & Blynk */
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
char auth[] = "yourblynkauthcodehere";

/* WiFi credentials */
char ssid[] = "yourwifissid";
char pass[] = "yourwifipass";

/* TIMER */
#include <SimpleTimer.h>
SimpleTimer timer;

/* DS18B20 Temperature Sensor */
#include <OneWire.h>
#include<DallasTemperature.h>
#define ONE_WIRE_BUS 2 // DS18B20 on arduino pin2 corresponds to D4 on physical board
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
float tempfridge;

//Libraries
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include "DHT.h"

//Define Screen dimensions
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//Define Sensor Pins and Type
#define DHTPIN 14     // what pin we're connected to
//Choose Sensor Type
#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino

//Variables
int chk;
float hum;  //Stores humidity value
float temp; //Stores temperature value
const int HigherTemp = 22; //The higher room temperature we want to trigger the red light

//Define LED pins
const int LED1_Green = 12;
const int LED2_Red = 13;

void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  DS18B20.begin();
  timer.setInterval(1000L, getSendData);
  pinMode(LED1_Green, OUTPUT);
  pinMode(LED2_Red, OUTPUT);
  dht.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  delay(2000);
  display.clearDisplay();
  display.setTextColor(WHITE);
}

void loop() {
  timer.run(); // Initiates SimpleTimer
  Blynk.run();

  //Read data and store it to variables hum and temp
  hum = dht.readHumidity();
  temp = dht.readTemperature();
  //Print temp and humidity values to serial monitor
  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.print(" %, Temp: ");
  Serial.print(temp);
  Serial.println(" Celsius");
  Serial.print(" , Fridge Temp: ");
  Serial.print(tempfridge);
  Serial.println(" Celsius");
  delay(5000); //Delay 5 sec.

  // clear display
  display.clearDisplay();

  // display temperature
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Temperature: ");
  display.setTextSize(2);
  display.setCursor(0, 10);
  display.print(temp);
  display.print(" ");
  display.setTextSize(1);
  display.cp437(true);
  display.write(167);
  display.setTextSize(2);
  display.print("C");

  // display humidity
  display.setTextSize(1);
  display.setCursor(0, 35);
  display.print("Humidity: ");
  display.setTextSize(2);
  display.setCursor(0, 45);
  display.print(hum);
  display.print(" %");

  display.display();

  delay(5000); //Delay 5 sec.
  // clear display
  display.clearDisplay();
  // display Fridge temperature
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Fridge Temperature: ");
  display.setTextSize(2);
  display.setCursor(0, 10);
  display.print(tempfridge);
  display.print(" ");
  display.setTextSize(1);
  display.cp437(true);
  display.write(167);
  display.setTextSize(2);
  display.print("C");

  display.display();

  //Check if Temperature is higher than HigherTemp
  if (temp > HigherTemp) //Check if Temperature is higher than HigherTemp
  {
    digitalWrite(LED1_Green, LOW);
    digitalWrite(LED2_Red, HIGH);
  }
  else
  {
    digitalWrite(LED1_Green, HIGH);
    digitalWrite(LED2_Red, LOW);
  }

}

void getSendData()
{
  DS18B20.requestTemperatures();
  tempfridge = DS18B20.getTempCByIndex(0);
  Blynk.virtualWrite(1, temp); //virtual pin V1
  Blynk.virtualWrite(2, hum); //virtual pin V2
  Blynk.virtualWrite(3, tempfridge); //virtual pin V3
}
