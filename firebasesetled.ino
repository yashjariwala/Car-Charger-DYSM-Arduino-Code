#include <Firebase.h>  
#include <FirebaseArduino.h>  
#include <FirebaseCloudMessaging.h>  
#include <FirebaseError.h>  
#include <FirebaseHttpClient.h>  
#include <FirebaseObject.h>  
  
//  
// Copyright 2015 Google Inc.  
//  
// Licensed under the Apache License, Version 2.0 (the "License");  
// you may not use this file except in compliance with the License.  
// You may obtain a copy of the License at  
//  
//     http://www.apache.org/licenses/LICENSE-2.0  
//  
// Unless required by applicable law or agreed to in writing, software  
// distributed under the License is distributed on an "AS IS" BASIS,  
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  
// See the License for the specific language governing permissions and  
// limitations under the License.  
//  
  
// FirebaseDemo_ESP8266 is a sample that demo the different functions  
// of the FirebaseArduino API.  
  
#include <ESP8266WiFi.h>  
#include <FirebaseArduino.h>  
  
// Set these to run example.  
#define FIREBASE_HOST "car-charger-dysm-default-rtdb.firebaseio.com"  
#define FIREBASE_AUTH "KDJ0Jv62214LEN5TNzSlA8GI0lfXSxqE5CNIjJUG"  
#define WIFI_SSID "Hidden Network"  
#define WIFI_PASSWORD "JariwalaYARUAS"  
  
void setup() {  
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
    
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);  
  pinMode(LED_BUILTIN ,OUTPUT); 
  digitalWrite(LED_BUILTIN,LOW);
  Firebase.set("LED_STATUS",0);  
}  
  
int n = 0;  
  
void loop() {  
  // set value  
  n=Firebase.getInt("LED_STATUS");  
  // handle error  
  if (n==1) {  
      Serial.print("LED is ON");  
      digitalWrite(LED_BUILTIN,HIGH);  
      Serial.println(Firebase.error());    
      return;  
       delay(100);  
  }  
 else{  
   Serial.print("LED is OFF");  
   digitalWrite(LED_BUILTIN,LOW);  
 }  
    
  // update value  
    
}  
