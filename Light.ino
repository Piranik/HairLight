
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include "FastLED.h"

#include "FS.h"

#define NUM_LEDS 9
#define DATA_PIN D3
#define CLOCK_PIN D2

/* Set these to your desired credentials. */
const char *ssid = "DiscoHair";
const char *password = "brownlywedding";

String color = "";
int brightness = 50;
String type = "snow";
int onoff = 1;

long red;
long green;
long blue;

int tick = 0;

const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;


CRGB leds[NUM_LEDS];

ESP8266WebServer server(80);

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
 * connected to this access point to see it.
 */
void handleRoot() {

  String returnHtml = "<script type=\"text/javascript\">"
  "function brightness(value){"
  "    var xmlHttp = new XMLHttpRequest(); alert(value);"
  "    xmlHttp.open( \"POST\", \"http://192.168.4.1/change?brightness=\"+value, false );"
  "    xmlHttp.send( null );"
  "    alert(xmlHttp.responseText);"
  "}"
  "  function color(value) {"
  "    value = value.replace(\"#\", \"\");"
  "    var xmlHttp = new XMLHttpRequest();"
  "    xmlHttp.open( \"POST\", \"http://192.168.4.1/change?color=\"+value, false );"
  "    xmlHttp.send( null );"
  "    return xmlHttp.responseText;"
  "  }"
  "  function changeType(typeName) {"
  "    var xmlHttp = new XMLHttpRequest();"
  "    xmlHttp.open( \"POST\", \"http://192.168.4.1/change?type=\"+typeName, false );"
  "    xmlHttp.send( null );"
  "    return xmlHttp.responseText;"
  "  }"
  "</script>"
  "<style type=\"text/css\">"
  "  body, html{"
  "    font-family:Helvetica;"
  "  }"
  "</style>"
  "Brightness:"
  "<input type=\"range\" min=\"0\" max=\"100\" step=\"1\" id=\"brightness\" onchange=\"brightness(this.value);\" /><br />"
  "Color: <input type=\"color\" id=\"ledcolor\" onchange=\"color(this.value);\" /><br />"
  "<input type=\"button\" id=\"snow\" value=\"Snow\" onclick=\"changeType('snow');\" /><br />"
  "<input type=\"button\" id=\"candycane\" value=\"candycane\" onclick=\"changeType('candycane');\" /><br />"
  "<input type=\"button\" id=\"christmascombo\" value=\"christmascombo\" onclick=\"changeType('christmascombo');\" /><br />"
  "<input type=\"button\" id=\"colormode\" value=\"Color Mode\" onclick=\"changeType('color');\" /><br />";

  server.send(200, "text/html", returnHtml);
  
}



void handleChange() {
  for (uint8_t i=0; i<server.args(); i++){
    Serial.println(server.argName(i));
    Serial.println(server.arg(i));
    
    if (server.argName(i) == "color") {
      
      color = "#" + server.arg(i);

      long number = strtol( &color[1], NULL, 16);
  
      // Split them up into r, g, b values
      red = number >> 16;
      green = number >> 8 & 0xFF;
      blue = number & 0xFF;
      Serial.println(red);
      Serial.println(green);
      Serial.println(blue);  
    }

    if (server.argName(i) == "brightness") {
      brightness = server.arg(i).toInt();
    }

    if (server.argName(i) == "type") {
      type = server.arg(i);
    }
    
    if (server.argName(i) == "onoff") {
      onoff = server.arg(i).toInt();
    }

    server.send(200, "text/html", "ok");
  }
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);

  server.on("/change", handleChange);
  server.begin();
  Serial.println("HTTP server started");

  FastLED.addLeds<WS2801, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);

  SPIFFS.begin();
  
}

void loop() {

  server.handleClient();

  FastLED.setBrightness(brightness);

  if (type == "color") {
    for(int i = 0; i < NUM_LEDS; i++) {
      leds[i].setRGB( red, green, blue);
    }
  } else if (type == "snow") {
    for(int i = 0; i < NUM_LEDS; i++) {
      leds[i].setRGB( 0,0,0);
    }
    int target = (int)tick/25;
    leds[8-target].setRGB( 255,255,255);
  } else if (type == "christmascombo") {
    for(int i = 0; i < NUM_LEDS; i++) {
      if ((i+1)%2 == 0) {
        leds[i].setRGB( 255,0,0);
      } else {
        leds[i].setRGB( 0,255,0);
      }
    }
  } else if (type == "candycane") {
    for(int i = 0; i < NUM_LEDS; i++) {
      if ((i+1)%2 == 0) {
        leds[i].setRGB( 255,0,0);
      } else {
        leds[i].setRGB( 125,125,125);
      }
    }
  }
  FastLED.show();
  delay(5);

  tick++;

  if (tick == 224) {
    tick = 0;
  }
}
