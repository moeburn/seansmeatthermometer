#include <TFT_eSPI.h> 
#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <NonBlockingDallas.h>                  //Include the NonBlockingDallas library

#define ONE_WIRE_BUS 4                          //PIN of the Maxim DS18B20 temperature sensor
#define TIME_INTERVAL 750                      //Time interval among sensor readings [milliseconds]

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature dallasTemp(&oneWire);
NonBlockingDallas temperatureSensors(&dallasTemp);   

TFT_eSPI tft = TFT_eSPI();   
int i;

float tempC, tempF;

void handleTemperatureChange(int deviceIndex, int32_t temperatureRAW)
{
  tempC = temperatureSensors.rawToCelsius(temperatureRAW);
  tempF = temperatureSensors.rawToFahrenheit(temperatureRAW);
}



void setup() {
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(10, 10);
  tft.setTextColor(TFT_WHITE, TFT_BLACK, true);
  tft.setTextWrap(true); // Wrap on width
  tft.setTextFont(6);
  tft.setTextSize(1);
  tft.print("Connecting...");  //display wifi connection progress
  temperatureSensors.begin(NonBlockingDallas::resolution_11, TIME_INTERVAL);
  temperatureSensors.onTemperatureChange(handleTemperatureChange);
  tft.fillScreen(TFT_BLACK);
}

void loop() {
  temperatureSensors.update(); 
  tft.setTextFont(6);
  tft.setCursor(1, 1);
  tft.print(tempC, 1);
  tft.print(" C");
  tft.setCursor(1, 120);
  tft.print(tempF, 1);
  tft.print(" F");
  tft.setTextFont(2);
  tft.setCursor(1, 220);
  tft.print(i);
  i++;
  if (i>99){i=1;}
  delay(10);
}
