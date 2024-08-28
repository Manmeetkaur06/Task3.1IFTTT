#include <Wire.h>           // Include the Wire library for I2C communication
#include <BH1750.h>         // Include the BH1750 library for using the light sensor
#include <WiFiNINA.h>       // Include the WiFiNINA library for WiFi capabilities

// WiFi network credentials
const char* ssid = "Redmi 12 5G";        // WiFi SSID (network name)
const char* password = "varsha0654";     // WiFi password

// IFTTT Webhook settings
const char* host = "maker.ifttt.com";            // Hostname for the IFTTT Webhook
const char* eventSunlightHit = "sunlight_hit";   // Event name to trigger on IFTTT
const char* apiKey = "n4LeNWblS78T5cGpV-ALEi4zTC9062PM0NPvMbmNqRx";  // API Key for IFTTT Webhook

BH1750 lightMeter;  // Create an instance of the BH1750 light sensor

void setup() {
  Serial.begin(9600);   // Start serial communication at 9600 baud
  Wire.begin();         // Initialize the I2C communication
  lightMeter.begin();   // Initialize the BH1750 light sensor

  // Initialize WiFi
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);  // Connect to the WiFi network
  while (WiFi.status() != WL_CONNECTED) {  // Wait until connected
    delay(500);  // Delay to allow time for connecting
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");  // Print a confirmation once connected
}

void loop() {
  float lux = lightMeter.readLightLevel();  // Read the light level in lux from the BH1750 sensor
  Serial.println("Light: " + String(lux) + " lux");  // Print the light level to the serial monitor

  if (lux > 1000) {  // Check if the light level exceeds 1000 lux
    sendNotification(eventSunlightHit, lux);  // Send a notification if the light level is above the threshold
  }

  delay(10000);  // Delay for 10 seconds before the next reading
}

void sendNotification(const char* eventName, float lux) {
  WiFiClient client;  // Create a WiFi client to send data
  const int httpPort = 80;  // HTTP port to use for the connection
  if (!client.connect(host, httpPort)) {  // Attempt to connect to the server
    Serial.println("Connection failed");  // Print an error message if the connection fails
    return;
  }

  // Construct the URL for the GET request
  String url = "/trigger/";
  url += eventName;
  url += "/with/key/";
  url += apiKey;
  url += "?value1=" + String(lux);  // Append the light level as a query parameter

  // Send the HTTP GET request
  client.println("GET " + url + " HTTP/1.1");
  client.println("Host: " + String(host));
  client.println("Connection: close");
  client.println();  // End of headers

  unsigned long timeout = millis();  // Timeout for the response
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {  // Check if no response is received within 5 seconds
      Serial.println("Client Timeout !");
      client.stop();  // Stop the client if timeout
      return;
    }
  }

  // Read and print the server response
  while (client.available()) {
    String line = client.readStringUntil('\r');  // Read each line from the response
    Serial.print(line);  // Print the line to the serial monitor
  }
}

