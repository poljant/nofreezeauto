/*
 * NoFreeze.h
 *
 *  Created on: 22.09.2016
 *      Author: jant
 *
 * Układ zabezpieczajacy przed zamarzaniem.
 * W zależności od temperatury zewnętrznej i wenętrznej załączany jest przekaźnik,
 * który powoduje załączenie grzejnika  podnoszącego temperaturę w pomieszczeniu.
 * Gdy temperatura wewnątrz spadnie do temperatury tempi i temperatura na zewnątrz
 * spadnie poniżej temperatury tempo - załączany jest grzejnik.
 * Gdy temperatura wewnątrz spadnie poniżej tempi2,
 * (niezależnie od temperatury tempout) też załączany jest grzejnik.
 * Grzejnik załączany jest na czas minutesOn, przez ten czas nie jest kontrolowana temperatura.
 * Po tym czasie obliczany jest czas minutesOff (czas wyłączenia grzejnika),
 * w tym czasie też nie jest kontrolowana temperatura.
 * Temperatura jest kontrolowana, gdy miną oba czasy (minutesOn i minutesOff).
 * Układ może pracować bez czujnika temperatury zewnętrznej, grzejnik
 * załącza się gdy temperatura wewnątrz spadnie poniżej tempi2.
 * Gdy brak czujnika temperatury, to wyświetlana jest temperatura -127 ºC.
 * Wejście w trybu MANUAL po zalogowaniu się http://IP/login lub wciśnięciu tryb MANUAL.
 * tryb "MANUAL" umożliwia ręczne przełączanie stanu przekaźnika
 * po określomym czasie (ileM), gdy nic nie jest zmieniane -
 * automatycznie przechodzi w stan AUTO.
 * MANUAL sygnalizowany jest migotaniem LED-a wmontowanego w układ ESP8266 ESP-12E,
 * istnieje możliwość ręcznego załączenia przkaźnika.
 * Tryb "AUTO" - automatyczne sterowanie przekaźnikiem
 *                zależnie od temperatury
 */


#ifndef NOFREEZE_H_
#define NOFREEZE_H_

#include "Relay.h"
#include <DallasTemperature.h>
// oblicza czas w ms kiedy ma sie zakończyć czynnoś,
// która ma trwać podaną ilość minut
unsigned long fminutes( int );
//zbiór licz szesnastkowych potrzebny do zamiany
// adresu na string
char const hex_chars[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A',   'B','C','D','E','F'};

class NoFreeze {
public:
	float tempout = 30.0; //temperatura na zewnatrz
	float tempinput = 31.0; // teperatura wewnątrz
	DeviceAddress AddrTemp; //adres pomocniczy czujnika temperatury
	DeviceAddress AddrTempIn; //adres wewnętrznego czujnika temperatury
	DeviceAddress AddrTempOut; //adres zewnętrznego czujnika temperatury
	int minutesOn = 10; // ile minut ma być włączony grzejnik
	int minutesOff = 3; // ile minut czekać na ponowne załączenie
	unsigned long timeOn = 0; //czas załączenia przekaźnika
	unsigned long timeOff = 0;  //czas wyłączenia przekaźnika
	double long timeM = 0; //czas rozpoczecia trybu manual
	float tempo = -3; // próg temperatury zewnętrznej
	float tempi = 5; // próg pierwszy temperatury wewnętrznej
	float tempi2 = 4; // próg drugi temperatury wewnętrznej
	float temp = 0; // zmienna pomocnicza
	float tempMiniIn = 30; //osiagnięta temperatura mininimalna IN
	float tempMiniOut = 30; //osiagnięta temperatura mininimalna OUT
	bool bmode = true; // tryb pracy true = AUTO, false = MANUAL
	Relay relay1; // przekaźnik 1
	Relay led0; // LED wbudowany
	volatile unsigned long timeSetOn = 0; //suma czasu (minut) załączenia grzejnika

public:
	NoFreeze();
	virtual ~NoFreeze();
	void begin(); //uruchom obiekt
	void setAuto(); //ustaw tryb AUTO
	void setManual(); //ustaw tryb MANUAL
	void readTemp(); //czyta wszystkie czujniki temperatury
	float readTempIn(); //podaje temperaturę wewnętrzną
	float readTempOut(); //podaje temperaturę zewnętrzną
	void readAddressTemp(); //czyta adres czujnika na pozycji 0
	void setAddressTempIn(void); //odczytany adres przydziela czujnikowi wewnętrznemu
	void setAddressTempOut(void); //odczytany adres przydziela czujnikowi zewnętrznemu
	bool readMode(); //podaje jaki tryb ustawiony true dla AUTO lub false dla MANUAL
	void working(); //główny program
	void setTimeOn();  //czas załaczenia przekaźnika
	void setTimeOff(); //czas wyłączenia przekaźnika
    String adr2str(DeviceAddress); //zamienia adres na string
    void readEEProm(void); //czyta adresy z EEPROM
    void saveEEProm(void); //zapisuje adresy do EEPROM
};
#endif /* NOFREEZE_H_ */

