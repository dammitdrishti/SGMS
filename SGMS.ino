#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPL3eLNw581N"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "Kr0hBZ8oYpKFOYuNH1DS4LWiHv7fIF55"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

// Your Blynk cloud credentials

// Your WiFi credentials.
char ssid[] = "tyui";
char pass[] = "starlight";

// Define the DHT sensor pin and type
#define DHTPIN D2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Define the soil moisture sensor pin
#define SOIL_MOISTURE_PIN A0

// A timer to send data every 2 seconds
BlynkTimer timer;

// Define your calibrated values here (fill these in after running the calibration)
int dryValue = 620;  // Raw value when sensor is in dry soil
int wetValue = 320;   // Raw value when sensor is in water

// This function reads sensors and sends data to Blynk
void sendSensorData() {
  // Read Soil Moisture
  int soilValue = analogRead(SOIL_MOISTURE_PIN);
  int soilMoisturePercent = map(soilValue, dryValue, wetValue, 0, 100);
  soilMoisturePercent = constrain(soilMoisturePercent, 0, 100);
  Blynk.virtualWrite(V0, soilMoisturePercent); // Send to Virtual Pin V0

  // Read Temperature and Humidity
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Blynk.virtualWrite(V1, t); // Send temp to Virtual Pin V1
  Blynk.virtualWrite(V2, h); // Send humidity to Virtual Pin V2

  // Simple alert
  if (soilMoisturePercent < 30) {
    // Send a Blynk event instead of a direct notification
    Blynk.logEvent("plant_needs_water");
    Serial.println("Plant is too dry! Event sent.");
  }

  // Print all values to the Serial Monitor (for debugging)
  Serial.print("Soil Moisture: ");
  Serial.print(soilMoisturePercent);
  Serial.print("%, Temp: ");
  Serial.print(t);
  Serial.print("°C, Humidity: ");
  Serial.println(h);
}

// Function to assist with sensor calibration
void calibrateSensors() {
  Serial.println("--- SENSOR CALIBRATION ---");
  Serial.println("Place the sensor in dry soil and note the value.");
  Serial.println("Then place it in water and note the value.");
  Serial.println("Update the 'dryValue' and 'wetValue' variables in the code.");
  Serial.println("--------------------------");
}

void setup() {
  // Start the Serial monitor for debugging
  Serial.begin(9600);
  // Start the DHT sensor
  dht.begin();
  // Call the calibration function
  calibrateSensors();
  // Connect to Blynk and WiFi
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  // Setup a function to run every 2000 milliseconds (2 seconds)
  timer.setInterval(2000L, sendSensorData);
}

void loop() {
  // Run Blynk and the timer
  Blynk.run();
  timer.run();
}