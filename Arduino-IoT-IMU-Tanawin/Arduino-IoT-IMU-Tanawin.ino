#include "SPI.h"
#include "Ethernet.h"

/*
  by MohammedDamirchi
  Home
*/
#include <MPU9250_asukiaaa.h>
#include <Adafruit_BMP280.h>
//#include <Wire.h>
//#include <SPI.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
EthernetServer server(80);
int v;
float voltage;

int trigPin = 2;  //Assign D2 as trigPin
int echoPin = 4;  //Assign D4 as echoPin
long duration;
float distance;

Adafruit_BMP280 bme; // I2C
MPU9250_asukiaaa mySensor;
float aX, aY, aZ, aSqrt, gX, gY, gZ, mDirection, mX, mY, mZ;


void setup() {
  //  pinMode(trigPin, OUTPUT);
  //  pinMode(echoPin, INPUT);
  Serial.begin(115200); //Set Serial Communication
  Ethernet.begin(mac);  //
  server.begin();       //Start Arduino as Server role
  Serial.print("Arduino as Server Role IPaddress: ");  Serial.println(Ethernet.localIP());

  //  Serial.begin(115200);
  while (!Serial);

#ifdef _ESP32_HAL_I2C_H_ // For ESP32
  Wire.begin(SDA_PIN, SCL_PIN);
  mySensor.setWire(&Wire);
#else
  Wire.begin();
  mySensor.setWire(&Wire);
#endif

  bme.begin();
  mySensor.beginAccel();
  mySensor.beginGyro();
  mySensor.beginMag();

  // You can set your own offset for mag values
  // mySensor.magXOffset = -50;
  // mySensor.magYOffset = -55;
  // mySensor.magZOffset = -10;
}

void loop() {
  EthernetClient client = server.available(); //Wait connection from TCP/IP
  if (client) {
    Serial.println("Hi...New Client");
    while (client.connected()) {
      while (client.available()) {
        char data = client.read();
        Serial.println(data);
        switch (data) {
          case 'a':                   // data='a'--> Do nothing
            Serial.print("");
            client.println(""); //Arduino as Server send data to LabVIEW as Client
            break;
          case 'b':      // data='b' Read voltage from A0 and send back to LabVIEW
            if (mySensor.accelUpdate() == 0) {
              aX = mySensor.accelX();
              aY = mySensor.accelY();
              aZ = mySensor.accelZ();
              aSqrt = mySensor.accelSqrt();
              Serial.print(String(aX)); Serial.print(",");
              Serial.print(String(aY)); Serial.print(",");
              Serial.print(String(aZ)); Serial.print(",");
              Serial.print(String(aSqrt)); Serial.print(",");
              //---------------------------------------------------//
              client.print(String(aX)); client.print(",");
              client.print(String(aY)); client.print(",");
              client.print(String(aZ)); client.print(",");
              client.print(String(aSqrt)); client.print(",");
            }

            if (mySensor.gyroUpdate() == 0) {
              gX = mySensor.gyroX();
              gY = mySensor.gyroY();
              gZ = mySensor.gyroZ();
              client.print(String(gX)); client.print(",");
              client.print(String(gY)); client.print(",");
              client.print(String(gZ)); client.print(",");
              //-------------------------------------------------//
              Serial.print(String(gX)); Serial.print(",");
              Serial.print(String(gY)); Serial.print(",");
              Serial.print(String(gZ)); Serial.print(",");
            }

            if (mySensor.magUpdate() == 0) {
              mX = mySensor.magX();
              mY = mySensor.magY();
              mZ = mySensor.magZ();
              mDirection = mySensor.magHorizDirection();
              Serial.print(String(mX)); Serial.print(",");
              Serial.print(String(mY)); Serial.print(",");
              Serial.print(String(mZ)); Serial.print(",");
              Serial.print(String(mDirection)); Serial.print(",");
              //--------------------------------------------------//
              client.print(String(mX)); client.print(",");
              client.print(String(mY)); client.print(",");
              client.print(String(mZ)); client.print(",");
              client.print(String(mDirection)); client.print(",");
            }

            //  Serial.print("\tTemperature(*C): ");
            Serial.print(bme.readTemperature()); Serial.print(",");
            //-----------------------------------------------------//
            client.print(bme.readTemperature()); client.print(",");

            //  Serial.print("\tPressure(Inches(Hg)): ");
            Serial.print(bme.readPressure() / 3377); Serial.print(",");
            //-------------------------------------------------------//
            client.print(bme.readPressure() / 3377); client.print(",");


            //  Serial.print("\tApproxAltitude(m): ");
            Serial.print(bme.readAltitude(1013.25)); // this should be adjusted to your local forcase
            //-----------------------------------------//
            client.print(bme.readAltitude(1013.25)); // this should be adjusted to your local forcase

            Serial.println(""); // Add an empty line
            //-----------------------------------------//
            client.println("");
            break;
        }
      }
      }
    }
  }
