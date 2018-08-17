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

//Load needed libries
#include <SPI.h>
#include <SD.h>
File dataFile;
#include <Wire.h> //I2C Communication
#include <LiquidCrystal_I2C.h>
#include <DS3232RTC.h>
#include <DS3231.h>
// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);
#include <OneWire.h>
#include <DallasTemperature.h>
// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 2
// Setup a oneWire instance to communicate with any OneWire devices
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
#define I2C_ADDR 0x27  // Add your address here.
#define Rs_pin 0
#define Rw_pin 1
#define En_pin 2
#define BACKLIGHT_PIN 3
#define D4_pin 4
#define D5_pin 5
#define D6_pin 6
#define D7_pin 7
//Set temp allert marker temprature
const int allert = 32;

//Setup LCD Pins etc...
LiquidCrystal_I2C lcd(I2C_ADDR, En_pin, Rw_pin, Rs_pin, D4_pin, D5_pin, D6_pin, D7_pin);

void setup()
{
  lcd.begin (20, 4); //  our LCD is a 20x4, change for your LCD if needed

  // LCD Backlight ON
  lcd.setBacklightPin(BACKLIGHT_PIN, POSITIVE);
  lcd.setBacklight(HIGH);

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

  //LCD Welcome Message
  lcd.setCursor(4, 0); // go home on LCD
  lcd.print("Shack Temps");
  lcd.setCursor(7, 1);
  lcd.print("V0.0.2");
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
  //Change 10 below to Set CS Pin
  if (!SD.begin(10)) {
    Serial.println("SD inisalisation Failed");
    lcd.print("ERROR!");
    delay(2000);
    headderNOSD();
    return;
  } else {
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
  lcd.print("S5 :");
  lcd.setCursor(10, 3);
  lcd.print("S6 :");
}

void headderNOSD() //Setup LCD with logging inactive message
{
  lcd.clear();
  lcd.home();
  lcd.print("Temp Log inActive");
  lcd.setCursor(0, 1);
  lcd.print("VHF:");
  lcd.setCursor(11, 1);
  lcd.print("IN:");
  lcd.setCursor(0, 2);
  lcd.print("OUT:");
  lcd.setCursor(10, 2);
  lcd.print("HF:");
}

void loop()
{
  sensors.requestTemperatures(); // Send the command to get temperature readings
  //Sensor 1
  lcd.setCursor(4, 1);
  if (sensors.getTempCByIndex(0) == -127.00) {
    lcd.print("N/A");
  } else {
    if (sensors.getTempCByIndex(0) >= allert) {
      lcd.print(sensors.getTempCByIndex(0)); // Why "byIndex"?
      // You can have more than one DS18B20 on the same bus.
      // 0 refers to the first IC on the wire
      lcd.print("*");
    } else {
      lcd.print(sensors.getTempCByIndex(0)); // Why "byIndex"?
      // You can have more than one DS18B20 on the same bus.
      // 0 refers to the first IC on the wire
      lcd.print(" ");
    }
  }
  //Sensor 2
  lcd.setCursor(14, 1);
  if (sensors.getTempCByIndex(1) == -127.00) {
    lcd.print("N/A");
  } else {
    if (sensors.getTempCByIndex(1) >= allert) {
      lcd.print(sensors.getTempCByIndex(1)); // Why "byIndex"?
      // You can have more than one DS18B20 on the same bus.
      // 0 refers to the first IC on the wire
      lcd.print("*");
    } else {
      lcd.print(sensors.getTempCByIndex(1)); // Why "byIndex"?
      // You can have more than one DS18B20 on the same bus.
      // 0 refers to the first IC on the wire
      lcd.print(" ");
    }
  }
  //Sensor 3
  lcd.setCursor(4, 2);
  if (sensors.getTempCByIndex(2) == -127.00) {
    lcd.print("N/A");
  } else {
    if (sensors.getTempCByIndex(2) >= allert) {
      lcd.print(sensors.getTempCByIndex(0)); // Why "byIndex"?
      // You can have more than one DS18B20 on the same bus.
      // 0 refers to the first IC on the wire
      lcd.print("*");
    } else {
      lcd.print(sensors.getTempCByIndex(2)); // Why "byIndex"?
      // You can have more than one DS18B20 on the same bus.
      // 0 refers to the first IC on the wire
      lcd.print(" ");
    }
  }
  //Sensor 4
  lcd.setCursor(14, 2);
  if (sensors.getTempCByIndex(3) == -127.00) {
    lcd.print("N/A");
  } else {
    if (sensors.getTempCByIndex(3) >= allert) {
      lcd.print(sensors.getTempCByIndex(3)); // Why "byIndex"?
      // You can have more than one DS18B20 on the same bus.
      // 0 refers to the first IC on the wire
      lcd.print("*");
    } else {
      lcd.print(sensors.getTempCByIndex(3)); // Why "byIndex"?
      // You can have more than one DS18B20 on the same bus.
      // 0 refers to the first IC on the wire
      lcd.print(" ");
    }
  }
    //Sensor 5
  lcd.setCursor(4, 3);
  if (sensors.getTempCByIndex(4) == -127.00) {
    lcd.print("N/A");
  } else {
    if (sensors.getTempCByIndex(4) >= allert) {
      lcd.print(sensors.getTempCByIndex(0)); // Why "byIndex"?
      // You can have more than one DS18B20 on the same bus.
      // 0 refers to the first IC on the wire
      lcd.print("*");
    } else {
      lcd.print(sensors.getTempCByIndex(4)); // Why "byIndex"?
      // You can have more than one DS18B20 on the same bus.
      // 0 refers to the first IC on the wire
      lcd.print(" ");
    }
  }
  //Sensor 6
  lcd.setCursor(14, 3);
  if (sensors.getTempCByIndex(5) == -127.00) {
    lcd.print("N/A");
  } else {
    if (sensors.getTempCByIndex(5) >= allert) {
      lcd.print(sensors.getTempCByIndex(3)); // Why "byIndex"?
      // You can have more than one DS18B20 on the same bus.
      // 0 refers to the first IC on the wire
      lcd.print("*");
    } else {
      lcd.print(sensors.getTempCByIndex(5)); // Why "byIndex"?
      // You can have more than one DS18B20 on the same bus.
      // 0 refers to the first IC on the wire
      lcd.print(" ");
    }
  }
  if (((minute() == 00) && (second() == 00)) || ((minute() == 10) && (second() == 00)) || ((minute() == 20) && (second() == 00)) || ((minute() == 30) && (second() == 0)) || ((minute() == 40) && (second() == 0)) || ((minute() == 50) && (second() == 0))) {
    sensors.requestTemperatures(); // Send the command to get temperature readings
    /********************************************************************/
    dataFile = SD.open("temps.txt", FILE_WRITE);
    dataFile.print(rtc.getTimeStr());
    dataFile.print(" ");
    dataFile.print("VHF:");
    dataFile.print(" ");
    dataFile.print(sensors.getTempCByIndex(0));
    dataFile.print(" ");
    dataFile.print("IN:");
    dataFile.print(" ");
    dataFile.print(sensors.getTempCByIndex(1));
    dataFile.print(" ");
    dataFile.print("OUT:");
    dataFile.print(" ");
    dataFile.print(sensors.getTempCByIndex(2));
    dataFile.print(" ");
    dataFile.print("HF:");
    dataFile.print(" ");
    dataFile.println(sensors.getTempCByIndex(3));
    dataFile.print(" ");
    dataFile.print("S5:");
    dataFile.print(" ");
    dataFile.println(sensors.getTempCByIndex(4));
    dataFile.print(" ");
    dataFile.print("S6:");
    dataFile.print(" ");
    dataFile.println(sensors.getTempCByIndex(5));
    dataFile.close();
  }
}


