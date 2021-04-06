/**
 * Created by K. Suwatchai (Mobizt)
 * 
 * Email: k_suwatchai@hotmail.com
 * 
 * Github: https://github.com/mobizt
 * 
 * Copyright (c) 2021 mobizt
 *
*/

//This example shows how to backup and restore database data

#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

#define FIREBASE_HOST "PROJECT_ID.firebaseio.com"

/** The database secret is obsoleted, please use other authentication methods, 
 * see examples in the Authentications folder. 
*/
#define FIREBASE_AUTH "DATABASE_SECRET"

//Define Firebase Data object
FirebaseData fbdo;

void setup()
{

  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  //Set the size of WiFi rx/tx buffers in the case where we want to work with large data.
  fbdo.setBSSLBufferSize(1024, 1024);

  //Set the size of HTTP response buffers in the case where we want to work with large data.
  fbdo.setResponseSize(1024);

  Serial.println("------------------------------------");
  Serial.println("Backup test...");

  //Download and save data to SD card.
  //{TARGET_NODE_PATH} is the full path of database to backup and restore.
  //{FILE_NAME} is file name in 8.3 DOS format (max. 8 bytes file name and 3 bytes file extension)

  if (!Firebase.backup(fbdo, StorageType::SD, "/{TARGET_NODE_PATH}", "/{FILE_NAME}"))
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.fileTransferError());
	Serial.println("------------------------------------");
    Serial.println();
  }
  else
  {
    Serial.println("PASSED");
    Serial.println("BACKUP FILE: " + fbdo.getBackupFilename());
    Serial.println("FILE SIZE: " + String(fbdo.getBackupFileSize()));
	Serial.println("------------------------------------");
    Serial.println();
  }

  Serial.println("------------------------------------");
  Serial.println("Restore test...");

  //Restore data to defined database path using backup file on SD card.
  //{TARGET_NODE_PATH} is the full path of database to restore
  //{FILE_NAME} is file name in 8.3 DOS format (max. 8 bytes file name and 3 bytes file extension)

  if (!Firebase.restore(fbdo, StorageType::SD, "/{TARGET_NODE_PATH}", "/{FILE_NAME}"))
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.fileTransferError());
	Serial.println("------------------------------------");
    Serial.println();
  }
  else
  {
    Serial.println("PASSED");
    Serial.println("BACKUP FILE: " + fbdo.getBackupFilename());
	Serial.println("------------------------------------");
    Serial.println();
  }
}

void loop()
{
}
