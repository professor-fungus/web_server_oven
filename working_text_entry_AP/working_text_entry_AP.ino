#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <SPI.h>
#include <Wire.h>
//#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h>


String form =                                             // String form to sent to the client-browser
  "<p>"
  "<center>"
  "<h1>Who DaMan? Portal</h1>"
  "<br>"
  "<br>"
  "<h1>Type something nice...</h1>"
  "<br>"
  "<form action='msg'><p><input type='text' name='msg' size=20 autofocus> <input type='submit' value='Submit'></form>"
  "</center>";

/* Set Display */
//Adafruit_SSD1306 display = Adafruit_SSD1306();

/* Check if Display is displaying correct Height */
//#if (SSD1306_LCDHEIGHT != 32)
// #error("Height incorrect, please fix Adafruit_SSD1306.h!");
//#endif


/* Set these to your desired credentials. */
const char *ssid = "SSID";
const char *password = "12345678";

ESP8266WebServer server(80);          // String form to sent to the client-browser

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
 * connected to this access point to see it.
 */


void setup() {
 delay(1000);
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


  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  //display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)


  // Clear the buffer.
  //display.clearDisplay();                              // Clears Display
  //display.display();                                   // Sends Command to Display
  
  Serial.println("Ready!");
  
/*
  // text display tests
  display.setTextSize(1);                             // Set Text Size
  display.setTextColor(WHITE);                        // Set Text Color
  display.setCursor(0,0);                             // Set Cursor Location
  display.print("SSID: ");                            
  display.println(ssid);                              // Display SSID
  display.print("PASS: ");
  display.println(password);                          // Display Pasword
  display.print("IP: ");                              
  display.println(myIP);                              // Display IP Address
  display.display();                                  // actually display all of the above
  */
}

void loop() {
 server.handleClient();
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
  
  //display.clearDisplay();                                 // Clear OLED Display
  //display.setCursor(0,0);                                 // Set Cursor to 0,0 Position
  //display.println(decodedMsg);                            // Display Message
  //display.display();
  
  unsigned int lengte = decodedMsg.length();              // length of received message



}
