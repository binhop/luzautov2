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
  else
  {
    // Modo Gaming (mais rápido)
    paj7620SelectBank(BANK1); 
    delay(1);
    paj7620WriteReg(0x65, 0xB7); // Altera o tempo de IDLE
    delay(1);
    paj7620SelectBank(BANK0);
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
        delay(GES_ENTRY_TIME);
        paj7620ReadReg(0x43, 1, &data);

        // Frente
        if(data == GES_FORWARD_FLAG) 
        {
          return 3;
          //delay(GES_QUIT_TIME);
        }
        // Trás
        else if(data == GES_BACKWARD_FLAG) 
        {
          return 4;
          //delay(GES_QUIT_TIME);
        }
        // Direita
        else
        {
          return 1;
        }          
        break;
        
      case GES_LEFT_FLAG:
        delay(GES_ENTRY_TIME);
        paj7620ReadReg(0x43, 1, &data);

        // Frente
        if(data == GES_FORWARD_FLAG) 
        {
          return 3;
          //delay(GES_QUIT_TIME);
        }
        // Trás
        else if(data == GES_BACKWARD_FLAG) 
        {
          return 4;
          //delay(GES_QUIT_TIME);
        }
        // Esquerda
        else
        {
          return 2;
        }          
        break;
        
      case GES_UP_FLAG:
        delay(GES_ENTRY_TIME);
        paj7620ReadReg(0x43, 1, &data);

        // Frente
        if(data == GES_FORWARD_FLAG) 
        {
          return 3;
          //delay(GES_QUIT_TIME);
        }
        // Trás
        else if(data == GES_BACKWARD_FLAG) 
        {
          return 4;
          //delay(GES_QUIT_TIME);
        }
        // Cima
        else
        {
          return 5;
        }          
        break;
      case GES_DOWN_FLAG:
        delay(GES_ENTRY_TIME);
        paj7620ReadReg(0x43, 1, &data);

        // Frente
        if(data == GES_FORWARD_FLAG) 
        {
          return 3;
          //delay(GES_QUIT_TIME);
        }
        // Trás
        else if(data == GES_BACKWARD_FLAG) 
        {
          return 4;
          //delay(GES_QUIT_TIME);
        }
        else
        {
          return 6;
        }          
        break;
        
      case GES_FORWARD_FLAG:
        // Frente
        return 3;
        //delay(GES_QUIT_TIME);
        break;
        
      case GES_BACKWARD_FLAG:   
        // Trás  
        return 4;
        //delay(GES_QUIT_TIME);
        break;
        
      case GES_CLOCKWISE_FLAG:
        // Horário
        return 7;
        break;
        
      case GES_COUNT_CLOCKWISE_FLAG:
        // Anti-horário
        return 8;
        break;
        
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
