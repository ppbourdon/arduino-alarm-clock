#include <RTClib.h>
#include <Wire.h>
RTC_DS1307 rtc;

#include <boarddefs.h>
#include <ir_Lego_PF_BitStreamEncoder.h>
#include <IRremote.h>
#include <IRremoteInt.h>
#include <LiquidCrystal.h>

const int rs = 7, en = 8, d4 = 9, d5 = 10, d6 = 11, d7 = 12;

int RECV_PIN = 6;
int buzzer = 5;
int red = 4;
int green = 3;
int blue = 2;

int LEDstatus = 0;

String currentMenu = "null";
String selectedTopMenuItem = "setup";

int selectedClockHour = 0;
int selectedClockMinute = 0;
int selectedAlarmHour = 0;
int selectedAlarmMinute = 0;
int selectedSnoozeMinute = 0;
String selectedClockAmPm = "AM";
String selectedAlarmAmPm = "AM";
boolean clockIsRunning = false;  

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
}

void resetClockVariables() {
	currentMenu = "null";
	selectedClockHour = 0;
	selectedClockMinute = 0;
	selectedAlarmHour = 0;
	selectedAlarmMinute = 0;
	selectedSnoozeMinute = 0;
	selectedClockAmPm = "AM";
	selectedAlarmAmPm = "AM";
	clockIsRunning = false;
}

void enterOrExitMenu() {
	if(currentMenu == "null") {
		currentMenu = "top";
		displayMenu();	
	} else {
		exitMenu();
	}
}

void displayTopMenu() {
	if(selectedTopMenuItem == "setup") {
		lcd.print("SETUP >");
  		lcd.setCursor(2,1);
		lcd.print("RUN");
	} else {
		lcd.print("SETUP");
  		lcd.setCursor(2,1);
		lcd.print("RUN   >");
	}
}

void clearScreen() {
    lcd.clear();
    lcd.setCursor(2,0);
}

void displaySetClock() {
	lcd.print("SET: CLOCK");
	lcd.setCursor(3, 1);

	if(selectedClockHour < 10) lcd.print("0");
	lcd.print(selectedClockHour);
	lcd.print(":");
	
	if(selectedClockMinute < 10) lcd.print("0");
	lcd.print(selectedClockMinute);
	lcd.print(" ");
	
	lcd.print(selectedClockAmPm);
}

void displaySetAlarm() {
	lcd.print("SET: ALARM");
	lcd.setCursor(3, 1);

	if(selectedAlarmHour < 10) lcd.print("0");
	lcd.print(selectedAlarmHour);
	lcd.print(":");
	
	if(selectedAlarmMinute < 10) lcd.print("0");
	lcd.print(selectedAlarmMinute);
	lcd.print(" ");
	
	lcd.print(selectedAlarmAmPm);
}

void displaySetSnooze() {
	lcd.print("SET: SNOOZE");
	lcd.setCursor(6, 1);

	if(selectedSnoozeMinute < 10) lcd.print("0");
	lcd.print(selectedSnoozeMinute);
}

void displayMenu() {
	clearScreen();

	if(currentMenu == "top") displayTopMenu();
	else if(currentMenu == "top|clock") displaySetClock();
	else if(currentMenu == "top|alarm") displaySetAlarm();
	else if(currentMenu == "top|snooze") displaySetSnooze();
	else if(currentMenu == "top|clock|hour" || currentMenu == "top|alarm|hour") lcd.print("SET: HOUR");
	else if(currentMenu == "top|clock|hour|select") printSelectedValue(selectedClockHour);
	else if(currentMenu == "top|alarm|hour|select") printSelectedValue(selectedAlarmHour);
	else if(currentMenu == "top|clock|minute" || currentMenu == "top|alarm|minute" || currentMenu == "top|snooze|minute") lcd.print("SET: MINUTE");
	else if(currentMenu == "top|clock|minute|select") printSelectedValue(selectedClockMinute);
	else if(currentMenu == "top|alarm|minute|select") printSelectedValue(selectedAlarmMinute);
	else if(currentMenu == "top|snooze|minute|select") printSelectedValue(selectedSnoozeMinute);
	else if(currentMenu == "top|clock|ampm" || currentMenu == "top|alarm|ampm") lcd.print("SET: AM/PM");
	else if(currentMenu == "top|clock|ampm|select") printSelectedAmPm(selectedClockAmPm);
	else if(currentMenu == "top|alarm|ampm|select") printSelectedAmPm(selectedAlarmAmPm);
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
	resetClockVariables();
}

void cycleMenu(String direction) {
	if(currentMenu == "top") {
		if(selectedTopMenuItem == "setup") selectedTopMenuItem = "run";
		else selectedTopMenuItem = "setup";
	} else if(currentMenu == "top|clock") {
		if(direction == "up") currentMenu = "top|snooze";
		else currentMenu = "top|alarm";
	} else if(currentMenu == "top|alarm") {
		if(direction == "up") currentMenu = "top|clock";
		else currentMenu = "top|snooze";
	} else if(currentMenu == "top|snooze") {
		if(direction == "up") currentMenu = "top|alarm";
		else currentMenu = "top|clock";
	} else if(currentMenu == "top|clock|hour") {
		if(direction == "up") currentMenu = "top|clock|ampm";
		else currentMenu = "top|clock|minute";
	} else if(currentMenu == "top|clock|minute") {
		if(direction == "up") currentMenu = "top|clock|hour";
		else currentMenu = "top|clock|ampm";
	} else if(currentMenu == "top|clock|ampm") {
		if(direction == "up") currentMenu = "top|clock|minute";
		else currentMenu = "top|clock|hour";
	} else if(currentMenu == "top|alarm|hour") {
		if(direction == "up") currentMenu = "top|alarm|ampm";
		else currentMenu = "top|alarm|minute";
	} else if(currentMenu == "top|alarm|minute") {
		if(direction == "up") currentMenu = "top|alarm|hour";
		else currentMenu = "top|alarm|ampm";
	} else if(currentMenu == "top|alarm|ampm") {
		if(direction == "up") currentMenu = "top|alarm|minute";
		else currentMenu = "top|alarm|hour";
	} else if(currentMenu == "top|snooze|minute") {
		//do nothing
	}

	displayMenu();
}

void displayClock() {
	DateTime now = rtc.now();
  lcd.setCursor(2,0);

	// display hour
	if(now.hour() > 12) {
		displayPaddedValue(now.hour() - 12);
	} else {
		displayPaddedValue(now.hour());
	}

	// display semicolon
	lcd.print(":");

	// display minute
	displayPaddedValue(now.minute());

	// display semicolon
	lcd.print(":");

	// display second
	displayPaddedValue(now.second());

	lcd.print(" ");

	// display am/pm
	lcd.print(selectedClockAmPm);

  lcd.setCursor(0, 1);
  lcd.print("                ");
}

void displayPaddedValue(int val) {
	if(val < 10) {
		lcd.print("0");
		lcd.print(val);
	} else {
		lcd.print(val);
	}
}

void adjustClock() {
	if(selectedClockAmPm == "AM")
		rtc.adjust(DateTime(0,0,0, selectedClockHour, selectedClockMinute, 0));
	else
		rtc.adjust(DateTime(0,0,0, selectedClockHour + 12, selectedClockMinute, 0));
}

void runClock() {
	adjustClock();
	clearScreen();
	displayClock();
	rtc.begin();
	clockIsRunning = true;
}

void selectCurrentMenuItem() {
	if(currentMenu == "top") {
		if(selectedTopMenuItem == "setup") {
			currentMenu = "top|clock";
		} else {
			runClock();
		}
	}
	else if(currentMenu == "top|clock") currentMenu = "top|clock|hour";
	else if(currentMenu == "top|clock|hour") currentMenu = "top|clock|hour|select";
	else if(currentMenu == "top|clock|minute") currentMenu = "top|clock|minute|select";
	else if(currentMenu == "top|clock|ampm") currentMenu = "top|clock|ampm|select";

	else if(currentMenu == "top|alarm") currentMenu = "top|alarm|hour";
	else if(currentMenu == "top|alarm|hour") currentMenu = "top|alarm|hour|select";
	else if(currentMenu == "top|alarm|minute") currentMenu = "top|alarm|minute|select";
	else if(currentMenu == "top|alarm|ampm") currentMenu = "top|alarm|ampm|select";

	else if(currentMenu == "top|snooze") currentMenu = "top|snooze|minute";
	else if(currentMenu == "top|snooze|minute") currentMenu = "top|snooze|minute|select";

	displayMenu();
}

void gotoParentMenu() {
	if(currentMenu == "top|clock" || currentMenu == "top|alarm" || currentMenu == "top|snooze") currentMenu = "top";
	else if(currentMenu == "top|clock|hour") currentMenu = "top|clock";
	else if(currentMenu == "top|clock|minute") currentMenu = "top|clock";
	else if(currentMenu == "top|clock|ampm") currentMenu = "top|clock";
	else if(currentMenu == "top|clock|hour|select") currentMenu = "top|clock|hour";
	else if(currentMenu == "top|clock|minute|select") currentMenu = "top|clock|minute";
	else if(currentMenu == "top|clock|ampm|select") currentMenu = "top|clock|ampm";
	else if(currentMenu == "top|alarm|hour") currentMenu = "top|alarm";
	else if(currentMenu == "top|alarm|minute") currentMenu = "top|alarm";
	else if(currentMenu == "top|alarm|ampm") currentMenu = "top|alarm";
	else if(currentMenu == "top|alarm|hour|select") currentMenu = "top|alarm|hour";
	else if(currentMenu == "top|alarm|minute|select") currentMenu = "top|alarm|minute";
	else if(currentMenu == "top|alarm|ampm|select") currentMenu = "top|alarm|ampm";
	else if(currentMenu == "top|snooze|minute") currentMenu = "top|snooze";
	else if(currentMenu == "top|snooze|minute|select") currentMenu = "top|snooze|minute";

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
	if(currentMenu == "top|clock|hour|select") cycleHourSelection(direction, &selectedClockHour);
	else if(currentMenu == "top|alarm|hour|select") cycleHourSelection(direction, &selectedAlarmHour);
	else if(currentMenu == "top|clock|minute|select") cycleMinuteSelection(direction, &selectedClockMinute);
	else if(currentMenu == "top|alarm|minute|select") cycleMinuteSelection(direction, &selectedAlarmMinute);
	else if(currentMenu == "top|snooze|minute|select") cycleMinuteSelection(direction, &selectedSnoozeMinute);
	else if(currentMenu == "top|clock|ampm|select") cycleClockAmPmSelection();
	else if(currentMenu == "top|alarm|ampm|select") cycleAlarmAmPmSelection();
	else cycleMenu(direction);
}

void handleRCEvents() {
	if (results.value==0xFFB04F) { // ST/REPT
		enterOrExitMenu();
	}
	if (results.value==0xFF906F ) { // upArrow
		handleArrowEvent("up");
	}
    if (results.value==0xFFE01F ) { // downArrow
		handleArrowEvent("down");
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
  	} else {
		if(clockIsRunning)
			displayClock();
	}
}
