#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include "DHT.h"
#define DHTTYPE DHT11
int soilWet=14; //D5=GPIO14
int soilDry=12; //D6=GPIO12
int Sense_Pin=0;
int val=0;
const int DHTpin = 5; //D1=GPIO5
DHT dht(DHTpin, DHTTYPE);
#define ON_Board_LED 2
const char* Host = "script.google.com";
const int HttpsPort = 443;
WiFiClientSecure client;
String GAS_ID =
"AKfycbxdkbojel0QrcWbzco2al-Tt0VpUQylrT6gk88r30ts3RWOQlqbtduFZMIbUn7Yi
Ws1TA"; //--> spreadsheet script ID
void setup() {
Serial.begin(115200);
delay(500);
pinMode(soilWet,OUTPUT);
pinMode(soilDry,OUTPUT);
delay(500);
dht.begin();
delay(500);
WiFi.begin(ssid, password);
Serial.println("");
pinMode(ON_Board_LED,OUTPUT);
digitalWrite(ON_Board_LED, HIGH);
Serial.print("Connecting");
while (WiFi.status() != WL_CONNECTED) {
Serial.print(".");
digitalWrite(ON_Board_LED, LOW);
delay(250);
digitalWrite(ON_Board_LED, HIGH);
delay(250);
}
digitalWrite(ON_Board_LED, HIGH);
Serial.println("");
Serial.print("Successfully connected to : ");
Serial.println(ssid);
Serial.print("IP address: ");
Serial.println(WiFi.localIP());
Serial.println();
client.setInsecure();
}
void loop() {
int hu = dht.readHumidity();
float te = dht.readTemperature();
if (isnan(hu) || isnan(te)) {
Serial.println("Failed to read from DHT sensor !");
delay(500);
return; }
String Temp = "Temperature : " + String(te) + " °C";
String Humi = "Humidity : " + String(hu) + " %";
Serial.println(Temp);
Serial.println(Humi);
sendData(te, hu);
}
void sendData(float tem, int hum) {
Serial.println("==========");
Serial.print("connecting to ");
Serial.println(Host);
if (!client.connect(Host, HttpsPort)) {
Serial.println("connection failed");
return;
}
String string_temperature = String(tem);
// String string_temperature = String(tem, DEC);
String string_humidity = String(hum, DEC);
String url = "/macros/s/" + GAS_ID + "/exec?temperature=" + string_temperature +
"&humidity=" + string_humidity;
Serial.print("requesting URL: ");
Serial.println(url);
client.print(String("GET ") + url + " HTTP/1.1\r\n" +
"Host: " + Host + "\r\n" +
"User-Agent: BuildFailureDetectorESP8266\r\n" +
"Connection: close\r\n\r\n");
Serial.println("request sent");
while (client.connected()) {
String line = client.readStringUntil('\n');
if (line == "\r") {
Serial.println("headers received");
break;
}
}
String line = client.readStringUntil('\n');
if (line.startsWith("{\"state\":\"success\"")) {
Serial.println("esp8266/Arduino CI successfull!");
} else {
Serial.println("esp8266/Arduino CI has failed");
}
Serial.print("reply was : ");
Serial.println(line);
Serial.println("closing connection");
Serial.println("==========");
Serial.println();
delay(100);
Serial.print("MOISTURE LEVEL : ");
val= analogRead(Sense_Pin);
val= value/10;
Serial.println(val);
if(val>70)
{
digitalWrite(soilWet,LOW);
digitalWrite(soilDry,HIGH);
}
else
{
digitalWrite(soilWet,HIGH);
digitalWrite(soilDry,LOW);
}
delay(200);
}

