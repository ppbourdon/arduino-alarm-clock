
#include <boarddefs.h>
#include <ir_Lego_PF_BitStreamEncoder.h>
#include <IRremote.h>
#include <IRremoteInt.h>

#include <LiquidCrystal.h>


const int rs = 7, en = 8, d4 = 9, d5 = 10, d6 = 11, d7 = 12;
unsigned long time_1 = 0;
unsigned long time_2 = 0;
unsigned long time_3 = 0;
boolean clockIsRunning = true;
int foobar1 = 0; //these are just random ints to help keep spots within certain functions
int foobar2 = 0;
int foobar3 = 0;
int barbar1 = 0;
int barbar2 = 0;
int barbar3 = 0;
int bazbazbaz = 0;
int soundAlarmGlitch = 0; // this int prevents the alarm from going off when trying to change the time
int alarmHour = 0; 
int alarmMinute = 0;
int alarmDayLightCheck = 0;
int alarmSet = 0;
int timeHour = 0; // seperate ints to change time
int timeMinute = 0; // ^
int timeDayLightCheck = 0; // this int changes the clock from AM to PM
int RECV_PIN = 6;
int buzzer = 5;
int dayLightCheck = 0;
int oneSecond = 58; // set starting second
int oneMinute = 59; // set starting minute
int oneHour = 11; // set starting hour
int alarmCount = 0;
int clockDisplayPosition = 0;
int soundAlarmCount = 0;
int upArrowPressed = 0;
int downArrowPressed = 0;
int stopAlarmCount = 0;
int controlLightCount =0;
int keepAlarmGoing = 0;
int red = 4;
int green = 3;
int blue = 2;
int LEDstatus = 0;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
IRrecv irrecv(RECV_PIN);
decode_results results;

void setup() {
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(buzzer, OUTPUT);
  lcd.begin(16, 2);  
  irrecv.enableIRIn();
  alarmDisplay();
}

void Clock() {
  if(clockIsRunning) {
    if(oneSecond >= 60) {
      oneSecond = 0;
      oneMinute ++;
    }
    if(oneMinute >= 60) {
      oneMinute = 0;
      oneHour ++;
    }
    if(oneHour == 12) {
      if(oneMinute == 0) {
        if(oneSecond == 0) {
          dayLightCheck ++;
        }
      }
    }
    if(oneHour >= 13) {
      oneHour = 1;
    }
    if(dayLightCheck == 1) {
      lcd.setCursor(10, 0);
      lcd.print(" pm");
      dayLightCheck = -1;
    } 
    if(dayLightCheck == 0) {
      lcd.setCursor(10,0);
      lcd.print(" am");
    }
    if(oneHour < 10) {
      lcd.setCursor(3,0);
      lcd.print(oneHour);
      lcd.setCursor(2,0);
      lcd.print("0");
    } else {
      lcd.setCursor(2,0);
      lcd.print(oneHour);
    } 
    if(oneMinute < 10) {
      lcd.setCursor(6,0);
      lcd.print(oneMinute);
      lcd.setCursor(5,0);
      lcd.print("0");
      lcd.setCursor(4,0);
      lcd.print(":");
    } else {
      lcd.setCursor(5,0);
      lcd.print(oneMinute);
      lcd.setCursor(4,0);
      lcd.print(":");
    }
    if(oneSecond < 10) {
      lcd.setCursor(9,0);
      lcd.print(oneSecond);
      lcd.setCursor(8,0);
      lcd.print("0");
      lcd.setCursor(7,0);
      lcd.print(":");
      oneSecond ++;
    } else {
      lcd.setCursor(8,0);
      lcd.print(oneSecond);
      lcd.setCursor(7,0);
      lcd.print(":");
      oneSecond ++;
    }
  }
}

void alarmDisplay() {
  lcd.setCursor(0,1);
  lcd.print("Alarm >");
}

void positionClockDisplay() {
  if(clockIsRunning) {
    stopClock();
    initClockDisplay();
  }

  clockDisplayPosition ++;
  if(clockDisplayPosition == 4){
    resetClockDisplayData();
    startClock();
  }
}

void stopClock() {
  clockIsRunning = false;
}

void startClock() {
  clockIsRunning = true;
}

void initClockDisplay() {
    lcd.setCursor(2,0);
    lcd.print("00:00 am         ");
    oneHour = 0;
    oneMinute = 0;
    oneSecond = 0;
    dayLightCheck = 0;
    soundAlarmGlitch = 1;  
}

void resetClockDisplayData() {
    clockDisplayPosition = 0;
    timeHour = 0;
    timeMinute = 0;
    soundAlarmGlitch = 0;  
}

void setTime() {
  if(clockDisplayPosition == 1) {
     if(upArrowPressed == 1) {
       if(timeHour == 12) {
        lcd.setCursor(2,0);
        lcd.print("01");
        timeHour = 0;
        oneHour = 0;
       } 
       if(timeHour < 9) {
        timeHour ++;
        lcd.setCursor(3,0);
        lcd.print(timeHour);
        oneHour ++;
      } else {
         timeHour ++;
         lcd.setCursor(2,0);
         lcd.print(timeHour);
         oneHour ++;
      }
    }
    if(downArrowPressed == 1) {
       if(timeHour <= 1) {
        lcd.setCursor(2,0);
        lcd.print("12");
        timeHour = 13;
        oneHour = 13;
       } 
       if(timeHour < 10) {
        timeHour --;
        lcd.setCursor(3,0);
        lcd.print(timeHour);
        oneHour --;
      } else {
         timeHour --;
         lcd.setCursor(2,0);
         lcd.print(timeHour);
         oneHour --;
      }
    }
  }
  if(clockDisplayPosition == 2) {
     if(upArrowPressed == 1) {
       if(timeMinute == 59) {
         lcd.setCursor(5,0);
         lcd.print("00");
         timeMinute = 0;
         timeHour --;
         oneMinute = 0;
       } 
       if(timeMinute < 9) {
        timeMinute ++;
        lcd.setCursor(6,0);
        lcd.print(timeMinute);
        oneMinute ++;
      } else {
         timeMinute ++;
         lcd.setCursor(5,0);
         lcd.print(timeMinute);
         oneMinute ++;
      }
    }
    if(downArrowPressed == 1) {
       if(timeMinute <= 1) {
         lcd.setCursor(5,0);
         lcd.print("59");
         timeMinute = 60;
         oneMinute = 60;
       } 
       if(timeMinute < 9) {
        timeMinute --;
        lcd.setCursor(6,0);
        lcd.print(timeMinute);
        oneMinute --;
      } else {
         timeMinute --;
         lcd.setCursor(5,0);
         lcd.print(timeMinute);
         oneMinute --;
      }
    }
  }
  if(clockDisplayPosition == 3) {
    if(timeDayLightCheck <= 0) {
      lcd.setCursor(8,0);
      lcd.print("am");
      timeDayLightCheck ++;
      dayLightCheck = 0;
    }
    if(timeDayLightCheck == 1) {
      lcd.setCursor(8,0);
      lcd.print("pm");
      timeDayLightCheck = -1;
      dayLightCheck = 1;
    }
  }
}

void blinkTimeSet() {
  if(clockDisplayPosition == 1) {
    if(barbar1 <= 0) {
      if(timeHour < 10) {
        lcd.setCursor(3, 0);
        lcd.print(timeHour);
        lcd.setCursor(2, 0);
        lcd.print("0");
        barbar1 ++;
      } else {
        lcd.setCursor(2, 0);
        lcd.print(timeHour);
        barbar1 ++;
      }
    }
    if(barbar1 == 1) {
      lcd.setCursor(2,0);
      lcd.print("  ");
      barbar1 = -1;
    }
  }
  if(clockDisplayPosition == 2) {
    if(barbar2 <= 0) {
      if(timeMinute < 10) {
        lcd.setCursor(6,0);
        lcd.print(timeMinute);
        lcd.setCursor(5,0);
        lcd.print("0");
        barbar2 ++;
      } else {
        lcd.setCursor(5,0);
        lcd.print(timeMinute);
        barbar2 ++;
      }
    }
    if(barbar2 == 1) {
      lcd.setCursor(5,0);
      lcd.print("  ");
      barbar2 = -1;
    }
    if(barbar1 != 0) {
      if(timeHour < 10) {
        lcd.setCursor(3, 0);
        lcd.print(timeHour);
        lcd.setCursor(2, 0);
        lcd.print("0");
        barbar1 ++;
      } else {
        lcd.setCursor(2, 0);
        lcd.print(timeHour);
        barbar1 ++;
      }
    }
  }
  if(clockDisplayPosition == 3) {
    if(barbar3 <= 0) {
      if(timeDayLightCheck == 0) {
        lcd.setCursor(8,0);
        lcd.print("am");
        barbar3 ++;
      } else {
        lcd.setCursor(8,0);
        lcd.print("pm");
        barbar3 ++;
      }
    }
    if(barbar3 == 1) {
      lcd.setCursor(8,0);
      lcd.print("  ");
      barbar3 = -1;
    }
    if(barbar2 != 0) {
       if(timeMinute < 10) {
         lcd.setCursor(6,0);
         lcd.print(timeMinute);
         lcd.setCursor(5,0);
         lcd.print("0");
         barbar2 ++;
        } else {
          lcd.setCursor(5,0);
          lcd.print(timeMinute);
          barbar2 ++;
      } 
    }
  }
  if(clockDisplayPosition == 0) {
    if(barbar3 != 0) {
       if(timeDayLightCheck == 0) {
        lcd.setCursor(8,0);
        lcd.print("am");
        barbar3 ++;
      } else {
        lcd.setCursor(8,0);
        lcd.print("pm");
        barbar3 ++;
      }
    }
  }
}

void setAlarm() {
  if(alarmCount == 1) {
     if(upArrowPressed == 1) {
       if(alarmHour == 12) {
        lcd.setCursor(8,1);
        lcd.print("01");
        alarmHour = 0;
       } 
       if(alarmHour < 9) {
        alarmHour ++;
        lcd.setCursor(9,1);
        lcd.print(alarmHour);
      } else {
         alarmHour ++;
         lcd.setCursor(8,1);
         lcd.print(alarmHour);
      }
    }
    if(downArrowPressed == 1) {
        if(alarmHour <= 1) {
          lcd.setCursor(8,1);
          lcd.print("12");
          alarmHour = 13;
         } 
         if(alarmHour < 11) {
           alarmHour --;
           lcd.setCursor(9,1);
           lcd.print(alarmHour);
         } else {
           alarmHour --;
           lcd.setCursor(8,1);
           lcd.print(alarmHour);
      }
    }
  }
  if(alarmCount == 2) {
     if(upArrowPressed == 1) {
       if(alarmMinute == 59) {
         lcd.setCursor(11,1);
         lcd.print("00");
         alarmMinute = 0;
       } 
       if(alarmMinute < 9) {
        alarmMinute ++;
        lcd.setCursor(12,1);
        lcd.print(alarmMinute);
      } else {
         alarmMinute ++;
         lcd.setCursor(11,1);
         lcd.print(alarmMinute);
      }
    }
    if(downArrowPressed == 1) {
        if(alarmMinute <= 1) {
         lcd.setCursor(11,1);
         lcd.print("59");
         alarmMinute = 60;
       } 
       if(alarmMinute < 10) {
        alarmMinute --;
        lcd.setCursor(12,1);
        lcd.print(alarmMinute);
      } else {
         alarmMinute --;
         lcd.setCursor(11,1);
         lcd.print(alarmMinute);
      }
    }
  }
  if(alarmCount == 3) {
    if(alarmDayLightCheck <= 0) {
      lcd.setCursor(14,1);
      lcd.print("am");
      alarmDayLightCheck ++;
    }
    if(alarmDayLightCheck == 1) {
      lcd.setCursor(14,1);
      lcd.print("pm");
      alarmDayLightCheck = -1;
    }
  }
}

void changeAlarm() {
  if(alarmCount == 0) {
    lcd.setCursor(8,1);
    lcd.print("00:00 am");
  }
  alarmCount ++;
  stopAlarmCount = 0;
  if(alarmCount == 4) {
    alarmCount = 0;
  }
}

void soundAlarm() {
  if(soundAlarmGlitch == 0) {
   if(alarmCount == 0) {
    if(alarmHour == oneHour) {
      soundAlarmCount = 1;
       if(alarmMinute == oneMinute) {
         soundAlarmCount = 2;
          if(alarmDayLightCheck == dayLightCheck) {
            soundAlarmCount = 3;
         }  
      }
    }
    if(soundAlarmCount == 3) {
      keepAlarmGoing = 1;
    }
   }
  }
  if(keepAlarmGoing == 1) {
    if(bazbazbaz <= 0) {
      digitalWrite(buzzer, HIGH);
      bazbazbaz ++;
    }
    if(bazbazbaz == 1) {
      digitalWrite(buzzer, LOW);
      bazbazbaz = -1;
    }
  }
}

void blinkAlarmSet() {
  if(alarmCount == 1) {
    if(foobar1 <= 0) {
      if(alarmHour < 10) {
        lcd.setCursor(9, 1);
        lcd.print(alarmHour);
        lcd.setCursor(8, 1);
        lcd.print("0");
        foobar1 ++;
      } else {
        lcd.setCursor(8, 1);
        lcd.print(alarmHour);
        foobar1 ++;
      }
    }
    if(foobar1 == 1) {
      lcd.setCursor(8,1);
      lcd.print("  ");
      foobar1 = -1;
    }
  }
  if(alarmCount == 2) {
    if(foobar2 <= 0) {
      if(alarmMinute < 10) {
        lcd.setCursor(12,1);
        lcd.print(alarmMinute);
        lcd.setCursor(11,1);
        lcd.print("0");
        foobar2 ++;
      } else {
        lcd.setCursor(11,1);
        lcd.print(alarmMinute);
        foobar2 ++;
      }
    }
    if(foobar2 == 1) {
      lcd.setCursor(11,1);
      lcd.print("  ");
      foobar2 = -1;
    }
    if(foobar1 != 0) {
      if(alarmHour < 10) {
        lcd.setCursor(9, 1);
        lcd.print(alarmHour);
        lcd.setCursor(8, 1);
        lcd.print("0");
        foobar1 ++;
      } else {
        lcd.setCursor(8, 1);
        lcd.print(alarmHour);
        foobar1 ++;
      }
    }
  }
  if(alarmCount == 3) {
    if(foobar3 <= 0) {
      if(alarmDayLightCheck == 0) {
        lcd.setCursor(14,1);
        lcd.print("am");
        foobar3 ++;
      } else {
        lcd.setCursor(14,1);
        lcd.print("pm");
        foobar3 ++;
      }
    }
    if(foobar3 == 1) {
      lcd.setCursor(14,1);
      lcd.print("  ");
      foobar3 = -1;
    }
    if(foobar2 != 0) {
       if(alarmMinute < 10) {
         lcd.setCursor(12,1);
         lcd.print(alarmMinute);
         lcd.setCursor(11,1);
         lcd.print("0");
         foobar2 ++;
        } else {
          lcd.setCursor(11,1);
          lcd.print(alarmMinute);
          foobar2 ++;
      } 
    }
  }
  if(alarmCount == 0) {
    if(foobar3 != 0) {
       if(alarmDayLightCheck == 0) {
        lcd.setCursor(14,1);
        lcd.print("am");
        foobar3 ++;
      } else {
        lcd.setCursor(14,1);
        lcd.print("pm");
        foobar3 ++;
      }
    }
  }
}

void stopAlarm() {
  keepAlarmGoing = 0;
  stopAlarmCount = 1;
  soundAlarmCount = 0;
  alarmCount = 0;
  alarmHour = 0;
  alarmMinute = 0;
}

void turnOnLight() {
  controlLightCount = 1;
  setColor(255, 255, 255);
}

void turnOffLight() {
  setColor(0, 0, 0);
  controlLightCount = 0;
  LEDstatus = 0;
}

void setColor(int redValue, int greenValue, int blueValue) {
  analogWrite(red, redValue);
  analogWrite(green, greenValue);
  analogWrite(blue, blueValue);
}

void changeLEDcolor() {
  if(controlLightCount == 1) {
    if(LEDstatus == 1) {
      setColor(255, 0, 0);
    }
    if(LEDstatus == 2) {
      setColor(0, 255, 0);
    }
    if(LEDstatus == 3) {
      setColor(0, 0, 255);
    }
    if(LEDstatus == 4) {
      setColor(160, 0, 255);
    }
    if(LEDstatus == 0) {
      setColor(255, 255, 255);
    }
    if(LEDstatus > 4) {
      LEDstatus --;
    }
    if(LEDstatus < 0) {
      LEDstatus ++;
    }
  }
}

void loop() {
  if(millis() > time_1 + 998) {
    time_1 = millis();
    Clock();
  }
  if(millis() > time_2 + 300) {
    time_2 = millis();
    blinkAlarmSet();
    blinkTimeSet();
  }
  if(millis() > time_3 + 300) {
    time_3 = millis();
    if(stopAlarmCount == 0){
      soundAlarm();
    } else {
        digitalWrite(buzzer, LOW);
        lcd.setCursor(8,1);
        lcd.print("        ");
    }
  }
  if (irrecv.decode(&results)) {
    if (results.value==0xFF906F ){ // upArrow
        upArrowPressed = 1;
        downArrowPressed = 0;
        setAlarm();
        setTime();
    }
    if (results.value==0xFFE01F ){ // downArrow
        downArrowPressed = 1;
        upArrowPressed = 0;
        setAlarm();
        setTime();
    }
    if (results.value==0xFFA857 ){ // vol- button = "OK" button
       changeAlarm();
    }
    if (results.value==0xFF629D ) { // vol+ button
        positionClockDisplay();
    }
    if (results.value==0xFFE21D ) { // func/stop button
        stopAlarm();
    }
    if (results.value==0xFFA25D ) { // power button
        if(controlLightCount == 0) {
          turnOnLight();
        } else {
          turnOffLight();
        }
    }
    if (results.value==0xFFC23D ) { // >>/ button
        LEDstatus ++;
        changeLEDcolor();
    }
    if (results.value==0xFF22DD ) { // /<< button
        LEDstatus --;
        changeLEDcolor();
    }
    irrecv.resume();
  }
}
