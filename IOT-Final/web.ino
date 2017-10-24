//Visit www.roboshala.com for more details on NodeMCU and other projects.

#include <ESP8266WiFi.h>
#include "EmonLib.h"                   // Include Emon Library
 
const char* ssid = "Rihusoft"; // SSID i.e. Service Set Identifier is the name of your WIFI
const char* password = "R1hu50ft"; // Your Wifi password, in case you have open network comment the whole statement.
 
int ledPin = 13; // GPIO13 or for NodeMCU you can directly write D7 
WiFiServer server(80); // Creates a server that listens for incoming connections on the specified port, here in this case port is 80.


EnergyMonitor emon1;                   // Create an instance

 

void setup() {
  Serial.begin(115200);
  
  delay(10);
  emon1.current(0, 28.5);       // Current: input pin, calibration.
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
 
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
  // Start the server
  server.begin();
  Serial.println("Server started");
 
  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP()); //Gets the WiFi shield's IP address and Print the IP address of serial monitor
  Serial.println("/");
 
}
 
void loop() {








  
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
 
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
 
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
 
  // Match the request
 
  int value = LOW;
  if (request.indexOf("/LED=ON") != -1)  {
    digitalWrite(ledPin, HIGH);
    value = HIGH;
  }
  if (request.indexOf("/LED=OFF") != -1)  {
    digitalWrite(ledPin, LOW);
    value = LOW;
  }


double Irms = emon1.calcIrms(1430);  // Calculate Irms only
  
  Serial.print("Power: ");
  Serial.print(Irms*230.0);         // Apparent power
  Serial.println("watts");
  Serial.print("current");
  Serial.println(Irms); // Irms
  Serial.println("Amps");
  //delay(500);






 
// Set ledPin according to the request
//digitalWrite(ledPin, value);
 
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<head>");
  client.println("<style>");
  client.println(".center {");
  client.println(" margin: auto;");
  client.println(" width: 80%;");
  client.println("border: 1px solid #4d79ff;");
  client.println("padding: 10px;");
  client.println("}");
  client.println("body {");
  client.println("background-color: lightblue;");
  client.println("}");
  client.println("</style>");
  client.println("</head>");
  client.println("<body>");
  client.println("<br></br>");
  client.println("<br></br>");
  client.println("power:");
  client.println(Irms*230.0);
  client.print("  Watts");
  client.println("<br></br>");
  client.println("<br></br>");
  client.println("current:");
  client.print(Irms);
  client.print("  Amps");
  client.println("</div>");
  client.println("</body>");

  
  

  client.println("</html>");
  
 
  //delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
 
}
