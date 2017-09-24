#include <FS.h>
#include <ArduinoHttpClient.h> //v0.3.0

#include <TimeLib.h>              https://github.com/PaulStoffregen/Time

#include <ESP8266WiFi.h>          //http://esp8266.github.io/Arduino/versions/2.3.0/
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //https://github.com/kentaylor/WiFiManager 
#include <ESP8266HTTPClient.h>

#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson

//define your default values here
// change test
char server[40];
char port[6];

//flag for saving data
bool shouldSaveConfig = false;

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}


int lowPin = 5;    // D1
int highPin = 2;   // D4
int outPutPin = 4;  // D2


void setup() {

  Serial.begin (115200);

  //clean FS, for testing
  //SPIFFS.format();

  //FILE SYSTEM START
   //read configuration from FS json
   Serial.println("mounting FS...");
   
     if (SPIFFS.begin()) {
       Serial.println("mounted file system");
       if (SPIFFS.exists("/config.json")) {
         //file exists, reading and loading
         Serial.println("reading config file");
         File configFile = SPIFFS.open("/config.json", "r");
         if (configFile) {
           Serial.println("opened config file");
           size_t size = configFile.size();
           // Allocate a buffer to store contents of the file.
           std::unique_ptr<char[]> buf(new char[size]);
   
           configFile.readBytes(buf.get(), size);
           DynamicJsonBuffer jsonBuffer;
           JsonObject& json = jsonBuffer.parseObject(buf.get());
           json.printTo(Serial);
           if (json.success()) {
             Serial.println("\nparsed json");
   
             strcpy(server, json["server"]);
             strcpy(port, json["port"]);
   
           } else {
             Serial.println("failed to load json config");
           }
         }
       }
     } else {
       Serial.println("failed to mount FS");
     }
     //end read

    WiFiManagerParameter custom_server("server", "server", server, 40);
    WiFiManagerParameter custom_port("port", "port", port, 6);

    //FILE SYSTEM END

  //WiFi.disconnect();  //remove this before sending to Hal
  
  WiFiManager wifiManager; // init wifi manager

  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  //add all your parameters here
  wifiManager.addParameter(&custom_server);
  wifiManager.addParameter(&custom_port);

  
  wifiManager.autoConnect("h2oSensor", "h2osensor");
  Serial.println("Connected..");
  delay(3000);

  //read updated parameters
  strcpy(server, custom_server.getValue());
  strcpy(port, custom_port.getValue());

  //save the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["server"] = server;
    json["port"] = port;

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }

    json.printTo(Serial);
    json.printTo(configFile);
    configFile.close();
    //end save
  }


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

  char *_server = strcpy(server, custom_server.getValue());
  char *_port = strcpy(port, custom_port.getValue());

  std::string s(_server);
  
  Serial.print("Server: ");
  Serial.println(_server);
  Serial.print("Port: ");
  Serial.println(_port);
  
    
  Serial.println("[HTTP] begin...\n");
  http.begin(_server,3333,"/");
  http.addHeader("Content-Type", "application/json");
  http.POST(json);
  http.end();
 
}

void loop() {

  timeStatus();
  
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
