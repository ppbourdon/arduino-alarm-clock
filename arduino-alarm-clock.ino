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

int RECV_PIN = 6;
int buzzer = 5;
int red = 4;
int green = 3;
int blue = 2;

int LEDstatus = 0;
String currentMenu = "null";
int selectedClockHour = 0;
int selectedClockMinute = 0;
int selectedAlarmHour = 0;
int selectedAlarmMinute = 0;
int selectedSnoozeMinute = 0;
String selectedClockAmPm = "AM";
String selectedAlarmAmPm = "AM";

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

  // initialize menu state variables
  //currentMenu = "null";
}

void enterOrExitMenu() {
	if(currentMenu == "null") {
		currentMenu = "clock";
		displayMenu();	
	} else {
		exitMenu();
	}
}

void displayMenu() {
	lcd.setCursor(2,0);
	lcd.print("            ");
  	lcd.setCursor(2,0);

	if(currentMenu == "clock") lcd.print("SET: CLOCK");
	else if(currentMenu == "alarm") lcd.print("SET: ALARM");
	else if(currentMenu == "snooze") lcd.print("SET: SNOOZE");
	else if(currentMenu == "clock|hour" || currentMenu == "alarm|hour") lcd.print("SET: HOUR");
	else if(currentMenu == "clock|hour|select") printSelectedValue(selectedClockHour);
	else if(currentMenu == "alarm|hour|select") printSelectedValue(selectedAlarmHour);
	else if(currentMenu == "clock|minute" || currentMenu == "alarm|minute" || currentMenu == "snooze|minute") lcd.print("SET: MINUTE");
	else if(currentMenu == "clock|minute|select") printSelectedValue(selectedClockMinute);
	else if(currentMenu == "alarm|minute|select") printSelectedValue(selectedAlarmMinute);
	else if(currentMenu == "snooze|minute|select") printSelectedValue(selectedSnoozeMinute);
	else if(currentMenu == "clock|ampm" || currentMenu == "alarm|ampm") lcd.print("SET: AM/PM");
	else if(currentMenu == "clock|ampm|select") printSelectedAmPm(selectedClockAmPm);
	else if(currentMenu == "alarm|ampm|select") printSelectedAmPm(selectedAlarmAmPm);
}

void printSelectedValue(int selectedValue) {
	if(selectedValue < 10) {
		lcd.print("SELECT: 0");
		lcd.setCursor(11,0);		
	} else {
		lcd.print("SELECT: ");
		lcd.setCursor(10,0);		
	}
	lcd.print(selectedValue);
}

void printSelectedAmPm(String selectedValue) {
	if(selectedValue == "AM") lcd.print("SELECT: AM"); 
	else lcd.print("SELECT: PM");
}

void exitMenu() {
	lcd.clear();
	currentMenu = "null";
}

void cycleMenu(String direction) {
	if(currentMenu == "clock") {
		if(direction == "up") currentMenu = "snooze";
		else currentMenu = "alarm";
	} else if(currentMenu == "alarm") {
		if(direction == "up") currentMenu = "clock";
		else currentMenu = "snooze";
	} else if(currentMenu == "snooze") {
		if(direction == "up") currentMenu = "alarm";
		else currentMenu = "clock";
	} else if(currentMenu == "clock|hour") {
		if(direction == "up") currentMenu = "clock|ampm";
		else currentMenu = "clock|minute";
	} else if(currentMenu == "clock|minute") {
		if(direction == "up") currentMenu = "clock|hour";
		else currentMenu = "clock|ampm";
	} else if(currentMenu == "clock|ampm") {
		if(direction == "up") currentMenu = "clock|minute";
		else currentMenu = "clock|hour";
	} else if(currentMenu == "alarm|hour") {
		if(direction == "up") currentMenu = "alarm|ampm";
		else currentMenu = "alarm|minute";
	} else if(currentMenu == "alarm|minute") {
		if(direction == "up") currentMenu = "alarm|hour";
		else currentMenu = "alarm|ampm";
	} else if(currentMenu == "alarm|ampm") {
		if(direction == "up") currentMenu = "alarm|minute";
		else currentMenu = "alarm|hour";
	} else if(currentMenu == "snooze|minute") {
		//do nothing
	}

	displayMenu();
}

void selectCurrentMenuItem() {
	if(currentMenu == "clock") currentMenu = "clock|hour";
	else if(currentMenu == "clock|hour") currentMenu = "clock|hour|select";
	else if(currentMenu == "clock|minute") currentMenu = "clock|minute|select";
	else if(currentMenu == "clock|ampm") currentMenu = "clock|ampm|select";

	else if(currentMenu == "alarm") currentMenu = "alarm|hour";
	else if(currentMenu == "alarm|hour") currentMenu = "alarm|hour|select";
	else if(currentMenu == "alarm|minute") currentMenu = "alarm|minute|select";
	else if(currentMenu == "alarm|ampm") currentMenu = "alarm|ampm|select";

	else if(currentMenu == "snooze") currentMenu = "snooze|minute";
	else if(currentMenu == "snooze|minute") currentMenu = "snooze|minute|select";

	displayMenu();
}

void gotoParentMenu() {
	if(currentMenu == "clock|hour") currentMenu = "clock";
	else if(currentMenu == "clock|minute") currentMenu = "clock";
	else if(currentMenu == "clock|ampm") currentMenu = "clock";
	else if(currentMenu == "clock|hour|select") currentMenu = "clock|hour";
	else if(currentMenu == "clock|minute|select") currentMenu = "clock|minute";
	else if(currentMenu == "clock|ampm|select") currentMenu = "clock|ampm";
	else if(currentMenu == "alarm|hour") currentMenu = "alarm";
	else if(currentMenu == "alarm|minute") currentMenu = "alarm";
	else if(currentMenu == "alarm|ampm") currentMenu = "alarm";
	else if(currentMenu == "alarm|hour|select") currentMenu = "alarm|hour";
	else if(currentMenu == "alarm|minute|select") currentMenu = "alarm|minute";
	else if(currentMenu == "alarm|ampm|select") currentMenu = "alarm|ampm";
	else if(currentMenu == "snooze|minute") currentMenu = "snooze";
	else if(currentMenu == "snooze|minute|select") currentMenu = "snooze|minute";

	displayMenu();
}

void cycleHourSelection(String direction, int *selectedHour) {
	int hour = *selectedHour;

	if(direction == "up") {
		if(hour == 12) hour = 1;
		else hour ++;
	} else {
		if(hour <= 1) hour = 12;
		else hour --;
	}

	*selectedHour = hour;

	displayMenu();
}

void cycleMinuteSelection(String direction, int *selectedMinute) {
	int minute = *selectedMinute;

	if(direction == "up") {
		if(minute == 59) minute = 0;
		else minute ++;
	} else {
		if(minute == 0) minute = 59;
		else minute --;
	}

	*selectedMinute = minute;

	displayMenu();
}

void cycleClockAmPmSelection() {
	
	if(selectedClockAmPm == "AM") selectedClockAmPm = "PM";
	else selectedClockAmPm = "AM";

	displayMenu();
}

void cycleAlarmAmPmSelection() {
  
  if(selectedAlarmAmPm == "AM") selectedAlarmAmPm = "PM";
  else selectedAlarmAmPm = "AM";

  displayMenu();
}

void handleArrowEvent(String direction) {
	if(currentMenu == "clock|hour|select") cycleHourSelection(direction, &selectedClockHour);
	else if(currentMenu == "alarm|hour|select") cycleHourSelection(direction, &selectedAlarmHour);
	else if(currentMenu == "clock|minute|select") cycleMinuteSelection(direction, &selectedClockMinute);
	else if(currentMenu == "alarm|minute|select") cycleMinuteSelection(direction, &selectedAlarmMinute);
	else if(currentMenu == "snooze|minute|select") cycleMinuteSelection(direction, &selectedSnoozeMinute);
	else if(currentMenu == "clock|ampm|select") cycleClockAmPmSelection();
	else if(currentMenu == "alarm|ampm|select") cycleAlarmAmPmSelection();
	else cycleMenu(direction);
}

void handleRCEvents() {
	if (results.value==0xFFB04F) { // ST/REPT
		enterOrExitMenu();
	}
	if (results.value==0xFF906F ) { // upArrow
		handleArrowEvent("up");
		//cycleMenu("up");
	}
    if (results.value==0xFFE01F ) { // downArrow
		handleArrowEvent("down");
		//cycleMenu("down");
	}
    if (results.value==0xFFA857 ) { // vol- button = "OK" button
	}
    if (results.value==0xFF629D ) { // vol+ button
	}
    if (results.value==0xFFE21D ) { // func/stop button
	}
    if (results.value==0xFFA25D ) { // power button
    }
    if (results.value==0xFFC23D ) { // >>| button
		selectCurrentMenuItem();
    }
    if (results.value==0xFF22DD ) { // |<< button
		gotoParentMenu();
    }
    if (results.value==0xFF9867 ) { // EQ button
    }
    if (results.value==0xFF18E7 ) { // 2 button
    }
}

void loop() {
  if (irrecv.decode(&results)) {
	handleRCEvents();
    irrecv.resume();
  }
}
