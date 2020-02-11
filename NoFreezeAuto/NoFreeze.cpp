/*
 * NoFreeze.cpp
 *
 *  Created on: 21.09.2016
 *      Author: jant
 */

//#include <Arduino.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include "NoFreeze.h"
#include "Relay.h"
#include <EEPROM.h>

#define pin_relay1 D1  // pin z przekaźnikiem
#define pin_LED LED_BUILTIN // pin z wbudowanym LED
#define pinOW D2	// pin z szyną 1-Wire

OneWire oneWire(pinOW);
DallasTemperature sensors(&oneWire);

unsigned long fminutes( int minutes)
{
	return (millis()+(60000*minutes)); //1000ms*60sek
}

NoFreeze::NoFreeze() {
	led0.setPin(pin_LED);
//	led0.begin();
	relay1.setPin(pin_relay1);
//	relay1.begin();
	EEPROM.begin(64);
}

NoFreeze::~NoFreeze() {
	// TODO Auto-generated destructor stub
}

void NoFreeze::setAuto(){bmode = true;}

void NoFreeze::setManual(){bmode = false;}

bool NoFreeze::readMode(){return bmode;}

void NoFreeze::setTimeOn(){
	timeOn = fminutes(minutesOn);
}
void NoFreeze::readAddressTemp(void){
	//DeviceAddress AddrTemp;
	sensors.getAddress(AddrTemp, 0);
	return ;
}

String NoFreeze::adr2str(DeviceAddress Adr){
//	char const hex_chars[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	String adrstr;
	for( int i = 0; i < 8; i++ )
	{
	    char const byte = Adr[i];

	    adrstr += hex_chars[ ( byte & 0xF0 ) >> 4 ];
	    adrstr += hex_chars[ ( byte & 0x0F ) >> 0 ];
	}

	  return adrstr;
}

void NoFreeze::setAddressTempIn(void){
	readAddressTemp();
	memcpy(AddrTempIn,AddrTemp,sizeof(AddrTemp));
	saveEEProm();
	return;
}
void NoFreeze::setAddressTempOut(void){
	readAddressTemp();
	memcpy(AddrTempOut,AddrTemp,sizeof(AddrTemp));
	saveEEProm();
	return ;
}
void NoFreeze::readEEProm(void){
	int offset = 16;
	for (int i=0; i<8; i++){
		AddrTempIn[i] = EEPROM.read(i);
		AddrTempOut[i] = EEPROM.read(i+offset);
	}
	return;
}
void NoFreeze::saveEEProm(void){
	int offset = 16;
	for (int i=0; i<8; i++){
		EEPROM.write(i,AddrTempIn[i]);
		EEPROM.write(i+offset,AddrTempOut[i]);
	}
	EEPROM.commit();
	return;
}
void NoFreeze::setTimeOff(){
	timeOff = fminutes(minutesOff);
}
void NoFreeze::begin(){
//	NoFreeze();
	led0.begin();
	relay1.begin();
	readEEProm();
	readTemp();
}
void NoFreeze::readTemp(){
	sensors.requestTemperatures(); //uruchom odczyt czyjników temperatury
	delay(1000);
	temp = sensors.getTempC((uint8_t*)AddrTempIn);  //czytaj temperaturę w ºC
	// gdy brak czujnika ustaw temperaturę dodatnią
	tempinput =  (temp==-127) ? 30 : temp;
	if (tempinput<tempMiniIn) tempMiniIn=tempinput;
	temp = sensors.getTempC((uint8_t*)AddrTempOut);  //czytaj temperaturę w ºC
	// gdy brak czujnika ustaw temperaturę dodatnią
	tempout =  (temp==-127) ? 31 : temp;
	if (tempout<tempMiniOut) tempMiniOut=tempout;
}
float NoFreeze::readTempIn(){
	sensors.requestTemperatures();
	delay(1000);
	return sensors.getTempC((uint8_t*)AddrTempIn);
}
float NoFreeze::readTempOut(){
	sensors.requestTemperatures();
	delay(1000);
	return sensors.getTempC((uint8_t*)AddrTempOut);
}

void NoFreeze::working(){
	if (timeM <= millis()){ setAuto();} // sprawdzaj czas trwnia MANUAL, gdy minie przełącz na AUTO
		/////// AUTO
	if (bmode) {  // praca w trybie AUTO
		if (timeOn<=millis())// || timeOff<=millis())  // gdy minął czas załączenia i rozłączenia
		  {
			readTemp();
			// gdy temperatura wewnątrz spadnie poniżej tempi2
			// lub gdy temperatura na zewnątrz spadnie poniżej tempo
			// i temperatura wewnątrz spadnie poniżej tempi
			// włącz przkaźnik i oblicz czas załączenia
			if (((tempinput <= tempi) || (tempout <= tempo)) && (tempinput <= tempi2))
			{
			 timeOn = fminutes(minutesOn);  // oblicz czas załączenia
			 relay1.setOn(); // załącz przekaźnik 1
			 timeSetOn += minutesOn;
			}

			else
			 {  // w przeciwnym razie, gdy przekaźnik załączony, wyłącz przekaźnik
				// i oblicz czas wyłączenia
				if (relay1.read()==1){
				 relay1.setOff();
			 	 timeOff = fminutes(minutesOff);  // oblicz czas wyłączenia
				}
			 }
		  }
	  }
	////////end AUTO
}

