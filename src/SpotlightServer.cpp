/**
 * @file SpotlightServer.cpp
 * @brief Implementation file for the Spotlight Web Server class.
 */

#include "SpotlightServer.h"
#include "ColorSpace.h"
#include "Easing.h"
#include "config.h"
#include <ESP8266mDNS.h>
#include <LittleFS.h>

// --- Helper functions for the web server ---

/**
 * @brief Gets a float value from a web server argument, with a default value.
 * @param name The name of the argument.
 * @param defaultValue The value to return if the argument is not found.
 * @return The float value of the argument, or the default value.
 */
float SpotlightServer::getFloatArg(const String &name, float defaultValue) {
  if (_server.hasArg(name)) {
    return _server.arg(name).toFloat();
  }
  return defaultValue;
}

/**
 * @brief Gets an integer value from a web server argument, with a default
 * value.
 * @param name The name of the argument.
 * @param defaultValue The value to return if the argument is not found.
 * @return The integer value of the argument, or the default value.
 */
int SpotlightServer::getIntArg(const String &name, int defaultValue) {
  if (_server.hasArg(name)) {
    return _server.arg(name).toInt();
  }
  return defaultValue;
}

// --- API Endpoint Handlers ---

void SpotlightServer::handleSetRGB() {
  Serial.print("handleSetRGB() called: ");
  uint8_t r = getIntArg("r", 0);
  uint8_t g = getIntArg("g", 0);
  uint8_t b = getIntArg("b", 0);
  Serial.printf("decoded rgb: %d, %d, %d\n", r, g, b);
  _spotlight->setRGB(r, g, b);
  _server.send(200, "text/plain", "OK");
}

void SpotlightServer::handleSetKelvin() {
  Serial.print("handleSetKelvin() called: ");
  float kelvin = getFloatArg("kelvin", 6500.0);
  float brightness = getFloatArg("brightness", 1.0);

  Serial.printf("decoded kelvin: %f, decoded brightness %f\n", kelvin,
                brightness);
  _spotlight->setColorTemperature(kelvin, brightness);
  _server.send(200, "text/plain", "OK");
}

void SpotlightServer::handleSetWheelMode() {
  Serial.print("handleSetWheelMode() called: ");
  float period = getFloatArg("period", 10.0);
  String directionStr =
      _server.hasArg("direction") ? _server.arg("direction") : "clockwise";

  Serial.printf("period: %f, direction %s\n", period,
                directionStr.c_str());
  Spotlight::RotationDirection direction =
      Spotlight::RotationDirection::Clockwise;
  if (directionStr.equalsIgnoreCase("counterclockwise")) {
    direction = Spotlight::RotationDirection::CounterClockwise;
  }
  _spotlight->enableColorWheelMode(period, direction);
  _server.send(200, "text/plain", "OK");
}

void SpotlightServer::handleSetCycleMode() {
  Serial.print("handleSetCycleMode() called: ");
  if (!_server.hasArg("colors")) {
    _server.send(400, "text/plain", "Missing colors parameter");
    return;
  }

  String colorsStr = _server.arg("colors");
  bool isRandom = _server.hasArg("random") &&
                  _server.arg("random").equalsIgnoreCase("true");

  Serial.printf("colors: %s, isRandom %d\n", colorsStr.c_str(),
      isRandom);


  ColorSpace::RGB colors[Constants::MAX_COLORS];
  size_t count = 0;

  // Parse the comma-separated hex values
  size_t lastIndex = 0;
  int commaIndex = colorsStr.indexOf(',');
  while (commaIndex >= 0 && count < Constants::MAX_COLORS) {
    String hex = colorsStr.substring(lastIndex, commaIndex);
    colors[count++] = ColorSpace::hexToRgb(hex);
    lastIndex = commaIndex + 1;
    commaIndex = colorsStr.indexOf(',', lastIndex);
  }
  if (lastIndex < colorsStr.length() && count < Constants::MAX_COLORS) {
    colors[count++] = ColorSpace::hexToRgb(colorsStr.substring(lastIndex));
  }

  _spotlight->enableColorCycleMode(colors, count, isRandom);
  _server.send(200, "text/plain", "OK");
}

void SpotlightServer::handleSetCycleDuration() {
  Serial.print("handleSetCycleDuration() called: ");
  float duration = getFloatArg("duration", 2.0);
  Serial.printf("duration: %f\n", duration);
  _spotlight->setCycleDuration(duration);
  _server.send(200, "text/plain", "OK");
}

void SpotlightServer::handleSetCycleEasing() {
  Serial.print("handleSetCycleEasing() called: ");
  String easingStr =
      _server.hasArg("easing") ? _server.arg("easing") : "linear";
  Serial.printf("easing: %s\n", easingStr.c_str());
  Easing::EasingFunction easing = Easing::easingFromString(easingStr);
  _spotlight->setCycleEasing(easing);
  _server.send(200, "text/plain", "OK");
}

void SpotlightServer::handleSetTransitionDuration() {
  Serial.print("handleSetTransitionDuration() called: ");
  float duration = getFloatArg("duration", 0.2);
  Serial.printf("duration: %f\n", duration);
  _spotlight->setTransitionDuration(duration);
  _server.send(200, "text/plain", "OK");
}

void SpotlightServer::handleSetTransitionEasing() {
  Serial.print("handleSetTransitionEasing() called: ");
  String easingStr =
      _server.hasArg("easing") ? _server.arg("easing") : "cubic-in-out";
  Serial.printf("easing: %s\n", easingStr.c_str());
  Easing::EasingFunction easing = Easing::easingFromString(easingStr);
  _spotlight->setTransitionEasing(easing);
  _server.send(200, "text/plain", "OK");
}

// Private helper to serve files from LittleFS
bool SpotlightServer::handleFileRequest(const String &path) {
  Serial.print("handleFileRequest called for path: ");
  Serial.println(path);

  String fullPath = path;
  if (fullPath.endsWith("/")) {
    fullPath += "index.html"; // Serve index.html for root requests
  }

  Serial.print("Checking for file at fullPath: ");
  Serial.println(fullPath);

  // Check if the file exists and has a content type
  String contentType = getContentType(fullPath);
  if (LittleFS.exists(fullPath)) {
    Serial.println("File exists!");
    File file = LittleFS.open(fullPath, "r");
    if (!file) {
      Serial.println("Failed to open file for reading.");
      return false;
    }
    _server.streamFile(file, contentType);
    file.close();
    Serial.println("File served successfully.");
    return true;
  }

  Serial.println("File does NOT exist.");
  return false;
}

// Private helper to get content type from file extension
String SpotlightServer::getContentType(const String &filename) {
  if (filename.endsWith(".html"))
    return "text/html";
  if (filename.endsWith(".css"))
    return "text/css";
  if (filename.endsWith(".js"))
    return "application/javascript";
  if (filename.endsWith(".json"))
    return "application/json";
  if (filename.endsWith(".png"))
    return "image/png";
  if (filename.endsWith(".jpg"))
    return "image/jpeg";
  if (filename.endsWith(".gif"))
    return "image/gif";
  return "text/plain";
}

// Constructor
SpotlightServer::SpotlightServer(Spotlight *spotlightInstance)
    : _server(80), _spotlight(spotlightInstance) {}

// Initializes the pins and sets up WiFi and WebServer
void SpotlightServer::begin() {
  Serial.begin(115200);

  // Initialize the file system first
  if (!LittleFS.begin()) {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }
  // List all files on the file system for debugging purposes
  listDir("/", 0);

  // Connect to WiFi
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Initialize mDNS to resolve the hostname "spotlight.local"
  if (MDNS.begin("spotlight")) {
    Serial.println("mDNS responder started");
  } else {
    Serial.println("Error setting up mDNS responder!");
  }
  MDNS.addService("http", "tcp", 80);

  // Register API endpoints. The std::bind() is used to pass the member
  // function and the 'this' pointer to the server.
  _server.on("/rgb", HTTP_GET, std::bind(&SpotlightServer::handleSetRGB, this));
  _server.on("/kelvin", HTTP_GET,
             std::bind(&SpotlightServer::handleSetKelvin, this));
  _server.on("/wheel", HTTP_GET,
             std::bind(&SpotlightServer::handleSetWheelMode, this));
  _server.on("/cycle", HTTP_GET,
             std::bind(&SpotlightServer::handleSetCycleMode, this));
  _server.on("/setCycleDuration", HTTP_GET,
             std::bind(&SpotlightServer::handleSetCycleDuration, this));
  _server.on("/setCycleEasing", HTTP_GET,
             std::bind(&SpotlightServer::handleSetCycleEasing, this));
  _server.on("/setTransitionDuration", HTTP_GET,
             std::bind(&SpotlightServer::handleSetTransitionDuration, this));
  _server.on("/setTransitionEasing", HTTP_GET,
             std::bind(&SpotlightServer::handleSetTransitionEasing, this));

  // catch-all handler for all GET requests to serve files from LittleFS
  _server.onNotFound([this]() {
    if (!handleFileRequest(_server.uri())) {
      _server.send(404, "text/plain", "404: Not Found");
    }
  });

  _server.begin();
  Serial.println("Web server started!");
}

// Main update method.
void SpotlightServer::update() {
  // Handle incoming client requests
  _server.handleClient();
  MDNS.update();
}

void SpotlightServer::listDir(const char *dirname, uint8_t numTabs) {
  Serial.printf("Listing directory: %s\n", dirname);

  Dir root = LittleFS.openDir(dirname);

  while (root.next()) {
    File f = root.openFile("r");
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.printf(" - %s, size: %u\n", f.name(), f.size());
    f.close();
  }
}
