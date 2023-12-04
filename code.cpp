
//-------------Blynk Cloud Authentication-------//
#define BLYNK_TEMPLATE_ID "TMPL6iFDUxRtU"
#define BLYNK_TEMPLATE_NAME "utrack"
#define BLYNK_AUTH_TOKEN "qJdWD6zZeHx5EHsg38nluUTAW6udTjyD"
//---------------------------------------------//
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <AsyncTCP.h>
#include <BlynkSimpleEsp32.h>

//------------Sensors Pinouts------------------//
const int acs712Pin = 32; // Use GPIO32 as the analog input pin for ACS712 sensor
const float voltageReference = 3.3; // Replace with the actual reference voltage of your ESP32
const float ACS712_SENSITIVITY = 0.66; // 66 mV/A converted to volts per ampere (V/A)
const int yfs201Pin = 12; // Use GPIO27 as the input pin for the YFS201 flow sensor


//--------Global Variables for sensors---------//
float totalFlow = 0;
float totalPower = 0;
volatile long waterFlow;

//-----------WiFi connection Segment-----------//
const char* auth = BLYNK_AUTH_TOKEN;
const char* ssid = "Shafin";
const char* password = "Hideyourself1";

WiFiClientSecure client;
BlynkTimer timer;

void myTimer() {
  Blynk.virtualWrite(V0, totalPower);
  Blynk.virtualWrite(V1, waterFlow);
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  attachInterrupt(digitalPinToInterrupt(yfs201Pin), pulse, RISING);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Blynk.begin(auth, ssid, password);
  timer.setInterval(1000L, myTimer); 
}

void loop() {
  ///-------- ACS712 Current Flow Sensor ---------///
  int sensorValue = analogRead(acs712Pin);
  float voltage = (sensorValue * voltageReference) / 4095.0;
  float current = voltage / ACS712_SENSITIVITY; 
  float power = (voltage * current) / 1000.0;
  totalPower = totalPower + power;
  Serial.print("Electricity: ");
  Serial.print(totalPower, 2); 
  Serial.println(" W\t"); 
  int pulseCount = 0;

  ///-------- YFS201 Water Flow Sensor -----------///
  Serial.print("Water Flow: ");
  Serial.print(totalFlow, 2); // Display two decimal places
  Serial.println(" L");
  
  Blynk.run(); 
  timer.run();
  delay(3000); 
}

void pulse() {
  waterFlow += 1.0 / 450.0;
}

