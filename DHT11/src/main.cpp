#include <Arduino.h>
#include <ThingSpeak.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <WiFi.h>

#define DHTTYPE DHT11
#define DHTPIN 4
DHT_Unified dht(DHTPIN, DHTTYPE);

WiFiClient wificlient;

const char* ssid = "Hocvienso1Hadong";
const char* password = "dua500nghinday";

unsigned long ChannelNumber = 1;
const char* APIKey = "NY4JUG4OTL9GBOVR";

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("\nConnecting to Wifi network ..");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(100);
  }
  Serial.println("\nConnected");
  Serial.println("Local ESP32 IP: ");
  Serial.println(WiFi.localIP());
  dht.begin();
  ThingSpeak.begin(wificlient);
}

void loop() {
  
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature))
  {
    Serial.println(F("Error reading temperature!"));
  }else
  {
    Serial.println(F("Temperature: "));
    Serial.println(event.temperature);
    Serial.println(F("℃"));
    int x = ThingSpeak.writeField(ChannelNumber,1,event.temperature,APIKey);
    if (x==200)
    {
      Serial.println("\nChannel updated!");
    }
    
  }
  delay(1000);

  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity))
  {
    Serial.println(F("Error reading humidity!"));
  }else
  {
    Serial.println(F("Humidity: "));
    Serial.println(event.relative_humidity);
    Serial.println(F("%"));
    int y = ThingSpeak.writeField(ChannelNumber,2,event.relative_humidity,APIKey);
    if (y==200)
    {
      Serial.println("\nChannel updated!");
    }
  }
  delay(1000);
  
  
}

