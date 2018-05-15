/*
  LCD Temp Sensor logger

  Logs temps to SD Card + LCD

  The circuit:
  - LCD and RTC connected pin4 + pin5 I2c bus
  - Temps OneWire to pin2
  
  created 2018
  by Stephen McBain <http://mcbainsite.duckdns.org>
  test
*/
#include <SPI.h>
#include <SD.h>
File dataFile;

#include <Wire.h>
#include <LCD.h>
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

LiquidCrystal_I2C lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);
int SDstate=0;
void setup()
{
lcd.begin (20,4); //  our LCD is a 20x4, change for your LCD if needed

// LCD Backlight ON
lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
lcd.setBacklight(HIGH);

    setSyncProvider(RTC.get);  // Library function to get the time from the RTC module.
    if(timeStatus() != timeSet) 
        Serial.println("System Time Cannot be Set. Check Connections.");
    else
        Serial.println("System Time is Set.");
// Initialize the rtc object
rtc.begin();
// Start up the sensors library 
 sensors.begin();

  //Welcome Message
  lcd.home(); // go home on LCD
  lcd.print("TheMcBain Temp V0.1");
  delay(5000);
  
//SetupSD Card
  Serial.print("\nInitializing SD card...");
  lcd.setCursor(0,2);
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
    Serial.println("SD Ready");
    dataFile = SD.open("temps.txt", FILE_WRITE);
    dataFile.println("Temprature Log");
    dataFile.println("Written By Stephen McBain");
    dataFile.println();
    dataFile.println(rtc.getDateStr());
    dataFile.println(" ");
    dataFile.println("Data");
    dataFile.println();
    dataFile.close();
    lcd.print("Ready!");
    delay(2000);
    headder();
  } 

}

void headder()
{
  lcd.clear();
  lcd.home();
  lcd.print("Temp Logging Active");
}

void headderNOSD()
{
  lcd.clear();
  lcd.home();
  lcd.print("Temp No Logging");
}

void loop()
{    
    lcd.setCursor(0,2);
    lcd.print("Sensor 1 is: "); 
    lcd.print(sensors.getTempCByIndex(0)); // Why "byIndex"?  
      // You can have more than one DS18B20 on the same bus.  
      // 0 refers to the first IC on the wire
 if (minute() == 00 || minute() == 15 || minute() == 30 || minute() == 45)
 {
    sensors.requestTemperatures(); // Send the command to get temperature readings  
    /********************************************************************/

    dataFile = SD.open("temps.txt", FILE_WRITE);
    dataFile.print(rtc.getTimeStr());
    dataFile.print(" ");
    dataFile.println(sensors.getTempCByIndex(0));
    dataFile.close();   
} else {
  }
}

