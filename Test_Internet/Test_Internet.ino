#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h> 
#include <ArduinoJson.h>
#include <ESP8266Ping.h>

const char* ssid = "Alegre 2.4GHz";
const char* password = "familia2020";
#define BOTtoken "6605703445:AAGYnxji-z2lSjjiQc0EJZpn1IyqNMxkX4s" 
#define CHAT_ID "1172351395"

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int botRequestDelay = 100;
unsigned long lastTimeBotRan;
const int internetStatusPin = 4;

void handleNewMessages(int numNewMessages) {
  for (int i=0; i<numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID) {
      bot.sendMessage(chat_id, "USUARIO NO AUTORIZADO", "");
      continue;
    }
    
    String text = bot.messages[i].text;
    String from_name = bot.messages[i].from_name;

if (text == "/start") {
    String welcome = "BIENVENIDO, " + from_name + ".\n";
    welcome += "USE LOS SIGUIENTES COMANDOS PARA SABER EL ESTADO DE INTERNET.\n\n";
    welcome += "/estado PARA SABER EL ESTADO DE INTERNET (irrelevante)\n";
    welcome += "/ping PARA HACER UN PING A GOOGLE \n";
    welcome += "/calidad PARA SABER LA CALIDAD DE CONEXIÓN DEL ESP AL ROUTER \n";
    welcome += "/PRENDER_DISPOSITIVO por ejemplo para prender un relé/transistor/contactor. No existe \n\n\n";
    welcome += "La calidad de la señal se mide en dBm y puede interpretarse de la siguiente manera:\n";
    welcome += "Excelente: -30 a -50 dBm\n";
    welcome += "Buena: -50 a -60 dBm\n";
    welcome += "Aceptable: -60 a -70 dBm\n";
    welcome += "Pobre: -70 a -80 dBm\n";
    welcome += "Muy pobre: -80 a -90 dBm\n";
    welcome += "Sin señal: Menos de -90 dBm\n\n";
    bot.sendMessage(chat_id, welcome, "");
}

    
    if (text == "/estado") {
      if (digitalRead(internetStatusPin)){
        bot.sendMessage(chat_id, "EL INTERNET VOLVIO.", "");
      }
      else{
        bot.sendMessage(chat_id, "EL INTERNET SE CORTO", "");
      }
    }
    if (text == "/calidad") {
  int rssi = WiFi.RSSI();
  String message = "La calidad de la señal del router es: " + String(rssi) + " dBm.\n";
  bot.sendMessage(chat_id, message, "");
}


    if (text == "/ping") {
      if(Ping.ping("google.com")) {
        bot.sendMessage(chat_id, "PING A GOOGLE EXITOSO.", "");
      } else {
        bot.sendMessage(chat_id, "PING A GOOGLE FALLIDO.", "");
      }
    }
  }
}


void setup() {
  Serial.begin(9600);
  #ifdef ESP8266
    configTime(0, 0, "pool.ntp.org");
    client.setTrustAnchors(&cert); 
  #endif

  pinMode(internetStatusPin, OUTPUT);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  #ifdef ESP32
    client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  #endif
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("CONECTANDO A SU RED WI-FI..");
  }
  Serial.println(WiFi.localIP());
}

void loop() {
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }

  if(Ping.ping("google.com")) {
    digitalWrite(internetStatusPin, HIGH);
  } else {
    digitalWrite(internetStatusPin, LOW);
  }
}
