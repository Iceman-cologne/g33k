// Adafruit IO REST API access with ESP8266
//
// For use with ESP8266 Arduino from:
//   https://github.com/esp8266/Arduino
//
// Works great with ESP8266 modules like the Adafruit Huzzah ESP:
//  ----> https://www.adafruit.com/product/2471
//
// Written by Tony DiCola for Adafruit Industries.  
// MIT license, all text above must be included in any redistribution.
#include <ESP8266WiFi.h>
#include "Adafruit_IO_Client.h"


// Configure WiFi access point details.
#define WLAN_SSID  "Firefly24"
#define WLAN_PASS  "Buckbeak&Stu"

// Configure Adafruit IO access.
#define AIO_KEY    "a812b5a6e55b471c00c9af222bd4ea82382403e6"


// Create an ESP8266 WiFiClient class to connect to the AIO server.
WiFiClient client;

// Create an Adafruit IO Client instance.  Notice that this needs to take a
// WiFiClient object as the first parameter, and as the second parameter a
// default Adafruit IO key to use when accessing feeds (however each feed can
// override this default key value if required, see further below).
Adafruit_IO_Client aio = Adafruit_IO_Client(client, AIO_KEY);

// Finally create instances of Adafruit_IO_Feed objects, one per feed.  Do this
// by calling the getFeed function on the Adafruit_IO_FONA object and passing
// it at least the name of the feed, and optionally a specific AIO key to use
// when accessing the feed (the default is to use the key set on the
// Adafruit_IO_Client class).
Adafruit_IO_Feed testFeed = aio.getFeed("esptestfeed");

// Alternatively to access a feed with a specific key:
//Adafruit_IO_Feed testFeed = aio.getFeed("esptestfeed", "...esptestfeed key...");

// Global state to increment a number and send it to the feed.
unsigned int count = 66;
//float count = 66.01;

//------------------- SENSOR CODE -----------------------

#include "DHT.h"

#define DHTPIN 0     // what digital pin we're connected to

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);

//-------------------------------------------------------

void setup() {
  // Setup serial port access.
  Serial.begin(115200);
  delay(10);
  Serial.println(); Serial.println();
  Serial.println(F("Adafruit IO ESP8266 test!"));

  // Connect to WiFi access point.
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");  
  Serial.println("IP address: "); Serial.println(WiFi.localIP());
  
  // Initialize the Adafruit IO client class (not strictly necessary with the
  // client class, but good practice).
  aio.begin();

  Serial.println(F("Ready!"));

  dht.begin();
  delay(2000);
}

void loop() {

//------------ SENSOR CODE -------------------
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  //float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  //float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(f)) { // isnan(h) || isnan(t) || 
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  else
  {
      Serial.print(f);
  Serial.print(" *F\t");
  }
//-------------------------------------------------
  
  // Increment the count value and write it to the feed.
  count = f;
  // To write a value just call the feed's send function and pass it the value.
  // Send will create the feed on Adafruit IO if it doesn't already exist and
  // then add the value to it.  Send returns a boolean that's true if it works
  // and false if it fails for some reason.
  if (testFeed.send(count)) {
    Serial.print(F("Wrote value to feed: ")); Serial.println(count, DEC);
  }
  else {
    Serial.println(F("Error writing value to feed!"));
  }

  // Now wait 10 seconds and read the current feed value.
  Serial.println(F("Waiting 10 seconds and then reading the feed value."));
  delay(10000);

  // To read the latest feed value call the receive function on the feed.
  // The returned object will be a FeedData instance and you can check if it's
  // valid (i.e. was successfully read) by calling isValid(), and then get the
  // value either as a text value, or converted to an int, float, etc.
  FeedData latest = testFeed.receive();
  if (latest.isValid()) {
    Serial.print(F("Received value from feed: ")); Serial.println(latest);
    // By default the received feed data item has a string value, however you
    // can use the following functions to attempt to convert it to a numeric
    // value like an int or float.  Each function returns a boolean that indicates
    // if the conversion succeeded, and takes as a parameter by reference the
    // output value.
    float i;
    if (latest.floatValue(&i)) {
      Serial.print(F("Value as an int: ")); Serial.println(i, DEC);
    }
    // Other functions that you can use include:
    //  latest.uintValue() (unsigned int)
    //  latest.longValue() (long)
    //  latest.ulongValue() (unsigned long)
    //  latest.floatValue() (float)
    //  latest.doubleValue() (double)
  }
  else {
    Serial.print(F("Failed to receive the latest feed value!"));
  }

  // Now wait 10 more seconds and repeat.
  Serial.println(F("Waiting 10 seconds and then writing a new feed value."));
  delay(10000);
}
