#include "conexao.h"

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;


/*
 * Inicia a conexão Wifi e o webserver
 */
void conexao_begin()
{
  conexao_wifi_begin();
  conexao_ws_begin();
}

/*
 * Inicializa a comunicação com a rede WiFi
 */
void conexao_wifi_begin()
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
  WiFi.begin(CONEXAO_NW_NAME, CONEXAO_NW_PASSWORD);

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
void conexao_wifi_reconnect()
{
  if(WiFi.status() != WL_CONNECTED)
  {
    WiFi.disconnect();
    WiFi.begin(CONEXAO_NW_NAME, CONEXAO_NW_PASSWORD);
    
    //if (WiFi.waitForConnectResult() != WL_CONNECTED) 
    //{
    // Serial.println("Erro na conexão");
    //}
  }
}

/*
 * Inicializa o web server com a página do OTA
 */
void conexao_ws_begin()
{
  // Página do OTA
  httpUpdater.setup(&server, CONEXAO_OTA_PATH, CONEXAO_OTA_USER, CONEXAO_OTA_PASSWORD);

  // Web_server
  server.on("/", conexao_ws_pagina_init);

  // Ligar rele
  //server.on("/rele_change", conexao_ws_rele_change);

  server.onNotFound(conexao_ws_not_found);
  
  server.begin();

  // Por 8 segundos fica esperando alguma requisição de firmware
  // Isso é feito em caso do programa principal conter algum erro
  // (durante o desenvolvimento) e impedir a conexão para reupload
  // do código via OTA
  unsigned long delta_t = millis();
  while((millis() - delta_t) < 8000)
  {
    yield();
    server.handleClient();
  }

  // Configura o DNS
  MDNS.begin(CONEXAO_DNS_NAME);
  MDNS.addService("http", "tcp", 80);

}

/*
 * Evento que ocorre quando o cliente acessar uma página inexistente
 */
void conexao_ws_not_found() 
{
  conexao_ws_pagina_init();
}

/*
 * Evento que ocorre quando o cliente acessa a página inicial
 */
void conexao_ws_pagina_init()
{
  // Verifica se houve alguma requisição POST
  // (Ocorre quando o botão é pressionado)
  for (uint8_t i = 0; i < server.args(); i++) 
  {
    if(server.argName(i).indexOf("rele_bot.x") >= 0)
    {
      conexao_ws_rele_change();
    }
    else if(server.argName(i).indexOf("manual_bot.x") >= 0)
    {
      sensores.manual = !sensores.manual;

      // Grava o valor na memória
      sensores_manual_write();
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
  
  if(!sensores.presenca)
  {
    message += "Não tem ninguém no cômodo.</p>";
  }
  else
  {
    message += "Tem alguém no cômodo.";
  }
  
  message += "<hr><h2 class=\"titulo\" style=\"width:170px\">Controles <img width=\"25px\" \
  src=\"https://image.flaticon.com/icons/svg/832/832662.svg\" style='vertical-align:middle'/></h2> \
  <p>Modo: ";


  if(sensores.manual)
  {
    message += "Manual";
  }
  else
  {
    message += "Automático";
  }
  
  message += "<form action=\"/\" method=\"POST\"><input type=\"image\" width=\"10%\" name=\"manual_bot\" alt=\"Submit\" src=\"https://image.flaticon.com/icons/svg/724/724979.svg\">";
  
  if(sensores.manual)
  {
    message += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<input type=\"image\" width=\"10%\" name=\"rele_bot\" alt=\"Submit\" src=\"";
  
    if(digitalRead(RELE))
    {
      message += "https://image.flaticon.com/icons/svg/840/840513.svg\">";
    }
    else
    {
      message += "https://image.flaticon.com/icons/svg/840/840554.svg\">";
    }
  }

  message += "</form><hr><h2 class=\"titulo\">Sensor de luz <img width=\"32px\" src=\"https://image.flaticon.com/icons/svg/865/865245.svg\" style='vertical-align:middle'/></h2><p>";

  if(sensores.escuro)
  {
    message += "O ambiente está escuro.";
  }
  else
  {
    message += "O ambiente está claro.";
  }
  
  message += "</p><p>Valor lido pelo LDR: ";
  message += String(analogRead(A0));
    
  message += "</center></div></div></body></html>";

  server.send(200, "text/html", message);
}


/*
 * Comuta o estado atual do relé e seta/reseta
 * o acionamento entre manual/automatico
 */
void conexao_ws_rele_change()
{
  // Se estava ligado
  if(digitalRead(RELE)) 
  {
    digitalWrite(RELE, LOW);
  }
  // Se estava desligado
  else 
  {
    digitalWrite(RELE, HIGH);
  }
}

void conexao_ws_update()
{
  server.handleClient();
  MDNS.update();
}
