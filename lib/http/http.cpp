#include <secrets.h>

#include <Arduino.h>
#include <HTTPClient.h>

bool sendToInflux(String payload) {
  // Check if the ESP32 is connected to Wi-Fi
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    // Begin HTTP request
    http.begin(String(INFLUX_URL) + "/api/v2/write?org=" + String(INFLUX_ORG) + "&bucket=" + String(INFLUX_BUCKET) + "&precision=s");

    // Set content type to text/plain
    http.addHeader("Content-Type", "text/plain");

    // Add Authorization Header
    String token = "Token " + String(INFLUX_API_TOKEN);
    http.addHeader("Authorization", token);

    // Send HTTP POST request
    int httpCode = http.POST(payload);
    
    // Check for a successful response
    bool success;
    if (httpCode == 204) {
      Serial.println("HTTP GET request sent successfully. Response code: " + String(httpCode));
      success = true;
    } else {
      // If the request failed, print the error code
      Serial.println("Error in HTTP request: " + String(httpCode));
      success = false;
    }

    // End the HTTP request
    http.end();
    return success;
  } else {
    Serial.println("Error: Not connected to WiFi");
    return false;
  }
}