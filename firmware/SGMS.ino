#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPL3eLNw581N"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "Kr0hBZ8oYpKFOYuNH1DS4LWiHv7fIF55"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

//  WiFi credentials.
char ssid[] = "tyui";
char pass[] = "starlight";

//  DHT sensor pin and type
#define DHTPIN D2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// soil moisture sensor pin
#define SOIL_MOISTURE_PIN A0

// A timer to send data every 2 seconds
BlynkTimer timer;

// calibrated values here 
int dryValue = 620;  // Raw value when sensor is in dry soil
int wetValue = 320;   // Raw value when sensor is in water

// Alert flags to prevent spam notifications
bool waterAlertSent = false;
bool tempAlertSent = false;

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

  // SOIL MOISTURE ALERTS (Notifications)
  if (soilMoisturePercent < 20 && !waterAlertSent) {
    Blynk.logEvent("critical_alert", " CRITICAL: Soil moisture only " + String(soilMoisturePercent) + "%! Water immediately!");
    waterAlertSent = true;
    Serial.println("CRITICAL water alert sent!");
  }
  else if (soilMoisturePercent < 30 && !waterAlertSent) {
    Blynk.logEvent("water_alert", " REMINDER: Soil moisture at " + String(soilMoisturePercent) + "%. Time to water plant!");
    waterAlertSent = true;
    Serial.println("Water alert sent!");
  }
  else if (soilMoisturePercent > 40 && waterAlertSent) {
    waterAlertSent = false; // Reset alert when moisture improves
    Blynk.logEvent("plant_ok", " PLANT RECOVERED: Soil moisture now " + String(soilMoisturePercent) + "%. Good job!");
    Serial.println("Plant recovered alert sent!");
  }

  //TEMPERATURE ALERTS
  if (t > 35 && !tempAlertSent) {
    Blynk.logEvent("heat_alert", " HEAT WARNING: Temperature " + String(t) + "°C! Move plant to cooler area.");
    tempAlertSent = true;
    Serial.println("Heat alert sent!");
  }
  else if (t > 40 && !tempAlertSent) {
    Blynk.logEvent("critical_heat", "CRITICAL HEAT: " + String(t) + "°C! Plant needs immediate attention!");
    tempAlertSent = true;
    Serial.println("Critical heat alert sent!");
  }
  else if (t < 30 && tempAlertSent) {
    tempAlertSent = false; // Reset alert when temperature normalizes
    Blynk.logEvent("temp_normal", " TEMPERATURE NORMAL: Now " + String(t) + "°C. Plant is safe.");
    Serial.println("Temperature normal alert sent!");
  }

  // Print all values to the Serial Monitor
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
