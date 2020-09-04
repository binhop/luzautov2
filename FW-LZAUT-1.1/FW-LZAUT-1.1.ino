#include "gesto.h"
#include "conectividade.h"

// Calcula se delta_t é maior ou igual ao intervalo (timeout)
// Usa-se o 'abs', pois o valor de millis() pode estourar e voltar a 0
#define TIMEOUT(delta_t, interval)  (abs(millis() - delta_t) >= interval)

// Timeout quando entra em um estado invalido
#define ESTADO_TIMEOUT  2000

// Timeout para verificar se está desconectado
#define RECONNECT_TIMEOUT 10000

// RELE
#define RELE            14
#define RELE_TIMEOUT  3000

// LDR
#define LDR_TIMEOUT   3000
#define LDR_LIMIAR      45


uint8_t ldr_escuro();

unsigned long rele_timeout = 0;
unsigned long ldr_timeout = 0;
unsigned long reconnect_timeout = 0;


// DEBUG
unsigned long elapsed = 0;

// Estado da comunicação com o sensor
uint8_t gesto_conectado = 0;

// Contagem de pessoa
uint8_t cont_pessoas = 0;
// Ultimo gesto reconhecido (pra exibir no web server)
uint8_t ultimo_gesto = 0;

void setup() 
{
  // Inicializa o Serial para debug
  Serial.begin(115200);

  // Inicializa o wifi e os parâmetros do OTA
  wifi_begin();
  ws_begin();

  gesto_conectado = gesto_begin();

  Serial.println("Iniciando...");

  // Inicializa os pinos
  pinMode(RELE, OUTPUT);
  digitalWrite(RELE, LOW);
}

void loop() 
{
  // ----- Processa as requisições do web server e do debugger ----- 
  ws_update();

  // ----- Lê o sensor de gestos ----- 
  uint8_t gesto = 0;
  // Espera um tempo pra não ficar ligando e desligando o relé imediatamente
  if(TIMEOUT(gesto_timeout, GES_QUIT_TIME))
  {
    if(gesto_conectado)
    {
      gesto = gesto_get_status();
    }
     
     // Se for um gesto válido
     if(gesto)
     {
        ultimo_gesto = gesto;
     }
  }
  

  // ----- Processa o gesto realizado -----
  // Direita (Entrou)
  if(gesto == 1)
  {
    cont_pessoas++;
    Serial.println("Entrou");
  }
  // Esquerda (Saiu)
  else if(gesto == 2)
  {
    cont_pessoas = cont_pessoas > 0? cont_pessoas-1: 0;
    Serial.println("Saiu");
  }
  // Giro sentido horário (Liga a luz)
  else if(gesto == 7)
  {
    digitalWrite(RELE, HIGH);
    cont_pessoas = 1;
  }
  // Giro sentido anti-horário (Desliga a luz)
  else if(gesto == 8)
  {
    digitalWrite(RELE, LOW);
    cont_pessoas = 0;
  }

  // ----- Acionamento do relé -----
  // Espera um tempo pra não ficar ligando e desligando o relé imediatamente
  if(TIMEOUT(rele_timeout, RELE_TIMEOUT))
  {
    if (cont_pessoas > 0)
    {
      if(!digitalRead(RELE) && ldr_escuro())
      {
        digitalWrite(RELE, HIGH);
        rele_timeout = millis();
      }
    }
    else
    {
      if(digitalRead(RELE))
      {
        digitalWrite(RELE, LOW);
        rele_timeout = millis();
      }
    }
  }

  // Verifica se está conectado, senão reconecta
  if(TIMEOUT(reconnect_timeout, RECONNECT_TIMEOUT))
  {
    reconnect_timeout = millis();
    wifi_reconnect();
  }
}

/*
 * Informa se o ambiente está escuro (1) ou não (0)
 */
uint8_t ldr_escuro()
{
  // A leitura AD não pode ser feita constantemente, deve haver um atraso entre
  // cada medição, senão o webserver fica congelado
  // Assim, a função só atualiza a leitura, de fato, a cada 50ms
  // E, se não tiver passado 50ms, a função retorna o último valor 
  // (Por isso a variável abaixo é do tipo static)
  static uint8_t escuro = 0;

  static unsigned long last_adc_read = 0; // Variável static para saber quando ler novamente

  if(TIMEOUT(last_adc_read, 50)) // A cada 50ms a leitura é feita
  {
    uint16_t ldr = analogRead(A0);
    
    last_adc_read = millis();    
  
    if(ldr > LDR_LIMIAR) // Claro
    {
        ldr_timeout = millis();
        escuro = 0;
    }
    else // Escuro
    {
      if(TIMEOUT(ldr_timeout, LDR_TIMEOUT))
      {
        escuro = 1;
      }
      else
      {
        escuro = 0;
      }
    }
  }
  
  return escuro;
}
