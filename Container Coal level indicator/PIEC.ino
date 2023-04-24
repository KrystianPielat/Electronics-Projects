
//---------------------------------------------------------------------------------------------------------------------------------
//Deklaracje Pinów
//---------------------------------------------------------------------------------------------------------------------------------

#define D2 2
#define D3 3
#define D4 4
#define D5 5
#define D6 6
#define D7 7
#define D8 8//Buzzer
#define trig 12
#define echo 11

//---------------------------------------------------------------------------------------------------------------------------------
//Deklaracje zmiennych i timerów
//---------------------------------------------------------------------------------------------------------------------------------
long dystans;
int x = 0;
int sr[100];
int wynik; //Wynik to uśredniona warotść po średniej kroczącej
int odlczujnika = 0;
int glebokosc = 0;
int czas = 0;
int cykl = 0;
int gornagranica = 23;//Odl od czujnika w maksymalnym poziomie wegla [cm]
int dolnagranica =55;//Odl od czujnika przy minimalnym poziomie wegla [cm]
long timer = 0;
long timer2 = 0;
long WyzBuzzer = 0;
bool BuzzerZolte = false;
bool BuzzerCzerwone = false;
bool Sygn1h = false;
bool Sygn2h = false;



void setup() {
glebokosc = dolnagranica - gornagranica;
odlczujnika = gornagranica;

//---------------------------------------------------------------------------------------------------------------------------------
//Deklaracje wejsc i wyjsc
//---------------------------------------------------------------------------------------------------------------------------------
  pinMode(D2,OUTPUT);
  pinMode(D3,OUTPUT);
  pinMode(D4,OUTPUT);
  pinMode(D5,OUTPUT);
  pinMode(D6,OUTPUT);
  pinMode(D7,OUTPUT);
  pinMode(D8,OUTPUT);
  pinMode(trig,OUTPUT);
  pinMode(echo,INPUT);
  Serial.begin(38400);

}


void loop() {


cykl = millis() - czas;
czas = millis();
timer = timer + cykl;
timer2 = timer2 + cykl;


Serial.println(wynik);
/*
Serial.print("Sygn1h: ");
Serial.print(Sygn1h);
Serial.print("   ");
Serial.print("Sygn2h: ");
Serial.print(Sygn2h);
Serial.print("   ");
Serial.print("WyzBuzzer: ");
Serial.println(WyzBuzzer);
Serial.print("BuzzCz: ");
Serial.print(WyzBuzzerCzerwone.time());
Serial.print("BuzzZol: ");
Serial.print(WyzBuzzerZolte.time());
Serial.print("Timer: ");
Serial.println(timer);
*/


if(timer2>100){
  dystans = pomiar();
  timer2 = 0;
  sr[x]=dystans;
  x++;
  if(x>99){
    x=0;
  }
   wynik=0;
   for(int q=0; q<=99; q++){
    wynik = wynik + sr[q];
   }
   wynik = wynik/100;
}

//---------------------------------------------------------------------------------------------------------------------------------
//Umieszczanie dystansu w tablicy 0-9 znaków żeby obliczyc pozniej srednia kroczaca
//---------------------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------------------
//Wywolywanie funkcji
//---------------------------------------------------------------------------------------------------------------------------------
Wyswietlanie(wynik);
WyzwalanieBuzzer(wynik);



if (wynik<=(0.85*glebokosc+odlczujnika)  and wynik > (0.5*glebokosc+odlczujnika)){
    if(WyzBuzzer>7200000 or WyzBuzzer == 8){//WyzBuzzer>   <-- Tutaj daje czas po jakim ma sie uruchamiac buzzer zoltych
      WyzBuzzer=0;
      Sygn2h = false;
      Sygn1h = false;  
      }else{
        Sygn2h = false;
        //Sygn1h = false;
      }

    WyzBuzzer = WyzBuzzer + cykl;
    
}else if(wynik > (0.85*glebokosc+odlczujnika)){
    if(WyzBuzzer>10000 or WyzBuzzer == 0){////WyzBuzzer>   <-- Tutaj daje czas po jakim ma sie uruchamiac buzzer zoltych
      WyzBuzzer=0;
      Sygn1h = true;
      Sygn2h = false;
    }else{
      Sygn1h = false;
      //Sygn2h = false;  
     }
    WyzBuzzer = WyzBuzzer + cykl;
    
}else{
    Sygn2h = false;
    Sygn1h = false;
    WyzBuzzer = 0;
}

}



  


//---------------------------------------------------------------------------------------------------------------------------------
//Funkcja mierząca odleglosc ultradzwiekowym
//---------------------------------------------------------------------------------------------------------------------------------
int pomiar(){

long czas;
long odleglosc;
digitalWrite(trig,LOW);
delayMicroseconds(2);
digitalWrite(trig,HIGH);
delayMicroseconds(10);
digitalWrite(trig,LOW);

czas = pulseIn(echo,HIGH);

odleglosc = czas/58.2;

return odleglosc;
}
//---------------------------------------------------------------------------------------------------------------------------------
//Funkcja wyswietlajaca poziom wegla na diodach
//---------------------------------------------------------------------------------------------------------------------------------
int Wyswietlanie(int z){
  if(z <= 0.15*glebokosc+odlczujnika){
    digitalWrite(D2,HIGH);//Zielona
    digitalWrite(D3,HIGH);//Zielona
    digitalWrite(D4,HIGH);//Zielona
    digitalWrite(D5,HIGH);//Żółta
    digitalWrite(D6,HIGH);//Żółta
    digitalWrite(D7,HIGH);//Czerwona
  }
  if(z <=(0.3*glebokosc+odlczujnika) and z > (0.15*glebokosc+odlczujnika)){
    digitalWrite(D2,LOW);//Zielona
    digitalWrite(D3,HIGH);//Zielona
    digitalWrite(D4,HIGH);//Zielona
    digitalWrite(D5,HIGH);//Żółta
    digitalWrite(D6,HIGH);//Żółta
    digitalWrite(D7,HIGH);//Czerwona
  }
  if(z <=(0.5*glebokosc+odlczujnika) and z > (0.3*glebokosc+odlczujnika)){
    digitalWrite(D2,LOW);//Zielona
    digitalWrite(D3,LOW);//Zielona
    digitalWrite(D4,HIGH);//Zielona
    digitalWrite(D5,HIGH);//Żółta
    digitalWrite(D6,HIGH);//Żółta
    digitalWrite(D7,HIGH);//Czerwona
  }
  if(z <=(0.7*glebokosc+odlczujnika)  and z > (0.5*glebokosc+odlczujnika)){
    digitalWrite(D2,LOW);//Zielona
    digitalWrite(D3,LOW);//Zielona
    digitalWrite(D4,LOW);//Zielona
    digitalWrite(D5,HIGH);//Żółta
    digitalWrite(D6,HIGH);//Żółta
    digitalWrite(D7,HIGH);//Czerwona
  }
  if(z <=(0.85*glebokosc+odlczujnika)  and z >(0.7*glebokosc+odlczujnika)){
    digitalWrite(D2,LOW);//Zielona
    digitalWrite(D3,LOW);//Zielona
    digitalWrite(D4,LOW);//Zielona
    digitalWrite(D5,LOW);//Żółta
    digitalWrite(D6,HIGH);//Żółta
    digitalWrite(D7,HIGH);//Czerwona
  }
  if(z > (0.85*glebokosc+odlczujnika)){
    digitalWrite(D2,LOW);//Zielona
    digitalWrite(D3,LOW);//Zielona
    digitalWrite(D4,LOW);//Zielona
    digitalWrite(D5,LOW);//Żółta
    digitalWrite(D6,LOW);//Żółta
    digitalWrite(D7,HIGH);//Czerwona
  }

return 0;
}

//---------------------------------------------------------------------------------------------------------------------------------
//Aria na D8
//---------------------------------------------------------------------------------------------------------------------------------

int WyzwalanieBuzzer(int y){
    
if (Sygn2h == true){
    BuzzerZolte = true;
    timer = 0;
}
if(BuzzerZolte == true){
    if(timer<=500){digitalWrite(D8,LOW);}
    if(timer>500 and timer<=2500){digitalWrite(D8,HIGH);}
    if(timer>2500 and timer<=3500){digitalWrite(D8,LOW);}
    if(timer>3500 and timer<=4000){digitalWrite(D8,HIGH);}
    if(timer>4000){digitalWrite(D8,LOW); BuzzerZolte = false;}  

}

if(Sygn1h == true){
    BuzzerCzerwone = true;
    timer = 0;
    }
  
if(BuzzerCzerwone == true){
    if(timer>500 and timer<=2500){digitalWrite(D8,HIGH);}
    if(timer>2500 and timer<=2750){digitalWrite(D8,LOW);}
    if(timer>2750 and timer<=3000){digitalWrite(D8,HIGH);}
    if(timer>3000 and timer<=3250){digitalWrite(D8,LOW);}
    if(timer>3250 and timer<=3500){digitalWrite(D8,HIGH);}
    if(timer>3500 and timer<=3750){digitalWrite(D8,LOW);}
    if(timer>3750 and timer<=4000){digitalWrite(D8,HIGH);}
    if(timer>4000 and timer<=4250){digitalWrite(D8,LOW);}
    if(timer>4250 and timer<=4500){digitalWrite(D8,HIGH);}
    if(timer>4500 and timer<=4750){digitalWrite(D8,LOW);}
    if(timer>4750 and timer<=5000){digitalWrite(D8,HIGH);}
    if(timer>5000 and timer<=5250){digitalWrite(D8,LOW);}
    if(timer>5250 and timer<=5500){digitalWrite(D8,HIGH);}
    if(timer>5500 and timer<=5750){digitalWrite(D8,LOW);}
    if(timer>5750 and timer<=7750){digitalWrite(D8,HIGH);}
    if(timer>7750){digitalWrite(D8,LOW);BuzzerCzerwone=false;}
  }

return 0;
}
