/*
control-unit
This is the header file for the EepromController class.

SPDX-License-Identifier: GPL-3.0-or-later

Copyright (C) 2020 Benjamin Schilling
*/

#ifndef EepromController_h
#define EepromController_h

#include <EEPROM.h>
#include <Arduino.h>

// ---------------- EEPROM Layout ----------------

class EepromController
{

public:
  /**
   * @brief Construct a new Scp Password object
   * 
   */
  EepromController();

  // ====== Flags ======

  /**
   * @brief 
   * 
   * @return true if Wifi credentials are set
   */
  bool getFlagWifiCredentials();

  /**
   * @brief 
   * 
   */
  void setFlagWifiCredentials();
  // ====== Values ======

  /**
   * @brief 
   * 
   * @return SSID 
   */
  String getWifiSSID();

  /**
   * @brief 
   * 
   * @param ssid 
   */
  void setWifiSSID(String ssid);

  /**
   * @brief 
   * 
   * @return Wifi Password 
   */
  String getWifiPassword();

  /**
   * @brief 
   * 
   * @param password 
   */
  void setWifiPassword(String password);

  /**
   * @brief Resets the EEPROM to the default state
   * 
   */
  void resetToDefault();

private:
  const int FLAGS_ADDRESS = 0; // res. | res. | rest | res. | res. |  res. | res. | WifiCredentials
  const int FLAGS_LENGTH = 1;  // Byte 0
  const int WIFI_SSID_ADDRESS = 1;
  const int WIFI_SSID_LENGTH = 32; // Bytes 1 - 32
  const int WIFI_PASSWORD_ADDRESS = 33;
  const int WIFI_PASSWORD_LENGTH = 32; // Bytes 33 - 64
};

#endif
