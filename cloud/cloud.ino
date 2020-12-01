#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ESP8266HTTPClient.h>
#include <FastLED.h>

#define NUM_LEDS 10
#define DATA_PIN D5

CRGB leds[NUM_LEDS];
int weatherstatus;

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  Alloff();
  WiFiManager wifiManager;
  wifiManager.setTimeout(10);
   
  if(!wifiManager.autoConnect("WeatherCloud")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again
    ESP.reset();
  }
}

void getURL(String url) {
  WiFiClient client = server.available();
 
  HTTPClient http;
  const int httpPort = 80;

  http.begin(url);

  int httpCode = http.GET();
  weatherstatus = http.getString().toInt();
  http.end();
  
  switch(weatherstatus) {
    case 1: // Thunder
      Thunder();
      break;
    case 2: // Drizzle
      Rain();
      break;
    case 3: // Rain
      Rain();
      break;
    case 4: // Snow
      Snow();
      break;
    case 5: // Misty/fog
      Cloudy();
      break;
    case 6: // Clear day
      ClearDay();
      break;
    case 7: // Clear night
      ClearNight();
      break;
    case 8: // Cloudy
      Cloudy();
      break;
    case 9: // Demo
      Serial.println("Demo");
      Thunder();
      Alloff();
      Rain();
      Alloff();
      Snow();
      Alloff();
      Cloudy();
      delay(5000);
      Alloff();
      ClearDay();
      delay(5000);
      Alloff();
      ClearNight();
      delay(5000);
      Alloff();
      break;      
    case 10: // Nightime - all lights off
      Alloff();
      break;
  }
}

void Alloff() {
  int ledctr;
  int fadectr;
  for(fadectr=255;fadectr>=0;fadectr--) {
    FastLED.setBrightness(fadectr);
    FastLED.show();
    delay(2);
  }

  // Set all LEDs off (I think this saves power, rather than just leaving brightness at 0)
  for(ledctr=0;ledctr<10;ledctr++) {
    leds[ledctr]=CRGB::Black;
    FastLED.show();
  }
}

void Thunder() {
  int ledctr;
  int flickerctr;

  Serial.println("Thunder");
  
  // Seed random num gen here, rather than on each call of GetRND
  randomSeed(analogRead(0));
  
  for(ledctr=0;ledctr<10;ledctr++) {
    setLED(ledctr,75,0,240);
  }
  
  FadeUp();

  for(flickerctr=0;flickerctr<=10;flickerctr++) {
    Flicker(9,4,255,255,255,50,50,random(500,2000));
  }
}

void Rain() {
  int ledctr;
  int flickerctr;

  Serial.println("Rain");
  
  // Seed random num gen here, rather than on each call of GetRND
  randomSeed(analogRead(0));
  
  for(ledctr=0;ledctr<10;ledctr++) {
    setLED(ledctr,0,0,255);
  }
  
  FadeUp();

  for(flickerctr=0;flickerctr<=100;flickerctr++) {
    Flicker(5,1,0,0,0,50,150,0);
  }
}

void Snow() {
  int ledctr;
  int flickerctr;

  Serial.println("Snow");
  
  // Seed random num gen here, rather than on each call of GetRND
  randomSeed(analogRead(0));
  
  for(ledctr=0;ledctr<10;ledctr++) {
    setLED(ledctr,128,128,128);
  }
  
  FadeUp();

  for(flickerctr=0;flickerctr<=100;flickerctr++) {
    Flicker(5,1,255,255,255,150,250,0);
  }
}

void Cloudy() {
  int ledctr;
  int fadectr;

  Serial.println("Cloudy");
  
  for(ledctr=0;ledctr<10;ledctr++) {
    setLED(ledctr,248,246,168);
  }
  FadeUp();
}

void ClearDay() {
  int ledctr;
  int fadectr;

  Serial.println("Clear day");
  
  for(ledctr=0;ledctr<10;ledctr++) {
    setLED(ledctr,255,255,0);
  }
  FadeUp();
}

void ClearNight() {
  int ledctr;
  int fadectr;

  Serial.println("Clear night");
  
  for(ledctr=0;ledctr<10;ledctr++) {
    setLED(ledctr,108,20,184);
  }
  FadeUp();
}

int GetRND(int retvalue) {
  return random(0,retvalue);
}

void FadeUp() {
  int fadectr;
  if (FastLED.getBrightness()!=255) {
    for(fadectr=0;fadectr<256;fadectr++) {
      FastLED.setBrightness(fadectr);
      FastLED.show();
      delay(2);
    }
  }
  FastLED.show();
}

void Flicker(int lednum, int howmany, int flickercolR, int flickercolG, int flickercolB, int speedL, int speedH, int afterDelay) {
int currcolR=leds[lednum].r;
int currcolG=leds[lednum].g;
int currcolB=leds[lednum].b;
int howmanyctr;
int ledctr;

  for(howmanyctr=0;howmanyctr<=howmany;howmanyctr++) {
    setLED(random(lednum),flickercolR,flickercolG,flickercolB);
  }
  FastLED.show();
  delay(random(speedL,speedH));
  for(ledctr=0;ledctr<10;ledctr++) {
    setLED(ledctr,currcolR,currcolG,currcolB);
  }
  FastLED.show();
  delay(afterDelay);
}

int setLED(int lednum, int r, int g, int b) {
  leds[lednum].r=r;
  leds[lednum].g=g;
  leds[lednum].b=b;
}

void loop() {
  getURL("http://192.168.0.27/weather/get_cond.php");
  delay(120000);
}
