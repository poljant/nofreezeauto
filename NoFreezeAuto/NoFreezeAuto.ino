#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include "NoFreeze.h"
#include "NoFreezeWeb.h"
#include "Relay.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include "secrets.h"

NoFreeze nf;

extern void setservers();
#define led D4  // sygnalizacja diody led wbudowanej
#define relay D1  // do tego pinu jest podłączony przekaźnik
//#define DATE_COMPIL __TIMESTAMP__	//czas kompilacji
//String version = "1.02 Compiled: "+__DATE__+" "+__TIME__;
//String compiledate = DATE_COMPIL;
String version = ("1.2.1 z dnia " __TIMESTAMP__);

//SSID i pass do WiFi
//const char* ssid = "SSID";   // SSID sieci WiFi
//const char* password = "pass";  // password do WiFi

//const int port = 80;                 // port serwera www
extern ESP8266WebServer server; //(port);

//#define DEBUG
String slocation="Strych - łazienka";
//String slocation="Hydrofornia";
#define IP_STATIC
#ifdef IP_STATIC
IPAddress IPadr(10,110,0,112); //stały IP łazienki na strychu
//IPAddress IPadr(10,110,0,114); //stały IP hydroforni
IPAddress netmask(255,255,0,0);
IPAddress gateway(10,110,0,1);
#endif
void setup()
{
#ifdef DEBUG
 Serial.begin(115200);
#endif
//EEPROM.begin(512);
 // wyzeruj chwilowo te wartości
// timeOn = 0; // czas załączenia
// timeOff = 0; // czas wyłaczenia
 //konfiguracja pinów leda i przekaźnika
// pinMode(led0, OUTPUT);
 //pinMode(relay1, OUTPUT);

#ifdef IP_STATIC
 WiFi.config(IPadr,gateway,netmask);
#endif
 WiFi.mode(WIFI_AP_STA);
 WiFi.begin(ssid, password);
  int i=0;
  while ((WiFi.status() != WL_CONNECTED) && ( i<=20)) {        //  czekaj na połączenie z wifi
    delay(500);
    i +=1;
#ifdef DEBUG
    Serial.print(".");
#endif
  }
 if (WiFi.status() != WL_CONNECTED){
  WiFi.mode(WIFI_AP_STA); //tryb AP+STATION
  nf.led0.setOff();}// włącz LED gdy nie jest połączenie z WiFi
 else {
  WiFi.mode(WIFI_STA); //tryb STATION
  nf.led0.setOn();}// wyłącz LED gdy jest połączenie z WiFi
#ifdef DEBUG
 Serial.println("");
 Serial.println("WiFi connected");
 Serial.println(WiFi.localIP());          // Wypisz IP serwera
 Serial.println(WiFi.macAddress());      // wypisz MAC adres
// Serial.println(IPtoStr(IPadr));
#endif
 nf.led0.setPin(led);
 nf.relay1.setPin(relay);
 nf.begin();

setservers();

}

// The loop function is called in an endless loop
void loop()
{
#ifdef DEBUG
//extern sensors;
 //sensors.requestTemperatures();  // uruchom odczyt czujników temperatury
 //delay(1000);
 //temp = sensors.getTempCByIndex(0); //czytaj temperaturę w ºC
 // gdy brak czujnika ustaw temperaturę dodatnią
// tempout =  (temp==-127) ? 20 : temp;
 Serial.println(nf.tempout);  // wypisz temperaturę
 //temp = sensors.getTempCByIndex(1);  //czytaj temperaturę w ºC
 // gdy brak czujnika ustaw temperaturę dodatnią
 //tempinput =  (temp==-127) ? 21 : temp;
 Serial.println(nf.tempinput);  // wypisz temperaturę
#endif

 nf.working();
 server.handleClient();
 // załączenie LED wbudowanej gdy brak połączenia z WiFi
 if (WiFi.status() != WL_CONNECTED){
   nf.led0.setOff();
   if (WiFi.getMode() != WIFI_AP_STA){
   WiFi.mode(WIFI_AP_STA);}
 }
 else{ // wyłącz LED gdy jest połączenie z WiFi
   nf.led0.setOn();
    if (WiFi.getMode() != WIFI_STA){
    WiFi.mode(WIFI_STA);}
 }
 if (!nf.bmode) {
  nf.led0.setOff(); // włącz sygnalizację trybu MANUAL (LED ESP8266)
  delay(250);
  nf.led0.setOn(); // wyłącz sygnalizację trybu MANUAL (LED ESP8266)
  delay(250);
 }
}

