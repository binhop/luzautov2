/*
 * Versão 0.2
 */

#include "sensores.h"
#include "conexao.h"

// Calcula se delta_t é maior ou igual ao intervalo (timeout)
// verifica se delta_t > millis(), pois o valor de millis() pode estourar e voltar a 0
#define TIMEOUT(delta_t, interval)  (((millis() - delta_t) >= interval) || delta_t > millis())


// RELE
#define RELE            14
#define RELE_TIMEOUT  3000

unsigned long rele_timer = 0;


void setup() 
{
  // Inicializa o Serial para debug
  Serial.begin(115200);
  
  // Inicializa os pinos do relé
  pinMode(RELE, OUTPUT);
  digitalWrite(RELE, LOW);
  
  // Inicializa os sensores
  sensores_begin();

  // Se anteriormente estava no modo manual, liga a luz
  if(sensores.manual)
  {
    digitalWrite(RELE, HIGH);
  }

  // Inicializa o wifi e os parâmetros do OTA
  conexao_begin();

  Serial.println("Iniciando...");  
}

void loop() 
{
  // ----- Processa as requisições do web server e reconecta no wifi se necessário ----- 
  conexao_ws_update();

  // Verifica se está conectado, senão reconecta
  if(TIMEOUT(conexao.reconnect_timer, CONEXAO_RECONNECT_TIMEOUT))
  {
    conexao.reconnect_timer = millis();
    
    conexao_wifi_reconnect();
  }
  

  // ----- Lê o sensor de presença e o LDR ----- 
  if(TIMEOUT(sensores.timer, SENSORES_TIME))
  {
    sensores.timer = millis();
    
    sensores.presenca = sensores_presenca_status();
    sensores.escuro = sensores_ldr_status();
  }
  

  // ----- Acionamento do relé -----
  // Espera um tempo pra não ficar ligando e desligando o relé imediatamente
  if(TIMEOUT(rele_timer, RELE_TIMEOUT))
  {
    // Só muda o estado se a lâmpada não estiver no modo manual
    if(!sensores.manual)
    {
      if (sensores.presenca)
      {
        if(!digitalRead(RELE) && sensores.escuro)
        {
          digitalWrite(RELE, HIGH);
          rele_timer = millis();
        }
      }
      else
      {
        if(digitalRead(RELE))
        {
          digitalWrite(RELE, LOW);
          rele_timer = millis();
        }
      }
    }
  }
}
