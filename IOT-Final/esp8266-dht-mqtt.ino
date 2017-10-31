//#include "DHT.h"
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include "EmonLib.h"     

#define WIFI_AP "Rihusoft"
#define WIFI_PASSWORD "R1hu50ft"

#define TOKEN "lOM4w0mvkJD8WhGxKRDD"

EnergyMonitor emon1;                   // Create an instance


/*// DHT
#define DHTPIN 2
#define DHTTYPE DHT22*/

char thingsboardServer[] = "demo.thingsboard.io";

WiFiClient wifiClient;

// Initialize DHT sensor.
//DHT dht(DHTPIN, DHTTYPE);

PubSubClient client(wifiClient);

int status = WL_IDLE_STATUS;
unsigned long lastSend;

void setup()
{
  Serial.begin(115200);
  //dht.begin();
  delay(10);
  InitWiFi();
  client.setServer( thingsboardServer, 1883 );
  lastSend = 0;
  emon1.current(0, 36.5);       // Current: input pin, calibration.
  
 
}

void loop()
{
  if ( !client.connected() ) {
    reconnect();
  }

  if ( millis() - lastSend > 1000 ) { // Update and send only after 1 seconds
    getAndSendTemperatureAndHumidityData();
    lastSend = millis();
  }

  client.loop();
}

void getAndSendTemperatureAndHumidityData()
{
  Serial.println("Collecting Energy Consumption data.");

  /*// Reading temperature or humidity takes about 250 milliseconds!
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }*/

  double Irms = emon1.calcIrms(1430);  // Calculate Irms only
  double p = Irms*230.0;
  Serial.print("Power: ");
  Serial.print(p);         // Apparent power
  Serial.println("watts");
  Serial.print("current");
  Serial.println(Irms); // Irms
  Serial.println("Amps");
  //delay(500);

  /*Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");*/

  String power = String(p);
  String current = String(Irms);


  // Just debug messages
  Serial.print( "Sending Energy consumption details : [" );

  Serial.print( power ); Serial.print( "," );
  
  Serial.print( current );
  Serial.print( "]   -> " );

  // Prepare a JSON payload string
  String payload = "{";
  payload += "\"Power\":"; payload += power; payload += ",";
  payload += "\"current\":"; payload += current;
  payload += "}";

  // Send payload
  char attributes[100];
  payload.toCharArray( attributes, 100 );
  client.publish( "v1/devices/me/telemetry", attributes );
  Serial.println( attributes );

}

void InitWiFi()
{
  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network

  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to AP");
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    status = WiFi.status();
    if ( status != WL_CONNECTED) {
      WiFi.begin(WIFI_AP, WIFI_PASSWORD);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.println("Connected to AP");
    }
    Serial.print("Connecting to ThingsBoard node ...");
    // Attempt to connect (clientId, username, password)
    if ( client.connect("ESP8266 Device", TOKEN, NULL) ) {
      Serial.println( "[DONE]" );
     /* double Irms = emon1.calcIrms(1430);  // Calculate Irms only
      while(Irms!=0){
          Serial.print("power=");
      Serial.println(Irms*230.0);
      Serial.print("current=");
      Serial.println(Irms);
      delay(1000);*/
      
      
    } else {
      Serial.print( "[FAILED] [ rc = " );
      Serial.print( client.state() );
      Serial.println( " : retrying in 5 seconds]" );
      // Wait 5 seconds before retrying
      delay( 5000 );
    }
  }
}




