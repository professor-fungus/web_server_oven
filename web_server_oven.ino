/*
  Web server for the heat treating oven
  based on https://www.youtube.com/watch?v=V8INmMY8GF8
  Consider https://randomnerdtutorials.com/esp32-esp8266-plot-chart-web-server/ for charts
  Pulled text handling from: https://www.youtube.com/watch?v=z16gdSpAut8  
  */
#include "max6675.h"
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>

//void update_form(double temperature);

//WIfi AP name and password
const char* ssid = "THERMOLYNE";
const char* password = "12345678";
const int thermoMISO = 12;
const int thermoCS = 15;
const int thermoCLK = 14;
int iter_count; //tracks the number of executions for the main loop; this way we update the "form" only on occasion

MAX6675 thermocouple;

//double temperature;

String form;

// Create an instance of the server and specify the port to listen on as an argument
//WiFiServer server(80);
ESP8266WebServer server(80);          // String form to sent to the client-browser



void setup() {
  
  thermocouple.begin(thermoCLK, thermoCS, thermoMISO);
  delay(1000);
  /*WiFi.softAP(ssid, password); // remove the password parameter if you want the AP to be open.
  server.begin();
  delay(2000);
   // prepare LED PIN, LED is connected to TX = BUILTIN_LED
   // TX is 1 = BUILTIN_LED
   // RX is 3
   // GPIO 0 is 0 
   // GPIO 2 is 2
  pinMode(BUILTIN_LED, OUTPUT);
  delay(100);
  digitalWrite(BUILTIN_LED, HIGH); // Turn OFF led at initial stage
  */
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);


  Serial.println("HTTP server started");
  
  // Set up the endpoints for HTTP server,  Endpoints can be written as inline functions:
  server.on("/", []()
  {
    server.send(200, "text/html", form);
  });

  server.on("/msg", handle_msg);                          // And as regular external functions:
  server.begin();                                         // Start the server
  Serial.println("Ready!");

}

void loop() {
  
  server.handleClient();
  //temperature = thermocouple.readFahrenheit();

  if(iter_count < 500){
    iter_count++;
  }else{
    update_form(thermocouple.readFahrenheit());
    iter_count = 0;
  }
  
  /*
  form =                                             // String form to sent to the client-browser
  "<p>"
  "<center>"
  "<h1>THERMOLYNE EGG FRYER</h1>"
  "<br>"
  "<h1>CURRENT FAHRENHEITS:</h1>";
  form += "<h1>";
  form += temperature;
  form += "</h1>";
  form += "<br>"
  "<h1>How many Fahrenheits would you like to have?</h1>"
  "<br>"
  "<form action='msg'><p><input type='text' name='msg' size=20 autofocus> <input type='submit' value='Submit'></form>"
  "</center>";
  delay(500);
  */
  /*
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  // Wait until the client sends some data
  while(!client.available()){
    delay(1);
  }
  
  // Read the first line of the request
  String req = client.readStringUntil('\r');
  client.flush();
  
  // Match the request
  if (req.indexOf("/LED_1/off") != -1)
    digitalWrite(BUILTIN_LED, HIGH); //LED OFF // Set Blue LED according to the request
  else if (req.indexOf("LED_1/on") != -1)
    digitalWrite(BUILTIN_LED, LOW); //LED ON // Set Blue LED according to the request
  else if (req.indexOf("/") != -1)
    ; //do nothing; just send html page
  else {
    client.stop();
    return;
  }
  client.flush();
  
  String HTML = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html> <h1>THERMOLYNE EGG FRYER</h1> <h2>ESP8266 Web Server</h2>";
  HTML += "<h3>Blue LED is now ";
  HTML += (digitalRead(BUILTIN_LED))?"OFF":"ON</h3>"; //Low = LED ON and High = LED OFF
  HTML += "<h3>Blue LED <button onclick=\"window.location.href='/LED_1/on'\">LED ON</button>&nbsp;<button onclick=\"window.location.href='/LED_1/off'\">LED OFF</button></h3>";
  HTML += "<h3>Temperature is: ";
  HTML += (thermocouple.readFahrenheit());
  HTML += "</h3></html>";


  client.print(HTML);
  delay(10);
  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed
*/
}

void handle_msg() 
{

  server.send(200, "text/html", form);                    // Send same page so they can send another msg

  String msg = server.arg("msg");
//  Serial.println(msg);
  String decodedMsg = msg;
  
  // Restore special characters that are misformed to %char by the client browser
  decodedMsg.replace("+", " ");
  decodedMsg.replace("%21", "!");
  decodedMsg.replace("%22", "");
  decodedMsg.replace("%23", "#");
  decodedMsg.replace("%24", "$");
  decodedMsg.replace("%25", "%");
  decodedMsg.replace("%26", "&");
  decodedMsg.replace("%27", "'");
  decodedMsg.replace("%28", "(");
  decodedMsg.replace("%29", ")");
  decodedMsg.replace("%2A", "*");
  decodedMsg.replace("%2B", "+");
  decodedMsg.replace("%2C", ",");
  decodedMsg.replace("%2F", "/");
  decodedMsg.replace("%3A", ":");
  decodedMsg.replace("%3B", ";");
  decodedMsg.replace("%3C", "<");
  decodedMsg.replace("%3D", "=");
  decodedMsg.replace("%3E", ">");
  decodedMsg.replace("%3F", "?");
  decodedMsg.replace("%40", "@");


  Serial.println(""); 
  Serial.println(decodedMsg);                             // print original string to monitor
  Serial.println(""); 
  
  unsigned int msg_length = decodedMsg.length();              // length of received message
}

void update_form(double temperature)
{
  form =                                             // String form to sent to the client-browser
  "<p>"
  "<center>"
  "<h1>THERMOLYNE EGG FRYER</h1>"
  "<br>"
  "<h1>CURRENT FAHRENHEITS:</h1>";
  form += "<h1>";
  form += temperature;
  form += "</h1>";
  form += "<br>"
  "<h1>How many Fahrenheits would you like to have?</h1>"
  "<br>"
  "<form action='msg'><p><input type='text' name='msg' size=20 autofocus> <input type='submit' value='Submit'></form>"
  "</center>";
}
