#include <Wire.h>
#include <Adafruit_GFX.h>
#include <RTClib.h>
#include "Adafruit_LEDBackpack.h"
#include "DHT.h"

#define DHTPIN 2   

// Set to false to display time in 12 hour format, or true to use 24 hour:
#define TIME_24_HOUR true

// I2C address of the display.  Stick with the default address of 0x70
// unless you've changed the address jumpers on the back of the display.
#define DISPLAY_ADDRESS   0x70
#define DHTTYPE DHT11   

// Create display and DS1307 objects.  These are global variables that
// can be accessed from both the setup and loop function below.
Adafruit_7segment clockDisplay = Adafruit_7segment();
RTC_DS1307 rtc = RTC_DS1307();
DHT dht(DHTPIN, DHTTYPE);

// Keep track of the hours, minutes, seconds displayed by the clock.
// Start off at 0:00:00 as a signal that the time should be read from
// the DS1307 to initialize it.
int hours = 0;
int minutes = 0;
int seconds = 0;

// Remember if the colon was drawn on the display so it can be blinked
// on and off every second.
bool blinkColon = true;


void setup() {
  // Setup functidon runs once at startup to initialize the display
  // and DS1307 clock.

  // Setup Serial port to print debug output.
  Serial.begin(57600);
  Serial.println("Clock starting!");

  // Setup the display.
  clockDisplay.begin(DISPLAY_ADDRESS);

  // Setup the DS1307 real-time clock.
  rtc.begin();
  dht.begin();
  
  Wire.begin();
  // Set the DS1307 clock if it hasn't been set before.
  bool setClockTime = !rtc.isrunning();
  // Alternatively you can force the clock to be set again by
  // uncommenting this line:
  setClockTime = true;
  if (setClockTime) {
    Serial.println("Setting DS1307 time!");
    // This line sets the DS1307 time to the exact date and time the
    // sketch was compiled:
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // Alternatively you can set the RTC with an explicit date & time,
    // for example to set January 21, 2014 at 3am you would uncomment:
    //rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
}

void loop() {
  // Loop function runs over and over again to implement the clock logic.

  // Check if it's the top of the hour and get a new time reading
  // from the DS1307.  This helps keep the clock accurate by fixing
  // any drift.
  if (minutes == 0) {
    DateTime now = rtc.now();
    hours = now.hour();
    minutes = now.minute();
  }

  int t = dht.readTemperature();
  int h = dht.readHumidity();

 
  double temp = (double)t;
  double higr = (double)h;

  clockDisplay.print(temp,2);
  clockDisplay.writeDigitAscii(3,94); //94
  clockDisplay.writeDigitAscii(4,67);
  
  clockDisplay.writeDisplay();
  delay(5000);
  seconds += 5;
  clockDisplay.println();


  clockDisplay.print(higr,2);
  clockDisplay.writeDigitAscii(3,0);
  clockDisplay.writeDigitAscii(4,72);
  clockDisplay.writeDisplay();
  delay(5000);
  seconds += 5;
  clockDisplay.println();


  int displayValue = hours*100 + minutes;

  // Do 24 hour to 12 hour format conversion when required.
  if (!TIME_24_HOUR) {
    // Handle when hours are past 12 by subtracting 12 hours (1200 value).
    if (hours > 12) {
      displayValue -= 1200;
    }
    // Handle hour 0 (midnight) being shown as 12.
    else if (hours == 0) {
      displayValue += 1200;
    }
  }
  
  // Now print the time value to the display.
  clockDisplay.print(displayValue, DEC);

  
  clockDisplay.print(displayValue, DEC);

  // Add zero padding when in 24 hour mode and it's midnight.
  // In this case the print function above won't have leading 0's
  // which can look confusing.  Go in and explicitly add these zeros.
  if (TIME_24_HOUR && hours == 0) {
    // Pad hour 0.
    clockDisplay.writeDigitNum(1, 0);
    // Also pad when the 10's minute is 0 and should be padded.
    if (minutes < 10) {
      clockDisplay.writeDigitNum(3, 0);
    }
  }

  // Blink the colon by flipping its value every loop iteration
  // (which happens every second).
  blinkColon = !blinkColon;
  clockDisplay.drawColon(blinkColon);

  // Now push out to the display the new values that were set above.
  clockDisplay.writeDisplay();
  
  // Pause for a second for time to elapse.  This value is in milliseconds
  // so 1000 milliseconds = 1 second.
  seconds += 3;
  delay(3000);
  // Now increase the seconds by one.
  seconds += 1;
  // If the seconds go above 59 then the minutes should increase and
  // the seconds should wrap back to 0.
  if (seconds > 59) {
    seconds = 0;
    minutes += 1;
    // Again if the minutes go above 59 then the hour should increase and
    // the minutes should wrap back to 0.
    if (minutes > 59) {
      minutes = 0;
      hours += 1;
      // Note that when the minutes are 0 (i.e. it's the top of a new hour)
      // then the start of the loop will read the actual time from the DS1307
      // again.  Just to be safe though we'll also increment the hour and wrap
      // back to 0 if it goes above 23 (i.e. past midnight).
      if (hours > 23) {
        hours = 0;
      }
    }
  }
  Serial.println(displayValue);
  Serial.println(seconds);
  // Loop code is finished, it will jump back to the start of the loop
  // function again!
}
