/*
  LiquidCrystal Library - display() and noDisplay()
 
 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the 
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.
 
 This sketch prints "Hello World!" to the LCD and uses the 
 display() and noDisplay() functions to turn on and off
 the display.
 
 The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 
 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe 
 modified 22 Nov 2010
 by Tom Igoe

 This example code is in the public domain.

 http://arduino.cc/en/Tutorial/LiquidCrystalDisplay

 */

// include the library code:
#include <Wire.h>
#include "RTClib.h"
#include <LiquidCrystal.h>

#if defined(ARDUINO_ARCH_SAMD)
// for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
   #define Serial SerialUSB
#endif

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Create a stub fot the Real Time Clock
RTC_DS1307 RTC;

// set pin numbers:
const int PinHrDa = 6;     // the number of the pushbutton pin Hour/Day
const int PinMnMh = 7;     // the number of the pushbutton pin Min/Month
const int PinStYr = 8;     // the number of the pushbutton pin Set/Year
const int PinTiDa = 9;     // the number of the switch pin Time/Date
const int PinHrs = A2;      // the number of the o/p of hour pin 
const int Pin5Mins = A1;    // the number of the o/p if 5 Minutes pin
const int PinSecs = A0;     // the number of the o/p of Seconds pin
const int PinClr = A3;     // the number of the o/p of Display Clear pin
const int PinHrPls = 10;     // the number of the o/p of Display Initial Hour Pulse pin

const int Period = 5;      // the number of microseconds for pulses 
const int Delay = 500;      // the number of milliseconds for LED display to set itself

// variables will change:
int IncMins = false;         // variable to signal change in Mins
int IncHrs = false;         // variable to signal change in Hrs
int Reset = false;         // variable to signal change values of Mins and Hours for clock
int HoldSecs = false;         // variable for Holding the seconds
int Hrs = 14;         // variable for storing the hour
int Mins =40;         // variable for storing the minutes
int Secs = 0;         // variable for storing the seconds
int Days = 29;         // variable for storing the Day
int Mths = 6;         // variable for storing the Month
int Year = 2016;         // variable for storing the Year
int StateHrs = 0;         // variable for reading the pushbutton status
int StateMins = 0;         // variable for reading the pushbutton status
int StateSet = 0;         // variable for reading the pushbutton status
int StateDays = 0;         // variable for reading the pushbutton status
int StateMths = 0;         // variable for reading the pushbutton status
int StateYear = 0;         // variable for reading the pushbutton status
int StateTiDa = 0;         // variable for reading the pushbutton status
int pushHrs = false;         // variable for reading the pushbutton status
int pushMins = false;         // variable for reading the pushbutton status
int pushSet = false;         // variable for reading the pushbutton status
int pushDays = false;         // variable for reading the pushbutton status
int pushMths = false;         // variable for reading the pushbutton status
int pushYear = false;         // variable for reading the pushbutton status
int OldHrs = Hrs;            // variable to check foe changes in Hrs
int OldMins = Mins;            // variable to check foe changes in Mins 
int OldSecs = Secs;            // variable to check foe changes in Secs


void setup() {
  
  // Set up the mechanism fo the RTC
  Serial.begin(57600);
  if (! RTC.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  // Include as the target arduino is a Micro
  #ifndef ESP8266
    while (!Serial); // for Leonardo/Micro/Zero
  #endif
  Wire.begin();
  
  // For initial setting up of DS1307  
  // RTC.adjust(DateTime(Year, Mths, Days, Hrs, Mins, Secs));
  
  // For subsequent switch on of unit
  //Set the stub for the current time
  DateTime now = RTC.now();
  Year = now.year();
  Mths = now.month();
  Days = now.day();
  Hrs = now.hour();
  Mins = now.minute();
  Secs = now.second();
  
  // Store these values before they change
  // store the old values of the Hrs, Mins, Secs
  OldHrs = Hrs;
  OldMins = Mins;
  OldSecs = Secs;
  
  // adjust the LED display to coincide with the time on the chip
  // first allow it to settle
  delay(Delay);
 
  // initialize the pushbutton pins as an input:
  pinMode(PinHrDa, INPUT);     
  pinMode(PinMnMh, INPUT);     
  pinMode(PinStYr, INPUT);     
  pinMode(PinTiDa, INPUT);
  // initialse o/p pins to led display
  pinMode(PinHrs, OUTPUT); 
  pinMode(Pin5Mins, OUTPUT); 
  pinMode(PinSecs, OUTPUT);
  pinMode(PinClr, OUTPUT);
  pinMode(PinHrPls, OUTPUT);
  // set correct o/p
  digitalWrite(PinHrs, LOW);
  digitalWrite(Pin5Mins, LOW);
  digitalWrite(PinSecs, LOW);
  digitalWrite(PinClr, HIGH);
  digitalWrite(PinHrPls, LOW);
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // Print a message to the LCD.
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(10, 1);
  lcd.print(":");
  // set the cursor to column 11, line 1
  lcd.setCursor(13, 1);
  lcd.print(":");
  // To set the RTC to the date and time of compilation
  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    // RTC.adjust(DateTime(2016, 6, 28, 21, 21, 0));
        // This line sets the RTC with an explicit date & time, for example to set
        // January 21, 2014 at 3am you would call:
        // RTC.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  
  // Clear the display by a pulse on the Clr line
    digitalWrite(PinClr, LOW);
    delayMicroseconds(Period);
    digitalWrite(PinClr, HIGH);
    delayMicroseconds(Period);

  // first puse must have HrPls line high then for subsequent pulses it is low
  digitalWrite(PinHrPls, HIGH);
  delayMicroseconds(Period);
  digitalWrite(PinHrs, HIGH);
  delayMicroseconds(Period);
  digitalWrite(PinHrs, LOW);
  delayMicroseconds(Period);
  digitalWrite(PinHrPls, LOW);
  delayMicroseconds(Period);

 
  // then pulse the Hours to set it to the correct time 
  for(int i=0;i<=Hrs - 1;i++) {
    digitalWrite(PinHrs, HIGH);
    delayMicroseconds(Period);
    digitalWrite(PinHrs, LOW);
    delayMicroseconds(Period);
  }
  
  // then pulse the 5Mins to set it to the correct time
  for(int i=0;i<Mins/5;i++) {
    digitalWrite(Pin5Mins, HIGH);
    delayMicroseconds(Period);
    digitalWrite(Pin5Mins, LOW);
    delayMicroseconds(Period);
  }   
}

void loop() {
  
  // read the state of the switch Time/Date:
  StateTiDa = digitalRead(PinTiDa);
  // check if the switch is set to Time or Date.
  // if it is set to Time, the buttonState is HIGH:
  if (StateTiDa == HIGH) {
    // the switch is set to Time so ...

    // read the state of the pushbutton Hrs:
    StateHrs = digitalRead(PinHrDa);
    // check if the pushbutton is pressed.
    // if it is, the buttonState is HIGH:
    if (StateHrs == HIGH && !pushHrs) {
      Hrs += 1;
      if (Hrs > 23) Hrs = 0;
      pushHrs = true;
      Reset = true;
    }
    if (StateHrs == LOW && pushHrs) pushHrs = false;
    
    // read the state of the pushbutton Mins:
    StateMins = digitalRead(PinMnMh);
    // check if the pushbutton is pressed.
    // if it is, the buttonState is HIGH:
    if (StateMins == HIGH && !pushMins) {
      Mins += 1;
      if (Mins > 59) Mins = 0;
      pushMins = true;
      Reset = true;
    }
    if (StateMins == LOW && pushMins) pushMins = false;
  
    // read the state of the pushbutton Set:
    StateSet = digitalRead(PinStYr);
    // check if the pushbutton is pressed.
    // if it is, the buttonState is HIGH:
    if (StateSet == HIGH && !pushSet) {
      Secs = 0; // reset Secs
      pushSet = true;
      Reset = true;
    }
    if (StateSet == HIGH && pushSet) {
      Secs = 0; // Hold the seconds at Zero until button released
      Reset = true;
    }
    if (StateSet == LOW && pushSet) pushSet = false; // when button released reset the pushSet variable
    
    
  }
  if (StateTiDa == LOW) {
    // the switch is set to Date so...
    
    // read the state of the pushbutton Days:
    StateDays = digitalRead(PinHrDa);
    // check if the pushbutton is pressed.
    // if it is, the buttonState is HIGH:
    if (StateDays == HIGH && !pushDays) {
      Days += 1;
      if (Days > EndMonth(Mths, Year)) Days = 1;
      Reset = true;
      pushDays = true;
    }
    if (StateDays == LOW && pushDays) pushDays = false;
    
     // read the state of the pushbutton Months:
    StateMths = digitalRead(PinMnMh);
    // check if the pushbutton is pressed.
    // if it is, the buttonState is HIGH:
    if (StateMths == HIGH && !pushMths) {
      Mths += 1;
      if (Mths > 12) Mths = 1;
      Reset = true;
      pushMths = true;
    }
    if (StateMths == LOW && pushMths) pushMths = false;
    
    // read the state of the pushbutton Years:
    StateYear = digitalRead(PinStYr);
    // check if the pushbutton is pressed.
    // if it is, the buttonState is HIGH:
    // and the Day button is not held down
    if (StateYear == HIGH && !pushYear && !pushMths) {
      Year += 1;
      Reset = true;
      pushYear = true;
    }
    // check if the pushbutton is pressed.
    // if it is, the buttonState is HIGH:
    // and the Day button is held down decrease the year value by one
    if (StateYear == HIGH && !pushYear && pushMths) {
      Year -= 1;
      Mths -= 1; // decriment Months back to non-alteration
      if(Mths == 0) Mths = 12; // ditto
      pushMths = false; // cancel this value so that the Months does not increase
      Reset = true;
      pushYear = true;
    }
    if (StateYear == LOW && pushYear) pushYear = false;
    
  }
  // Adjust the value of the clock chip as adjustments are being made 
  if(Reset) {
    RTC.adjust(DateTime(Year, Mths, Days, Hrs, Mins, Secs));
    
  }
  // January 21, 2014 at 3am you would call:
  // RTC.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  
  //Set the stub for the current time
  DateTime now = RTC.now();

  Serial.println(now.day(), DEC);


  // Display Day of Month
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 0);
  lcd.print(now.day(), DEC);
  // add st nd rd th ro numerals
  lcd.print(text(now.day()));

  // Display Month
  lcd.setCursor(5, 0);
  int Month = now.month();
  switch(Month){
    case 1:
    lcd.print(" Jan ");
    break;
    case 2:
    lcd.print(" Feb ");
    break;
    case 3:
    lcd.print("March");
    break;
    case 4:
    lcd.print("April");
    break;
    case 5:
    lcd.print(" May ");
    break;
    case 6:
    lcd.print(" June");
    break;
    case 7:
    lcd.print(" July");
    break;
    case 8:
    lcd.print(" Aug ");
    break;
    case 9:
    lcd.print(" Sept");
    break;
    case 10:
    lcd.print(" Oct ");
    break;
    case 11:
    lcd.print(" Nov ");
    break;
    case 12:
    lcd.print(" Dec ");
    break;
    // left on to handle case where result is 0
    case 0:
    lcd.print("error");
    break;
    delay(50);
  }
  
  // Display Year
  lcd.setCursor(12, 0);
  lcd.print(now.year(), DEC);
  
  // Display Day of the week
  lcd.setCursor(0, 1);
  int dayofweek = now.dayOfTheWeek();
  switch(dayofweek){
    case 1:
    lcd.print("Mon ");
    break;
    case 2:
    lcd.print("Tues");
    break;
    case 3:
    lcd.print("Wed ");
    break;
    case 4:
    lcd.print("Thur");
    break;
    case 5:
    lcd.print("Fri ");
    break;
    case 6:
    lcd.print("Sat ");
    break;
    case 0:
    lcd.print("Sun ");
    break;
    delay(50);
  }
    
  // Display hours
  lcd.setCursor(8, 1);
  if (now.hour() < 10) lcd.print('0');
  lcd.print(now.hour(), DEC);
  
  // display Mins
  lcd.setCursor(11, 1);
  if (now.minute() < 10) lcd.print('0');
  lcd.print(now.minute(), DEC);
  
  // Display Seconds
  lcd.setCursor(14, 1);
  if (now.second() < 10) lcd.print('0');
  lcd.print(now.second(), DEC);
  
  // if the Seconds have changed then change the Sec o/p
  if(now.second() != OldSecs) {
    if(now.second() % 2 == 0) digitalWrite(PinSecs, LOW);
    if(now.second() % 2 == 1) digitalWrite(PinSecs, HIGH);
  }
  
  // if hours or minutes have changed then reset the display
  if((now.minute() != OldMins && now.minute() % 5 == 0) || now.hour() != OldHrs) {
    
    // Clear the display by a pulse on the Clr line
    digitalWrite(PinClr, LOW);
    delayMicroseconds(Period);
    digitalWrite(PinClr, HIGH);
    delayMicroseconds(Period);
    
    // first puse must have HrPls line high then for subsequent pulses it is low
    digitalWrite(PinHrPls, HIGH);
    delayMicroseconds(Period);
    digitalWrite(PinHrs, HIGH);
    delayMicroseconds(Period);
    digitalWrite(PinHrs, LOW);
    delayMicroseconds(Period);
    digitalWrite(PinHrPls, LOW);
    delayMicroseconds(Period);
 
    // then pulse the Hours to set it to the correct time 
    for(int i=0;i<=now.hour() - 1;i++) {
      digitalWrite(PinHrs, HIGH);
      delayMicroseconds(Period);
      digitalWrite(PinHrs, LOW);
      delayMicroseconds(Period);
    }
  
    // then pulse the 5Mins to set it to the correct time
    for(int i=0;i<now.minute()/5;i++) {
      digitalWrite(Pin5Mins, HIGH);
      delayMicroseconds(Period);
      digitalWrite(Pin5Mins, LOW);
      delayMicroseconds(Period);
    }
  }
  
  // store the old values of the Hrs, Mins, Secs
  OldHrs = now.hour();
  OldMins = now.minute();
  OldSecs = now.second();

  // the values have change and recorded so reset the trip
  Reset = false;
}

// calculate the number of days in the month
int EndMonth(int MonthNo, int YearNo) {
  if(MonthNo == 4 || MonthNo == 6 || MonthNo == 9 || MonthNo == 11) {
    return 30;
  }
  if(MonthNo == 2) {
    if(YearNo%400 == 0) return 28;
    if(YearNo%100 == 0) return 29;
    if(YearNo%4 == 0) return 28;
    else return 29;
  }
  return 31;
}

// work out text after the numerals of the day of the month
String text(int DayNo) {
  if(DayNo == 1 || DayNo == 21 || DayNo == 31) return "st ";
  if(DayNo == 2 || DayNo == 22) return "nd";
  if(DayNo == 3 || DayNo == 23) return "rd";
  return "th";
}

  





