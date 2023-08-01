
#include <ESP8266WiFi.h>
#include <ezTime.h>
#include <Adafruit_NeoPixel.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
// define pins
#define NEOPIN D4
#define STARTPIXEL 0 // where do we start on the loop? use this to shift the arcs if the wiring does not start at the "12" point
Timezone myTZ;

// Define US or EU rules for DST comment out as required. More countries could be added with different rules in DST_RTC.cpp
const char rulesDST[] = "EU";   // EU DST rules

Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, NEOPIN, NEO_GRB + NEO_KHZ800); // strip object

byte pixelColorRed, pixelColorGreen, pixelColorBlue; // holds color values

// nighttime dimming constants
// brightness based on time of day- could try warmer colors at night?
#define DAYBRIGHTNESS 64
#define NIGHTBRIGHTNESS 20

// cutoff times for day / night brightness. feel free to modify.
#define MORNINGCUTOFF 7  // when does daybrightness begin?   7am
#define NIGHTCUTOFF 22 // when does nightbrightness begin? 10pm



void setup () {
  Serial.begin(115200);
  Serial.println();
  WiFiManager wifiManager; // wifi configuration wizard
  delay(500);
  Serial.println("Start AP");
  wifiManager.autoConnect("NeoPixel_Clock", "secret"); // configuration for the access point, set your own secret.
  Serial.println("WiFi Client connected!)");
  delay(500);
  // set pinmodes
  pinMode(NEOPIN, OUTPUT);
  Serial.println("wait for sync");
  //setDebug(INFO);
  waitForSync();
  Serial.println("time synced");
  if (!myTZ.setCache(0)) myTZ.setLocation(F("nl"));             // set to Dutch time incl DST
  Serial.println(myTZ.dateTime("H:i:s"));
  strip.begin();
  strip.setBrightness(DAYBRIGHTNESS); // set brightness
  // startup sequence
  delay(500);
  colorWipe(strip.Color(255, 0, 0), 20); // Red
  colorWipe(strip.Color(0, 255, 0), 20); // Green
  colorWipe(strip.Color(0, 0, 255), 20); // Blue
  rainbowCycle(20);
  delay(500);
}
boolean chimed;
void loop () {
  byte secondval = myTZ.second();  // get seconds
  byte minuteval = myTZ.minute();  // get minutes
  int hourval = myTZ.hour();   // get hours

  if (minuteval == 0) {
    if (!chimed) {
      rainbowCycle(20);
      chimed = true;
    }
  }
  else chimed = false;
  // change brightness if it's night time
  // check less often, once per minute
  if (secondval == 0) {
    if (hourval < MORNINGCUTOFF || hourval >= NIGHTCUTOFF) {
      strip.setBrightness(NIGHTBRIGHTNESS);
    } else {
      strip.setBrightness(DAYBRIGHTNESS);
    }
  }

  hourval = hourval % 12; // This clock is 12 hour, if 13-23, convert to 0-11`

  hourval = (hourval * 60 + minuteval) / 12; //each red dot represent 24 minutes.

  // arc mode
  for (uint8_t i = 0; i < strip.numPixels(); i++) {

    if (i == secondval) {
      //pixelColorBlue = (i + 1) * (255 / (secondval + 1));// calculates a faded arc from low to maximum brightness
      pixelColorBlue = 255;
      //pixelColorRed = 255;
      //pixelColorBlue = 255;
    }
    else {
      pixelColorBlue = 0;
    }
    
    if (i <= minuteval) {
      pixelColorGreen = (i + 1) * (255 / (minuteval + 1));
      //pixelColorGreen = 255;
    }
    else {
      pixelColorGreen = 0;

    }

    if (i <= hourval) {
      pixelColorRed = (i + 1) * (255 / (hourval + 1));
      //pixelColorRed = 255;
    }
    else {
      pixelColorRed = 0;
    }


    strip.setPixelColor((i + STARTPIXEL) % 60, strip.Color(pixelColorRed, pixelColorGreen, pixelColorBlue));
  }

  //display
  strip.show();
  Serial.println(myTZ.dateTime("H:i:s"));
  delay(500);

}
