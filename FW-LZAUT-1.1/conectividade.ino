#include "conectividade.h"

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

/*
 * Inicializa a comunicação com a rede WiFi
 */
void wifi_begin()
{
  WiFi.disconnect();

  Serial.println("Conectando na rede...");
  
  // 802.11b tem maior sensibilidade e maior potencia na transmissão
  // O sinal do Wi-fi no ponto de instalação é cerca de -80dBm
  // portanto, o único modo que consegue ler esse sinal é o b
  // O padrão é o modo 802.11n
  //WiFi.setPhyMode(WIFI_PHY_MODE_11B);

  // Mantem o Wifi do ESP8266 acordado para não ter atraso
  // significativo na resposta do Webserver
  //WiFi.setSleepMode(WIFI_NONE_SLEEP);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(NW_NAME, NW_PASSWORD);

  unsigned long timeout = millis();
  while (WiFi.waitForConnectResult() != WL_CONNECTED) 
  {
    if((millis() - timeout) > 3000)
    {
      Serial.println("Erro na conexão! Reiniciando...");
      delay(1000);
      ESP.restart();
    }
  }

  Serial.println("Conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

/*
 * Verifica se o dispositivo está conectado,
 * senão reconecta
 */
void wifi_reconnect()
{
  if(WiFi.status() != WL_CONNECTED)
  {
    WiFi.disconnect();
    WiFi.begin(NW_NAME, NW_PASSWORD);
    
    //if (WiFi.waitForConnectResult() != WL_CONNECTED) 
    //{
    // Serial.println("Erro na conexão");
    //}
  }
}

/*
 * Inicializa o web server com a página do OTA
 */
void ws_begin()
{
  // Página do OTA
  httpUpdater.setup(&server, OTA_PATH, OTA_USER, OTA_PASSWORD);

  // Web_server
  server.on("/", ws_pagina_init);

  // Ligar rele
  server.on("/rele_change", ws_rele_change);

  server.onNotFound(ws_not_found);
  
  server.begin();

  // Por 8 segundos fica esperando alguma requisição de firmware
  unsigned long delta_t = millis();
  while((millis() - delta_t) < 8000)
  {
    yield();
    server.handleClient();
  }
}

/*
 * Evento que ocorre quando o cliente acessar uma página inexistente
 */
void ws_not_found() 
{
  ws_pagina_init();
}

/*
 * Evento que ocorre quando o cliente acessa a página inicial
 */
void ws_pagina_init()
{
  // Verifica se houve alguma requisição POST
  // (Ocorre quando o botão é pressionado)
  for (uint8_t i = 0; i < server.args(); i++) 
  {
    if(server.argName(i).indexOf("rele_bot") >= 0)
    {
      ws_rele_change();
      break;
    }
  }
  
  String message = "<html style=\"height:100%\"><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"> \
  <link rel=\"icon\" href=\"https://image.flaticon.com/icons/svg/890/890345.svg\"> \
  <script>if ( window.history.replaceState ) { window.history.replaceState( null, null,  window.location.href); }</script>\
  <style>h2.titulo{  color:black; \
    background-color:#bdc3c7; \
    border-radius: 15px; \
    width:220px; \
    height:40px\
    display: inline-block; \
  } \
  </style> \
  <body style=\"background-image: url('https://i.pinimg.com/originals/b9/65/6f/b9656f5412f76e1a25396cde488c3eea.jpg'); background-repeat: round; color:#ecf0f1; height:100%;\"> \
  <div style=\"display: flex;  justify-content:center;  align-items:center; height:100%;\"> \
  <div style=\"background-color:rgba(149, 165, 185,0.9); line-height: 40px; border-radius: 40px; width:500px; border-style: outset; border-color:white\"> \
  <center><h1>Sistema de Controle da Luz</h1><hr> \
  <h2 class=\"titulo\" style=\"width:150px\">Status <img width=\"25px\" src=\"https://image.flaticon.com/icons/svg/890/890345.svg\" style='vertical-align:middle' /></h2>\
  <p>A lampada está ";

  if(digitalRead(RELE))
  {
    message += "acesa.";
  }
  else
  {
    message += "apagada.";
  }
  
  message += "</p><p>";
  
  if(!cont_pessoas)
  {
    message += "Não tem ninguém no cômodo.</p>";
  }
  else
  {
    message += "Tem ";
    message += String(cont_pessoas);
    message += " pessoa";
    if(cont_pessoas > 1)
    {
      message += "s ";
    }
    message += " no cômodo.</p>";
    for(uint8_t i = 0; i < cont_pessoas; i++)
    {
      message += "<img width=\"10%\" src=\"https://image.flaticon.com/icons/svg/10/10522.svg\"></img>";
    }
  }
  message += "<hr><h2 class=\"titulo\" style=\"width:170px\">Controles <img width=\"25px\" \
  src=\"https://image.flaticon.com/icons/svg/832/832662.svg\" style='vertical-align:middle'/></h2> \
  <form action=\"/\" method=\"POST\"><input type=\"image\" width=\"10%\" name=\"rele_bot\" alt=\"Submit\" src=\"";

  if(digitalRead(RELE))
  {
    message += "https://image.flaticon.com/icons/svg/840/840513.svg";
  }
  else
  {
    message += "https://image.flaticon.com/icons/svg/840/840554.svg";
  }

  message += "\"></form><hr><h2 class=\"titulo\">Sensor de luz <img width=\"32px\" src=\"https://image.flaticon.com/icons/svg/865/865245.svg\" style='vertical-align:middle'/></h2><p>";

  if(ldr_escuro())
  {
    message += "O ambiente está escuro.";
  }
  else
  {
    message += "O ambiente está claro.";
  }
  message += "</p><p>Valor lido pelo LDR: ";
  message += String(analogRead(A0));
  message += "<hr><h2 class=\"titulo\">Sensor de gesto <img width=\"35px\" src=\"https://image.flaticon.com/icons/svg/3234/3234059.svg\" style=\"vertical-align:middle; transform:rotate(-90deg);\"/></h2>\
  <p>";
  if(gesto_conectado)
  {
    message += "Último gesto reconhecido: ";
    message += String(gestos[ultimo_gesto]);  
  }
  else
  {
    message += "Sensor desconectado!";
  }
  
  message += "</p></center></div></div></body></html>";

  server.send(200, "text/html", message);
}


/*
 * Comuta o estado atual do relé e seta/reseta
 * a contagem de pessoas do comodo 
 */
void ws_rele_change()
{
  if(digitalRead(RELE)) // Se estava ligado
  {
    digitalWrite(RELE, LOW);
    cont_pessoas = 0;
  }
  else // Se estava desligado
  {
    cont_pessoas = 1;
    digitalWrite(RELE, HIGH);
  }
  //ws_pagina_init();
}

void ws_update()
{
  server.handleClient();
}
