#include <secrets.h>
#include "http.h"
#include "ble.h"
#include "wifi_helper.h"
#include "ntp.h"
#include "alarm.h"

#include <Arduino.h>

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
  set_vibration_weak();
}

void loop() {
  if (connected && is_wifi_connected()) {

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