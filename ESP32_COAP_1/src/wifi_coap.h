/*
 ****************************************************************************
 * @file	wifi_coap.h                                                           *
 * @author	Nguyen Thi Quynh                                             *
 * @date	15/01/2025
 * @ver 1.0                                                       *
 ****************************************************************************
 */

#include <WiFi.h>
#include <Arduino.h>
#include "Thing.CoAP.h"
#include <HTTPClient.h>
#include <df_mlx90614.h>

// Declare our CoAP server and the packet handler
extern Thing::CoAP::Server server;
extern Thing::CoAP::ESP::UDPPacketProvider udpProvider;

extern const char *ssid;
extern const char *password;
extern String URL;

extern const unsigned long interval;
extern unsigned long previousMillis;

void Wifi_Connection();
void Wifi_Coap_Setup();
void Run();

/********************************* END OF FILE ********************************/
/******************************************************************************/