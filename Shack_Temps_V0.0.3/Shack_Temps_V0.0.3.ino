/*
  LCD Temp Sensor logger

  Logs temps to SD Card + LCD

  The circuit:
  - LCD and RTC connected Analog pin4 + pin5 I2c bus
  - Temp Sensors OneWire to Digital pin2 (4.7K Resistor between VCC and Data Line)
  - SD Card -MISO pin12
            -MOSI pin11
            -SCK pin13
            -CS pin10 Changable, See //SetupSD below.

  created 2018
  by Stephen McBain <http://mcbainsite.co.uk>
*/

//Load needed libries / Basic Setup
#include <SPI.h>
#include <SD.h>
  // -- Set SD Card Variable Name
File dataFile;

#include <Wire.h> //I2C Communication
#include <LiquidCrystal_I2C.h>
#include <DS3232RTC.h>
#include <DS3231.h>
// -- Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);

#include <OneWire.h>
#include <DallasTemperature.h>
// -- Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 2
// -- Setup a oneWire instance to communicate with any OneWire devices
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// -- Assign sensor addresses to Variable Names
// Assign the addresses of your 1-Wire temp sensors.
// See the tutorial on how to obtain these addresses:
// http://www.hacktronics.com/Tutorials/arduino-1-wire-address-finder.html
DeviceAddress VHF = { 0x28, 0xFF, 0xCC, 0xD3, 0x31, 0x18, 0x02, 0x91 }; 
DeviceAddress IN = { 0x28, 0xFF, 0x36, 0x54, 0xC4, 0x17, 0x05, 0xF6 };
DeviceAddress OUT = { 0x28, 0xFF, 0xE1, 0x44, 0xC4, 0x17, 0x04, 0x9F };
DeviceAddress HF = { 0x28, 0xFF, 0x3D, 0xEA, 0x31, 0x18, 0x02, 0x21 };
DeviceAddress ATV = { 0x28, 0xFF, 0xCF, 0x53, 0x32, 0x18, 0x01, 0x00 };
//DeviceAddress Sensor6 = {};

// -- LCD Setup Assign pins to names
#define I2C_ADDR 0x27  //Address location of I2C LCD
#define Rs_pin 0
#define Rw_pin 1
#define En_pin 2
#define BACKLIGHT_PIN 3
#define D4_pin 4
#define D5_pin 5
#define D6_pin 6
#define D7_pin 7

//Setup LCD Pins etc...
LiquidCrystal_I2C lcd(I2C_ADDR, En_pin, Rw_pin, Rs_pin, D4_pin, D5_pin, D6_pin, D7_pin);

//Set temp allert marker temprature
const int allert = 40;

int SDpresent = 0; // -- Initialze variable for SD logging

void setup()
{
  Serial.begin(9600); //Start Serial
  lcd.begin (20, 4); //LCD Size (horrizontal, Vertical) 

  // LCD Backlight ON
  lcd.setBacklightPin(BACKLIGHT_PIN, POSITIVE);
  lcd.setBacklight(HIGH);

// -- Check Real Time Clock Is Set
  setSyncProvider(RTC.get);  // Library function to get the time from the RTC module. DS3232RTC.h Library
  if (timeStatus() != timeSet)
    Serial.println("System Time Cannot be Set. Check Connections.");
  else
    Serial.println("System Time is Set.");

  // Initialize the rtc clock. DS3231.h Library
  rtc.begin();

  // Start up the sensors library
  Serial.print("Locating devices...");
  sensors.begin();
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");

  // set the resolution to 10 bit (Can be 9 to 12 bits .. lower is faster)
  sensors.setResolution(10);
  //sensors.setResolution(IN, 10);
  //sensors.setResolution(OUT, 10);
  //sensors.setResolution(HF, 10);
  //sensors.setResolution(ATV, 10);
  //sensors.setResolution(S6, 10);

  // -- LCD Welcome Message
  lcd.setCursor(4, 0); // go home on LCD
  lcd.print("Shack Temps");
  lcd.setCursor(7, 1);
  lcd.print("V0.0.3");
  lcd.setCursor(2, 3);
  lcd.print("mcbainsite.co.uk");
  delay(3000);

  //SetupSD Card
  Serial.print("\nInitializing SD card...");
  lcd.clear();
  lcd.home();
  lcd.print("Initializing Log...");
  lcd.setCursor(0, 2);
  lcd.print("SD Card...");

  // -- Check for SD Card and Activate Logging
  // -- Change 10 below to Set CS Pin
  if (!SD.begin(10)) {
    Serial.println("SD inisalisation Failed");
    lcd.print("ERROR!");
    delay(2000);
    headderNOSD();
    return;
  } else {
    SDpresent = 1;
    dataFile = SD.open("startup.txt", FILE_WRITE);
    dataFile.print("Startup logged.....");
    dataFile.print(rtc.getDateStr());
    dataFile.print(", ");
    dataFile.println(rtc.getTimeStr());
    dataFile.close();
    SD.remove("temps.txt");
    Serial.println("Writing headder");
    dataFile = SD.open("temps.txt", FILE_WRITE);
    dataFile.println("Temprature Log");
    dataFile.println("Written By Stephen McBain");
    dataFile.println();
    dataFile.println(rtc.getDateStr());
    dataFile.println(" ");
    dataFile.println("Data");
    dataFile.println();
    dataFile.close();
    Serial.println("SD Ready");
    lcd.print("Ready!");
    delay(2000);
    headder();
  }
}

void headder() //Setup LCD with logging active message
{
  lcd.clear();
  lcd.home();
  lcd.print("Temp Log Active");
  lcd.setCursor(0, 1);
  lcd.print("VHF:");
  lcd.setCursor(10, 1);
  lcd.print("IN :");
  lcd.setCursor(0, 2);
  lcd.print("OUT:");
  lcd.setCursor(10, 2);
  lcd.print("HF :");
  lcd.setCursor(0, 3);
  lcd.print("ATV:");
  lcd.setCursor(10, 3);
  lcd.print("S6 :");
}

void headderNOSD() //Setup LCD with logging inactive message
{
  lcd.clear();
  lcd.home();
  lcd.print("Temp Log Active");
  lcd.setCursor(0, 1);
  lcd.print("VHF:");
  lcd.setCursor(10, 1);
  lcd.print("IN :");
  lcd.setCursor(0, 2);
  lcd.print("OUT:");
  lcd.setCursor(10, 2);
  lcd.print("HF :");
  lcd.setCursor(0, 3);
  lcd.print("ATV:");
  lcd.setCursor(10, 3);
  lcd.print("S6 :");
}

void loop()
{
  sensors.requestTemperatures(); // Send the command to get temperature readings
float S1 = sensors.getTempCByIndex(0);
float S2 = sensors.getTempCByIndex(1);
float S3 = sensors.getTempCByIndex(2);
float S4 = sensors.getTempCByIndex(3);
float S5 = sensors.getTempCByIndex(4);
float S6 = sensors.getTempCByIndex(5);
  
  //Sensor 1
  lcd.setCursor(4, 1);
  if (S1 == -127.00) {
    lcd.print("N/A   ");
  } else {
    if (S1 >= allert) {
      lcd.print(S1); // Why "byIndex"?
      // You can have more than one DS18B20 on the same bus.
      // 0 refers to the first IC on the wire
      lcd.print("*");
    } else {
      lcd.print(S1); // Why "byIndex"?
      // You can have more than one DS18B20 on the same bus.
      // 0 refers to the first IC on the wire
      lcd.print(" ");
    }
  }
  //Sensor 2
  lcd.setCursor(14, 1);
  if (S2 == -127.00) {
    lcd.print("N/A   ");
  } else {
    if (S2 >= allert) {
      lcd.print(S2); // Why "byIndex"?
      // You can have more than one DS18B20 on the same bus.
      // 0 refers to the first IC on the wire
      lcd.print("*");
    } else {
      lcd.print(S2); // Why "byIndex"?
      // You can have more than one DS18B20 on the same bus.
      // 0 refers to the first IC on the wire
      lcd.print(" ");
    }
  }
  //Sensor 3
  lcd.setCursor(4, 2);
  if (S3 == -127.00) {
    lcd.print("N/A   ");
  } else {
    if (S3 >= allert) {
      lcd.print(S3); // Why "byIndex"?
      // You can have more than one DS18B20 on the same bus.
      // 0 refers to the first IC on the wire
      lcd.print("*");
    } else {
      lcd.print(S3); // Why "byIndex"?
      // You can have more than one DS18B20 on the same bus.
      // 0 refers to the first IC on the wire
      lcd.print(" ");
    }
  }
  //Sensor 4
  lcd.setCursor(14, 2);
  if (S4 == -127.00) {
    lcd.print("N/A   ");
  } else {
    if (S4 >= allert) {
      lcd.print(S4); // Why "byIndex"?
      // You can have more than one DS18B20 on the same bus.
      // 0 refers to the first IC on the wire
      lcd.print("*");
    } else {
      lcd.print(S4); // Why "byIndex"?
      // You can have more than one DS18B20 on the same bus.
      // 0 refers to the first IC on the wire
      lcd.print(" ");
    }
  }
    //Sensor 5
  lcd.setCursor(4, 3);
  if (S5 == -127.00) {
    lcd.print("N/A   ");
  } else {
    if (S5 >= allert) {
      lcd.print(S5); // Why "byIndex"?
      // You can have more than one DS18B20 on the same bus.
      // 0 refers to the first IC on the wire
      lcd.print("*");
    } else {
      lcd.print(S5); // Why "byIndex"?
      // You can have more than one DS18B20 on the same bus.
      // 0 refers to the first IC on the wire
      lcd.print(" ");
    }
  }
  //Sensor 6
  lcd.setCursor(14, 3);
  if (S6 == -127.00) {
    lcd.print("N/A   ");
  } else {
    if (S6 >= allert) {
      lcd.print(S6); // Why "byIndex"?
      // You can have more than one DS18B20 on the same bus.
      // 0 refers to the first IC on the wire
      lcd.print("*");
    } else {
      lcd.print(S6); // Why "byIndex"?
      // You can have more than one DS18B20 on the same bus.
      // 0 refers to the first IC on the wire
      lcd.print(" ");
    }
  }

  if (SDpresent == 1) {
    if (((minute() == 00) && (second() == 00)) || ((minute() == 10) && (second() == 00)) || ((minute() == 20) && (second() == 00)) || ((minute() == 30) && (second() == 0)) || ((minute() == 40) && (second() == 0)) || ((minute() == 50) && (second() == 0))) {
      //sensors.requestTemperatures(); // Send the command to get temperature readings
      /********************************************************************/
      dataFile = SD.open("temps.txt", FILE_WRITE);
      dataFile.print(rtc.getTimeStr());
      dataFile.print(" ");
      dataFile.print("VHF:");
      dataFile.print(" ");
      dataFile.print(S1);
      dataFile.print(" ");
      dataFile.print("IN:");
      dataFile.print(" ");
      dataFile.print(S2);
      dataFile.print(" ");
      dataFile.print("OUT:");
      dataFile.print(" ");
      dataFile.print(S3);
      dataFile.print(" ");
      dataFile.print("HF:");
      dataFile.print(" ");
      dataFile.println(S4);
      dataFile.print(" ");
      dataFile.print("23cm ATV RX:");
      dataFile.print(" ");
      dataFile.println(S5);
      dataFile.print(" ");
      dataFile.print("S6:");
      dataFile.print(" ");
      dataFile.println(S6);
      dataFile.close();
    }
  } else {
  return;
    }
  delay(1000);
  }


