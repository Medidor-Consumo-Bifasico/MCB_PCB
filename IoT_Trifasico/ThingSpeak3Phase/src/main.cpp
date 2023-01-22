/*
  ********** 2pem100a ***********
  Autor: Vidal Bazurto (avbazurt@espol.edu.ec)
  Reviewed: vasanza
  GitHub: https://github.com/Medidor-Consumo-Bifasico/Curso_Simulacion_Sistemas_Electricos
  More information: https://2pem100a.blogspot.com/
  Practica 6: Envio Datos ThingSpeak
*/


#include <WiFi.h>
#include "secrets.h"
#include <PZEM004Tv30.h>
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros

//Credenciales WIFI
#define ssid SECRET_SSID
#define password SECRET_PASS

//Credenciales ThingSpeak
#define myChannelNumber 2013676
#define myWriteAPIKey "7Z7F81SF8GNHUTWN"


//Definimos los pines de comunicacion PZEM004T
#define PZEM_RX_PIN 19
#define PZEM_TX_PIN 18 

//Creamos dos objeto PZEM004Tv30 con los pines
PZEM004Tv30 faseA(Serial2, PZEM_RX_PIN, PZEM_TX_PIN, 0x01);
PZEM004Tv30 faseB(Serial2, PZEM_RX_PIN, PZEM_TX_PIN, 0x02);
PZEM004Tv30 faseC(Serial2, PZEM_RX_PIN, PZEM_TX_PIN, 0x03);

struct Sensor
{ float voltaje;    // Nombre del modelo
  float corriente;       // Numero de pines digitales
  float potencia;       // Numero de pines digitales
  float energia;       // Numero de pines digitales
  float frecuencia;       // Numero de pines digitales
  float fp;       // Numero de pines digitales
} ;

Sensor DatosFaseA = {0, 0, 0, 0, 0, 0};
Sensor DatosFaseB = {0, 0, 0, 0, 0, 0};
Sensor DatosFaseC = {0, 0, 0, 0, 0, 0};

//Funciones Correspondiente Sensor PZEM004T
float ValidarDato(float dato);
void MedicionSensor(Sensor &DatoFase, PZEM004Tv30 Fase);

//Creamos un objeto WiFiClient
WiFiClient  client;

//Funciones Correspondiente conexion Wifi
void connectToWifi();


void setup() { //Funcion de configuracion
  Serial.begin(115200); //Iniciamos el serial

  WiFi.mode(WIFI_STA);  //Definimos el modo WiFi

  ThingSpeak.begin(client);  // Iniciamos ThingSpeak

  connectToWifi(); //Nos conectamos al WiFi
}


void loop() { //Loop infinito
  MedicionSensor(DatosFaseA, faseA);
  MedicionSensor(DatosFaseB, faseB);
  MedicionSensor(DatosFaseC, faseC);

  ThingSpeak.setField(1, DatosFaseA.voltaje);
  ThingSpeak.setField(2, DatosFaseB.voltaje);
  ThingSpeak.setField(3, DatosFaseC.voltaje);
  ThingSpeak.setField(4, DatosFaseA.corriente);
  ThingSpeak.setField(5, DatosFaseB.corriente);
  ThingSpeak.setField(6, DatosFaseC.corriente);
  ThingSpeak.setField(7, DatosFaseA.potencia);
  ThingSpeak.setField(8, DatosFaseB.potencia);

  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if (x == 200) {
    Serial.println("Channel update successful.");
  }
  else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  delay(5000);
}


float ValidarDato(float dato) { //Funcion que nos permite validar el dato medido
  if (isnan(dato)) {
    return -1;
  }
  return dato;
}

void MedicionSensor(Sensor &DatoFase, PZEM004Tv30 Fase) { //Fucion que se encargara de realizar las consultas al modulo PZEM004T
  DatoFase.voltaje = ValidarDato(Fase.voltage());
  DatoFase.corriente = ValidarDato(Fase.current());
  DatoFase.potencia = ValidarDato(Fase.power());
  DatoFase.energia = ValidarDato(Fase.energy());
  DatoFase.frecuencia = ValidarDato(Fase.frequency());
  DatoFase.fp = ValidarDato(Fase.pf());
}

void connectToWifi() { //Funcion que se conectara a la Red WiFi definida
  delay(2000);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}