#include <WiFi.h>
#include "time.h"
#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold24pt7b.h>
#include "secrets.h"

// ------------------ Wi-Fi Credentials ------------------
const char* ssid     = WIFI_SSID;
const char* password = WIFI_PASS;

// ------------------ NTP Server & Timezone ------------------
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;          // e.g. 0 for GMT
const int   daylightOffset_sec = 3600;  // e.g. 3600 for DST

// ------------------ E-Paper Setup ------------------
// Adjust pins if your driver board differs
#define EPD_CS     5
#define EPD_DC     6
#define EPD_RST    7
#define EPD_BUSY   4

// For 7.5" b/w Waveshare panel (800x480) supported by GxEPD2
GxEPD2_BW<GxEPD2_750c_Z90c, GxEPD2_750c_Z90c::HEIGHT> display(GxEPD2_750c_Z90c(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY));

void setup() {
  Serial.begin(115200);

  // Initialize ePaper
  display.init(115200);
  display.setRotation(1);

  // Connect WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Connected!");

  // Init and get time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop() {
  // Get current time
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    delay(2000);
    return;
  }

  // Format time and date
  char timeStr[30];
  char dateStr[30];
  strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);
  strftime(dateStr, sizeof(dateStr), "%A, %d %B %Y", &timeinfo);

  // Display on ePaper
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    display.setTextColor(GxEPD_BLACK);

    display.setFont(&FreeMonoBold24pt7b);
    display.setCursor(50, 150);
    display.println(timeStr);

    display.setFont(&FreeMonoBold24pt7b);
    display.setCursor(50, 250);
    display.println(dateStr);

  } while (display.nextPage());

  Serial.printf("Displayed: %s %s\n", dateStr, timeStr);

  // Refresh every 60 seconds
  delay(60000);
}
