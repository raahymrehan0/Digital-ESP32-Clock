#include <TFT_eSPI.h>       // Graphics and font library for ST7735 driver chip
#include <SPI.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

// Wi-Fi credentials
#define USE_EDUROAM 1
#define WIFI_USER "username"
#define WIFI_SSID "SSID"
#define WIFI_PASS "Password"

#if USE_EDUROAM
#include "esp_eap_client.h"
const char* ssid = "eduroam";
const char* user = WIFI_USER;
#else
const char* ssid = WIFI_SSID;
#endif
const char* pass = WIFI_PASS;

// NTP Client settings
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);  // Time offset in seconds (3600 = GMT+1)

TFT_eSPI tft = TFT_eSPI();  // TFT instance
uint32_t targetTime = 0;     // for next 1 second timeout

bool initial = 1;
byte xcolon = 0;
unsigned int colour = 0;
byte omm = 99;
int hh, mm, ss;

void print_wifi_info() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected.");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi not connected.");
  }
}

void wifi_connect() {
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  
#if USE_EDUROAM
  esp_eap_client_set_identity((uint8_t *)user, strlen(user));
  esp_eap_client_set_username((uint8_t *)user, strlen(user));
  esp_eap_client_set_password((uint8_t *)pass, strlen(pass));
  esp_wifi_sta_enterprise_enable();
  WiFi.begin(ssid);
#else
  WiFi.begin(ssid, pass);
#endif

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  print_wifi_info();
}

void setup(void) {
  Serial.begin(115200);

  // Initialize the TFT screen
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  // Connect to Wi-Fi and start NTP client
  wifi_connect();
  timeClient.begin();

  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  targetTime = millis() + 1000;
}

void loop() {
  if (targetTime < millis()) {  // Check if it's time for the next update (once per second)
    targetTime = millis() + 1000;  // Set the target time for the next second
    timeClient.update();  // Update the NTP time

    // Print the formatted time to the serial monitor (once per second)
    Serial.println(timeClient.getFormattedTime());

    // Get hours, minutes, and seconds from the NTP client
    hh = timeClient.getHours();
    mm = timeClient.getMinutes();
    ss = timeClient.getSeconds();

    // Check if the minute has changed to minimize redraw
    if (ss == 0 || initial) {
      initial = 0;
      tft.setTextColor(TFT_GREEN, TFT_BLACK);
      tft.setCursor(8, 52);
      tft.print(timeClient.getFormattedTime());  // You can replace this with custom date format

      tft.setTextColor(TFT_BLUE, TFT_BLACK);
      tft.drawCentreString("Real-time Clock", 120, 48, 2);
    }

    // Update digital time display
    byte xpos = 6;
    byte ypos = 0;

    if (omm != mm) { // Only update every minute to minimize flicker
      tft.setTextColor(0x39C4, TFT_BLACK); // 7-segment ghost image
      tft.drawString("88:88", xpos, ypos, 7); // Clear the previous time
      tft.setTextColor(0xFBE0); // Orange for new time

      omm = mm;

      // Display the hours and minutes
      if (hh < 10) xpos += tft.drawChar('0', xpos, ypos, 7);
      xpos += tft.drawNumber(hh, xpos, ypos, 7);
      xcolon = xpos;
      xpos += tft.drawChar(':', xpos, ypos, 7);
      if (mm < 10) xpos += tft.drawChar('0', xpos, ypos, 7);
      tft.drawNumber(mm, xpos, ypos, 7);
    }

    // Flash the colon every second
    if (ss % 2) {
      tft.setTextColor(0x39C4, TFT_BLACK);
      xpos += tft.drawChar(':', xcolon, ypos, 7);
      tft.setTextColor(0xFBE0, TFT_BLACK);
    } else {
      tft.drawChar(':', xcolon, ypos, 7);
      colour = random(0xFFFF);
      tft.fillRect(0, 64, 160, 20, TFT_BLACK);
      tft.setTextColor(colour);
      tft.drawRightString("Colour", 75, 64, 4);
      String scolour = String(colour, HEX);
      scolour.toUpperCase();
      char buffer[20];
      scolour.toCharArray(buffer, 20);
      tft.drawString(buffer, 82, 64, 4);
    }
  }

}


