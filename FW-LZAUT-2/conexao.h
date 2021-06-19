#ifndef CONEXAO_H
#define CONEXAO_H

// ----- Bibliotecas -----
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266mDNS.h>

#include "sensores.h"


// ----- Definições -----
// Rede
#define CONEXAO_NW_NAME         "nome_da_rede"
#define CONEXAO_NW_PASSWORD     "senha_da_rede"

#define CONEXAO_OTA_USER        "admin"
#define CONEXAO_OTA_PASSWORD    "admin"
#define CONEXAO_OTA_PATH        "/firmware"

// Endereço mDNS (nome.local)
#define CONEXAO_DNS_NAME "luzauto"


// Procedimentos
// Timeout para verificar se está desconectado
#define CONEXAO_RECONNECT_TIMEOUT 10000


// ----- Variáveis globais -----
/*
 * Struct com variáveis úteis da conexão
 *  - reconnect_timer: timer para reconectar o sensor
 */
struct {
  unsigned long reconnect_timer;
}conexao;

// ----- Protótipo das funções -----
void conexao_begin();

// Funções do wifi
void conexao_wifi_begin();
void conexao_wifi_reconnect();

// Funções do web server
void conexao_ws_begin();
void conexao_ws_listen();

void conexao_ws_not_found();
void conexao_ws_pagina_init();

void conexao_ws_rele_change();

void conexao_ws_update();

#endif // CONEXAO_H
