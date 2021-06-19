#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>


#define NW_NAME       "nome_da_rede"
#define NW_PASSWORD   "senha_da_rede"

#define OTA_USER        "admin"
#define OTA_PASSWORD    "admin"
#define OTA_PATH        "/firmware"

// Funções do wifi
void wifi_begin();
void wifi_reconnect();

// Funções do web server
void ws_begin();
void ws_listen();

void ws_not_found();
void ws_pagina_init();

void ws_rele_change();

void ws_update();

#endif // WEBSERVER_H
