/*
control-unit
This is the header file for the WifiTab class.

SPDX-License-Identifier: GPL-3.0-or-later

Copyright (C) 2020 Benjamin Schilling
*/

#ifndef WifiTab_h
#define WifiTab_h

#include <lvgl.h>
#include <WiFi.h>

#include "EepromController.h"

class WifiTab {
public:
  
  static void init();
  static void createWifiTab(lv_obj_t * parent);

private:

  // ================ State ================
  
  static const int WIFI_STATE_DISCONNECTED = 0;
  static const int WIFI_STATE_CONNECTED = 1;
  static const int WIFI_STATE_CONNECTING = 2;
  
  static const int GUI_STATE_DEFAULT = 0;
  static const int GUI_STATE_ENTER_PASSWORD = 1;
  
  static int connectionState;
  static String selectedWifi;
  static int tries;
  static int guiState;
  
  // ================ Prototypes ================
  
  // ---------------- Function protypes ----------------
  
  static void setInitialState();
  static void updateGui();
  static void scanNetworks();
  
  // ---------------- Event handler prototypes ----------------
  static void eventHandlerPasswordTextArea(lv_obj_t * ta, lv_event_t event);
  static void eventHandlerConnect(lv_obj_t * obj, lv_event_t event);
  static void eventHandlerDisconnect(lv_obj_t * obj, lv_event_t event);
  static void eventHandlerScan(lv_obj_t * obj, lv_event_t event);
  static void eventHandlerSsidRoller(lv_obj_t * obj, lv_event_t event);
  static void eventHandlerClose(lv_obj_t * obj, lv_event_t event);
  
  // ---------------- Task callback prototypes ----------------
  static lv_task_t * taskWifiConnect;
  static void connectWifiTaskCb(lv_task_t * task);
  
  // ================ GUI objects ================
  
  // ---------------- password ----------------
  static lv_obj_t * textAreaPassword;
  static lv_obj_t * labelPassword;
  // ---------------- ssid ----------------
  static lv_obj_t * rollerSsid;
  static lv_obj_t * labelSsid;
  
  // ---------------- buttons ----------------
  static lv_obj_t * btnConnect;
  static lv_obj_t * labelConnectButton;
  static lv_obj_t * btnDisconnect;
  static lv_obj_t * labelDisconnectButton;
  static lv_obj_t * btnScan;
  static lv_obj_t * labelScanButton;
  static lv_obj_t * btnClose;
  static lv_obj_t * labelCloseButton;
  
  // ---------------- WiFi Info ----------------
  
  static lv_obj_t * labelConnectedSsid;
  static lv_obj_t * labelSignalStrength;
  static lv_obj_t * labelIpAddress;
  
  // ---------------- keyboard ----------------
  static lv_obj_t * keyboard;
  
  // ---------------- spinner ----------------
  static lv_obj_t * spinnerConnecting;

  // =============== EEPROM Controller
  static EepromController eepromController;
};
#endif
