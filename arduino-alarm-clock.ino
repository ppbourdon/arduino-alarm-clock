#include <RTClib.h>
#include <Wire.h>
RTC_DS1307 rtc;

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
boolean amFlag = false;
boolean updateAmPm = false;
boolean blinkFlag = true;

String amPmDisplayString = "";

int foobar1 = 0; //these are just random ints to help keep spots within certain functions
int foobar2 = 0;
int foobar3 = 0;
int barbar1 = 0;
int barbar2 = 0;
int barbar3 = 0;
int foobarbaz = 0;

int soundAlarmGlitch = 0; // this int prevents the alarm from going off when trying to change the time

int alarmHour = 0; 
int alarmMinute = 0;
int alarmDayLightCheck = 0;
int alarmSet = 0;

int timeHour = 0; // seperate ints to change time
int timeMinute = 0; // ^

int RECV_PIN = 6;
int buzzer = 5;
int red = 4;
int green = 3;
int blue = 2;

int currentSecond = 58; // set starting second
int currentMinute = 59; // set starting minute
int currentHour = 11; // set starting hour
int currentYear = 2019;
int currentMonth = 5;
int currentDay = 11;

int alarmCount = 0;
int clockDisplayPosition = 1;
int soundAlarmCount = 0;
int upArrowPressed = 0;
int downArrowPressed = 0;
int stopAlarmCount = 0;
int controlLightCount =0;
int keepAlarmGoing = 0;

int LEDstatus = 0;

int snoozeTime = 8;
int pressSnoozeOnce = 0;
int blinkSnoozeTime = 0;
int snoozeFoo = 0;
int pressSnoozeButton = -1;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
IRrecv irrecv(RECV_PIN);
decode_results results;

void setup() {
  Wire.begin();
  rtc.begin();
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(buzzer, OUTPUT);
  lcd.begin(16, 2);  
  irrecv.enableIRIn();
  alarmDisplay();
}

void toggleAmPm() {
  DateTime now = rtc.now();

  if(now.hour() == 12 && now.minute() == 0 && now.second() == 0) {
    if(amPmDisplayString == " am") {
      amPmDisplayString = " pm";
    } else {
      amPmDisplayString = " am";
    }
  }
}

void printAmPm() {
    toggleAmPm();
    if(updateAmPm) {
      lcd.setCursor(10, 0);
      lcd.print(amPmDisplayString);      
    }
}

void printPaddedHour(int hour) {
  if(hour < 1) hour = 12;
  if(hour > 12) hour = 1;
  
  lcd.setCursor(2,0);

  if(hour < 10) {
    lcd.print("0");
    lcd.setCursor(3,0);
  }

  lcd.print(hour, DEC);
}

void printHourMinuteColon() {
  lcd.setCursor(4,0);
  lcd.print(":");
}

void printPaddedMinute(int minute) {
  if(timeMinute < 0) timeMinute = 59;
  if(timeMinute > 59) timeMinut = 0;
  
  lcd.setCursor(5,0);

  if(minute < 10) {
    lcd.print("0");
    lcd.setCursor(6,0);
  }

  lcd.print(minute, DEC);
}

void printMinuteSecondColon() {
  lcd.setCursor(7, 0);
  lcd.print(":");
}

void printPaddedSecond() {
  lcd.setCursor(8,0);

  if(rtc.now().second() < 10) {
    lcd.print("0");
    lcd.setCursor(9,0);
  }

  lcd.print(rtc.now().second(), DEC);
}

void printTime() {
    printPaddedHour(rtc.now().hour());
    printHourMinuteColon();
    printPaddedMinute(rtc.now().minute());
    printMinuteSecondColon();
    printPaddedSecond();
    printAmPm(); 
}

void Clock() {
  if(clockIsRunning) {
    printTime();
  }
}

void alarmDisplay() {
  lcd.setCursor(0,1);
  lcd.print("Alarm > ");
}

void positionClockDisplay() {
  if(clockIsRunning) {
    stopClock();
  }

  initClockDisplay();
}

void stopClock() {
  clockIsRunning = false;
}

void startClock() {
  rtc.adjust(DateTime(currentYear, currentMonth, currentDay, currentHour, currentMinute, currentSecond));
  clockIsRunning = true;
}

void setClockDisplay() {
	if(clockDisplayPosition == 0) { // start state
		initClockDisplay();
	}
	if(clockDisplayPosition == 1) { // hour state
		setClockHour();
	}
	if(clockDisplayPosition == 2) { // minute state
		setClockMinute();
	}
	if(clockDisplayPosition == 3) { // ampm state
		setClockAmPm();
	}
}

void initClockDisplay() {
   	lcd.setCursor(2,0);
   	lcd.print("00:00 am         ");

    currentHour = 0;
    currentMinute = 0;
    currentSecond = 0;
    soundAlarmGlitch = 1;  
}

void incrementTimeHour() {
	if(timeHour == 12)
		timeHour = 1; 
	else
		timeHour ++;
}

void decrementTimeHour() {
	if(timeHour == 1) 
		timeHour = 12; 
	else
		timeHour --;	
}

void incrementTimeMinute() {
	if(timeMinute == 59)
		timeMinute = 0;
	else
		timeMinute ++;
}

void decrementTimeMinute() {
	if(timeMinute == 0) 
		timeMinute = 59; 
	else
		timeMinute --;	
}

void toggleTimeAmPm() {
	if(amPmDisplayString == " am")
		amPmDisplayString = " pm";
	else
		amPmDisplayString = " am";
}

void setClockHour() {
	printPaddedHour(timeHour);

	if(upArrowPressed == 1)
		incrementTimeHour();

	if(downArrowPressed == 1)
		decrementTimeHour();

	incrementDisplayPosition();
}

void setClockMinute() {
	printPaddedMinute(timeMinute);

	if(upArrowPressed == 1)
		incrementTimeMinute();

	if(downArrowPressed == 1)
		decrementTimeMinute();

	incrementDisplayPosition();
}

void setClockAmPm() {
	toggleTimeAmPm();	
	setAMPM();
	
	incrementDisplayPosition();
}

void incrementDisplayPosition() {
	if(clockDisplayPosition == 4) {
		clockDisplayPosition = 0;
	} else {
		clockDisplayPosition ++;
	}
}

void resetClockDisplayData() {
    clockDisplayPosition = 0;
    timeHour = 0;
    timeMinute = 0;
    soundAlarmGlitch = 0;  
}

void setAMPM() {
  lcd.setCursor(8,0);
  lcd.print(amPmDisplayString);

  updateAmPm = true;
}

void setTime() {
  if(clockDisplayPosition == 1) {
    setClockHour(); 
  }
  
  if(clockDisplayPosition == 2) {
    setClockMinute();
  }
  
  if(clockDisplayPosition == 3) {
    setAMPM();
  }
}

void printBlankHour() {
  lcd.setCursor(2,0);
  lcd.print("  ");
}

void printBlankMinute() {
  lcd.setCursor(5,0);
  lcd.print("  ");
}

void printBlinkHour() {
    if(blinkFlag) {
      printBlankHour();
      blinkFlag = false;
    } else {
      printPaddedHour(timeHour);
      blinkFlag = true;
    }  
}

void printBlinkMinute() {
    if(blinkFlag) {
      printBlankMinute();
      blinkFlag = false;
    } else {
      printPaddedMinute(timeHour);
      blinkFlag = true;
    }   
}

void blinkTimeSet() {
  if(clockDisplayPosition == 1) {
    printBlinkHour();
  }
  
  if(clockDisplayPosition == 2) {
    printBlinkMinute();
    // race condition warning
    // must print the hour again in case it was last cleared by millis loop
    printPaddedHour(timeHour);
  }

  if(clockDisplayPosition == 3) {
    printBlinkAmPm();
    // race condition warning
    // must print the hour again in case it was last cleared by millis loop
    printPaddedMinute(timeMinute);  
  }
 
  if(clockDisplayPosition == 0) {
    if(barbar3 != 0) {
       if(amPmDisplayString == " am") {
        lcd.setCursor(8,0);
        lcd.print(" am");
        barbar3 ++;
      } else {
        lcd.setCursor(8,0);
        lcd.print(" pm");
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
         alarmMinute = -1;
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
    if(alarmDayLightCheck == 0) {
      lcd.setCursor(14,1);
      lcd.print("am");
      alarmDayLightCheck ++;
    }
    if(alarmDayLightCheck == 1) {
      lcd.setCursor(14,1);
      lcd.print("pm");
      alarmDayLightCheck --;
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

int dayLightCheck() {
	if(amPmDisplayString == " am")
		return 0;
	else
		return 1;
}

void soundAlarm() {
  DateTime now = rtc.now();

  if(soundAlarmGlitch == 0) {
   if(alarmCount == 0) {
    if(now.hour() == alarmHour) {
      soundAlarmCount = 1;
       if(now.minute() == alarmMinute) {
         soundAlarmCount = 2;
          if(alarmDayLightCheck == dayLightCheck()) {
            soundAlarmCount = 3;
         }  
      }
    }
    if(soundAlarmCount == 3) {
      startAlarm();
    }
   }
  }
  if(keepAlarmGoing == 1) {
    if(foobarbaz <= 0) {
      digitalWrite(buzzer, HIGH);
      foobarbaz ++;
    }
    if(foobarbaz == 1) {
      stopBuzzer();
      foobarbaz = -1;
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
      if(amPmDisplayString == " am") {
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
      if(amPmDisplayString == " am") {
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
  pressSnoozeOnce = 0;
}

void startAlarm() {
  keepAlarmGoing = 1;
  pressSnoozeOnce = 0;
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

void snooze() {
  stopAlarmCount = 1;
  alarmMinute = alarmMinute + snoozeTime;
  lcd.setCursor(0,1);
  lcd.print("Snoozing | ");
  lcd.setCursor(11, 1);
  lcd.print(snoozeTime);
  lcd.setCursor(12, 1);
  lcd.print("mins");
  soundAlarmCount = 0;
  keepAlarmGoing = 0;
  stopAlarmCount = 0;
  stopBuzzer();
  pressSnoozeOnce = 1;
}

void initSnoozeDisplay() {
  pressSnoozeButton ++;
  if(pressSnoozeButton == 1) {
    lcd.setCursor(0,1);
    lcd.print("                ");
    alarmDisplay();
    pressSnoozeButton = -1;
    blinkSnoozeTime = 0;
  }
  if(pressSnoozeButton == 0) {
    lcd.setCursor(0,1);
    lcd.print("Snooze for");
    lcd.setCursor(12,1);
    lcd.print("mins");
    blinkSnoozeTime = 1;
  }
}

void blinkSnoozeDisplay() {
  if(blinkSnoozeTime == 1) {
    snoozeFoo ++;
    if(snoozeFoo == 0) {
      if(snoozeTime < 10) {
        lcd.setCursor(11,1);
        lcd.print(snoozeTime);
      } else {
        lcd.setCursor(10,1);
        lcd.print(snoozeTime);
      }
    }
    if(snoozeFoo == 1) {
      lcd.setCursor(10,1);
      lcd.print("  ");
      snoozeFoo = -1;
    }
  }
}

void setSnoozeTime() {
  if(blinkSnoozeTime == 1) {
    if(upArrowPressed == 1) {
      snoozeTime ++;
    }
    if(downArrowPressed == 1) {
      snoozeTime --;
    }
    if(snoozeTime < 10) {
     lcd.setCursor(11,1);
     lcd.print(snoozeTime);
    } else {
     lcd.setCursor(10,1);
     lcd.print(snoozeTime);
    }
  }
  if(snoozeTime <= 0) {
    snoozeTime = 1;
  }
}

void stopBuzzer() {
  digitalWrite(buzzer, LOW);
}

void loop() {
  DateTime now = rtc.now();
  Clock();
  
  if(millis() > time_2 + 300) {
    time_2 = millis();
    blinkAlarmSet();
    blinkTimeSet();
    blinkSnoozeDisplay();
  }
  if(millis() > time_3 + 300) {
    time_3 = millis();
    if(stopAlarmCount == 0){
      soundAlarm();
    } else {
        stopBuzzer();
    }
  }
  if (irrecv.decode(&results)) {
    if (results.value==0xFF906F ){ // upArrow
        upArrowPressed = 1;
        downArrowPressed = 0;
        setAlarm();
        setTime();
        setSnoozeTime();
    }
    if (results.value==0xFFE01F ){ // downArrow
        downArrowPressed = 1;
        upArrowPressed = 0;
        setAlarm();
        setTime();
        setSnoozeTime();
    }
    if (results.value==0xFFA857 ){ // vol- button = "OK" button
       alarmDisplay();
       changeAlarm();
    }
    if (results.value==0xFF629D ) { // vol+ button
        positionClockDisplay();
    }
    if (results.value==0xFFE21D ) { // func/stop button
        stopAlarm();
        lcd.setCursor(8,1);
        lcd.print("        ");
        alarmDisplay();
    }
    if (results.value==0xFFA25D ) { // power button
        if(controlLightCount == 0) {
          turnOnLight();
        } else {
          turnOffLight();
        }
    }
    if (results.value==0xFFC23D ) { // >>| button
        LEDstatus ++;
        changeLEDcolor();
    }
    if (results.value==0xFF22DD ) { // |<< button
        LEDstatus --;
        changeLEDcolor();
    }
    if (results.value==0xFF9867 ) { // EQ button
        if(pressSnoozeOnce == 0) {
          snooze();
        }
    }
    if (results.value==0xFF18E7 ) { // 2 button
        initSnoozeDisplay();
    }
    if (results.value==0xFFC23D ) { // >>| button
        initSnoozeDisplay();
    }
    irrecv.resume();
  }
}
