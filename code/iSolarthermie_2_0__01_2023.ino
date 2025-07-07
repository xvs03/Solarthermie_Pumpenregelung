
// +++ WICHTIG !!! +++
// Controller: Arduino UNO WIFI R2 | Einstellung über Werkzeuge > Arduino megaAVR Boards (Boardverwalter "Arduino megaAVR Boards" über Boardverwaltung installieren)

/* Programming Log:
                    - 09.01.23:
                                WiFI Connect zur Fritzbox einrichten
                                MQTT einrichten mit JSON Master Topic
                    - 22.01.23  HX711 mit PT500 Sensor "K" einbinden und per MQTT Sensordaten im JSON versenden
                    - 25.01.23  HX711 Messbrücke "K" kalibriert und OFFSET und DIVIDER eingestellt.
                    - 26.01.23  HX711 Messbrücke "P" kalibriert und OFFSET und DIVIDER von "K" übernommen.
                    - 10.02.23  BAs AUTO/HAND/URLAUB Pfad in neuen Tab angelegt
                    - 12.02.23  BAs via JSON mitteilen
                    - 16.02.23  BA Kommandos via MQTT/JSON empfangen
                    - 18.02.23  BAs anlegen und erkennen mit Merker
                    - 19.02.23  Pumpe in BA HAND EIN/AUS als Interrupt Taster eingebunden und PWM Ausgang für Pumpendrehzahl eingerichtet und in die BAs eingepflegt
                    - 23.02.23  BA Funktions debugging
                    - 24.02.23  Differenztemperatur P zu K

                    - 07.07.25  HMI LCD 16x2 Diplay Upgrade

                    To Do
                    - 22.01.24  Startparameter im EEPROM ablegen. Resiliens gegen Netzwerkausfall/OpenHAB
                    - 22.01.24  Display HMI Einbau
*/


// -------------------------- ### Bibliotheken ### ------------------------------
//#include <WiFi.h>
#include <WiFiNINA.h>
//#include <SPI.h>
#include "arduino_secrets.h"  // Router Zugang WIFI
#include <PubSubClient.h>     // MQTT Nick O'Leary
#include <ArduinoJson.h>      // JSON Format per MQTT verschicken
#include "HX711.h"            // AD Wandler HX711 über SPI
//#include <EEPROM.h>          // Speicherverwaltung für Start Konfig Variablen


// -------------------------- ### Variablen global ### --------------------------

// ### WIFI Connect ###
char ssid[] = SECRET_SSID;    // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int status = WL_IDLE_STATUS;  // the Wifi radio's status
byte mac[6];                  // the MAC address of your WiFi Module
long rssi = 0;                // WLAN Signalstärke in dB (aus Funkion "Data")

// ### MQTT Client Ident und Zugang Broker OH ###
const char* mqttServer = mqttSERVER;
const char* mqttUsername = mqttUSERNAME;
const char* mqttPassword = thingKEY;
const char* mqttDeviceId = thingID;


// ### MQTT Topic's Puplish ###
// Mastertopic (alle Infos in einem Topic an MQTT server senden brw. empfangen)
char PubMasterTopic[] = "iSolar_2.0/MasterStatus";    // alle States (außer LastWill) werden dort im JSON Format bereitgestellt
char SubMasterTopic[] = "iSolar_2.0/MasterCommand";   // alle Befehle vom OH empfangen. Werden im JSON Format bereitgestellt und aufgelöst
char SubMasterTopic_2[] = "iSolar_2.0/MasterConfig";  // alle Konfiguartionsdaten die vom OH werden. Hysterese,Tempdiffstufen,PuProzentTempDiff = 24 Datensätze

// LAST WILL TESTAMENT
const char willTopic[] = "iSolar_2.0/LastWill";
int willQoS = 0;
bool willRetain = true;
const char willMessage[] = "OFFLINE";
const char willMessageOn[] = "ONLINE";

// MQTT-Client erstellen
WiFiClient ArdWiFIiSol2Client;
PubSubClient client(ArdWiFIiSol2Client);

// # Betriebsarten #
// Betriebsartendurchwahl <BAs.ino>
volatile int BAselect = 1;  // 1 = AUTO ; 2 = HAND ; 3 = URLAUB über fallende Flanke ; Beim hochlauf der Steuerung wird automatisch BA AUTO = 1 initialisiert | volatile wegen Interrupt Anspruch
bool stateResetH = true;    // Pumpen Signale 1x auf 0 setzen bei HAND Anwahl

// Pumpe in HAND State / last State MERKER <BAs.ino>
volatile bool PiH_state = false;  //  | volatile wegen Interrupt Anspruch
bool PiHcom = false;              // Pumpe über REMOTE EIN/AUS

// Pumpe Ansteuerung Variablen
volatile int PuPwmWert = 0;  // Drehzahl über PWM Wert | volatile wegen Interrupt Anspruch
int PuPwmProz = 0;           // Drehzahl über Prozent
bool PuSpgV = false;         // Relais Spannungsversorgung

// Arrays für 11 Leistungsstufen der Pumpe
float PuTempDiffGrenz[] = { 15, 20, 25, 30, 35, 40, 42, 45, 50, 51, 55 };  // Array für die Pumpenleistungsregelung über Temperaturdifferenzgrenzwerte von K u. P | 11 Stufen
int PuPowerSteps[] = { 47, 50, 53, 56, 59, 62, 65, 70, 75, 85, 100 };      // Array Pumpenleistungsstufen für Temperaturdifferenzgrenzwerte | 11 Stufen

// Pumpe Hysterese | SETUP über REMOTE möglich
bool HyM = false;  // Hysteresemerker (Tempdiff im Bereich der Hysterese = true)
float P_Hy_E = 5;  // Pumpe soll ab Tempdiff >= 5 C° (Default) starten
float P_Hy_A = 3;  // Pumpe soll ab Tempdiff <= 3 C° (Default) stoppen

// +++ Variablen für Tempsensor und Wertaufbereitung mit Korrekturvariablen  >#0< Kollektor +++


// Variablen _K
float RohTemp_K = 0;  // Kollektor Rohtemperatur

// Variablen _P
float RohTemp_P = 0;  // Puffer Rohtemperatur

// Temperaturdiff P zu K
float TempDiff_K_P = 0;

// +++ Hardware ansprechen HX711 ("Wägezellen" für Temperaturerfassung) +++
HX711 loadcell0;  // +++ Temperatursensor >#0< Kollektor +++
HX711 loadcell1;  // +++ Temperatursensor >#1< Puffer +++


// +++ Betriebsarten (BA) AUTO / HAND +++

//  ### Eingangspin Tasterfeld Nummern ####
const int BA = 12;   //0 MKR; // Betriebsartenanwahl | AUTO ; HAND ; URLAUB | mit Interrupt
const int PiH = 13;  //1 MKR; Pumpe EIN/AUS in BA Hand

//  ### Ausgangspin zur Pumpe Nummern ####
const int PuPwmSig = 9;  //MKR 2; // Pumpe Signalausgang PWM Pumpendrehzahl
const int PuPower = 8;   //MKR 3;  // Pumpe Versorgungsspannung (Relais)

int BA_PinState = HIGH;

//int BAselect = 1; // 1 = AUTO ; 2 = HAND ; 3 = URLAUB über fallende Flanke ; Beim hochlauf der Steuerung wird automatisch BA AUTO = 1 initialisiert





// -------------------------------------- ### SETUP BEGIN ### ------------------------------

void setup() {

  /*
  // +++ Array Speicheroperationen Temperaturdifferenzgrenzen +++

  // Startadresse Array Temperaturdifferenzgrenzen
  int eeAddress = 8; // Fange bei EEPROM Adresse X an zu lesen | Dok: Speicher_EEPROM_dok

  // Schreiben der Array Defaultwerte in das EEPROM 
  Serial.print("Speichere die Array Defaultwerte für Temperaturdifferenzgrenzen im EEPROM");
  EEPROM.put(eeAddress, PuTempDiffGrenz);

  // Array Temperaturdifferenzgrenzen | Bestimmung der Bytelänge zur Ausgabe
  Serial.print("Die Bytelänge des Arrays Temperaturdifferenzgrenzen beträgt:  ");
  size_t Laenge_Td = sizeof(PuTempDiffGrenz);
  Serial.print(Laenge_Td);
  Serial.println(" Bytes");


  // EEPROM Startkonfiguration für Pumpenleistung Array Temperaturdifferenzgrenzen lesen

  Serial.print("Das Array Temperaturdifferenzgrenzen im Speicher wird gelesen und hat folgenden Inhalt:    ");

  for (int i = 0; i < Laenge_Td; i++) {
    EEPROM.get(eeAddress + i, PuTempDiffGrenz[i]);
    Serial.print(PuTempDiffGrenz[i]);
    Serial.print("\t");
  }
  */


  // Mode BA Eingänge DI 2 u. 3 konfiguieren
  pinMode(BA, INPUT);                                             // BA AUTO/HAND/URLAUB Erkennung über fallende Flanke | mit Interrupt
  attachInterrupt(digitalPinToInterrupt(BA), BA_A_H_U, FALLING);  // Beim drücken von BA Wahl Taster zur Funktion "BA_A_H_U()" im Tab <BAs.h> springen

  pinMode(PiH, INPUT);                                                      // Pumpe EIN/AUS in BA HAND | HIGH = AUS Erkennung über fallende Flanke
  attachInterrupt(digitalPinToInterrupt(PiH), TASTER_PUMPE_HAND, FALLING);  // Beim drücken in BA HAND und danach Pumpe EIN/AUS in HAND soll in die Funktion PUMPE_HAND in <BAs.ini> gesprungen werden

  // ### Ausgangspins Konfig ###
  pinMode(PuPwmSig, OUTPUT);  // PWM Signal Pumpe Eingang
  pinMode(PuPower, OUTPUT);   // Pumpe Versorungsspannung (Relais)

  // ### WIFI Connect ###

  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  //while (!Serial); // Warten bis der Port anwortet |

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to network: ");
    Serial.println(ssid);
    // print your MAC address:
    WiFi.macAddress(mac);
    Serial.print("MAC: ");
    Serial.print(mac[5], HEX);
    Serial.print(":");
    Serial.print(mac[4], HEX);
    Serial.print(":");
    Serial.print(mac[3], HEX);
    Serial.print(":");
    Serial.print(mac[2], HEX);
    Serial.print(":");
    Serial.print(mac[1], HEX);
    Serial.print(":");
    Serial.println(mac[0], HEX);

    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(5000);
  }

  // you're connected now, so print out the data:
  Serial.println("You're connected to the network");

  Serial.println("----------------------------------------");
  printData();
  Serial.println("----------------------------------------");
  // WIFI Connect END

  // MQTT Client

  client.setServer(mqttSERVER, 1883);

  // Verbindung zum MQTT-Broker herstellen und das "Kommando" -Thema abonnieren

  client.setCallback(callback);
  while (!client.connected()) {
    Serial.println("Verbindung zum MQTT-Broker herstellen...");
    if (client.connect(mqttDeviceId, mqttUsername, mqttPassword, willTopic, willQoS, willRetain, willMessage)) {
      Serial.println("Erfolgreich verbunden");
      client.subscribe(SubMasterTopic);
    } else {
      Serial.print("Fehler beim Verbinden mit Fehlercode: ");
      Serial.print(client.state());
      delay(2000);
    }
  }

}  // -------------------------- ### SETUP END ### -----------------------------------



// ---------------------------- ### LOOP BEGIN ### ----------------------------------

void loop() {

  unsigned long time1 = micros();

  BA_Detection();                    // BA Erkennung Switch Case <BAs.ino>
  analogWrite(PuPwmSig, PuPwmWert);  // Erzeuge PWM nach BA Vorgabe

  // ### WIFI Connect ###

  // check the network connection once every 10 seconds:
  //delay(10000);
  //printData();

  // WLAN QoS
  rssi = WiFi.RSSI();
  Serial.print("WLAN QoS: ");
  Serial.println(rssi);
  // WIFI Connect END


  // ### MQTT und JSON Payload ###

  if (!client.connected()) {
    reconnect();
  }

  client.loop();  // MQTT-Client auf eingehende Nachrichten prüfen

  MQTTandJSON();  // Sendet Payload MQTT und JSON


  // ------------------------------------ ### Temperaturerfassung ### -------------------------------------------------------

  // +++++++++++++++++++++++++++++++++++ Temperatursensor >#0< Kollektor +++++++++++++++++++++++++++++++++++++++++++++++++++++

  // +++ Temperatursensor >#0< Kollektor +++

  // 1. HX711 Verdrahtungsinformation zum MKR1010

  const int LOADCELL0_DOUT_PIN = 5;  // ArdWiFI = PWM Pin 5 black | DT vom HX711 auf MKR PIN 4
  const int LOADCELL0_SCK_PIN = 7;   // SCK vom HX711 auf MKR PIN 7 gelb


  // Skalierung und Bereichseinteilung "K"

  const long LOADCELL0_OFFSET = 0;       // vorher: -5000000 | Wertermittlung Beschreibung OFFSET siehe Dok File | Gain = 32 +-80mV | tatsächliche Eingangspannung von Meßbrücke 0 - max.68 mV = 0-150°C
  const long LOADCELL0_DIVIDER = 80411;  //80900; // vorher: 66667 = Fehler: + 5°C | Wertermittlung Beschreibung OFFSET siehe Dok File | Wert niedriger Temperwert steigt


  // 3. Initialize library +++ Temperatursensor >#0< Kollektor +++

  loadcell0.begin(LOADCELL0_DOUT_PIN, LOADCELL0_SCK_PIN);
  loadcell0.set_scale();  // mit diesem Wert wird skaliert s. Dok File
  loadcell0.set_offset();
  loadcell0.set_gain(32);


  // +++ Rohtemperaturwertermittlung Temperatursensor >#0< Kollektor +++

  float loadcellPosDigits0 = 0;                // Ergebnis des positiv rechnens der rohen loadcell Digits
  const float offsetPosFoldDigits0 = 5901600;  // Offset von dem höchsten negativ Digit ausgehend
  float loadcellRawDigits0 = 0;                // Rohdigits der Zelle
  const float scale0 = 75628;                  //77300;  //78517;  // Digits pro °C | Wert kleiner größerer Wert in °C

  loadcellRawDigits0 = loadcell0.get_units(5), 1;

  loadcellPosDigits0 = loadcellRawDigits0 + offsetPosFoldDigits0;

  // Berechnung von Digits zu °C | Kollektor

  RohTemp_K = loadcellPosDigits0 / scale0;  // Ausgabe des positiv gefoldeten Wertes in °C


  //RohTemp_K = loadcell0.get_units(5), 1; // Mittelwert aus 5 , 1 Kommastellen genau
  /*
  Serial.println("Temperatur K Rohwert: " );
  Serial.println(RohTemp_K);
  Serial.println(loadcell0.get_units(5), 1);

  //RohTemp_P = loadcell1.get_units(5), 1; // Mittelwert aus 5 , 1 Kommastellen genau

  Serial.println("Temperaturfindung K Rohwert: " );
  Serial.println("PosDigits: ");
  Serial.println(loadcellPosDigits0);
  Serial.println("RAW-Digits: ");
  Serial.println(loadcell0.get_units(5), 1);
  Serial.println("Temperatur Rohwert K:");
  Serial.println(RohTemp_K);
  Serial.println("-------------------------------");
  */


  // +++++++++++++++++++++++++++++++++++ Temperatursensor >#1< Puffer +++++++++++++++++++++++++++++++++++++++++++++++++++++

  // 1. HX711 Verdrahtungsinformation zum MKR1010

  const int LOADCELL1_DOUT_PIN = 3;  // DT vom HX711 "P" auf MKR PIN 5 grün
  const int LOADCELL1_SCK_PIN = 6;   // SCK vom HX711 "P" auf MKR PIN 6 rot


  // 2. Skalierung und Bereichseinteilung "P"| Messbrücke R1;R3 = 10k Ohm ; R2 nur Pt500 ; R4 auf RPt 500 75°C eingestellt | Messspannung ~ +-40mV

  const long LOADCELL1_OFFSET = 0;       //-5920000;//mehr ins Minus = TWert steigt | 7000000; //5000000; //-3641000 vorher: -5000000 | Wertermittlung Beschreibung OFFSET siehe Dok File | Gain = 32 +-80mV | tatsächliche Eingangspannung von Meßbrücke 0 - max.68 mV = 0-150°C
  const long LOADCELL1_DIVIDER = 78688;  //228689;//80900; //80900 vorher: 66667 = Fehler: + 5°C | Wertermittlung Beschreibung OFFSET siehe Dok File | Wert niedriger Temperwert steigt

  // Rohwert LOADCELL anzeigen.
  /*
  const long LOADCELL1_OFFSET =  1;
  const long LOADCELL1_DIVIDER = 1;
  */

  // 3. Initialize LOADCELL1 +++ Temperatursensor >#1< Puffer +++

  loadcell1.begin(LOADCELL1_DOUT_PIN, LOADCELL1_SCK_PIN);
  loadcell1.set_scale();   //(LOADCELL1_DIVIDER); // mit diesem Wert wird skaliert s. Dok File
  loadcell1.set_offset();  //(LOADCELL1_OFFSET);
  loadcell1.set_gain(32);  // 32 für Channel B 64 oder 128 für Channel A


  // +++ Rohtemperaturwertermittlung Temperatursensor >#1< Puffer +++

  float loadcellPosDigits1 = 0;                // Ergebnis des positiv rechnens der rohen loadcell Digits
  const float offsetPosFoldDigits1 = 5901600;  // Offset von dem höchsten negativ Digit ausgehend
  float loadcellRawDigits1 = 0;                // Rohdigits der Zelle
  const float scale1 = 75628;                  //77300;  //78517;  // Digits pro °C | Wert kleiner größerer Wert in °C

  loadcellRawDigits1 = loadcell1.get_units(5), 1;

  loadcellPosDigits1 = loadcellRawDigits1 + offsetPosFoldDigits1;

  // Berechnung von Digits zu °C | Puffer

  RohTemp_P = loadcellPosDigits1 / scale1;  // Ausgabe des positiv gefoldeten Wertes in °C

  CALC_DIFF(TempDiff_K_P, RohTemp_K, RohTemp_P);  // Differenztemperaturen von P zu K berechnen
  HYSTERESE(TempDiff_K_P, HyM, P_Hy_E, P_Hy_A);   // Hyterese Bereichsmerker erzeugen



  //RohTemp_P = loadcell1.get_units(5), 1; // Mittelwert aus 5 , 1 Kommastellen genau
  /*
  Serial.println("Temperaturfindung P: " );
  Serial.println("PosDigits: ");
  Serial.println(loadcellPosDigits1);
  Serial.println("RAW-Digits: ");
  Serial.println(loadcell1.get_units(5), 1);
  Serial.println("Temperatur Rohwert P:");
  Serial.println(RohTemp_P);
  Serial.println("-------------------------------");
  */

  // Zykluszeit berechnen
  Serial.print(" ### Neuer Zyklus ###");
  Serial.println();
  unsigned long time2 = micros();
  unsigned long zyklusTime = time2 - time1;
  Serial.print(" Die LOOP Zykluszeit beträgt: ");
  Serial.print(zyklusTime / 1000);
  Serial.print(" Millisekunden");
  Serial.println();
}  // ### LOOP END ###


// ------------------------- ### Funktionen ### -----------------------



// ### WIFI Connect DATA ###

void printData() {
  Serial.println("Board Information:");
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  Serial.println();
  Serial.println("Network Information:");
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the received signal strength:
  rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);

  // print your MAC address:
  WiFi.macAddress(mac);
  Serial.print("MAC: ");
  Serial.print(mac[5], HEX);
  Serial.print(":");
  Serial.print(mac[4], HEX);
  Serial.print(":");
  Serial.print(mac[3], HEX);
  Serial.print(":");
  Serial.print(mac[2], HEX);
  Serial.print(":");
  Serial.print(mac[1], HEX);
  Serial.print(":");
  Serial.println(mac[0], HEX);
}
