#include <WiFiNINA.h>  // Include the library for WiFi support on Arduino Nano 33 IoT
#include <BH1750.h>    // Include the library for the BH1750 light sensor
#include <Wire.h>      // Include the library for I2C communication, used by BH1750

// WiFi credentials
const char* ssid     = "Redmi 12 5G";      //  WiFi network name
const char* password = "varsha0654";  //  WiFi network password

// IFTTT Webhook settings
const char* host = "maker.ifttt.com";    // IFTTT server address
const char* event = "sunlight_hit";    // Event name to trigger on IFTTT
const char* key = "n4LeNWblS78T5cGpV-ALEi4zTC9062PM0NPvMbmNqRx";      // IFTTT Webhook key
const int httpsPort = 80;                // HTTP port (use 443 for HTTPS)

// Light sensor
BH1750 lightMeter;  // Create an instance of the BH1750 light sensor

// Control variables
bool sunlightCurrently = false;  // Flag to track current sunlight status
bool lastSunlightState = false;  // Flag to track last sunlight status for change detection

void setup() {
  Serial.begin(9600);  // Start serial communication at 9600 baud rate
  Wire.begin();        // Initialize the I2C communication
  lightMeter.begin();  // Initialize the BH1750 light sensor

  // Connect to WiFi
  WiFi.begin(ssid, password);  // Start connecting to WiFi network
  while (WiFi.status() != WL_CONNECTED) {  // Wait until connected
    delay(500);  // Wait for 500 milliseconds
    Serial.print(".");  // Print dots on the serial monitor to indicate waiting
  }
  Serial.println("Connected to WiFi");  // Print connection status
}

void loop() {
  float lux = lightMeter.readLightLevel();  // Read light level in lux from the BH1750 sensor
  Serial.print("\nLight: ");
  Serial.print(lux);
  Serial.println(" lux");  // Print the light level to the serial monitor

  sunlightCurrently = (lux > 1000);  // Check if the current light level exceeds the sunlight threshold

  // Check if the sunlight exposure status has changed
  if (sunlightCurrently != lastSunlightState) {
    if (sunlightCurrently) {
      Serial.println("Sunlight started.");
      sendNotification("Sunlight started");  // Send notification for sunlight start
    } else {
      Serial.println("Sunlight stopped.");
      sendNotification("Sunlight stopped");  // Send notification for sunlight stop
    }
    lastSunlightState = sunlightCurrently;  // Update the last known state
  }

  delay(60000);  // Delay for 1 minute before next check
}

void sendNotification(String message) {
  WiFiClient client;  // Create a WiFiClient to send data
  if (client.connect(host, httpsPort)) {  // Connect to the IFTTT Webhook URL
    String jsonPayload = "{\"value1\":\"" + message + "\"}";  // Create JSON payload
    Serial.println("Sending notification...");

    // Start forming the HTTP POST request
    client.println("POST /trigger/" + String(event) + "/with/key/" + String(key) + " HTTP/1.1");
    client.println("Host: " + String(host));
    client.println("Content-Type: application/json");
    client.println("Content-Length: " + String(jsonPayload.length()));
    client.println();
    client.print(jsonPayload);

    // Wait for the server's response
    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        Serial.println("Headers received");  // Indicate headers have been received
        break;
      }
    }
    while (client.available()) {
      char c = client.read();  // Read the response body
      Serial.write(c);
    }
    client.stop();  // Stop the client
  } else {
    Serial.println("Connection to IFTTT failed.");  // Error if connection failed
  }
}


