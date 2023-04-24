#include <Wire.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include "Chars.h"
#define up 8
#define down 7
#define ok 9
#define we 10
#define wy1 11
#define wy2 12
#define wy3 13
#define g1 digitalRead(up)
#define g2 digitalRead(down)
#define g3 digitalRead(ok)
#define weread digitalRead(we)
#define ilosc_impulsow_wy2  czasy[3]//conf
#define czas_impulsu_wy2  czasy[4] // conf
#define odleglosc_impulsu_wy3 czasy[5]//conf
#define czas_impulsu_wy3 czasy[6]//conf
#define czas_drugiego_impulsu_wy2 czasy[7]//conf
#define odleglosc_drugiego_impulsu_wy2 czasy[8]//conf
int licznik = 0;
int address, count = 0, count1 = 0, s = 0, s1 = 0, s2 = 0, a = 0, a1 = 0, a2 = 0;
bool byl[9], jest = 1, narbyl[9], narjest = 0;
int state = 1, statelast = 1, stateprog = 0, statewy = 0;
float czasy[9], czasysave[9], czasyload[9];
int numer_sterownika;
bool trig = false;
bool p = true;
bool p1 = true;
bool p2 = true;
float test = 0;

//ODMIERZANIE CZASU==================================================================================================
float milis = 0;
float milislast = 0;
float cycle = 0;
float timepassed = 0;
float timepassedtikers = 0;
float timepassedevent = 0;
//===================================================================================================================

//TIKER==============================================================================================================
bool tiker_przejscie = true;
bool tiker_wyjscie = false;
float tiker_timepassed = 0;
//===================================================================================================================

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
SoftwareSerial rfserial(14, 15);

void setup() {
  lcd.begin(16, 2);
  lcd.clear();
  lcd.noCursor();
  lcd.createChar(1, OFF);
  lcd.createChar(2, ON);
  lcd.createChar(3, Z);
  lcd.createChar(4, A);
  lcd.createChar(5, L);
  lcd.createChar(7, clr);
  load();
  pinMode(up, INPUT_PULLUP);
  pinMode(down, INPUT_PULLUP);
  pinMode(ok, INPUT_PULLUP);
  pinMode(we, INPUT_PULLUP);
  pinMode(wy1, OUTPUT);
  pinMode(wy2, OUTPUT);
  pinMode(wy3, OUTPUT);
  Serial.begin(9600);


  //CONF=============================================================================================================

  //  czasy[0] = 8.00;
  //  czasy[1] = 20.00;
  //  czasy[2] = 20.00;
  //  czasy[3] = 5.00;
  //  czasy[4] = 0.40;
  //  czasy[5] = 0.50;
  //  czasy[6] = 0.50;
  //  czasy[7] = 0.10;
  //  czasy[8] = 0.10;
  numer_sterownika = 2;
  //=================================================================================================================
  menu();
}

void loop() {

  //OBLICZANIE CZASU CYKLU=============================================================================================
  milis = millis();
  cycle = milis - milislast;
  milislast = milis;
  //=================================================================================================================


  //CONF=============================================================================================================
  /*
    Serial.println("CZASY:");
    Serial.println(czasy[0]);
    Serial.println(czasy[1]);
    Serial.println(czasy[2]);
    Serial.println(czasy[3]);
    Serial.println(czasy[4]);
    Serial.println(czasy[5]);
    Serial.println(czasy[6]);
    Serial.println(czasy[7]);
    Serial.println(czasy[8]);
    Serial.println("KONIEC");
  */
  //=================================================================================================================

  //-------------------------------------------------------------------------------------------------------------------
  //Na razie ustalam ze impulsy w miedzyczasie maja miec wypelnienie 50% wiec czas stanu niskiego = wysokiego
  //-------------------------------------------------------------------------------------------------------------------
  czasy[6] = czasy[5];
  czasy[8] = czasy[7];
  //-------------------------------------------------------------------------------------------------------------------

  //WEJSCIE W PROGRAMOWANIE(PRZYTRZYMANIE OK PRZEZ 5 SEKUND)=========================================================
  if (g3 == false) {
    timepassedevent += cycle;
  }
  else if (g3 == true) {
    timepassedevent = 0;
  }

  if (timepassedevent > 5000) {
    state = 2;
  }
  //=================================================================================================================

  //ODCZYT TRIGGERA==================================================================================================
  if (zboczeop(we, 3)) trig = true;
  //=================================================================================================================

  //MASZYNA STANÓW===================================================================================================
  switch (state) {
    case 1:
      //menu();
      break;
    case 2:
      prog();
      break;
  }

  if (trig == true and state != 2) {
    timepassedtikers += cycle;
    timepassed += cycle;
    in_out();

  }
  if (trig == false) {
    timepassed = 0;
    timepassedtikers = 0;
  }
  //=================================================================================================================


}

void menu() {
  stateprog = 0;
  lcd.setCursor(1, 0);
  lcd.print("STEROWNIK  DO");
  lcd.setCursor(2, 1);
  lcd.print("OSWIETLENIA");

}

void prog() {

  switch (stateprog) {
    case 0:
      lcd.clear();
      stateprog++;
      break;
    case 1:
      lcd.setCursor(0, 0);
      lcd.print("Prog. sterownika");
      lcd.setCursor(1, 1);
      lcd.print("numer 0");
      lcd.print(numer_sterownika);
      lcd.print("/2020");
      delay(8000);
      lcd.clear();
      stateprog++;
      break;
    case 2:
      lcd.setCursor(6, 0);
      lcd.print("WY1");
      stateprog++;
      break;
    case 3:
      lcd.setCursor(6, 1);
      if (zboczeop(up, 0)) czasy[0] += 0.5;
      if (zboczeop(down, 1)) czasy[0] -= 0.5;
      if (czasy[0] < 0) czasy[0] = 0;
      if (zboczeop(ok, 2)) stateprog++;

      lcd.print(czasy[0]);
      lcd.print("s");
      lcd.write(7);
      lcd.write(7);
      break;
    case 4:
      lcd.clear();
      stateprog++;
    case 5:
      lcd.setCursor(6, 0);
      lcd.print("WY2");
      stateprog++;
      break;
    case 6:
      lcd.setCursor(6, 1);
      if (zboczeop(up, 0)) czasy[1] += 0.5;
      if (zboczeop(down, 1)) czasy[1] -= 0.5;
      if (czasy[1] < 0) czasy[1] = 0;
      if (zboczeop(ok, 2)) stateprog++;

      lcd.print(czasy[1]);
      lcd.print("s");
      lcd.write(7);
      lcd.write(7);
      break;
    case 7:
      lcd.clear();
      stateprog++;
      break;
    case 8:
      lcd.setCursor(6, 0);
      lcd.print("WY3");
      stateprog++;
      break;
    case 9:
      lcd.setCursor(6, 1);
      if (zboczeop(up, 0)) czasy[2] += 0.5;
      if (zboczeop(down, 1)) czasy[2] -= 0.5;
      if (czasy[2] < 0) czasy[2] = 0;
      if (zboczeop(ok, 2)) stateprog++;

      lcd.print(czasy[2]);
      lcd.print("s");
      lcd.write(7);
      lcd.write(7);
      break;
    case 10:
      lcd.clear();
      stateprog++;
      break;
    case 11:
      lcd.setCursor(1, 0);
      lcd.print("Ilosc impulsow");
      lcd.setCursor(1, 1);
      lcd.print("WY2: ");
      stateprog++;
      break;
    case 12:
      lcd.setCursor(6, 1);
      if (zboczeop(up, 0)) czasy[3] += 1;
      if (zboczeop(down, 1)) czasy[3] -= 1;
      if (czasy[3] < 0) czasy[3] = 0;
      if (zboczeop(ok, 2)) stateprog++;

      lcd.print(int(czasy[3]));
      lcd.write(7);
      lcd.write(7);
      break;
    case 13:
      lcd.clear();
      stateprog++;
      break;
    case 14:
      lcd.setCursor(1, 0);
      lcd.print("Dlugosc jednego");
      lcd.setCursor(1, 1);
      lcd.print("imp. WY2: ");
      stateprog++;
      break;
    case 15:
      lcd.setCursor(11, 1);
      if (zboczeop(up, 0)) czasy[4] += 0.1;
      if (zboczeop(down, 1)) czasy[4] -= 0.1;
      if (czasy[4] < 0) czasy[4] = 0;
      if (zboczeop(ok, 2)) stateprog++;

      lcd.print(czasy[4]);
      lcd.print("s");
      lcd.write(7);
      lcd.write(7);
      break;
    case 16:
      lcd.clear();
      stateprog++;
      break;
    case 17:
      lcd.setCursor(1, 0);
      lcd.print("Dl. dodatkowych");
      lcd.setCursor(1, 1);
      lcd.print("imp. WY2: ");
      stateprog++;
      break;
    case 18:
      lcd.setCursor(11, 1);
      if (zboczeop(up, 0)) czasy[5] += 0.1;
      if (zboczeop(down, 1)) czasy[5] -= 0.1;
      if (czasy[5] < 0) czasy[5] = 0;
      if (zboczeop(ok, 2)) stateprog++;

      lcd.print(czasy[5]);
      lcd.print("s");
      lcd.write(7);
      lcd.write(7);
      break;
    case 19:
      lcd.clear();
      stateprog++;
      break;
    case 20:
      lcd.setCursor(1, 0);
      lcd.print("Dl. dodatkowych");
      lcd.setCursor(1, 1);
      lcd.print("imp. WY3: ");
      stateprog++;
      break;
    case 21:
      lcd.setCursor(11, 1);
      if (zboczeop(up, 0)) czasy[7] += 0.1;
      if (zboczeop(down, 1)) czasy[7] -= 0.1;
      if (czasy[7] < 0) czasy[7] = 0;
      if (zboczeop(ok, 2)) stateprog++;

      lcd.print(czasy[7]);
      lcd.print("s");
      lcd.write(7);
      lcd.write(7);
      break;
    case 22:
      lcd.clear();
      save();
      stateprog++;
      break;
    case 23:
      menu();
      state = 1;
      break;
  }
}


void in_out() {


  switch (statewy) {
    case 0:
      s = 0;
      count = 0;
      statewy++;
    case 1:
      timepassed = 0;
      statewy++;
      break;
    case 2:
      digitalWrite(wy1, HIGH);
      zalwy1();
      if (timepassed >= (czasy[0] * 1000.0)) {
        digitalWrite(wy1, LOW);
        wylwy1();
        statewy++;
      }
      break;
    case 3:
      timepassed = 0;
      statewy++;
      break;
    case 4:
      a = tiker(czas_impulsu_wy2, czas_impulsu_wy2, timepassedtikers);
      if (p == false and a == 1) {
        count++;
        s = 1;
        zalwy2();
        p = true;
      }
      if (p == true and a == 0) {
        p = false;
        s = 0;
        wylwy2();
      }
      if (count >= ilosc_impulsow_wy2 + 1) {
        s = 1;
        count = 0;
        statewy++;
      }

      digitalWrite(wy2, s);

      break;
    case 5:
      timepassed = 0;
      statewy++;
      break;
    case 6:
      if (timepassed > 0.6 * czasy[1] * 1000) {
        a1 = tiker(czas_impulsu_wy3, odleglosc_impulsu_wy3, timepassedtikers);
      }
      else a1 = 0;
      if (p1 == false and a1 == 1) {
        s1 = 1;
        zalwy3();
        p1 = true;
      }
      if (p1 == true and a1 == 0) {
        p1 = false;
        s1 = 0;
        wylwy3();
      }
      digitalWrite(wy2, HIGH);
      zalwy2();
      if (timepassed >= (czasy[1] * 1000.0)) {
        digitalWrite(wy2, LOW);
        wylwy2();
        statewy++;
      }
      digitalWrite(wy3, s1);
      break;
    case 7:
      timepassed = 0;
      statewy++;
      break;
    case 8:
      test = (czasy[2] / 2.0) * 1000.0;
      if (timepassed >= test) {
        //a2 = tiker(czas_drugiego_impulsu_wy2, odleglosc_drugiego_impulsu_wy2, timepassedtikers);
        a2 = 0;
      }
      else {
        a2 = 0;
      }
      if (p2 == false and a2 == 1 and licznik < 3) {
        licznik++;
        s2 = 1;
        zalwy2();
        p2 = true;
      }
      if (p2 == true and a2 == 0) {
        p2 = false;
        s2 = 0;
        wylwy2();
      }
      digitalWrite(wy2, s2);
      digitalWrite(wy3, HIGH);
      zalwy3();
      if (timepassed >= (czasy[2] * 1000.0)) {
        digitalWrite(wy3, LOW);
        licznik = 0;
        wylwy2();
        wylwy3();
        statewy++;
      }
      break;
    case 9:
      timepassed = 0;
      statewy++;
      break;
    case 10:
      licznik = 0;
      statewy = 0;
      trig = false;
      timepassed = 0;
      break;
  }
}

bool zboczeop(int wejscie, int n) {
  bool wyjscie = false;
  jest = digitalRead(wejscie);
  if (jest < byl[n]) {
    wyjscie = true;
  }
  byl[n] = jest;
  return wyjscie;
}

bool zboczenar(int wejscie, int n) {
  bool wyjscienar = false;
  narjest = digitalRead(wejscie);
  if (narjest > narbyl[n]) {
    wyjscienar = true;
  }
  narbyl[n] = narjest;
  return wyjscienar;
}


void save() {
  address = 0;
  for (address; address < 9; address++) {
    czasysave[address] = czasy[address] * 10;
    EEPROM.write(address, czasysave[address]);
  }
  address = 0;
}

void load() {
  address = 0;
  for (address; address < 9; address++) {
    czasyload[address] = EEPROM.read(address);
    czasy[address] = czasyload[address] / 10;
  }
  address = 0;
}


void zalwy1() {
  //ZAŁ1:
  //  lcd.setCursor(1, 1);
  //  lcd.write(3);
  //  lcd.write(4);
  //  lcd.write(5);
}

void zalwy2() {
  //ZAŁ2:
  //  lcd.setCursor(6, 1);
  //  lcd.write(3);
  //  lcd.write(4);
  //  lcd.write(5);
}

void zalwy3() {
  //ZAŁ3:
  //  lcd.setCursor(11, 1);
  //  lcd.write(3);
  //  lcd.write(4);
  //  lcd.write(5);
}
void wylwy1() {
  //  lcd.setCursor(1, 1);
  //  lcd.write(7);
  //  lcd.write(7);
  //  lcd.write(7);
}
void wylwy2() {
  //  lcd.setCursor(6, 1);
  //  lcd.write(7);
  //  lcd.write(7);
  //  lcd.write(7);
}
void wylwy3() {
  //  lcd.setCursor(11, 1);
  //  lcd.write(7);
  //  lcd.write(7);
  //  lcd.write(7);
}

bool tiker(float czas_niskiego, float czas_wysokiego, float &tiker_timepassed) {

  if (tiker_timepassed >= (czas_wysokiego * 1000.0) & tiker_przejscie == true) {
    tiker_wyjscie = true;
    tiker_przejscie = false;
    tiker_timepassed = 0;
  }
  if (tiker_timepassed >= (czas_niskiego * 1000.0) & tiker_przejscie == false) {
    tiker_wyjscie = false;
    tiker_przejscie = true;
    tiker_timepassed = 0;
  }
  return tiker_wyjscie;
}
