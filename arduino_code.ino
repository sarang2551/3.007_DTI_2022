/**************************************************************************
 This is an example for our Monochrome OLEDs based on SSD1306 drivers

 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 This example is for a 128x64 pixel display using SPI to communicate
 4 or 5 pins are required to interface.

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source
 hardware by purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries,
 with contributions from the open source community.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be
 included in any redistribution.
 **************************************************************************/
#include "Firebase_Arduino_WiFiNINA.h"

#define DATABASE_URL "fkyoufirebase-default-rtdb.asia-southeast1.firebasedatabase.app" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app
#define DATABASE_SECRET "7Kv94sp8bLwg1Og4oive4YxMp6iOz51mDTTH1WDQ"

#define WIFI_SSID "Redmi"
#define WIFI_PASSWORD "arduinoTest123"
int motorPin = 6;

int sensorPin = A7;
//Define Firebase data object
FirebaseData fbdo;
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_MOSI   11
#define OLED_CLK   13
#define OLED_DC    9
#define OLED_CS    8
#define OLED_RESET 10
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);


#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16


void setup() {
    Serial.begin(9600);
  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  Serial.print("Connecting to Wi-Fi");
  int status = WL_IDLE_STATUS;
  while (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }
  while (status != WL_CONNECTED)
  {
    status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.println("wifi connecting");
    delay(200);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  //Provide the authentication data
  Firebase.begin(DATABASE_URL, DATABASE_SECRET, WIFI_SSID, WIFI_PASSWORD);
  Firebase.reconnectWiFi(true);
  pinMode(motorPin,OUTPUT);
  //pinMode(motorPin,OUTPUT);
  fbdo.clear();

  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  //display.drawPixel(10, 10, SSD1306_WHITE);
  display.display();
  delay(2000);
  
  display.invertDisplay(true);
  delay(1000);
  display.invertDisplay(false);
  delay(1000);
}

void loop() {
  int sensorReading = analogRead(A0);
  String testing = String(sensorReading);
  Serial.println("Battery outside: " + testing);
  if(sensorReading > 400){
    showCharging(sensorReading);
    delay(3000);
  }else{
  drawProjectlines();
  String path = "/buzzer_1/buzz";
  String path_people = "/buzzer_1/people";
  if (Firebase.getInt(fbdo, path)) // also can use Firebase.get(fbdo, path)
  {
    if (fbdo.dataType() == "int"){
      int val_no = fbdo.intData();
      Serial.println("Received buzz data");
      // display the data on the screen
      if(Firebase.getInt(fbdo,path_people)){
        if(fbdo.dataType()=="int"){
          int people = fbdo.intData();
          String peeps = String(people);
          Serial.println("Received people: " + peeps);
          if (people > 0){
            drawProjectData(val_no,people);
            display.display();
            delay(5000);
            } else {
              display.clearDisplay();
              orderReady(val_no);
              digitalWrite(motorPin,HIGH);
              delay(2000);
              digitalWrite(motorPin,LOW);
              delay(3000);
              }
          }
        }else{
          Serial.println("Error retrieving people data: " + fbdo.errorReason());
          }
    }
  }
  else
  {
    Serial.println("error database connection: , " + fbdo.errorReason());
  }
  }
}


void drawProjectlines(){
  display.clearDisplay();
  display.drawLine(0,display.height()/2,display.width(),display.height()/2,SSD1306_WHITE);
  //display.display();
  //display.drawLine(display.width()/2,display.height()/2,display.width()/2,display.height(),SSD1306_WHITE);
  display.display();
}
void drawProjectData(int order_number, int ppl){
  // order no in the bottom left
  // number of ppl in the bottom right
  String order_no = String(order_number);
  String people = String(ppl);
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1.5);
  display.setCursor(10,display.height()*0.20);
  display.println("Order number: ");
  display.setCursor(100, display.height()*0.20);
  display.println(order_no);     // Show initial text
  display.setCursor(10,display.height()*0.60);
  display.println("People infront: ");
  display.setCursor(100,display.height()*0.60);
  display.println(ppl);
}
void orderReady(int order_no){
  String order = String(order_no);
  display.setTextSize(1.5);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(5,display.height()*0.25);
  display.println("ORDER READY!");
  display.setCursor(5,display.height()*0.75);
  display.println("Order no: "+order);
  display.drawLine(0,display.height()/2,display.width(),display.height()/2,SSD1306_BLACK);
  display.display();
  }

int checkPercentage(int sensorReading){
  //float voltage = (((sensorReading*3.3)/1024) * 2 + 7.4);
  float voltage = ((sensorReading-735)/(1023-735))*100;
//  bat_percentage = mapFloat(voltage,2.8,5,0,100);
  //float voltage = (sensorReading/1023)*100;
  int bat_percentage = (int) voltage;
//  String testing = String(sensorReading);
//  Serial.println("Battery: " + testing);
  if (bat_percentage >= 100){
    bat_percentage = 100;
  }
  if (bat_percentage <= 0){
    bat_percentage = 1;
  }
  return bat_percentage;
}
void showCharging(int sensorReading){
  int battery_perc = checkPercentage(sensorReading);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10,display.height()*0.5);
  String bat = String(battery_perc);
  display.println("CHARGING! ");
  display.display();
}
