#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <HTTPClient.h>


// Data wire is connected to GPIO15
#define ONE_WIRE_BUS 4
// Setup a oneWire instance to communicate with a OneWire device
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

const char* ssid = "Bruh";
const char* passwd = "bruh12354";

struct ListOfSensors {
    DeviceAddress list[10];
};

ListOfSensors lst;

ListOfSensors detectSensors() {
    struct ListOfSensors lOs;
    for (int i = 0; i < 10; i++) {
        if (!oneWire.search(lOs.list[i])) {
            return lOs;
        }
    }
    return lOs;
}




void setup(void){
    Serial.begin(9600);
    sensors.begin();


    WiFi.begin(ssid, passwd);
    Serial.println("\nConnecting");

    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
    }

    Serial.println("\nConnected to the WiFi network");
    Serial.print("Local ESP32 IP: ");
    Serial.println(WiFi.localIP());
    

    lst = detectSensors();

    delay(1000);
}

void loop(void){ 
    Serial.print("Requesting temperatures...");
    sensors.requestTemperatures(); // Send the command to get temperatures
    Serial.println("DONE");

    for (int i = 0; i < 10; i++) {
        float val = sensors.getTempC(lst.list[i]);
        if (val != -127.00) {
            Serial.print("Sensor ");
            Serial.print(i);
            Serial.print("[*C]: ");
            Serial.println(val); 

            if (WiFi.status() == WL_CONNECTED) {
                HTTPClient http;
                WiFiClient wclient;

                http.begin(wclient, "http://192.168.138.198:9091/metrics/job/temps/instance/" + String(i));
                	
                // http.addHeader("Content-Type", "text/plain");
                int c = http.POST("t " + String(val) + "\n");
                if (c != 200) {
                    Serial.println("ERROR: " + String(c));
                } else {
                    Serial.println("SUCCESS");
                }
                http.end();
            }
        }
    }

    delay(5000);
}


