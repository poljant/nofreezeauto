#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include "NoFreeze.h"
#include "NoFreezeWeb.h"
#include "Relay.h"
#include <OneWire.h>
#include <DallasTemperature.h>

NoFreeze nf;

extern void setservers();
#define led D4  // sygnalizacja diody led wbudowanej
#define relay D1  // do tego pinu jest podłączony przekaźnik

String version = "1.0 z dnia 07-05-2017";

//SSID i pass do WiFi
const char* ssid = "SSID";   // SSID sieci WiFi
const char* password = "pass";  // password do WiFi

//const int port = 80;                 // port serwera www
extern ESP8266WebServer server; //(port);

#define DEBUG
//String slocation="Strych - łazienka";
String slocation="Hydrofornia";
//#define IP_STATIC
#ifdef IP_STATIC
//IPAddress IPadr(10,110,0,112); //stały IP łazienki na strychu
IPAddress IPadr(10,110,0,114); //stały IP hydroforni
IPAddress netmask(255,255,0,0);
IPAddress gateway(10,110,0,1);
#endif
void setup()
{
#ifdef DEBUG
 Serial.begin(115200);
#endif

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
#endif
 nf.led0.setPin(led);
 nf.relay1.setPin(relay);
 nf.begin();

setservers();

}

void loop()
{
#ifdef DEBUG
 Serial.println(nf.tempout);  // wypisz temperaturę zewnętrzną
 Serial.println(nf.tempinput);  // wypisz temperaturę wewnętrzną
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

