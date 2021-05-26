//Library for firebase and wifi
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>

//Temp sensor library
#include <DHT.h>

//Temp sensor pin setting
#define DHT11_PIN 2
DHT DHT(DHT11_PIN, DHT11);

//Current Sensor Settings
const int sensorIn = A0;
int mVperAmp = 66; //185mV for 5Amp module , 100mV for 10A , 66mv for 20 & 30 Amp module
float Voltage = 0;
float Vp = 0;
float Vrms = 0;
float Irms = 0;
float errorinmetersensor = 0;
float Vrmserror = 0;
//setting voltage value
float Voltagesupply = 230.00;

//For calculation of billing
char watt[5];
unsigned long last_time = 0;
unsigned long current_time = 0;
float Wh = 0 ;
float watt1;
float bill = 0;

//Temp Limit for fire cut off!
float templimit = 45;

//Settings to connect to wifi and firebase
#define FIREBASE_HOST "URL"
#define FIREBASE_AUTH "PASS"
#define WIFI_SSID "SSID"
#define WIFI_PASSWORD "SSIDPASS"

//Exeuted only once
void setup() {
  //Setting input and output pins
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(A0, INPUT_PULLUP);

  //Setting serial data for troubleshooting
  Serial.begin(9600);

  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  //Connecting to firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  //Temprature Sensor Beigin
  DHT.begin();


  //Current sensor Error Adjustment
  Voltage = getVPP();
  Vrmserror = (Voltage / 2.0) * 0.707; // sq root
  //Setting offset for zero value
  errorinmetersensor = (((Vrmserror * 1000) / mVperAmp))  ;

}
//Intializing Object for lamppost method
FirebaseData fbdo1;
//Initalizing Object for charger method
FirebaseData fbdo2;
//Initalizing Object for enrgymeter and other method
FirebaseData fbdo3;

void loop() {

  //calling lamp post method
  lamppostmethod();
  //slow down processor for processing and posting data
  delay(500);
  //calling charging method
  chargermethod();
  //slow down processor for processing and posting data
  delay(500);
  //calling temprate sensor method
  tempraturemeasuresensor();
  delay(500);
}

//lamppost method
void lamppostmethod() {
  //Check if firebase has the object created
  if (Firebase.getBool(fbdo1, "/LAMPPOST_STATUS")) {
    //Check if firebase has the object created
    if (fbdo1.dataType() == "boolean") {
      //if bool data is true
      if (fbdo1.boolData() == 1) {
        digitalWrite(D0, HIGH);
        delay(500);
      } else {
        //if bool data is false
        digitalWrite(D0, LOW);
        delay(500);
      }
    } else {
      //Error if any print to com port
      Serial.println("Check if you have same dataType: " + fbdo1.dataType());
    }
  } else {
    //Error if any print to com port
    Serial.println(fbdo1.errorReason());
  }
}
//Charger method
void chargermethod() {
  //Check if firebase has the object created
  if (Firebase.getBool(fbdo2, "/CHARGER_STATUS")) {
    //Check if firebase has the object created
    if (fbdo2.dataType() == "boolean") {
      //if bool data is true
      if (fbdo2.boolData() == 1) {
        //Calling Energy Meter Method
        energymeter();
        delay(3000);
        //Turning on the relay
        digitalWrite(D1, HIGH);
        delay(500);
        //caliing Method off if charge value is equal to recharge amount
        offchargerifrechargecomplete();
      } else {
        //if bool data is false
        digitalWrite(D1, LOW);
        //Resetting to zero once turned off
        watt[0] = 0.0000;
        watt[1] = 0.0000;
        watt[2] = 0.0000;
        watt[3] = 0.0000;
        watt[4] = 0.0000;
        Wh = 0.0000;
        current_time = 0;
        last_time = 0;
      }
    } else {
      //Errror if any print to com port
      Serial.println("Check if you have same dataType: " + fbdo2.dataType());
    }
  } else {
    //Errror if any print to com port
    Serial.println(fbdo2.errorReason());
  }
}
//Energy Meter Method1
void energymeter() {
  //Calling sensor method to measure
  Voltage = getVPP();
  //Voltage calculation
  Vrms = (Voltage / 2.0) * 0.707; // sq root
  //Measure Current value
  Irms = (((Vrms * 1000) / mVperAmp)-errorinmetersensor)  ;
  //Posting Value of ampere to database
  Firebase.setFloat(fbdo3, "/EnergyMeter/Ampere", Irms);
  //Calculate power
  double Power = Voltagesupply * Irms;
  //Fot watt hour calculating time
  last_time = current_time;
  current_time = millis();
  //Watt hour calculation
  Wh = Wh +  Power * (( current_time - last_time) / 3600000.0);
  dtostrf(Wh, 4, 2, watt);
  //Rounding off
  watt1 = atof(watt);
  //Reporting wattage to Firebase
  Firebase.setFloat(fbdo3, "/EnergyMeter/WattHour", watt1);

  //Calculating Amount
  //bill_amount = watt1*(energyTariff/1000); For our use case: 10/1000
  bill = watt1 * 0.01;
  //Reporting Amount To Firebase
  Firebase.setFloat(fbdo3, "/EnergyMeter/Bill", bill);
}
double getVPP()
{
  float result;
  //value read from the sensor
  int readValue;
  // store max value here
  int maxValue = 0;
  // store min value here
  int minValue = 1024;
  uint32_t start_time = millis();
  while ((millis() - start_time) < 1000) //sample for 1 Sec
  {
    readValue = analogRead(sensorIn);
    // see if you have a new maxValue
    if (readValue > maxValue)
    {
      /*record the menimum sensor value*/
      maxValue = readValue;
    }
    if (readValue < minValue)
    {
      /*record the menimum sensor value*/
      minValue = readValue;
    }
  }
  // Subtract min from max
  result = ((maxValue - minValue) * 3.3) / 1024.0;
  return result;
}

//Temprature Method
void tempraturemeasuresensor() {
  //get value from sensor
  float tempC = DHT.readTemperature();
  //Reporting value of temp to Firebase
  Serial.print(tempC);
  Firebase.setFloat(fbdo1, "/TempraturePole", tempC);
  //Cut off everything if temp goes above limit
  if (tempC > templimit) {
    Firebase.setBool(fbdo1, "/CHARGER_STATUS", false);
    Firebase.setBool(fbdo1, "/LAMPPOST_STATUS", false);
    Firebase.setBool(fbdo1, "/Emergency", true);
  }
  delay(500);
}
void  offchargerifrechargecomplete() {
  //Getting value from database
  if (Firebase.getString(fbdo3, "/RechargeAmount")) {
    //check data type
    if (fbdo3.dataType() == "string") {
      //convert for comparison
      float usedamountfloat = fbdo3.stringData().toFloat();
      //Comparison of bill amount and used amount
      if (bill >= usedamountfloat) {
        //Stop charger
        Firebase.setBool(fbdo3, "/CHARGER_STATUS", false);
      }
    }
    else
    {
      //Error
      Serial.print("offchargermethError in if 2");
    }

  }
  else {
    //Error
    Serial.print("offchargermethError in if 1");
  }
}
