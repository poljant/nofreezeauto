/*
 * NoFreezeWeb.cpp
 *
 *  Created on: 22.09.2016
 *      Author: jant
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "NoFreeze.h"

extern String version;
extern String slocation;

// login i hasło do sytemu
const char* www_login = "admin";
const char* www_pass = "esp8266";

const int port = 80;                 // port serwera www
ESP8266WebServer server(port);
ESP8266HTTPUpdateServer httpUpdate;

extern NoFreeze nf;

unsigned int ilM=10;
unsigned long fminutes( unsigned int ile) {
	return (millis()+(1000*60*ile));
}
// funkcja zamieniająca adres IP na string
String IPtoStr(IPAddress IP){
  String result;
  for (int i = 0; i < 4; ++i) {
    result += String(IP[i]);
    if (i < 3)
      result += '.';
  }
  return result;
}


String HTMLHeader() {           //  nagłówek strony
String  h = "<!DOCTYPE html>\n";
  h += "<html>";
  h += "<head>";
  h += "<title>"+slocation+"</title>";
 // h += "<meta http-equiv=\"Refresh\" content=\"30\" />";  //odświerzaj stronę co 30 sek.
  h += "<meta charset=\"utf-8\">";
  h += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  h += "<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.4/css/bootstrap.min.css\" >";
  h += "</head>";
  h += "<body style=\"text-align: center;color: white; background: black;font-size: 1.5em;\">\n";
  return h;
}

String HTMLFooter() {             //  stopka strony www
String  f = "<p><a href = \"/\"><button class=\"btn btn-info\">Odświerz stronę</button></a></p>";
  f += "<p>Jan Trzciński &copy; 2016-2017</p></td></tr>";
  f += "</body>\n";
  f += "</html>\n";
  return f;
}

String HTMLPage1() {      // pierwsza część strony www
 String t;
 unsigned long sec = millis() / 1000;
 unsigned long min = (sec / 60);
 unsigned long hour = (min / 60);
 unsigned long days = hour / 24;
 sec = sec % 60;
 min = min % 60;
 hour = hour % 24;

 t  = "";
// t  += "<h1><p>The system antifreeze.</p></h1>";
 t  += "<h2><p>Układ zapobiegający zamarzaniu</p></h2>";
 t  += "<h1><p>"+slocation+"</p></h1>";
 t += "<p> Wersja ";
 t += (version);
  t += "</p>";
// t += "<p>The time from the start, days: ";
 t += "<p>Czas od uruchomienia dni: ";
 t += (days);
// t += " hours:";
 t += " godz:" ;
 t += ((hour<10) ? "0" : "");
 t += (hour);
 t += ":";
 t += ((min<10) ? "0" : "");
 t += (min);
 t += ":";
 t += ((sec < 10) ? "0" : "");
 t += (sec);
 t += "</p>";
// t += "<p>Outside temperature: ";
 t += "<p> Temperatura zewnętrzna: ";
 t += (nf.readTempOut());
 t += " ºC</p>";
// t += "The internal temperature: ";
 t += "<p> Temperatura wewnętrzna: ";
 t += (nf.readTempIn());
 t += " ºC</p>";
 t += "<p> Temperatura zew. minimalna: ";
 t += (nf.tempMiniOut);
 t += " ºC</p>";
 t += "<p> Temperatura wew. minimalna: ";
 t += (nf.tempMiniIn);
 t += " ºC</p>";
 t += "<p> Suma czasu załączeń grzejnika: ";
 t += (nf.timeSetOn);
 t += " minut</p>";
 return t;
}

String HTMLPage2() {            // główna strona www
 String p = "";
  if (!nf.bmode) { //gdy ustawiony tryb reczny
   p += ( (nf.relay1.read()) ? "<p><a href = \"/relay1/0\"><button class=\"btn btn-danger\">Przekaźnik ON</button></a></p>\n" : "<p><a href = \"/relay1/1\"><button class=\"btn btn-success\">Przekaźnik OFF</button></a></p>\n");
  // na jakie piny co podłaczono
  // stopka strony
// p += ( "<p> 1-Wire connected to D2</p>\n");
   p += ( "<p>Połączenia: 1-Wire na D2</p>\n");
// p += ( "<p> Relay connected to D1</p>\n");
   p += ( "<p> Przekaźnik na D1</p>\n");
   p += ( "<p><div>Aby ustawić adres czujnika podłącz tylko ten czujnik.</div>");
   p += ( "<div>W przeglądarce www wpisz http://");
   p += IPtoStr(WiFi.localIP());
   p += (+"/setadrin (dla wewnętrzengo) </div>");
   p += ( "<div>lub http://");
   p += IPtoStr(WiFi.localIP());
   p += ("/setadrout (dla zewnętrznego czujnika.) </div></p>");
   p += ( "<p> Adres czujnika zewnętrznego:");
   p += ( nf.adr2str(nf.AddrTempOut));
   p += ( "</p>\n");
   p += ( "<p> Adres czujnika wewnętrznego:");
   p += ( nf.adr2str(nf.AddrTempIn));
   p += ( "</p>\n");
   p += ( "<p><div> http://");
   p += IPtoStr(WiFi.localIP());
   p += ("/setadr (tylko czyta adres czujnika.) </div></p>");
   p += ( "<p> Adres czujnika:");
   p += ( nf.adr2str(nf.AddrTemp));
   p += ( "</p>\n");
  }
  else { // gdy tryb AUTO
   p += ( (nf.relay1.read()) ? "<p><button class=\"btn btn-danger\">Przekaźnik ON</button></p>\n" : "<p><button class=\"btn btn-success\">Przekaźnik OFF</button></p>\n");
   }
  // wyświetl jaki tryb wybrany
  p += ( (nf.bmode) ? "<p><a href = \"/login\"><button class=\"btn btn-success\">AUTO On</button></a></p>\n" \
  : "<p><a href = \"/auto\"> <button class=\"btn btn-danger\"> MANUAL On</button></a></p>\n");
  return p;
}


 String WebPage() {       // połącz wszystkie części strony www
  return (HTMLHeader()+HTMLPage1()+ HTMLPage2()+HTMLFooter());
 }

// funkcja ustawia wszystkie strony www
void setservers(){
 httpUpdate.setup(&server,"/update", www_login, www_pass); // umożliwia aktualizację poprzez WiFi

 server.on("/", [](){      // odświerz stronę www
    server.send(200, "text/html", WebPage());
  });

 server.on("/login", [](){
   if(!server.authenticate(www_login, www_pass))
     return server.requestAuthentication();
   nf.setManual(); // = false;
   nf.led0.setOff(); ////sygnalizacja załączenia trybu MANUAL (LED ESP8266)
   nf.timeM = fminutes(ilM);  // odnów czas trybu MANUAL
   server.send(200, "text/html", WebPage());
 });

  server.on("/auto", [](){      // ustaw trybu AUTO
    nf.setAuto(); //= true;
    nf.led0.setOff();
    server.send(200, "text/html", WebPage());
  });

  server.on("/relay1/0", [] ()     //  wyłącz przekaźnik 1
  {
   if (!nf.bmode) nf.relay1.setOff();
    server.send(200, "text/html", WebPage());
    nf.timeM = fminutes(ilM);   // odnów czas trybu MANUAL
  });

  server.on("/relay1/1", []()      // załącz przekaźnik 1
  {
   if (!nf.bmode) nf.relay1.setOn();
   server.send(200, "text/html", WebPage());
   nf.timeM = fminutes(ilM); // odnów czas trybu MANUAL
  });

  server.on("/tempminiin", []()      // resetuj tempMiniIn
   { if (!nf.bmode) {
 	  nf.tempMiniIn = 30;
 	  nf.timeM = fminutes(ilM); // odnów czas trybu MANUAL
   }
     server.send(200, "text/html", WebPage());
   });

  server.on("/tempminiout", []()      // resetuj tempMiniOut
   { if (!nf.bmode) {
 	  nf.tempMiniOut = 30;
 	  nf.timeM = fminutes(ilM); // odnów czas trybu MANUAL
   }
     server.send(200, "text/html", WebPage());
   });

  server.on("/timeseton", []()      // resetuj sumę czasu załączenia grzejnika
   { if (!nf.bmode) {
 	  nf.timeSetOn = 0;
 	  nf.timeM = fminutes(ilM); // odnów czas trybu MANUAL
   }
     server.send(200, "text/html", WebPage());
   });

  server.on("/setadrin", []()      // ustaw adres czujnika wewnętrznego
     { if (!nf.bmode) {
   	  nf.timeSetOn = 0;
   	  nf.timeM = fminutes(ilM); // odnów czas trybu MANUAL
   	  nf.setAddressTempIn();
     }
      server.send(200, "text/html", WebPage());
    });

  server.on("/setadrout", []()      // ustaw adres czujnika zewnętrznego
     { if (!nf.bmode) {
       nf.timeSetOn = 0;
       nf.timeM = fminutes(ilM); // odnów czas trybu MANUAL
      nf.setAddressTempOut();
     }
      server.send(200, "text/html", WebPage());
    });

  server.on("/setadr", []()      // czytaj adres czujnika
     { if (!nf.bmode) {
       nf.timeSetOn = 0;
       nf.timeM = fminutes(ilM); // odnów czas trybu MANUAL
      nf.readAddressTemp();
     }
      server.send(200, "text/html", WebPage());
    });

 server.begin();                // Start serwera www
#ifdef DEBUG
 Serial.println("Server started");
#endif
}



