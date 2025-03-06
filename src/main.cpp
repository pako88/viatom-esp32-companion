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

  struct tm timeinfo = get_time_raw();
  if (trigger_alarm(&timeinfo, 20, 0, 23, 30)) {
    set_vibration_weak();
  }

  // Initialize the watchdog timer
  esp_task_wdt_init(30, true); // Set 30 seconds timeout and reset the system on timeout
  esp_task_wdt_add(NULL); // Add the current task (main loop) to the watchdog
}

void loop() {
  if (connected && is_wifi_connected()) {
    struct tm timeinfo = get_time_raw();
    if (trigger_alarm(&timeinfo, 5, 50, 6, 0) && is_device_worn()) {
      Serial.println("ALARM!");
      set_vibration_weak();
    }

    influxBlePayload = get_influx_payload();
    if (influxBlePayload != "") {
      bool success = sendToInflux(influxBlePayload);
      if (success) {
        // Reset the watchdog timer to prevent a reset
        esp_task_wdt_reset();
      }
      set_influx_payload("");
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