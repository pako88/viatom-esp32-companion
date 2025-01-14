#include <secrets.h>
#include "http.h"
#include "ble.h"
#include "wifi_helper.h"
#include "ntp.h"
#include "alarm.h"

#include <Arduino.h>
#include <esp_task_wdt.h>

String influxBlePayload = "";
bool connected;
time_t now;
time_t unixtime_last_success;

void setup() {
  Serial.begin(9600);

  connect_wifi();
  setup_ntp();
  init_ble();
  while(!is_time_available()) {
    delay(1000);
  }
  connected = connect_device();
  String time = get_time();
  Serial.print("Set date and time on ring: ");
  Serial.println(time);
  set_time(time);
  set_vibration_weak();

  // initialize restart counters
  now = get_unixtime();
  unixtime_last_success = now;

  // Initialize the watchdog timer
  esp_task_wdt_init(30, true); // Set 30 seconds timeout and reset the system on timeout
  esp_task_wdt_add(NULL); // Add the current task (main loop) to the watchdog
}

void loop() {

  now = get_unixtime();
  long diffInSeconds = now - unixtime_last_success;
  if (diffInSeconds > 30) {
    // restart in case of inactivity/errors
    ESP.restart();
  }

  if (connected && is_wifi_connected()) {
    // Reset the watchdog timer to prevent a reset
    esp_task_wdt_reset();

    struct tm timeinfo = get_time_raw();
    if (trigger_alarm(&timeinfo) && is_device_worn()) {
      Serial.println("ALARM!");
      set_vibration_weak();
    }

    influxBlePayload = get_influx_payload();
    if (influxBlePayload != "") {
      bool success = sendToInflux(influxBlePayload);
      if (success) {
        set_influx_payload("");
        unixtime_last_success = now;
      }
    } else {
      request_data();
    }
  } else {
    if (!connected) {
      Serial.print("Disconnected. ");
      connected = connect_device();
    }
    if (!is_wifi_connected()) {
      connect_wifi();
    }
  }
  delay(2000);
}