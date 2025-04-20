#include <WiFi.h>           // For WiFi connection
#include <WebServer.h>      // For handling HTTP server
#include <Update.h>         // For firmware/SPIFFS updates
#include <SPIFFS.h>         // For SPIFFS file system
#include "esp_partition.h"  // For low-level partition access
#include <ArduinoOTA.h>     // For OTA (Over-The-Air) updates

// WiFi credentials
const char* ssid = "SSID";
const char* password = "PASSWORD";

// HTTP web server on port 80
WebServer server(80);

// Function to get the size of the SPIFFS partition
size_t getSPIFFSPartitionSize() {
  const esp_partition_t* spiffs_partition = esp_partition_find_first(
    ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_SPIFFS, NULL);

  if (spiffs_partition) {
    return spiffs_partition->size;
  } else {
    Serial.println("Failed to find SPIFFS partition!");
    return 0;
  }
}

void setup() {
  Serial.begin(115200);

  // Start WiFi connection
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.print("WiFi connected. IP: ");
  Serial.println(WiFi.localIP());

  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS initialization failed!");
    return;
  }

  // OTA setup (optional)
  ArduinoOTA.begin();

  // --- Web Routes ---

  // SPIFFS Upload Form Page
  server.on("/spiffs/flash", HTTP_GET, []() {
    server.send(200, "text/html",
                "<form method='POST' action='/spiffs/flash' enctype='multipart/form-data'>"
                "<input type='file' name='spiffs'>"
                "<input type='submit' value='Upload SPIFFS Bin'>"
                "</form>"
                "<br><a href='/'>Home page</a>");
  });

  // Handle SPIFFS Upload (POST Request)
  server.on(
    "/spiffs/flash", HTTP_POST, []() {
      if (Update.hasError()) {
        server.send(500, "text/plain", "SPIFFS Update Failed!");
      } else {
        server.send(200, "text/plain", "SPIFFS Update Done. Rebooting...");
        delay(1000);
        ESP.restart();
      }
    },
    []() {
      HTTPUpload& upload = server.upload();
      static size_t maxSize = getSPIFFSPartitionSize();  // Get SPIFFS partition size
      static size_t uploadedSize = 0;

      if (upload.status == UPLOAD_FILE_START) {
        uploadedSize = 0;
        Serial.printf("Start updating SPIFFS from file: %s\n", upload.filename.c_str());

        // Begin update
        if (!Update.begin(maxSize, U_SPIFFS)) {
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_WRITE) {
        uploadedSize += upload.currentSize;

        // Check if file exceeds partition size
        if (uploadedSize > maxSize) {
          Serial.println("Upload too large for SPIFFS partition!");
          Update.abort();
        } else if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) {
          Serial.printf("SPIFFS Update Success: %u bytes\n", upload.totalSize);
        } else {
          Update.printError(Serial);
        }
      }
    });

  // List all files in SPIFFS
  server.on("/spiffs/list", HTTP_GET, []() {
    String html = "<h2>SPIFFS File List</h2><ul>";

    File root = SPIFFS.open("/");
    if (!root || !root.isDirectory()) {
      server.send(500, "text/html", "Failed to open SPIFFS directory.");
      return;
    }

    File file = root.openNextFile();
    while (file) {
      String path = String(file.path());  // Full file path
      html += "<li><strong>" + path + "</strong> - " + String(file.size()) + " bytes</li>";
      file = root.openNextFile();
    }

    html += "</ul><br><a href='/'>Home page</a>";
    server.send(200, "text/html", html);
  });

  // Format SPIFFS - confirmation page
  server.on("/spiffs/format", HTTP_GET, []() {
    server.send(200, "text/html",
                "<h2>Format SPIFFS</h2>"
                "<p>This will delete all files stored in SPIFFS.</p>"
                "<form method='POST' action='/spiffs/format'>"
                "<input type='submit' value='Yes, Format SPIFFS'>"
                "</form>"
                "<br><a href='/'>Home page</a>");
  });

  // Perform SPIFFS format
  server.on("/spiffs/format", HTTP_POST, []() {
    if (SPIFFS.format()) {
      server.send(200, "text/html",
                  "<h2>SPIFFS formatted successfully.</h2>"
                  "<a href='/'>Home page</a>");
    } else {
      server.send(500, "text/html",
                  "<h2>Failed to format SPIFFS.</h2>"
                  "<a href='/'>Home page</a>");
    }
  });

  // Optional home page with navigation
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html",
                "<h2>ESP32 SPIFFS Tools</h2>"
                "<ul>"
                "<li><a href='/spiffs/flash'>Upload SPIFFS .bin</a></li>"
                "<li><a href='/spiffs/list'>List SPIFFS Files</a></li>"
                "<li><a href='/spiffs/format'>Format SPIFFS</a></li>"
                "</ul>");
  });

  // Start web server
  server.begin();
}

void loop() {
  ArduinoOTA.handle();    // Handle OTA updates
  server.handleClient();  // Handle HTTP client requests

  // Place any runtime logic here
}
