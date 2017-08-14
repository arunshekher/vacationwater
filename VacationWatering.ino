/**
 * Copyright (C) 2016 Arun S. Sekher <arunshekher@users.noreply.github.com>
 *
 * This file is part of Arduino Vacation Watering Automation Project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 *
 * @version  0.2.3
 * @link  <https://github.com/arunshekher/vacationwatering>
 * @license  <https://www.gnu.org/licenses/agpl.html>
 */

 /* 
  *  Vacation Watering v0.2.3
  *  
  *  Simple Arduino sketch that automates the task of turning on and off two 
  *  DC water pumps via two 5V DC relays based on elapsed time and soil 
  *  hygrometer sensor reading.
  *  
  *  06 Aug 2016
  */
const int pumpOnePin = 13;     // pump-1 Relay Pin
const int pumpTwoPin = 8;      // pump-2 Relay Pin
const int sensorOnePin = A0;   // moisture Sensor-1
const int sensorTwoPin = A1;   // moisture Sensor-2
const int sensorOneVccPin = 7; // sensor-1 VCC Pin
const int sensorTwoVccPin = 6; // sensor-2 VCC Pin
const int twLedPin = 2;        // upcoming timed watering Notification LED

unsigned long lastWaterTime = 0;
unsigned long lastMoistureTime = 0;

const long waterInterval = 43200000;    // 12 hours
const long waterRuntime = 10000;        // 10 seconds
const long moistureInterval = 10800000; // 3 hours

const long minHumidity = 25; // minimum humidity threshold
bool pumpOn = false;         // pump state flag variable

void setup() {
  pinMode(pumpOnePin, OUTPUT);
  pinMode(pumpTwoPin, OUTPUT);
  pinMode(sensorOneVccPin, OUTPUT);
  pinMode(sensorTwoVccPin, OUTPUT);
  pinMode(sensorOnePin, INPUT);
  pinMode(sensorTwoPin, INPUT);
  pinMode(twLedPin, OUTPUT);
  pumpOneOff();
  pumpTwoOff();
  Serial.begin(9600);
  Serial.println("+----- Vacation Watering Initialized -----+");
}

void loop() {
  timerMode();
  sensorMode();
  notify();
}

// Read Soil Humidity

int readSensorOne() {
  int rawOne, realOne;
  digitalWrite(sensorOneVccPin, HIGH);
  delay(2000);
  rawOne = analogRead(sensorOnePin);
  rawOne = constrain(rawOne, 0, 1023);
  realOne = map(rawOne, 1023, 0, 0, 100);
  digitalWrite(sensorOneVccPin, LOW);
  return realOne;
}

int readSensorTwo() {
  int rawTwo, realTwo;
  digitalWrite(sensorTwoVccPin, HIGH);
  delay(2000);
  rawTwo = analogRead(sensorTwoPin);
  rawTwo = constrain(rawTwo, 0, 1023);
  realTwo = map(rawTwo, 1023, 0, 0, 100);
  digitalWrite(sensorTwoVccPin, LOW);
  return realTwo;
}


// Time Mode

void timerMode() {
  if( !pumpOn && millis() - lastWaterTime >= waterInterval ) { // time to turn on pump
    lastWaterTime = millis();
    pumpOneOn(); // turn on pump
    Serial.println("Timer Mode Watering: On"); 
    pumpOn = true; // flag on
  }

  if( pumpOn && (millis() >= lastWaterTime + waterRuntime) ) {
    pumpOneOff(); // turn off pump
    Serial.println("Timer Mode Watering: Off"); 
    pumpOn = false; //flag off
  }
}


// Moisture Mode

void sensorMode() {
  int moistureOne, moistureTwo;
  if( millis() - lastMoistureTime >= moistureInterval ) {
    lastMoistureTime = millis();
    moistureOne = readSensorOne();
    moistureTwo = readSensorTwo();
    Serial.print(moistureOne);Serial.print(" | "); Serial.println(moistureTwo);
    if( !pumpOn && ( moistureOne < minHumidity || moistureTwo < minHumidity ) ) {
      pumpTwoOn(); //turn on pump-2
      Serial.println("Humidity Mode Watering: On"); 
      pumpOn = true; // flag on
      delay(waterRuntime); // water for water delay value
      pumpTwoOff(); //turn off pump-2
      Serial.println("Humidity Mode Watering: Off"); 
      pumpOn = false; //flag off
    }
  }
}

// Pump Controllers
void pumpOneOn() {
  digitalWrite(pumpOnePin, LOW); // relay turn on
}

void pumpOneOff() {
  digitalWrite(pumpOnePin, HIGH); // relay turn off
}

void pumpTwoOn() {
  digitalWrite(pumpTwoPin, LOW); // relay turn on
}

void pumpTwoOff() {
  digitalWrite(pumpTwoPin, HIGH); // relay turn off
}

// Notification

void notify() {
  upcomingTimedWatering();
}

void upcomingTimedWatering() {
  const long notifyTime = 900000; //30 minutes
  if( !pumpOn && millis() - lastWaterTime >= ( waterInterval - notifyTime ) ) {
    digitalWrite(twLedPin, HIGH);
  }
  if( !pumpOn && millis() - lastWaterTime >= ( waterInterval - 10000 ) ) {
    digitalWrite(twLedPin, LOW);
  }
}


