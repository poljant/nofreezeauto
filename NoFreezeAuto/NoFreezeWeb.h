/*
 * NoFreezeWeb.h
 *
 *  Created on: 22.09.2016
 *      Author: jant
 */

#ifndef NOFREEZEWEB_H_
#define NOFREEZEWEB_H_

//#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>

String HTMLHeader();
String HTMLFooter();
String HTMLPage();
String HTMLPage1();
String HTMLPage2();
void setservers(void);

#endif /* NOFREEZEWEB_H_ */
