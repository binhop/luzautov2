#ifndef SENSORES_H
#define SENSORES_H

// ----- Bibliotecas -----
#include <EEPROM.h>


// ----- Definições -----
// Pino do sensor de presença
#define SENSORES_PRESENCA_PIN     12
// Tempo que o sensor de presença mantem a saída em alta após parar de detectar
#define SENSORES_PRESENCA_TIME    5000

// Limiar de detecção do LDR
#define SENSORES_LDR_LIMIAR       45
// Tempo necessário de sombra para o LDR detectar ambiente escuro
#define SENSORES_LDR_TIME         5000    

// Delay para leitura dos sensores
#define SENSORES_TIME   1000


// ----- Variáveis globais -----
/*
 * Struct com variáveis úteis relativas aos sensores
 *  - presenca: informa se tem algum no cômodo (1) ou não (0)
 *  - manual: informa se o acionamento da lâmpada é manual ou não
 *  - timer: timer para leitura dos sensores
 *  - ldr_timer: timer para LDR detectar longos períodos de sombra
 *  - presenca_timer: timer para sensor de presença ficar mais tempo acionado
 *  - escuro: informa se está escuro (1) ou não (0)
 */
struct{
  uint8_t presenca;
  uint8_t manual;
  unsigned long timer;
  unsigned long ldr_timer;
  unsigned long presenca_timer;
  uint8_t escuro;
} sensores;


// ----- Protótipo das funções -----
void sensores_begin();

uint8_t sensores_presenca_status();

uint8_t sensores_ldr_status();

#endif // SENSORES_H
