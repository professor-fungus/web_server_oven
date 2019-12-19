/*
   Copyright (c) 2015, Majenko Technologies
   All rights reserved.

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

 * * Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.

 * * Redistributions in binary form must reproduce the above copyright notice, this
     list of conditions and the following disclaimer in the documentation and/or
     other materials provided with the distribution.

 * * Neither the name of Majenko Technologies nor the names of its
     contributors may be used to endorse or promote products derived from
     this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
   ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* Create a WiFi access point and provide a web server on it. */

#include "max6675.h"
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#ifndef APSSID
#define APSSID "THERMOLYNE"
#define APPSK  "12345678"
#endif

/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;
const int thermoMISO = 12;
const int thermoCS = 15;
const int thermoCLK = 14;
unsigned long interval = 1000;
unsigned long prev_millis = 0;
double hysteresis = 3.0;
bool hot = false;
double temp_reading = 10000;

MAX6675 thermocouple;

ESP8266WebServer server(80);

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
   connected to this access point to see it.
*/

void handleRoot() {
  server.send(200, "text/html", get_form());
}


void setup() {
  thermocouple.begin(thermoCLK, thermoCS, thermoMISO);
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  unsigned long curr_millis = millis();
  
  if((unsigned long)(curr_millis - prev_millis) >= interval){
    handle_heater();
    prev_millis = millis();
    //Serial.println(temp_reading);
  }
  server.handleClient();
}

String get_form()
{
  String first = "<center><h1>THERMOLYNE EGG FRYER</h1><br><h2>Current temperature (F):</h2>";
  String second = "<br><h2>Requested temperature (F):</h2>";
  //String third = "<br><form action='msg'><p><input type='text' name='msg' size=20 autofocus> <input type='submit' value='Submit'></form></center>";
  String third = "<br><form action='/'><p><input type='text' name='msg' size=20 autofocus> <input type='submit' value='Submit'></form></center>";
  return first + thermocouple.readFahrenheit() + second + server.arg("msg") + third;
}

double handle_heater()
{
  /*
  String temp = server.arg("msg");
  if(temp.toDouble() > thermocouple.readFahrenheit()) {
    digitalWrite(LED_BUILTIN, LOW);
  }else{
    digitalWrite(LED_BUILTIN, HIGH);
  }
  */
  String temp_string = server.arg("msg");
  double temp_setP = temp_string.toDouble(); //The setpoint entered on the server
  temp_reading = thermocouple.readFahrenheit(); //The reading from the MAX6675
  
  if(temp_setP > temp_reading && !hot) {
    digitalWrite(LED_BUILTIN, LOW);
  }else if(temp_setP < temp_reading){
    hot = true;
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("Coasting!");
    Serial.println(temp_reading);
  }else if((temp_setP-hysteresis) > temp_reading && hot){
    hot = false;
    Serial.println("Heating!");
    Serial.println(temp_reading);
  }
  //return temp_reading;
}
