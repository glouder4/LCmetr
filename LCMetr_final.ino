#include <Wire.h> 
#include <FreqCount.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);

float C0 = 897.744; 
float L0 = 100.866; 

int DEBUG = 4; 
float L;
float C;
float F= 0; 
float P = 3.141592; 
static char Foutstr[15];
static char Loutstr[15];
static char Coutstr[15];
unsigned long newCount;

void(* resetFunc) (void) = 0; 


void setup() {
  pinMode(DEBUG,INPUT); 
  Serial.begin(9600);
  digitalWrite(DEBUG,HIGH);
  FreqCount.begin(1000);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(1,0);
  lcd.print("Please,");
  lcd.setCursor(6,1);
  lcd.print("stand by");
  if (digitalRead(DEBUG) == 0){  
    if(digitalRead(DEBUG) == HIGH) resetFunc();
    unsigned long count = FreqCount.read();
    count= count/1000;
    if(count != 0){
      lcd.clear();
      do{
        count = FreqCount.read();
        count= count/1000;
        if (digitalRead(DEBUG) == HIGH) { resetFunc();} 
      }while(count != 0);
      if(count ==0){resetFunc();}
    }
    else{
      lcd.clear();
    do{
      lcd.setCursor(0,0);
      lcd.print("Please,select");
      lcd.setCursor(0,1); 
      lcd.print("C mode");
      }while(digitalRead(DEBUG) == LOW);
      if(digitalRead(DEBUG) == HIGH) resetFunc();
    }
  }  
  else{
    if(digitalRead(DEBUG) == LOW) resetFunc();
    unsigned long count = FreqCount.read();
    count= count/1000;
    if(digitalRead(DEBUG) == LOW) resetFunc();
    if(count > 550 || count < 510){
      if(count > 550){ 
        do{
        count = FreqCount.read();
        count= count/1000;
        if(digitalRead(DEBUG) == LOW) resetFunc();
        }while(count> 550);
      }  
      else{
        do{
          count = FreqCount.read();
          count= count/1000;
          if(digitalRead(DEBUG) == LOW)resetFunc();
        }while(count < 510);
      } 
      if(count< 550 && count > 510) resetFunc();
    }
    C = 1/( (4*(P * P))*(count * count)*L0 );
    C = (C*1000000000000)-C0; 
    if(C > 0 && C < 1){
      C0 = C0 - C;
      L0 = 1/( (4*(P*P)) * (count*count) *C0);
      L0 = L0 * 1000000000000;
    }
    else if(C <0){ 
      C0 = C0 + C;
      L0 = 1/( (4*(P*P)) * (count*count) *C0);
      L0 = L0 * 1000000000000;
    }
    if(digitalRead(DEBUG) == LOW)resetFunc(); 
  }
}

void loop() { 
  if (FreqCount.available()) {
    unsigned long count = FreqCount.read(); 
    count= count/1000; 
    if (digitalRead(DEBUG) == HIGH){ 
      C = 1/( (4*(P * P))*(count * count)*L0 ); 
      C = (C*1000000000000)-C0; 
      if(C > 1000){
        C = C/ 1000;
        dtostrf(C,1, 2, Coutstr);
        lcd.clear();
        lcd.setCursor(0,1);
        lcd.print("C: ");
        lcd.print(Coutstr);
        lcd.print(" nF");
      }
      else if(C == 0){ 
        lcd.clear();
        lcd.setCursor(0,1);
        lcd.print("C: ");
        lcd.print("not included");
      }
      else{
        dtostrf(C,1, 2, Coutstr);
        lcd.clear();
        lcd.setCursor(0,1);
        lcd.print("C: ");
        lcd.print(Coutstr);
        lcd.print(" pF");
      }   
    }
  
    if (digitalRead(DEBUG) == LOW){
        if(count == 0){
          dtostrf(L,1, 3, Loutstr);
          lcd.clear();
          lcd.setCursor(0,1);
          lcd.print("L: ");
          lcd.print("not included");
        }
      else{
        L = 1/( (4*(P*P)) * (count*count) *C0);
        L = (L* 1000000000000)- L0;
        if(L < 0){
          L = L*1000;
          dtostrf(L,1, 3, Loutstr);
          lcd.clear();
          lcd.setCursor(0,1);
          lcd.print("L: ");
          lcd.print(Loutstr); 
          lcd.print(" mHn");
        }
        else{
          if(L > 1000){
            L = L/1000;
            dtostrf(L,1, 3, Loutstr);
            lcd.clear();
            lcd.setCursor(0,1);
            lcd.print("L: ");
            lcd.print(Loutstr);
            lcd.print(" nHn");
          }
          else{
            dtostrf(L,1, 3, Loutstr);
            lcd.clear();
            lcd.setCursor(0,1);
            lcd.print("L: ");
            lcd.print(Loutstr);
            lcd.print(" mkHn");
          } 
        }
      }
    } 
  }
delay(1000);
}

