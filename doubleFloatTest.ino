#include <ArduinoHttpClient.h>

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>  



int lowPin = 5;    // D1
int highPin = 2;   // D4
int outPutPin = 4;  // D2


char serverAddress[] = "http://192.168.0.108";  // server address
int port = 3333;

HttpClient client = HttpClient(serverAddress, port);
int status = WL_IDLE_STATUS;
String response;
int statusCode = 0;






void setup() {
  Serial.begin (115200);





  //HTTPClient http;
  //http.begin("http://192.168.0.108:3333");
  //http.addHeader("Content-Type", "application/json");
  //http.POST("data: etc..");
  //http.writeToStream(&Serial);
  //http.end();


  

  //WiFi.disconnect();  //remove this before sending to Hal
  
  WiFiManager wifiManager; // init wifi manager
  wifiManager.autoConnect("AutoConnectAP");
  Serial.println("Connected..");

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

  ///////////////

  Serial.println("making POST request");
  String contentType = "Content-Type", "application/json";
  String postData = "{'name': 'Sean'}";

  client.post("/", contentType, postData);

  // read the status code and body of the response
  statusCode = client.responseStatusCode();
  response = client.responseBody();

  Serial.print("Status code: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);

  Serial.println("Wait five seconds");
  delay(5000);


  ///////////////
  
  Serial.println("Going into deep sleep for 10 seconds"); 
  ESP.deepSleep(10e6);
  delay(1000);

}
