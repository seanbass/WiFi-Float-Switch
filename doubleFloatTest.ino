#include <Time.h>
#include <TimeLib.h>

#include <ArduinoHttpClient.h>
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>  
#include <ESP8266HTTPClient.h>


int lowPin = 5;    // D1
int highPin = 2;   // D4
int outPutPin = 4;  // D2


void setup() {

  Serial.begin (115200);


  WiFi.disconnect();  //remove this before sending to Hal
  
  WiFiManager wifiManager; // init wifi manager
  //wifiManager.resetSettings();
  //WiFiManagerParameter custom_text("<p>Create a text box for server IP here</p>");
  //wifiManager.addParameter(&custom_text);

  //IPAddress _ip = IPAddress(108, 161, 21, 27);
  //IPAddress _gw = IPAddress(10, 0, 1, 1); //gateway IP
  //IPAddress _sn = IPAddress(255, 255, 255, 0); //serial number?

  //wifiManager.setSTAStaticIPConfig(_ip, _gw, _sn);
  
  wifiManager.autoConnect("h2oSensor", "h2osensor");
  Serial.println("Connected..");
  delay(3000);


  pinMode (lowPin, INPUT_PULLUP);
  pinMode (highPin, INPUT_PULLUP);
  
  pinMode (outPutPin, OUTPUT);


  digitalWrite(outPutPin, 0);

  Serial.print("Water High ");
  int highInput = digitalRead(highPin);
  Serial.println(highInput);
  
  Serial.print("Water Low ");
  int input = digitalRead(lowPin);
  Serial.println(input);

  Serial.print("Output ");
   int output = digitalRead(outPutPin);
  Serial.println(output);
  
  

  Serial.setTimeout(2000);

  // Wait for serial to initialize.
  while(!Serial) { }
  Serial.println("I'm awake.");
 
}

void loop() {

  int lowLevel = digitalRead (lowPin);
  int highLevel = digitalRead (highPin);

  //String date = "";

  //String d_id = "";

  String low = "\"Low\": ";
  low += lowLevel;
  low.concat (", ");

  String high = "\"High\": ";
  high += highLevel;

  String json = "{";
  json += low += high += "}";
  Serial.println(json);
   
   
  //Setup HTTP Client - Send sensor data to server
  
  HTTPClient http;
    
  Serial.println("[HTTP] begin...\n");
  http.begin("192.168.0.108",3333,"/");
  http.addHeader("Content-Type", "application/json");
  http.POST(json);
  http.end();

  
  if (digitalRead (lowPin) == LOW)
  {Serial.println ("Low Open");}
  else 
  {Serial.println ("Low Closed");}


  if (digitalRead (highPin) == LOW)
  {Serial.println ("High Open");}
  else 
  {Serial.println ("High Closed");}
  


  digitalWrite (outPutPin, LOW);
  Serial.println(digitalRead(outPutPin));
  delay(1000);


  
  Serial.println("Going into deep sleep for 10 seconds"); 
  ESP.deepSleep(10e6);
  delay(1000);

}
