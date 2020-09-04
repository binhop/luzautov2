#include "gesto.h"

unsigned long gesto_timeout = 0;
// Variável auxiliar para ajudar na exibição do web server
char * gestos[10] = {"Nenhum", "Entrada", "Saída", "Frente", "Trás", "Baixo", "Cima", "Giro sentido horário", "Giro sentido anti-horário", "Aceno"};


/*
 * Inicializa e configura o sensor
 */
uint8_t gesto_begin()
{
  Wire.begin();

  uint8_t error = paj7620Init();
  if(error)
  {
    Serial.println("Erro na inicialização do sensor");
  }

  return !error;
}

/*
 * Retorna qual foi o gesto realizado
 * 0 - Nenhum
 * 1 - Direita
 * 2 - Esquerda
 * 3 - Frente
 * 4 - Trás
 * 5 - Baixo
 * 6 - Cima
 * 7 - Giro sentido horário
 * 8 - Giro sentido anti-horário
 * 9 - Aceno
 */
uint8_t gesto_get_status()
{
  uint8_t erro, data, data1;
  erro = paj7620ReadReg(0x43, 1, &data);

  gesto_timeout = millis();
  if (!erro) 
  {
    switch (data)
    {
      case GES_RIGHT_FLAG:
        return 1;
        
      case GES_LEFT_FLAG:
        return 2;
        
      case GES_UP_FLAG:
        return 5;

      case GES_DOWN_FLAG:
        return 6;
        
      case GES_FORWARD_FLAG:
        // Frente
        return 3;
        
      case GES_BACKWARD_FLAG:   
        // Trás  
        return 4;
        
      case GES_CLOCKWISE_FLAG:
        // Horário
        return 7;
        
      case GES_COUNT_CLOCKWISE_FLAG:
        // Anti-horário
        return 8;
        
      default:
        paj7620ReadReg(0x44, 1, &data1);
        if (data1 == GES_WAVE_FLAG) 
        {
          return 9;
        }
        break;
    }
  }
  // Se deu erro, reinicia o timeout para ler instantaneamente
  else
  {
    gesto_timeout = 0;
  }

  return 0;
}
