/*
  control-unit
  This is the source file for the EepromController class.

  SPDX-License-Identifier: GPL-3.0-or-later

  Copyright (C) 2020 Benjamin Schilling
*/

#include "WifiTab.h"

void WifiTab::init() {
  connectionState = WIFI_STATE_DISCONNECTED;
  selectedWifi = "";
  tries = 10;
  guiState = GUI_STATE_DEFAULT;
}

void WifiTab::createWifiTab(lv_obj_t * parent) {
  /* Create the password box */
  textAreaPassword = lv_textarea_create(parent, NULL);
  lv_textarea_set_text(textAreaPassword, "");
  lv_textarea_set_pwd_mode(textAreaPassword, true);
  lv_textarea_set_one_line(textAreaPassword, true);
  lv_textarea_set_cursor_hidden(textAreaPassword, true);
  lv_obj_align(textAreaPassword, NULL, LV_ALIGN_IN_TOP_RIGHT, -10, 20);
  lv_obj_set_width(textAreaPassword, LV_HOR_RES / 2 - 20);
  lv_obj_set_event_cb(textAreaPassword, eventHandlerPasswordTextArea);

  /* Create a label and position it above the text box */
  labelPassword = lv_label_create(parent, NULL);
  lv_label_set_text(labelPassword, "Password:");
  lv_obj_align(labelPassword, textAreaPassword, LV_ALIGN_OUT_TOP_LEFT, 0, 0);

  /* Create the SSID roller */
  rollerSsid = lv_roller_create(parent, NULL);
  lv_roller_set_visible_row_count(rollerSsid, 2);
  lv_obj_align(rollerSsid, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 20);
  lv_obj_set_event_cb(rollerSsid, eventHandlerSsidRoller);

  /* Create a label and position it above the text box */
  labelSsid = lv_label_create(parent, NULL);
  lv_label_set_text(labelSsid, "SSID:");
  lv_obj_align(labelSsid, rollerSsid, LV_ALIGN_OUT_TOP_LEFT, 0, 0);

  // Initialize buttons
  // Connect button
  btnConnect = lv_btn_create(parent, NULL);
  lv_obj_set_event_cb(btnConnect, eventHandlerConnect);
  lv_obj_set_size(btnConnect,  LV_HOR_RES / 2.5, 40);
  lv_obj_align(btnConnect, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -30);

  labelConnectButton = lv_label_create(btnConnect, NULL);
  lv_label_set_text(labelConnectButton, "Connect");

  // Disconnect button
  btnDisconnect = lv_btn_create(parent, NULL);
  lv_obj_set_event_cb(btnDisconnect, eventHandlerDisconnect);
  lv_obj_set_size(btnDisconnect,  LV_HOR_RES / 2.5, 40);
  lv_obj_align(btnDisconnect, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -30);

  labelDisconnectButton = lv_label_create(btnDisconnect, NULL);
  lv_label_set_text(labelDisconnectButton, "Disconnect");

  // Scan button
  btnScan = lv_btn_create(parent, NULL);
  lv_obj_set_event_cb(btnScan, eventHandlerScan);
  lv_obj_set_size(btnScan,  LV_HOR_RES / 2.5, 30);
  lv_obj_align(btnScan, rollerSsid, LV_ALIGN_OUT_BOTTOM_MID, 60, 10);

  labelScanButton = lv_label_create(btnScan, NULL);
  lv_label_set_text(labelScanButton, "Scan");

  // Close button
  btnClose = lv_btn_create(parent, NULL);
  lv_obj_set_event_cb(btnClose, eventHandlerClose);
  lv_obj_set_size(btnClose,  50, 50);
  lv_obj_align(btnClose, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 0);

  labelCloseButton = lv_label_create(btnClose, NULL);
  lv_label_set_text(labelCloseButton, "X");

  /* Create the keyboard */
  keyboard = lv_keyboard_create(parent, NULL);
  lv_obj_set_size(keyboard,  LV_HOR_RES - 50, LV_VER_RES / 2);
  lv_obj_align(keyboard, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -90);

  lv_keyboard_set_textarea(keyboard, textAreaPassword); /* Focus it on one of the text areas to start */
  lv_keyboard_set_cursor_manage(keyboard, true); /* Automatically show/hide cursors on text areas */

  /* Spinner */
  spinnerConnecting = lv_spinner_create(parent, NULL);
  lv_obj_set_size(spinnerConnecting, 100, 100);
  lv_obj_align(spinnerConnecting, NULL, LV_ALIGN_CENTER, 0, 0);

  /* Connection info labels*/
  labelConnectedSsid = lv_label_create(parent, NULL);
  lv_label_set_text(labelConnectedSsid, "SSID: ");
  lv_obj_align(labelConnectedSsid, NULL, LV_ALIGN_IN_TOP_MID, 0, 20);

  labelSignalStrength = lv_label_create(parent, NULL);
  lv_label_set_text(labelSignalStrength, "RSSI: ");
  lv_obj_align(labelSignalStrength, NULL, LV_ALIGN_IN_TOP_MID, 0, 40);

  labelIpAddress = lv_label_create(parent, NULL);
  lv_label_set_text(labelIpAddress, "IP Address: ");
  lv_obj_align(labelIpAddress, NULL, LV_ALIGN_IN_TOP_MID, 0, 60);

  setInitialState();
}

void WifiTab::setInitialState() {
  if (eepromController.getFlagWifiCredentials()) {
    // If wifi credentials are set, set WIFI_STATE_CONNECTED
    connectionState = WIFI_STATE_CONNECTED;
  }
  else {
    // If not scan networks and set WIFI_STATE_DISCONNECTED
    scanNetworks();
    connectionState = WIFI_STATE_DISCONNECTED;
  }
  updateGui();
}

void WifiTab::updateGui() {
  if (guiState == GUI_STATE_ENTER_PASSWORD) {
    // show keyboard and close button
    lv_obj_set_hidden(keyboard, false);
    lv_obj_set_hidden(btnClose, false);

    // move password text area to top mid
    lv_obj_align(textAreaPassword, NULL, LV_ALIGN_IN_TOP_MID, -10, 20);
    lv_obj_align(labelPassword, textAreaPassword, LV_ALIGN_OUT_TOP_LEFT, 0, 0);

    //hide all other objects
    lv_obj_set_hidden(rollerSsid, true);
    lv_obj_set_hidden(labelSsid, true);
    lv_obj_set_hidden(btnConnect, true);
    lv_obj_set_hidden(labelConnectButton, true);
    lv_obj_set_hidden(btnScan, true);
    lv_obj_set_hidden(labelScanButton, true);

  } else if (guiState == GUI_STATE_DEFAULT) {
    if (connectionState == WIFI_STATE_DISCONNECTED) {
      // Hide keyboard and close button
      lv_obj_set_hidden(keyboard, true);
      lv_obj_set_hidden(btnClose, true);
      // Hide spinner
      lv_obj_set_hidden(spinnerConnecting, true);

      // Hide connected objects
      lv_obj_set_hidden(btnDisconnect, true);
      lv_obj_set_hidden(labelDisconnectButton, true);
      lv_obj_set_hidden(labelConnectedSsid, true);
      lv_obj_set_hidden(labelSignalStrength, true);
      lv_obj_set_hidden(labelIpAddress, true);

      //show all other objects
      lv_obj_set_hidden(textAreaPassword, false);
      lv_obj_set_hidden(labelPassword, false);
      lv_obj_set_hidden(rollerSsid, false);
      lv_obj_set_hidden(labelSsid, false);
      lv_obj_set_hidden(btnConnect, false);
      lv_obj_set_hidden(labelConnectButton, false);
      lv_obj_set_hidden(btnScan, false);
      lv_obj_set_hidden(labelScanButton, false);


      // move password text area to default position
      lv_obj_align(textAreaPassword, NULL, LV_ALIGN_IN_TOP_RIGHT, -10, 20);
      lv_obj_align(labelPassword, textAreaPassword, LV_ALIGN_OUT_TOP_LEFT, 0, 0);
    } else if (connectionState == WIFI_STATE_CONNECTED) {
      // Hide spinner
      lv_obj_set_hidden(spinnerConnecting, true);
      // Delete possibly running task
      lv_task_del(taskWifiConnect);
      // Show connected Wifi SSID
      lv_obj_set_hidden(labelConnectedSsid, false);
      String ssid = "SSID: " + String(WiFi.SSID());
      lv_label_set_text(labelConnectedSsid, ssid.c_str());
      // Show signal strength
      lv_obj_set_hidden(labelSignalStrength, false);
      String rssi = "RSSI: " + String(WiFi.RSSI());
      lv_label_set_text(labelSignalStrength, rssi.c_str());
      // Show IP address
      lv_obj_set_hidden(labelIpAddress, false);
      String ipAddress = "IP Address: " + WiFi.localIP().toString();
      lv_label_set_text(labelIpAddress, ipAddress.c_str());
      // show disconnect button
      lv_obj_set_hidden(btnDisconnect, false);
      lv_obj_set_hidden(labelDisconnectButton, false);
    } else if (connectionState == WIFI_STATE_CONNECTING) {
      // Hide all objects
      lv_obj_set_hidden(textAreaPassword, true);
      lv_obj_set_hidden(labelPassword, true);
      lv_obj_set_hidden(rollerSsid, true);
      lv_obj_set_hidden(labelSsid, true);
      lv_obj_set_hidden(btnConnect, true);
      lv_obj_set_hidden(labelConnectButton, true);
      lv_obj_set_hidden(btnScan, true);
      lv_obj_set_hidden(labelScanButton, true);
      lv_obj_set_hidden(btnDisconnect, true);
      lv_obj_set_hidden(labelDisconnectButton, true);
      lv_obj_set_hidden(labelConnectedSsid, true);
      lv_obj_set_hidden(labelSignalStrength, true);
      lv_obj_set_hidden(labelIpAddress, true);

      // Show circular progress indicator
      lv_obj_set_hidden(spinnerConnecting, false);

      // Start connecting to wifi
      String password = lv_textarea_get_text(textAreaPassword);
      WiFi.begin(selectedWifi.c_str(), password.c_str());
      Serial.println(password);
      Serial.println(selectedWifi);
      tries = 20;
      taskWifiConnect = lv_task_create(connectWifiTaskCb, 500, LV_TASK_PRIO_MID, NULL);
      lv_task_set_repeat_count(taskWifiConnect, 21);
      lv_task_ready(taskWifiConnect);
    } else {
      Serial.println("Undefined Wifi Connection state");
    }
  } else {
    Serial.println("Undefined GUI State");
  }

}

void WifiTab::connectWifiTaskCb(lv_task_t * task) {
  // If connection established done do anything
  if (connectionState == WIFI_STATE_CONNECTED) {
    return;
  }
  if (WiFi.status() != WL_CONNECTED && tries > 0) {
    // Continue trying to connect
    Serial.print("Tries: ");
    Serial.println(tries);
    tries--;
  } else if (WiFi.status() != WL_CONNECTED && tries == 0) {
    // If connection failed after 10 seconds (20 * 500ms)
    Serial.println("Connection failed");
    connectionState = WIFI_STATE_DISCONNECTED;
    updateGui();
  } else if (WiFi.status() == WL_CONNECTED) {
    // If connection is established
    Serial.println("Connection successfull");
    connectionState = WIFI_STATE_CONNECTED;
    String password = lv_textarea_get_text(textAreaPassword);
    eepromController.setWifiSSID(selectedWifi);
    eepromController.setWifiPassword(password);
    eepromController.setFlagWifiCredentials(0x01);
    updateGui();
  }
}

void WifiTab::scanNetworks() {

  int numberOfNetworks = WiFi.scanNetworks();

  String ssids = "";

  for (int i = 0; i < numberOfNetworks; i++) {
    if (i + 1 < numberOfNetworks) {
      ssids = ssids + WiFi.SSID(i) + "\n";
    } else {
      ssids = ssids + WiFi.SSID(i);
    }
  }

  lv_roller_set_options(rollerSsid, ssids.c_str(), LV_ROLLER_MODE_NORMAL);
}


void WifiTab::eventHandlerPasswordTextArea(lv_obj_t * ta, lv_event_t event)
{
  if (event == LV_EVENT_CLICKED) {
    /* Focus on the clicked text area */
    if (keyboard != NULL)
      lv_keyboard_set_textarea(keyboard, ta);
    guiState = GUI_STATE_ENTER_PASSWORD;
    updateGui();
  }
}

void WifiTab::eventHandlerSsidRoller(lv_obj_t * obj, lv_event_t event)
{
  if (event == LV_EVENT_VALUE_CHANGED) {
    char buf[32];
    lv_roller_get_selected_str(obj, buf, sizeof(buf));
    selectedWifi = String(buf);
  }
}

void WifiTab::eventHandlerClose(lv_obj_t * obj, lv_event_t event)
{
  if (event == LV_EVENT_CLICKED) {
    guiState = GUI_STATE_DEFAULT;
    updateGui();
  }
}

void WifiTab::eventHandlerConnect(lv_obj_t * obj, lv_event_t event)
{
  if (event == LV_EVENT_CLICKED) {
    if (connectionState == WIFI_STATE_DISCONNECTED) {
      connectionState = WIFI_STATE_CONNECTING;
      updateGui();
    }
  }
}

void WifiTab::eventHandlerDisconnect(lv_obj_t * obj, lv_event_t event)
{
  if (event == LV_EVENT_CLICKED) {
    if (connectionState == WIFI_STATE_CONNECTED) {
      WiFi.disconnect();
      connectionState = WIFI_STATE_DISCONNECTED;
      updateGui();
    }
  }
}

void WifiTab::eventHandlerScan(lv_obj_t * obj, lv_event_t event)
{
  if (event == LV_EVENT_CLICKED) {
    scanNetworks();
    updateGui();
  }
}
