#include <secrets.h>

#include <Arduino.h>
#include <WiFi.h>

bool is_wifi_connected() {
    return WiFi.status() == WL_CONNECTED;
}

void connect_wifi() {
    bool connected = false;

    while(!connected) {
        WiFi.begin(WIFI_SSID, WIFI_PASS);

        const int limit = 20;
        int i = 0;

        while (WiFi.status() != WL_CONNECTED && i <= limit) {
            delay(500);
            Serial.print(".");
            i++;
        }
        if (is_wifi_connected) {
            connected = true;
        }
    }

    Serial.print("WiFi connected\tIP address: ");
    Serial.print(WiFi.localIP());
    Serial.println();
}