#include "sensores.h"

/*
 * Inicializa o sensor e a EEPROM
 */
void sensores_begin()
{
  EEPROM.begin(4);

  // Verifica se o acionamento está gravado na memória
  sensores_manual_read();
  
  pinMode(SENSORES_PRESENCA_PIN, INPUT);
}

/*
 * Retorna o estado do sensor
 * 1 = ativo e 0 = desativo
 */
uint8_t sensores_presenca_status()
{
  uint8_t aux = digitalRead(SENSORES_PRESENCA_PIN);

  // Se detectou alguém, reseta o timer
  if(aux)
  {
    sensores.presenca_timer = millis();
  }
  // Senão, verifica se passou do timeout
  else
  {
    // Se não passou o timeout, retorna 1
    if(!TIMEOUT(sensores.presenca_timer, SENSORES_PRESENCA_TIME))
    {
      aux = 1;
    }
  }

  return aux;
}

/*
 * Informa se o ambiente está escuro (1) ou não (0)
 */
uint8_t sensores_ldr_status()
{
  // A leitura AD não pode ser feita constantemente, deve haver um atraso entre
  // cada medição, senão o webserver fica congelado
  // No código principal, esta função é chamada a cada 1000ms
  int16_t ldr = analogRead(A0);

  // Ambiente iluminado
  if(ldr > SENSORES_LDR_LIMIAR) 
  {
    sensores.ldr_timer = millis();
    ldr = 0;
  }
  // Ambiente escuro
  else 
  {
    if(TIMEOUT(sensores.ldr_timer, SENSORES_LDR_TIME))
    {
      ldr = 1;
    }
    else
    {
      ldr = 0;
    }
  }
  
  return ldr;
}


/*
 * Lê o valor do acionamento da memória (manual/automatico)
 */
void sensores_manual_read()
{
  uint8_t aux; 
  
  // Endereço 0: valor do acionamento
  // Endereço 2: valor para verificar se o acionamento foi gravado     ('r')
  
  aux = EEPROM.read(2);
  if(aux == 'r')
  {
    sensores.manual = EEPROM.read(0);
  }
}

/*
 * Grava o valor do acionamento na memória
 */
void sensores_manual_write()
{
  // Endereço 0: valor do acionamento
  // Endereço 2: valor para verificar se o acionamento foi gravado     ('r')
  
  EEPROM.write(0, sensores.manual);
  EEPROM.write(2, 'r');
  EEPROM.commit();
}
