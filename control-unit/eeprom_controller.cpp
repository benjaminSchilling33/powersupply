/*
control-unit
This is the source file for the EepromController class.

SPDX-License-Identifier: GPL-3.0-or-later

Copyright (C) 2020 Benjamin Schilling
*/

#include <EEPROM.h>

#include "Arduino.h"
#include "eeprom_controller.h"

EepromController::EepromController()
{
   
}

// ====== Flags ======

/**
   * @brief 
   * 
   * @return is password set 
   */
bool EepromController::getFlagWifiCredentials()
{
   if ((EEPROM.read(FLAGS_ADDRESS) & 0x01) == 0x01)
   {
      return true;
   }
   else
   {
      return false;
   }
}

/**
   * @brief 
   * 
   */
void EepromController::setFlagWifiCredentials()
{
   uint8_t byteOne = EEPROM.read(FLAGS_ADDRESS);
   EEPROM.write(FLAGS_ADDRESS, (byteOne | 0x01));
   EEPROM.commit();
   Serial.println("Wifi Creds Flag set");
}

// ====== Values ======

/**
   * @brief 
   * 
   * @return Wifi SSID 
   */
String EepromController::getWifiSSID()
{
   String ssid;
   for (int i = 0; i < WIFI_SSID_LENGTH; i++)
   {
      ssid = ssid + char(EEPROM.read(WIFI_SSID_ADDRESS + i));
   }
   return ssid;
}

/**
   * @brief 
   * 
   * @param ssid 
   */
void EepromController::setWifiSSID(String ssid)
{
   for (int i = 0; i < WIFI_SSID_LENGTH; i++)
   {
      EEPROM.write(WIFI_SSID_ADDRESS + i, ssid[i]);
   }
   EEPROM.commit();
   Serial.println("Wifi SSID Set");
}

/**
   * @brief 
   * 
   * @return Wifi Password 
   */
String EepromController::getWifiPassword()
{
   String password;
   for (int i = 0; i < WIFI_PASSWORD_LENGTH; i++)
   {
      password = password + char(EEPROM.read(WIFI_PASSWORD_ADDRESS + i));
   }
   return password;
}

/**
   * @brief 
   * 
   * @param password 
   */
void EepromController::setWifiPassword(String password)
{
   for (int i = 0; i < WIFI_PASSWORD_LENGTH; i++)
   {
      EEPROM.write(WIFI_PASSWORD_ADDRESS + i, password[i]);
   }
   EEPROM.commit();
   Serial.println("Wifi Password Set");
}

/**
   * @brief 
   * 
   * 
   */
void EepromController::resetToDefault()
{
   for (int i = 0; i < 511; i++)
   {
      EEPROM.write(i, 0);
   }
   EEPROM.commit();
   Serial.println("Reset eeprom to default");
}
