/*
  Alexandre Barlet
  26 Mai 2020
  243-631-MA Projet de fin d'étude
  Travail présenté à David Legris Tremblay et Normand Provencher

  Compilateur: Arduino 1.8.12

  Ce programe est inspiré du programmer de Rui Santos: 
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-esp8266-mysql-database-php/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  Utilisation: 

  Ce programme est à utilisé avec le la carte d'Alexandre Barlet sur laquel se trouve un ESP32-WROOM-32,
  un CP2102n et un DHT22.

  Une connextion est établie avec le Wifi de votre chjoix, quelques parametres serons à changer dans le code
  pour que la connection avec votre reseaux se fasse. 

  Puis les valeurs de température et d'humidité sont envoyé à une base de donnée MySQL / PHP grace a une clé API. 

  Pour changer la frequence d'envoie d'informations, derniere ligne de ce code
*/



#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"
#include <string.h>


// Changer ces parametres pour vous connecter sur votre reseau
const char* ssid     = "CuisseDanglasme";
const char* password = "tzaZak1Oli";

// Ne pas changer cette ligne, elle permet la connection avec le script PHP qui permet l'envoie d'information à la base de donnée MySQL
const char* serverName = "http://192.168.2.49/post-esp-data.php";

// Ne pas changer cette cle API qui permet la connection avec le script PHP qui permet l'envoie d'information à la base de donnée MySQL
String apiKeyValue = "tPmAT5Ab3j7F9";

// Quel capteur est entrain d'envoyer de l'information
String sensorLocation = "FrigoPrincipal";


// Pin qui recoit l'information du DHT22
uint8_t DHTPin = 14;

// Pin pour le capteur magnétique de la porte
uint8_t PORTEPin = 27;
String PORTE = "Fermee";

// Creation de l'objet DHT22
DHT dht(DHTPin, DHT22);

void setup() {
  Serial.begin(115200);
  delay(100);

  pinMode(DHTPin, INPUT);

  dht.begin();

  //Connection au Wifi
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  
  while(WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connecté au Wifi avec l'addresse IP : ");
  Serial.println(WiFi.localIP());

}

void loop() {

  if(digitalRead(PORTEPin) == 0) {
    PORTE = "Ouverte";
  } else {
    PORTE = "Fermée"; 
  }
  //Verification de la connection Wifi
  if(WiFi.status()== WL_CONNECTED){
    HTTPClient http;
    
    // Connection au script PHP
    http.begin(serverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    // Infomations à envoyer
    String httpRequestData = "api_key=" + apiKeyValue + 
                          + "&lieu=" + sensorLocation + "&temp=" + String(dht.readTemperature())
                          + "&hum=" + String(dht.readHumidity()) + "&porte=" + PORTE + "";
    Serial.print("\n\rhttpRequestData: ");
    Serial.println(httpRequestData);
    
    //Envoie d'une requet POST
    int httpResponseCode = http.POST(httpRequestData);

    // Verification de la bonne connection
    if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Vide le buffer
    http.end();
  }
  else {
    Serial.println("Deconnecté du Wifi");
  }

  Serial.print("\n\r Temp: ");
  Serial.print(dht.readTemperature());
  Serial.print("\n\r Hum: ");
  Serial.print(dht.readHumidity());
  Serial.print("\n\rPorte: ");
  Serial.print(PORTE);
 
  //Réglage de la frequence d'envoie d'information (1000 = 1 seconde)
  delay(5000);  
}
