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

#include "MusicDefinitions.h"

int8_t PROGMEM TwinkleTwinkle[] = {
  NOTE_C5,NOTE_C5,NOTE_G5,NOTE_G5,NOTE_A5,NOTE_A5,NOTE_G5,BEAT_2,
  NOTE_F5,NOTE_F5,NOTE_E5,NOTE_E5,NOTE_D5,NOTE_D5,NOTE_C5,BEAT_2,
  NOTE_G5,NOTE_G5,NOTE_F5,NOTE_F5,NOTE_E5,NOTE_E5,NOTE_D5,BEAT_2,
  NOTE_G5,NOTE_G5,NOTE_F5,NOTE_F5,NOTE_E5,NOTE_E5,NOTE_D5,BEAT_2,
  NOTE_C5,NOTE_C5,NOTE_G5,NOTE_G5,NOTE_A5,NOTE_A5,NOTE_G5,BEAT_2,
  NOTE_F5,NOTE_F5,NOTE_E5,NOTE_E5,NOTE_D5,NOTE_D5,NOTE_C5,BEAT_4,  
  NOTE_SILENCE,BEAT_5,SCORE_END
};

XT_MusicScore_Class Music(TwinkleTwinkle,TEMPO_ALLEGRO,INSTRUMENT_PIANO); 


#include "SPIFFS.h"
#include <Arduino_JSON.h>


#define SPEAKER_PIN 25
#define MUTE_PIN 2
#define ONE_WIRE_BUS 4   
#define PWR_LED_PIN 3
#define button1 16 //RX2
#define button2 17 //TX2
#define alarm_hyst 0.2

bool b1pressed = false;
bool b2pressed = false;



XT_Wav_Class Sound(RingOfFire); 

XT_DAC_Audio_Class DacAudio(SPEAKER_PIN,0);   //Set up the DAC on pin 25

Preferences preferences;




SteinhartHart thermistor(9479,13027,15419, 347.35, 320.85, 286.45);

Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */

AsyncWebServer server(80);

AsyncEventSource events("/events");

// Json Variable to Hold Sensor Readings
JSONVar readings;

float tempC, tempF, tempA0, tempA1, tempA0f, tempA1f;
  int16_t adc0, adc1, adc2, adc3, therm1, therm2, therm3;
  float temp1, temp2, temp3;
  float volts0, volts1, volts2, volts3;

String getSensorReadings(){  //JSON constructor

  readings["sensor1"] = String(tempA0f);
  readings["sensor2"] = String(tempA1f);
  readings["sensor3"] = String(tempC);
  readings["sensor4"] = String(tempA0);

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

  String dallasString,  temp1string,temp2string,temp3string ;

void drawTemps() {
  img.fillSprite(TFT_BLUE);
  img.setCursor(0,0);
  img.print("Temperature:");
  img.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK, true);
  tft.setTextWrap(true); // Wrap on width
  tft.setTextFont(3);
  tft.setTextSize(2);

  if (digitalRead(button1)) { b1String = "B1: ON";}
  else { b1String = "B1: OFF";}
  if (digitalRead(button2)) { b2String = "B2: ON";}
  else { b2String = "B2: OFF";}

  if (dallasConnected) {tft.setTextFont(3); 
   dallasString = String(tempC, 1) + " C";}
  else {tft.setTextFont(2);
   dallasString = "DS18B20 NOT FOUND";}
  img.drawString(dallasString, 10,20);
  tft.setTextFont(3);
  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
  adc2 = ads.readADC_SingleEnded(2);
  volts0 = ads.computeVolts(adc0);
  volts1 = ads.computeVolts(adc1);
  volts2 = ads.computeVolts(adc2) * 2.0;
  String a0String = "A0: " + String(adc0);
  String a1String = "A1: " + String(adc1);
  String v0String = "V0: " + String(volts0,3) + "v";
  String v1String = "V1: " + String(volts1,3) + "v";
  String v2String = "Vs: " + String(volts2,3) + "v";
  tempA0 = thermistor.resistanceToTemperature(adc0) - 273.15;
  tempA1 = thermistor.resistanceToTemperature(adc1) - 273.15;
  tempA0f = (tempA0 * 1.8) + 32;
  tempA1f = (tempA1 * 1.8) + 32;
  String tempA0string = "T0: " + String(tempA0,2) + " C";
  String tempA1string = "T1: " + String(tempA1,2) + " C";
  String tempA0fstring = "T0: " + String(tempA0f,2) + " F";
  String tempA1fstring = "T1: " + String(tempA1f,2) + " F";
  String debugstring = String(temp1) + "," + String(temp2) + "," + String(temp3);
  //String debugstring2 = String(therm1) + "," + String(therm2) + "," + String(therm3);
  
  img.drawString(a0String, 10,40);
  img.drawString(a1String, 10,60);
  img.drawString(v0String, 10,80);
  img.drawString(v1String, 10,100);
  img.drawString(tempA0string, 10,120);
  img.drawString(tempA1string, 10,140);
  img.drawString(b1String, 10,160);
  img.drawString(b2String, 10,180);
  img.drawString(debugstring, 10,200);
  img.drawString(v2String, 10,220);
  img.pushSprite(0, 0);
}

void drawCalib(){
  img.fillSprite(TFT_RED);
  img.setCursor(0,0);
  img.print("Temperature:");
  img.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK, true);
  tft.setTextWrap(true); // Wrap on width
  tft.setTextFont(3);
  tft.setTextSize(2);
  adc0 = ads.readADC_SingleEnded(0);
  String modeString = "Calibration Mode!";
   dallasString = String(tempC, 1) + " C, A0:" + String(adc0);
  img.drawString(dallasString, 10,20);
  tft.setTextFont(3);
  if ((tempC >= 75.0) && (tempC <= 75.2)) { 
    temp1 = tempC;
    therm1 = ads.readADC_SingleEnded(0);
  }
  if ((tempC >= 50.0) && (tempC <= 50.2)) { 
    temp2 = tempC;
    therm2 = ads.readADC_SingleEnded(0);
  }
  if ((tempC >= 25.0) && (tempC <= 25.2)) { 
    temp3 = tempC;
    therm3 = ads.readADC_SingleEnded(0);
  }
  if (temp1 > 0) {
     temp1string = String(temp1) + "C = " +String(therm1);
    img.drawString(temp1string, 10,40);
  }
  if (temp2 > 0) {
     temp2string = String(temp2) + "C = " +String(therm2);
    img.drawString(temp2string, 10,60);
  }
  if (temp3 > 0) {
     temp3string = String(temp3) + "C = " +String(therm3);
    img.drawString(temp3string, 10,80);
  }
  if ((temp3 > 0) && (temp2 > 0) && (temp1 > 0)) {
        img.fillSprite(TFT_YELLOW);
        img.setCursor(0,0);
        img.print("Calibration complete!");
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


void setup() {
  DacAudio.StopAllSounds();
  ads.begin();
  ads.setGain(GAIN_ONE);
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
  preferences.end();

  if ((temp3 > 0) && (temp2 > 0) && (temp1 > 0)) {
        thermistor.setTemperature1(temp1 + 273.15);
        thermistor.setTemperature2(temp2 + 273.15);
        thermistor.setTemperature3(temp3 + 273.15);
        thermistor.setResistance1(therm1);
        thermistor.setResistance2(therm2);
        thermistor.setResistance3(therm3);
        thermistor.calcCoefficients();
  }

  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_YELLOW);
  tft.setCursor(0, 0);
  tft.setTextColor(TFT_WHITE, TFT_BLACK, true);
  tft.setTextWrap(true); // Wrap on width
  tft.setTextFont(2);
  tft.setTextSize(2);
  tft.println("Connecting to wifi...");
  WiFi.mode(WIFI_STA);
  WiFiManager wm;
  if ((!digitalRead(button1) && !digitalRead(button2)) || !wm.getWiFiIsSaved()){
  tft.fillScreen(TFT_ORANGE);
  tft.setCursor(0, 0);
  tft.println("WIFI SETTINGS RESET.  PLEASE CONNECT TO 'MR MEAT SETUP' WIFI AP, AND BROWSE TO 192.168.4.1");
  wm.resetSettings();
  }

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
  }
  digitalWrite(PWR_LED_PIN, HIGH);
  initSPIFFS();

  temperatureSensors.begin(NonBlockingDallas::resolution_11, TIME_INTERVAL);
  if (temperatureSensors.getSensorsCount() > 0)
  {
    temp1 = 0;
    temp2 = 0;
    temp3 = 0;
    calibrationMode = true;
    temperatureSensors.onIntervalElapsed(handleIntervalElapsed);
    temperatureSensors.onTemperatureChange(handleTemperatureChange);
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

  tft.fillScreen(TFT_RED);
  tft.setCursor(0, 0);
  tft.println(WiFi.localIP());
  //digitalWrite(MUTE_PIN, HIGH);
  img.setColorDepth(8);
  img.createSprite(239, 239);
  img.fillSprite(TFT_BLUE);
}

void loop() {
  temperatureSensors.update(); 
  every(10000) {       //Update web interface once every 10 seconds
    events.send("ping",NULL,millis());  
    events.send(getSensorReadings().c_str(),"new_readings" ,millis());
  }
  //delay(10);
  //if ((tempC > 76) && (!calibrationMode)) {calibrationMode = true;}
  every(100){
    if (!calibrationMode) {drawTemps();}
    if (calibrationMode) {drawCalib();}
  }
  DacAudio.FillBuffer(); 
  if (!digitalRead(button1)) {
    if(Music.Playing==false) {
      DacAudio.Play(&Music);}
  }


 if (!digitalRead(button2)) {
    if(Sound.Playing==false) {
       DacAudio.Play(&Sound);}
    }
  
  if ((Sound.Playing) || (Music.Playing)) {
    digitalWrite(MUTE_PIN, HIGH); 
  }
  else  {digitalWrite(MUTE_PIN, LOW);}
}
