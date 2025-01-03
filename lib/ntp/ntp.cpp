#include <secrets.h>

#include <Arduino.h>
#include "time.h"
#include "esp_sntp.h"

// TimeZone rule for Europe/Berlin including daylight adjustment rules
// Copied from https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h
#define TIMEZONE "CET-1CEST,M3.5.0,M10.5.0/3"

bool time_available = false;

String get_time() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return "";
  }
  // Create a String to store the formatted time
  char formattedTime[20]; // Enough space for the formatted string "YYYY-MM-DD,HH:MM:SS"
  // Format the time into the string
  strftime(formattedTime, sizeof(formattedTime), "%Y-%m-%d,%H:%M:%S", &timeinfo);
  return String(formattedTime);
}

struct tm get_time_raw() {
  struct tm timeinfo;
  getLocalTime(&timeinfo);
  return timeinfo;
}

bool is_time_available() {
    return time_available;
}


// Callback function (gets called when time adjusts via NTP)
void timeavailable(struct timeval *t) {
    Serial.println("Got time adjustment from NTP!");
    time_available = true;
}

void setup_ntp() {
    // set notification call-back function
    sntp_set_time_sync_notification_cb(timeavailable);

    configTzTime(TIMEZONE, NTP_SERVER);
}