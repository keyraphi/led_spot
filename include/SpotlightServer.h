/**
 * @file SpotlightServer.h
 * @brief Header file for the Spotlight Web Server class.
 *
 * This class handles all network-related functionality, including
 * connecting to WiFi and setting up the API endpoints for controlling
 * the Spotlight hardware.
 */

#ifndef SPOTLIGHTSERVER_H
#define SPOTLIGHTSERVER_H

#include "Spotlight.h"
#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>

class SpotlightServer {
public:
  /**
   * @brief Constructor for the SpotlightServer class.
   * @param spotlightInstance A pointer to the Spotlight object to control.
   */
  SpotlightServer(Spotlight *spotlightInstance);

  /**
   * @brief Initializes the WiFi connection and sets up all API routes.
   */
  void begin();

  /**
   * @brief Main update loop for the server. Should be called frequently
   * to handle incoming requests.
   */
  void update();

private:
  ESP8266WebServer _server;
  Spotlight *_spotlight;

  // API endpoint handlers
  void handleSetRGB();
  void handleSetKelvin();
  void handleSetWheelMode();
  void handleSetCycleMode();
  void handleSetCycleDuration();
  void handleSetCycleEasing();
  void handleSetTransitionDuration();
  void handleSetTransitionEasing();

  // Helper functions for parsing request arguments
  float getFloatArg(const String &name, float defaultValue);
  int getIntArg(const String &name, int defaultValue);
};

#endif
