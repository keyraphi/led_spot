/**
 * @file SpotlightServer.cpp
 * @brief Implementation file for the Spotlight Web Server class.
 */

#include "SpotlightServer.h"
#include "ColorSpace.h"
#include "Easing.h"
#include "config.h"

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
  uint8_t r = getIntArg("r", 0);
  uint8_t g = getIntArg("g", 0);
  uint8_t b = getIntArg("b", 0);
  _spotlight->setRGB(r, g, b);
  _server.send(200, "text/plain", "OK");
}

void SpotlightServer::handleSetKelvin() {
  float kelvin = getFloatArg("kelvin", 6500.0);
  float brightness = getFloatArg("brightness", 1.0);
  _spotlight->setColorTemperature(kelvin, brightness);
  _server.send(200, "text/plain", "OK");
}

void SpotlightServer::handleSetWheelMode() {
  float period = getFloatArg("period", 10.0);
  String directionStr =
      _server.hasArg("direction") ? _server.arg("direction") : "clockwise";
  Spotlight::RotationDirection direction =
      Spotlight::RotationDirection::Clockwise;
  if (directionStr.equalsIgnoreCase("counterclockwise")) {
    direction = Spotlight::RotationDirection::CounterClockwise;
  }
  _spotlight->enableColorWheelMode(period, direction);
  _server.send(200, "text/plain", "OK");
}

void SpotlightServer::handleSetCycleMode() {
  if (!_server.hasArg("colors")) {
    _server.send(400, "text/plain", "Missing colors parameter");
    return;
  }

  String colorsStr = _server.arg("colors");
  bool isRandom = _server.hasArg("random") &&
                  _server.arg("random").equalsIgnoreCase("true");

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
  float duration = getFloatArg("duration", 2.0);
  _spotlight->setCycleDuration(duration);
  _server.send(200, "text/plain", "OK");
}

void SpotlightServer::handleSetCycleEasing() {
  String easingStr =
      _server.hasArg("easing") ? _server.arg("easing") : "linear";
  Easing::EasingFunction easing = Easing::easingFromString(easingStr);
  _spotlight->setCycleEasing(easing);
  _server.send(200, "text/plain", "OK");
}

void SpotlightServer::handleSetTransitionDuration() {
  float duration = getFloatArg("duration", 0.2);
  _spotlight->setTransitionDuration(duration);
  _server.send(200, "text/plain", "OK");
}

void SpotlightServer::handleSetTransitionEasing() {
  String easingStr =
      _server.hasArg("easing") ? _server.arg("easing") : "cubic-in-out";
  Easing::EasingFunction easing = Easing::easingFromString(easingStr);
  _spotlight->setTransitionEasing(easing);
  _server.send(200, "text/plain", "OK");
}

// Constructor
SpotlightServer::SpotlightServer(Spotlight *spotlightInstance)
    : _server(80), _spotlight(spotlightInstance) {}

// Initializes the pins and sets up WiFi and WebServer
void SpotlightServer::begin() {
  Serial.begin(115200);

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

  _server.begin();
  Serial.println("Web server started!");
}

// Main update method.
void SpotlightServer::update() {
  // Handle incoming client requests
  _server.handleClient();
}
