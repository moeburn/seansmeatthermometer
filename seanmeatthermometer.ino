#include <TFT_eSPI.h> 
#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <NonBlockingDallas.h>                  //Include the NonBlockingDallas library
#include <Adafruit_ADS1X15.h>
#include <SteinhartHart.h>
#include <Preferences.h>
#include "XT_DAC_Audio.h"
#include "JohnnyCash.h"  
#include <WiFiManager.h> 
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <nvs_flash.h>
#include "MusicDefinitions.h"
#include "Fonts/Roboto_Condensed_32.h"

#define MYFONT32 &Roboto_Condensed_32

#define SPEAKER_PIN 25
#define MUTE_PIN 2
#define ONE_WIRE_BUS 4   
#define PWR_LED_PIN 3
#define button1 17 //RX2
#define button2 16 //TX2
#define alarm_hyst 0.2
#define is2connectedthreshold 26300
#define ETA_INTERVAL 15


int8_t PROGMEM TwinkleTwinkle[] = {
  NOTE_SILENCE,BEAT_2,NOTE_C5,NOTE_C5,NOTE_G5,NOTE_G5,NOTE_A5,NOTE_A5,NOTE_G5,BEAT_2,
  NOTE_F5,NOTE_F5,NOTE_E5,NOTE_E5,NOTE_D5,NOTE_D5,NOTE_C5,BEAT_2,
  NOTE_G5,NOTE_G5,NOTE_F5,NOTE_F5,NOTE_E5,NOTE_E5,NOTE_D5,BEAT_2,
  NOTE_G5,NOTE_G5,NOTE_F5,NOTE_F5,NOTE_E5,NOTE_E5,NOTE_D5,BEAT_2,
  NOTE_C5,NOTE_C5,NOTE_G5,NOTE_G5,NOTE_A5,NOTE_A5,NOTE_G5,BEAT_2,
  NOTE_F5,NOTE_F5,NOTE_E5,NOTE_E5,NOTE_D5,NOTE_D5,NOTE_C5,BEAT_4,  
  NOTE_SILENCE,BEAT_5,SCORE_END
};

int8_t PROGMEM TwoBits[] = {
  NOTE_SILENCE,BEAT_2,NOTE_C7,BEAT_2,NOTE_G6,NOTE_G6,NOTE_A6,BEAT_2,NOTE_G6,BEAT_2,
  NOTE_SILENCE,BEAT_2,NOTE_B6,BEAT_2,NOTE_C7,BEAT_2,NOTE_SILENCE,SCORE_END
};

int8_t PROGMEM UkranianBellCarol[] = {
  NOTE_SILENCE,BEAT_2,NOTE_DS6,BEAT_2,NOTE_D6,NOTE_DS6,NOTE_C6,BEAT_2,
  NOTE_DS6,BEAT_2,NOTE_D6,NOTE_DS6,NOTE_C6,BEAT_2,
  NOTE_DS6,BEAT_2,NOTE_D6,NOTE_DS6,NOTE_C6,BEAT_2,
  NOTE_DS6,BEAT_2,NOTE_D6,NOTE_DS6,NOTE_C6,BEAT_2,
  NOTE_G5, NOTE_A5, NOTE_B5, NOTE_C6, NOTE_D6,NOTE_DS6, NOTE_F6, NOTE_G6,  NOTE_F6, BEAT_2, NOTE_DS6,BEAT_2,//G Ab Bb C D Ds F G F
  NOTE_G5, NOTE_A5, NOTE_B5, NOTE_C6, NOTE_D6,NOTE_DS6, NOTE_F6, NOTE_G6,  NOTE_F6, BEAT_2, NOTE_DS6,BEAT_2,
  NOTE_SILENCE,SCORE_END
};

int8_t PROGMEM AlarmSong[] = {
  BEAT_1,NOTE_SILENCE,BEAT_1,
  BEAT_4,NOTE_C5,BEAT_4,
  BEAT_4,NOTE_C5,BEAT_4,
  BEAT_4,NOTE_C5,BEAT_4,
  NOTE_SILENCE,BEAT_5,SCORE_END
};

XT_MusicScore_Class Music(TwinkleTwinkle,TEMPO_ALLEGRO,INSTRUMENT_PIANO); 
XT_MusicScore_Class ShaveAndAHaircut(TwoBits,TEMPO_PRESTISSIMO  ,INSTRUMENT_HARPSICHORD ); 
XT_MusicScore_Class DingFriesAreDone(UkranianBellCarol,TEMPO_PRESTISSIMO  , INSTRUMENT_SAXOPHONE  ); 
XT_MusicScore_Class Alarm(AlarmSong,TEMPO_ALLEGRO,  INSTRUMENT_ORGAN );


#include "SPIFFS.h"
#include <Arduino_JSON.h>
  
uint16_t cmap[24];
const char* cmapNames[24];


void initializeCmap() {
    cmap[0] =  TFT_BLACK;
    cmapNames[0] = "BLACK";
    cmap[1] =  TFT_NAVY;
    cmapNames[1] = "NAVY";
    cmap[2] =  TFT_DARKGREEN;
    cmapNames[2] = "DARKGREEN";
    cmap[3] =  TFT_DARKCYAN;
    cmapNames[3] = "DARKCYAN";
    cmap[4] =  TFT_MAROON;
    cmapNames[4] = "MAROON";
    cmap[5] =  TFT_PURPLE;
    cmapNames[5] = "PURPLE";
    cmap[6] =  TFT_OLIVE;
    cmapNames[6] = "OLIVE";
    cmap[7] =  TFT_LIGHTGREY;
    cmapNames[7] = "LIGHTGREY";
    cmap[8] =  TFT_DARKGREY;
    cmapNames[8] = "DARKGREY";
    cmap[9] =  TFT_BLUE;
    cmapNames[9] = "BLUE";
    cmap[10] =  TFT_GREEN;
    cmapNames[10] = "GREEN";
    cmap[11] =  TFT_CYAN;
    cmapNames[11] = "CYAN";
    cmap[12] =  TFT_RED;
    cmapNames[12] = "RED";
    cmap[13] =  TFT_MAGENTA;
    cmapNames[13] = "MAGENTA";
    cmap[14] =  TFT_YELLOW;
    cmapNames[14] = "YELLOW";
    cmap[15] =  TFT_WHITE;
    cmapNames[15] = "WHITE";
    cmap[16] =  TFT_ORANGE;
    cmapNames[16] = "ORANGE";
    cmap[17] =  TFT_GREENYELLOW;
    cmapNames[17] = "GREENYELLOW";
    cmap[18] =  TFT_PINK;
    cmapNames[18] = "PINK";
    cmap[19] =  TFT_BROWN;
    cmapNames[19] = "BROWN";
    cmap[20] =  TFT_GOLD;
    cmapNames[20] = "GOLD";
    cmap[21] =  TFT_SILVER;
    cmapNames[21] = "SILVER";
    cmap[22] =  TFT_SKYBLUE;
    cmapNames[22] = "SKYBLUE";
    cmap[23] =  TFT_VIOLET;
    cmapNames[23] = "VIOLET";
}


double oldtemp, tempdiff, eta, eta2, oldtemp2, tempdiff2;
int etamins, etasecs;

int settemp = 145;
bool is2connected = false;
int channel = 0;
int setSelection = 0;
int setAlarm, setUnits, setBGC;
int setFGC = 15;
int setVolume = 100;
int setLEDmode = 0;

bool b1pressed = false;
bool b2pressed = false;
bool settingspage = false;
bool kincreased = false;
int setIcons = 1;


XT_Wav_Class Sound(RingOfFire); 

XT_DAC_Audio_Class DacAudio(SPEAKER_PIN,0);   //Set up the DAC on pin 25

Preferences preferences;




//SteinhartHart thermistor(10729,16535,20860, 348.15, 323.15, 303.15);
SteinhartHart thermistor(15062.08,36874.80,82837.54, 348.15, 323.15, 303.15);

Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */

AsyncWebServer server(80);

AsyncEventSource events("/events");

// Json Variable to Hold Sensor Readings
JSONVar readings;

float tempC, tempF, tempA0, tempA1, tempA0f, tempA1f;
  int16_t adc0, adc1, adc2, adc3, therm1, therm2, therm3;
  float temp1, temp2, temp3;
  float volts0, volts1, volts2, volts3;

double ADSToOhms(int16_t ADSreading) {
      float voltsX = ads.computeVolts(ADSreading);
      return (voltsX * 22000) / (3.3 - voltsX);
}

String getSensorReadings(){  //JSON constructor

  readings["sensor1"] = String(tempA0f);
  readings["sensor2"] = String(tempA1f);
  readings["sensor3"] = String(settemp);
  readings["sensor4"] = String(etamins);

  String jsonString = JSON.stringify(readings);
  return jsonString;
}

void initSPIFFS() {
  if (!SPIFFS.begin()) {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  else{
    Serial.println("SPIFFS mounted successfully");
  }
}


#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 240




                       //PIN of the Maxim DS18B20 temperature sensor
#define TIME_INTERVAL 750                      //Time interval among sensor readings [milliseconds]

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature dallasTemp(&oneWire);
NonBlockingDallas temperatureSensors(&dallasTemp);   

TFT_eSPI tft = TFT_eSPI();   
TFT_eSprite img = TFT_eSprite(&tft);
int i;
bool dallasConnected = false;
bool calibrationMode = false;
bool saved = false;
String b1String, b2String;
int animpos = 80;
float barx;
int32_t rssi;

//Macro for 'every' 
#define every(interval) \        
  static uint32_t __every__##interval = millis(); \
  if (millis() - __every__##interval >= interval && (__every__##interval = millis()))




void handleTemperatureChange(int deviceIndex, int32_t temperatureRAW)
{
  tempC = temperatureSensors.rawToCelsius(temperatureRAW);
  tempF = temperatureSensors.rawToFahrenheit(temperatureRAW);
}

void handleIntervalElapsed(int deviceIndex, int32_t temperatureRAW)
{
  tempC = temperatureSensors.rawToCelsius(temperatureRAW);
  tempF = temperatureSensors.rawToFahrenheit(temperatureRAW);  
}

long mapf(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


  String dallasString,  temp1string,temp2string,temp3string ;
  
void drawWiFiSignalStrength(int32_t x, int32_t y, int32_t radius) {
    // Get the RSSI value
    
    
    // Define colors
    uint32_t color;
    int numArcs;

    // Determine the color and number of arcs to draw based on RSSI value
    if (rssi > -60) {
        color = cmap[setFGC];
        numArcs = 3;
    } else if (rssi > -75) {
        color = cmap[setFGC];
        numArcs = 2;
    } else if (rssi > -85) {
        color = cmap[setFGC];
        numArcs = 2;
    } else {
        color = cmap[setFGC];
        numArcs = 1;
    }

    // Draw the base circle/dot
    img.fillCircle(x, y+1, 1, color);

    // Draw arcs based on the determined number of arcs and color
    if (numArcs >= 1) {
        img.drawArc(x, y, radius / 3, radius / 3 - 1, 135, 225, color, cmap[setBGC]);  // Arc 1
    }
    if (numArcs >= 2) {
        img.drawArc(x, y, 2 * radius / 3, 2 * radius / 3 - 1, 135, 225, color, cmap[setBGC]);  // Arc 2
    }
    if (numArcs >= 3) {
        img.drawArc(x, y, radius, radius - 1, 135, 225, color, cmap[setBGC]);  // Arc 3
    }
}

void drawTemps() {
  every(100){
    if (!digitalRead(button1) && !digitalRead(button2)) {
      settingspage = true;
    }

    else if (!digitalRead(button1)) {settemp--;}
    else if (!digitalRead(button2)) {settemp++;}
  }
  
  img.fillSprite(cmap[setBGC]);
  img.setCursor(0,0);
  img.setTextSize(1);
  img.setTextColor(cmap[setFGC]);
  img.setTextDatum(TC_DATUM);



  if (is2connected) { 
    img.setTextFont(6);
    img.drawFloat(tempA0f, 1, 60,5);
    img.drawFloat(tempA1f, 1, 180,5);
    img.drawFastVLine(120,0,85,cmap[setFGC]);
  }
  else {
    img.setTextFont(8);
    img.drawFloat(tempA0f, 1, 115,5);
    
  }
  img.drawFastHLine(0,85,240,cmap[setFGC]);
  img.setTextFont(1);
  img.setTextDatum(TR_DATUM);

    if (setUnits == 0) {img.drawCircle(229,2,1,cmap[setFGC]); img.drawString("C", 239,1);}
    else if (setUnits == 1) {img.drawCircle(229,2,1,cmap[setFGC]); img.drawString("F", 239,1);}
    else if (setUnits == 2) {img.drawString("K", 239,1);}

  
  
  img.setTextDatum(TL_DATUM);
  img.setFreeFont(MYFONT32);   
  img.setCursor(5,100+24);
  String settempstring = ">" + String(settemp) + "<";
  //img.drawString(settempstring, 5,100);
  img.print("Set Temp:");
  //img.setTextFont(6);
  img.setTextDatum(TR_DATUM);
  img.drawString(settempstring, 239, 100);
  img.setTextDatum(TL_DATUM);

  img.setCursor(5,170+24);
  //img.setTextFont(6);
  //img.setTextSize(2);
  img.print("ETA:"); 
  String etastring;
  if ((etamins < 1000) && (etamins >= 0)) {
    //img.print(etamins);
    etastring = String(etamins) + "mins";
    }
  else {
    //img.print("---"); 
    etastring = "---mins";
  }
  img.setTextDatum(TR_DATUM);
  img.drawString(etastring, 239, 170);
  //img.print("mins"); 
  //img.setTextSize(1);

  img.setTextFont(1);


  img.drawFastHLine(0,226,240,cmap[setFGC]);
  img.setCursor(1,231);
  img.print(WiFi.localIP());
  img.setTextDatum(BR_DATUM);


  
  //### Battery icon ###
  if (setIcons) {
    img.drawRect(214,230,20,9,cmap[setFGC]);
    img.fillRect(214,230,barx,9,cmap[setFGC]);
    img.drawFastVLine(234,232,4,cmap[setFGC]);
    img.drawFastVLine(235,232,4,cmap[setFGC]);
    if (WiFi.status() == WL_CONNECTED) {drawWiFiSignalStrength(200,237,9);}
  }
  else {
    String v2String = String(rssi) + "dB/" + String(volts2,2) + "v";
    img.drawString(v2String, 239,239);
  }

  img.fillRect(animpos, 232, 4, 4, cmap[setFGC]);
  animpos += 2;
  if (animpos > 140) {animpos = 100;}

  img.pushSprite(0, 0);
}

void drawCalib(){
  img.fillSprite(TFT_MAROON);
  img.setTextSize(2);
  img.setTextColor(TFT_WHITE, TFT_BLACK, true);
  img.setTextWrap(true); // Wrap on width
  img.setTextFont(1);
  img.setTextDatum(TL_DATUM);
  img.setCursor(0,0);
  img.println("Calibrating!");
  img.setTextSize(1);
  img.setCursor(0,200);
  img.println("Please wait for all 3 temperature points to be measured...");
  img.setTextSize(2);


   dallasString = String(tempC, 1) + " C, A0:" + String(adc0);
  img.drawString(dallasString, 10,20);

  if ((tempC >= 75.0) && (tempC <= 75.2)) { 
    temp1 = tempC;
    therm1 = ADSToOhms(adc0);
  }
  if ((tempC >= 50.0) && (tempC <= 50.2)) { 
    temp2 = tempC;
    therm2 = ADSToOhms(adc0);
  }
  if ((tempC >= 30.0) && (tempC <= 30.2)) { 
    temp3 = tempC;
    therm3 = ADSToOhms(adc0);
  }

     temp1string = "75C = " +String(therm1);
    img.drawString(temp1string, 10,40);

     temp2string = "50C = " +String(therm2);
    img.drawString(temp2string, 10,60);

     temp3string = "30C = " +String(therm3);
    img.drawString(temp3string, 10,80);

  if ((temp3 > 0) && (temp2 > 0) && (temp1 > 0)) {
        img.fillSprite(TFT_GREEN);
        img.setCursor(0,0);
        img.print("Calibration saved, please reboot!");
        img.setTextSize(2);
        thermistor.setTemperature1(temp1 + 273.15);
        thermistor.setTemperature2(temp2 + 273.15);
        thermistor.setTemperature3(temp3 + 273.15);
        thermistor.setResistance1(therm1);
        thermistor.setResistance2(therm2);
        thermistor.setResistance3(therm3);
        String coeffAstring = "A: " + String(thermistor.getCoeffA());
        String coeffBstring = "B: " + String(thermistor.getCoeffB());
        String coeffCstring = "C: " + String(thermistor.getCoeffC());

        img.drawString(temp1string, 10,40);
        img.drawString(temp2string, 10,60);
        img.drawString(temp3string, 10,80);
        img.drawString(coeffAstring, 10,100);
        img.drawString(coeffBstring, 10,120);
        img.drawString(coeffCstring, 10,140);

        if (!saved) {
          digitalWrite(MUTE_PIN, HIGH);
          preferences.begin("my-app", false);
          preferences.putInt("temp1", temp1);
          preferences.putInt("temp2", temp2);
          preferences.putInt("temp3", temp3);
          preferences.putInt("therm1", therm1);
          preferences.putInt("therm2", therm2);
          preferences.putInt("therm3", therm3);
          preferences.end();
          saved = true;
          DacAudio.Play(&Music); 
        }
  }
  img.pushSprite(0, 0);
}

void drawSettings() {
     
     
     
  img.fillSprite(TFT_BLACK);
  img.setCursor(0,0);
  img.setTextSize(3);
  img.setTextColor(TFT_WHITE);
  img.setTextDatum(TL_DATUM);
  img.setTextWrap(true); // Wrap on width
  img.setTextFont(1);  
  every(101) {
    if (!digitalRead(button1)) {b1pressed = true;}
    if (!digitalRead(button2)) {setSelection++;}
  }
  if (setSelection > 8) {setSelection = 0;}
  if (setSelection == 0) {img.setTextColor(TFT_BLACK, TFT_WHITE, true); if (b1pressed) {setAlarm++; b1pressed = false;}} else {img.setTextColor(TFT_WHITE);}

  img.println("Alarm:");

  if (setSelection == 1) {img.setTextColor(TFT_BLACK, TFT_WHITE, true); if (b1pressed) {setUnits++; b1pressed = false;}} else {img.setTextColor(TFT_WHITE);}

  img.println("Units:");

  if (setSelection == 2) {img.setTextColor(TFT_BLACK, TFT_WHITE, true); if (b1pressed) {setBGC++; b1pressed = false;}} else {img.setTextColor(TFT_WHITE);}

  img.println("BG Colour:");

  if (setSelection == 3) {img.setTextColor(TFT_BLACK, TFT_WHITE, true); if (b1pressed) {setFGC++; b1pressed = false;}} else {img.setTextColor(TFT_WHITE);}

  img.println("FG Colour:");

  if (setSelection == 4) {img.setTextColor(TFT_BLACK, TFT_WHITE, true); if (b1pressed) {setVolume++; DacAudio.DacVolume=setVolume; b1pressed = false;}} else {img.setTextColor(TFT_WHITE);}

  img.println("Volume:");

  if (setSelection == 5) {img.setTextColor(TFT_BLACK, TFT_WHITE, true); if (b1pressed) {setIcons++; b1pressed = false;}} else {img.setTextColor(TFT_WHITE);}

  img.println("Icons:");

  if (setSelection == 6) {img.setTextColor(TFT_BLACK, TFT_WHITE, true); if (b1pressed) {setLEDmode++; b1pressed = false;}} else {img.setTextColor(TFT_WHITE);}

  img.println("LED Mode:");

  if (setSelection == 7) {img.setTextColor(TFT_BLACK, TFT_WHITE, true); if (b1pressed) {doSound(); b1pressed = false;}} else {img.setTextColor(TFT_WHITE);}

  img.println(">Test Spk<");

  if (setSelection == 8) {img.setTextColor(TFT_BLACK, TFT_WHITE, true); if (b1pressed) {savePrefs(); b1pressed = false;}} else {img.setTextColor(TFT_WHITE);}
  img.println(">Save<");


  img.setTextColor(TFT_WHITE);
  if (setIcons > 1) {setIcons = 0;}
  if (setAlarm > 4) {setAlarm = 0;}
  if (setLEDmode > 3) {setLEDmode = 0;}
  if (setUnits > 2) {setUnits = 0; if (kincreased) {settemp-=273; forceADC(); kincreased = false;}}
  if (setBGC > 23) {setBGC = 0;}
  if (setFGC > 23) {setFGC = 0;}
  if (setVolume > 100) {setVolume = 0;}
  img.setCursor(240,0);
  img.setTextDatum(TR_DATUM);
  img.drawNumber(setAlarm, 240, 0);
  String setUnitString;
  String setIconString;
  if (setUnits == 0) {setUnitString = "C";} else if (setUnits == 1) {setUnitString = "F";} else {setUnitString = "K"; if (!kincreased) {settemp+=273; forceADC(); kincreased = true;}}
  if (setIcons == 0) {setIconString = "N";} else if (setIcons == 1) {setIconString = "Y";} 
  img.drawString(setUnitString, 240, 24);
  img.drawNumber(setBGC, 240, 24+24);
  img.drawNumber(setFGC, 240, 24+24+24);
  img.drawNumber(setVolume, 240, 24+24+24+24);
  img.drawString(setIconString, 240, 24+24+24+24+24);
  img.drawNumber(setLEDmode, 240, 24+24+24+24+24+24);
  img.setTextDatum(TC_DATUM);
  img.setTextColor(cmap[setFGC], cmap[setBGC], true);
  String sampleString = String(setFGC) +  cmapNames[setFGC];
  img.drawString(sampleString, 120, 216);
  img.pushSprite(0, 0);
}

void doADC() {
  if (ads.conversionComplete()) {
    if (channel == 0) {
      adc0 = ads.getLastConversionResults();
      if (setUnits == 0) {tempA0f = thermistor.resistanceToTemperature(ADSToOhms(adc0)) - 273.15;}
      else if (setUnits == 1) {
        tempA0 = thermistor.resistanceToTemperature(ADSToOhms(adc0)) - 273.15;
        tempA0f = (tempA0 * 1.8) + 32;
      }
      else if (setUnits == 2) {tempA0f = thermistor.resistanceToTemperature(ADSToOhms(adc0));}

      ads.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_1, false);
      channel = 1;
      return;
    }
    if (channel == 1) {
      adc1 = ads.getLastConversionResults();
      if (setUnits == 0) {tempA1f = thermistor.resistanceToTemperature(ADSToOhms(adc1)) - 273.15;}
      else if (setUnits == 1) {
        tempA1 = thermistor.resistanceToTemperature(ADSToOhms(adc1)) - 273.15;
        tempA1f = (tempA1 * 1.8) + 32;
      }
      else if (setUnits == 2) {tempA1f = thermistor.resistanceToTemperature(ADSToOhms(adc1));}
      ads.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_2, false);
      channel = 2;
      return;
    }
    if (channel == 2) {
      adc2 = ads.getLastConversionResults();
      ads.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_0, false);
      channel = 0;
      return;
    }
  }
  
  //String a0String = "A0: " + String(adc0);
  //String a1String = "A1: " + String(adc1);
  //String v0String = "V0: " + String(volts0,3) + "v";
  //String v1String = "V1: " + String(volts1,3) + "v";
  //String v2String = "Vbat: " + String(volts2,3) + "v";
  

}

void forceADC() {
  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
  adc2 = ads.readADC_SingleEnded(2);
  volts2 = ads.computeVolts(adc2) * 2.0;
  adc0 = ads.getLastConversionResults();
  if (setUnits == 0) {tempA0f = thermistor.resistanceToTemperature(ADSToOhms(adc0)) - 273.15; tempA1f = thermistor.resistanceToTemperature(ADSToOhms(adc1)) - 273.15;}
  else if (setUnits == 1) {
    tempA0 = thermistor.resistanceToTemperature(ADSToOhms(adc0)) - 273.15;
    tempA0f = (tempA0 * 1.8) + 32;
    tempA1 = thermistor.resistanceToTemperature(ADSToOhms(adc1)) - 273.15;
    tempA1f = (tempA1 * 1.8) + 32;
  }
  else if (setUnits == 2) {tempA0f = thermistor.resistanceToTemperature(ADSToOhms(adc0)); tempA1f = thermistor.resistanceToTemperature(ADSToOhms(adc1));}
  barx = mapf (volts2, 3.6, 4.1, 0, 20);
}

void savePrefs() {
  if (setFGC == setBGC) {setFGC = 15; setBGC = 0;}
  preferences.begin("my-app", false);
  preferences.putInt("setAlarm", setAlarm);
  preferences.putInt("setUnits", setUnits);
  preferences.putInt("setFGC", setFGC);
  preferences.putInt("setBGC", setBGC);
  preferences.putInt("setVolume", setVolume); 
  preferences.putInt("setIcons", setIcons);
  preferences.putInt("setLEDmode", setLEDmode);
  preferences.putInt("settemp", settemp);
  preferences.end();
  settingspage = false;
}

void doSound() {
  digitalWrite(MUTE_PIN, HIGH); 
  if (setAlarm == 0) {
    if(Sound.Playing==false)       
    DacAudio.Play(&Sound);
  }
  else if (setAlarm == 1) {
    if(Music.Playing==false)       
    DacAudio.Play(&Music);
  }
  else if (setAlarm == 2) {
    if(Alarm.Playing==false)       
    DacAudio.Play(&Alarm);
  }
  else if (setAlarm == 3) {
    if(ShaveAndAHaircut.Playing==false)       
    DacAudio.Play(&ShaveAndAHaircut);
  }
  else if (setAlarm == 4) {
    if(DingFriesAreDone.Playing==false)       
    DacAudio.Play(&DingFriesAreDone);
  }
}

void setup() {
  initializeCmap();
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(TFT_WHITE, TFT_BLACK, true);
  tft.setTextWrap(true); // Wrap on width
  tft.setTextFont(2);
  tft.setTextSize(1);
  img.setColorDepth(8);
  img.createSprite(239, 239);
  img.fillSprite(TFT_BLUE);
  
  DacAudio.StopAllSounds();
  
  



  ads.begin();
  ads.setGain(GAIN_ONE);
  ads.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_0, false);
  pinMode(MUTE_PIN, OUTPUT);
  pinMode(PWR_LED_PIN, OUTPUT);
  
  digitalWrite(MUTE_PIN, LOW);
  digitalWrite(PWR_LED_PIN, HIGH);
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  preferences.begin("my-app", false);
  temp1 = preferences.getInt("temp1", 0);
  temp2 = preferences.getInt("temp2", 0);
  temp3 = preferences.getInt("temp3", 0);
  therm1 = preferences.getInt("therm1", 0);
  therm2 = preferences.getInt("therm2", 0);
  therm3 = preferences.getInt("therm3", 0);
  setAlarm = preferences.getInt("setAlarm", 0);
  setUnits = preferences.getInt("setUnits", 1);
  setFGC = preferences.getInt("setFGC", 12);
  setBGC = preferences.getInt("setBGC", 4);
  setVolume = preferences.getInt("setVolume", 100);
  setLEDmode = preferences.getInt("setLEDmode", 2);
  setIcons = preferences.getInt("setIcons", 1);
  settemp = preferences.getInt("settemp", 145);
  preferences.end();
  DacAudio.DacVolume=setVolume;
  if (setFGC == setBGC) {setFGC = 15; setBGC = 0;}
  if ((temp3 > 0) && (temp2 > 0) && (temp1 > 0)) {
        thermistor.setTemperature1(temp1 + 273.15);
        thermistor.setTemperature2(temp2 + 273.15);
        thermistor.setTemperature3(temp3 + 273.15);
        thermistor.setResistance1(therm1);
        thermistor.setResistance2(therm2);
        thermistor.setResistance3(therm3);
        thermistor.calcCoefficients();
  }
  rssi = WiFi.RSSI();
  forceADC();
  drawTemps();

  WiFi.mode(WIFI_STA);
  WiFiManager wm;
  if ((!digitalRead(button1) && !digitalRead(button2)) || !wm.getWiFiIsSaved()){
    nvs_flash_erase(); // erase the NVS partition and...
    nvs_flash_init(); // initialize the NVS partition.
    tft.fillScreen(TFT_ORANGE);
    tft.setCursor(0, 0);
    tft.setTextFont(4);
    tft.setTextSize(1);
    tft.println("SETTINGS RESET.");
    tft.println("");
    tft.println("Please connect to");
    tft.println("'MR MEAT SETUP'");
    tft.println("WiFi, and browse to");
    tft.println("192.168.4.1.");
    wm.resetSettings();
    

    bool res;
    // res = wm.autoConnect(); // auto generated AP name from chipid
    // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
    res = wm.autoConnect("Mr Meat Setup"); // password protected ap

    if(!res) {
        tft.fillScreen(TFT_RED);
        tft.setCursor(0, 0);
        tft.println("Failed to connect, restarting");
        delay(3000);
        ESP.restart();
    } 
    else {
        //if you get here you have connected to the WiFi    
        tft.fillScreen(TFT_GREEN);
        tft.setCursor(0, 0);
        tft.println("Connected!");
        tft.println(WiFi.localIP());
        digitalWrite(PWR_LED_PIN, HIGH);
        delay(250);
        digitalWrite(PWR_LED_PIN, LOW);
        delay(250);
        digitalWrite(PWR_LED_PIN, HIGH);
        delay(250);
        digitalWrite(PWR_LED_PIN, LOW);
        delay(250);
        digitalWrite(PWR_LED_PIN, HIGH);
        delay(250);
        digitalWrite(PWR_LED_PIN, LOW);
        delay(250);
        digitalWrite(PWR_LED_PIN, HIGH);
        delay(250);
        digitalWrite(PWR_LED_PIN, LOW);
        delay(250);
        tft.fillScreen(TFT_BLACK);
        ESP.restart();
    }
  }
  else {
    WiFi.begin(wm.getWiFiSSID(), wm.getWiFiPass());

  }
  digitalWrite(PWR_LED_PIN, HIGH);
  initSPIFFS();

  temperatureSensors.begin(NonBlockingDallas::resolution_12, TIME_INTERVAL);
  if (temperatureSensors.getSensorsCount() > 0)
  {
    temp1 = 0;
    temp2 = 0;
    temp3 = 0;
    calibrationMode = true;
    temperatureSensors.onIntervalElapsed(handleIntervalElapsed);
    temperatureSensors.onTemperatureChange(handleTemperatureChange);
    tft.fillScreen(TFT_PURPLE);
    tft.setCursor(0, 0);
    tft.setTextFont(4);
    tft.println("Calibration Mode!");
    tft.println("To begin, connect 1 or 2 meat thermometer probes, immerse them and the calibration probe in a small cup of hot freshly boiled water (>75C), and press any button.");
    while (digitalRead(button1) && digitalRead(button2)) {}
  }
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){  //If someone connects to the root of our HTTP site, serve index.html
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.serveStatic("/", SPIFFS, "/");

  // Request for the latest sensor readings
  server.on("/readings", HTTP_GET, [](AsyncWebServerRequest *request){  //Serve the raw JSON data on /readings
    String json = getSensorReadings();
    request->send(200, "application/json", json);
    json = String();
  });

  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 10000);
  });
  server.addHandler(&events);
  AsyncElegantOTA.begin(&server);  //Start the OTA firmware updater on /update
  server.begin();

}

void loop() {
  temperatureSensors.update(); 
  doADC();
  every(2000) {
    rssi = WiFi.RSSI();
  }
  every(10000) {       //Update web interface once every 10 seconds
    volts2 = ads.computeVolts(adc2) * 2.0;
    barx = mapf (volts2, 3.5, 4.0, 0, 20);
    events.send("ping",NULL,millis());  
    events.send(getSensorReadings().c_str(),"new_readings" ,millis());
  }

  every(5){
    
    if (adc1 < is2connectedthreshold) {is2connected = true;} else {is2connected = false;} 
    if (!calibrationMode) {
      if (setLEDmode == 0) {analogWrite(PWR_LED_PIN, 0);}
      else if (setLEDmode == 1) {analogWrite(PWR_LED_PIN, 40);}
      else if (setLEDmode == 2) {analogWrite(PWR_LED_PIN, 255);}
      else if (setLEDmode == 3) {
          float led_brightness = mapf(tempA0f, 0, settemp, 0, 255); 
          analogWrite(PWR_LED_PIN, led_brightness);
        }
      

      if (!settingspage) {drawTemps();}
      else {drawSettings();}
      }
    else {drawCalib();}
  }
  DacAudio.FillBuffer(); 

  if ((Sound.Playing) || (Music.Playing) || (Alarm.Playing) || (DingFriesAreDone.Playing) || (ShaveAndAHaircut.Playing)) {
    digitalWrite(MUTE_PIN, HIGH); 
  }
  else  {digitalWrite(MUTE_PIN, LOW);}
  

  if ((tempA0f >= settemp) ||  (tempA1f >= settemp)) {  //If 2nd probe is connected and either temp goes above set temp, sound the alarm
    doSound();
  }

  

  every (15000) {  //manually set this to ETA_INTERVAL*1000, can't hardcode due to macro
        tempdiff = tempA0f - oldtemp;
    if (is2connected) {  //If 2nd probe is connected, calculate whichever ETA is sooner in seconds
      tempdiff2 = tempA1f - oldtemp2;
      eta = (((settemp - tempA0f)/tempdiff) * ETA_INTERVAL);
      eta2 = (((settemp - tempA1f)/tempdiff2) * ETA_INTERVAL);
      if ((eta2 > 0) && (eta2 < 1000) && (eta2 < eta)) {eta = eta2;}
      
      oldtemp2 = tempA1f;
    }
    else  //Else if only one probe is connected, calculate the ETA in seconds
    {
      eta = (((settemp - tempA0f)/tempdiff) * ETA_INTERVAL);
    }
    etamins = eta / 60;  //cast it to int and divide it by 60 to get minutes with no remainder, ignore seconds because of inaccuracy
    oldtemp = tempA0f;
    
  }

}
