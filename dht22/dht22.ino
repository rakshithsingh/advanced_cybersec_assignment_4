
#include<WiFi.h>
#include<AWS_IOT.h>
#include<DHT.h>
#include<ArduinoJson.h>
#define DHT_PIN 33 // pin connected to data pin of DHT11
#define DHT_TYPE DHT22  // Type of the DHT Sensor, DHT11/DHT22
#define WIFI_SSID "Wifi_only_for_babez" // your wifi ssid
#define WIFI_PASSWD "qwerty123" //your wifi password

#define CLIENT_ID "dth22"// thing unique ID, can be any unique id
#define MQTT_TOPIC "$aws/things/dht22/shadow/update" //topic for the MQTT data
#define AWS_HOST "a372gs4nmnvhgr-ats.iot.us-east-1.amazonaws.com" // your host for uploading data to AWS,

AWS_IOT aws;
DHT dht(DHT_PIN, DHT_TYPE);
void setup(){
  Serial.begin(9600);
  Serial.print("\nInitializing thing Temp_Humidity_DHT11_0 \n");

  Serial.print("\n  Initializing WIFI: Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWD);
  Serial.print("  ");
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }
  Serial.println("\n  Connected.\n  Done");

  Serial.print("\n  Initializing DHT22...");
  dht.begin();
  Serial.println("  Done.");

  Serial.println("\n  Initializing connetction to AWS....");
  if(aws.connect(AWS_HOST, CLIENT_ID) == 0){ // connects to host and returns 0 upon success
    Serial.println("  Connected to AWS\n  Done.");
  }
  else {
    Serial.println("  Connection failed!\n make sure your subscription to MQTT in the test page");
  }
  Serial.println("  Done.\n\nDone.\n");
}

void loop(){
  // read temperature and humidity
  float temp = dht.readTemperature(); // return temperature in °C
  float humidity = dht.readHumidity();// return humidity in %
  DynamicJsonDocument jsonBuffer(1024);
  JsonObject root = jsonBuffer.to<JsonObject>();
  JsonObject state = root.createNestedObject("state");
  JsonObject state_reported = state.createNestedObject("reported");
  state_reported["Tempreture"] = String(temp);
  state_reported["Humidity"] = String(humidity);
  serializeJson(root, Serial);
  Serial.println();
  char shadow[measureJson(root) + 1];
  serializeJson(root, shadow, sizeof(shadow));
  aws.publish(MQTT_TOPIC, shadow);
  delay(7000);
}
