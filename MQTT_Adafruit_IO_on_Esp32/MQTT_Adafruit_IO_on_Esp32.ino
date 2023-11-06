#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// Replace with your Wi-Fi and Adafruit IO credentials
#define WIFI_SSID "Evahns"
#define WIFI_PASS "evahns72"
#define AIO_USERNAME "evahns1"
#define AIO_KEY "aio_ZKBG17fgMFTxWImil0xTxvPUNYSt"

const char* AIO_SERVER = "io.adafruit.com";
const int AIO_PORT = 1883;

const int trigPin = 2; //trig pin connection 
const int echoPin = 4; //echopin connection 
long duration; 
float Height = 22.0;
float distanceCm;
// Create an MQTT client instance
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_PORT, AIO_USERNAME, AIO_KEY);

// Define an MQTT feed
Adafruit_MQTT_Publish Moses = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Moses");

void setup() {
  Serial.begin(115200);
  connectToWiFi();
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT); 
  connect();
}

void loop() {
    // ping adafruit io a few times to make sure we remain connected
  if(! mqtt.ping(3)) {
    // reconnect to adafruit io
    if(! mqtt.connected())
      connect();
  }
  // Read your sensor data here (e.g., temperature)
float volume2 = volume(); // Assuming volume() returns a float

char buffer[15]; // Adjust the buffer size based on your float's precision

dtostrf(volume2, 6, 2, buffer); // Converts the float to a string with 6 total width and 2 decimal places


  // Publish the data to Adafruit IO feed
 String volumeString = String(buffer); // Now volumeString contains the float as a string

//Publish the data to Adafruit IO feed using the Adafruit MQTT library
if (Moses.publish(volumeString.c_str())) {
  Serial.println("Published to Adafruit IO: " + String(volume2)); 
  } else {
    Serial.println("Failed to publish to Adafruit IO");
  }

  delay(5000); // Adjust the interval as needed
}


void connect() {
  Serial.print(F("Connecting to Adafruit IO... "));
  int8_t ret;
  while ((ret = mqtt.connect()) != 0) {
    switch (ret) {
      case 1: Serial.println(F("Wrong protocol")); break;
      case 2: Serial.println(F("ID rejected")); break;
      case 3: Serial.println(F("Server unavail")); break;
      case 4: Serial.println(F("Bad user/pass")); break;
      case 5: Serial.println(F("Not authed")); break;
      case 6: Serial.println(F("Failed to subscribe")); break;
      default: Serial.println(F("Connection failed")); break;
    }

    if(ret >= 0)
      mqtt.disconnect();

    Serial.println(F("Retrying connection..."));
    delay(10000);
  }
  Serial.println(F("Adafruit IO Connected!"));
}

void connectToWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

float volume() {
digitalWrite(trigPin, LOW); 
delayMicroseconds(2); 
digitalWrite(trigPin, HIGH); 
delayMicroseconds(10);
digitalWrite(trigPin, LOW); 
duration = pulseIn(echoPin, HIGH);
distanceCm= duration*0.034/2;

// Calculate volume based on the measured water level
float X = (Height - distanceCm);
float currentVolume = 9E-14*pow(X,4) - 1.072*pow (X,3) + 36.985* pow(X,2) + 217.99*X - 52.587;
float volume = (currentVolume/1000);
//delay(10);  
return volume;
}
