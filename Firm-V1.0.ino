#include <ESP8266WiFi.h>
#include <WiFiClientSecureBearSSL.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>

// -----------------------------------------------------------------------------
// Config Wi-Fi
// -----------------------------------------------------------------------------
const char* ssid     = "Alegre 2.4GHz";
const char* password = "familia2020";

// -----------------------------------------------------------------------------
// Configuración de Telegram (solo para leer /ON, no se responderá a Telegram)
// -----------------------------------------------------------------------------
#define BOT_TOKEN        "6605703445:AAGYnxji-z2lSjjiQc0EJZpn1IyqNMxkX4s"
#define CHAT_ID          "1172351395"    // Tu ID personal (opcional si quieres filtrar)
#define GROUP_CHAT_ID    "-4693782770"   // Grupo, si filtras por este

// -----------------------------------------------------------------------------
// Pines del ESP8266
// -----------------------------------------------------------------------------
const int relayPin       = 5;   // GPIO5 (D1)
const int powerDetectPin = 4;   // GPIO4 (D2)

// (OJO: Uso de GPIO0 y GPIO2 para SIM800 no es ideal, pero replicamos tu config)
const int simRxPin       = 0;   // GPIO0 => TX SIM800
const int simTxPin       = 2;   // GPIO2 => RX SIM800

// SIM800
SoftwareSerial sim800(simRxPin, simTxPin);

// -----------------------------------------------------------------------------
// Variables globales
// -----------------------------------------------------------------------------
long lastUpdateId  = 0;  // Para getUpdates
unsigned long lastPingTime       = 0;
unsigned long lastPowerCheckTime = 0;
int internetStatus = HIGH;
int powerStatus    = HIGH;

// -----------------------------------------------------------------------------
// Mensajes SMS
// -----------------------------------------------------------------------------
const char* phoneNumber          = "+542914198573";
const char* alternatePhoneNumber = "+542920504846";
const char* smsPowerDown         = "Se cortó la electricidad.";
const char* smsPowerBack         = "Se reestableció la electricidad.";
const char* smsInternetDown      = "Se perdió la conexión a internet.";
const char* smsInternetBack      = "La conexión a internet ha sido restablecida.";

// -----------------------------------------------------------------------------
// Enviar SMS con SIM800
// -----------------------------------------------------------------------------
void sendSMS(const char* number, const char* message) {
  sim800.println("AT+CMGF=1"); // Modo texto
  delay(500);
  sim800.println(String("AT+CMGS=\"") + number + "\"");
  delay(500);
  sim800.println(message);
  delay(500);
  sim800.write(26); // CTRL+Z
  delay(5000);
  Serial.println("Mensaje enviado por SMS a " + String(number) + ": " + String(message));
}

// -----------------------------------------------------------------------------
// Probar SIM800
// -----------------------------------------------------------------------------
void testSIM800() {
  Serial.println("Probando comunicación con SIM800...");
  sim800.println("AT");
  delay(1000);
  if (sim800.available()) {
    String response = sim800.readString();
    Serial.println("Respuesta del SIM800: " + response);
  } else {
    Serial.println("No hubo respuesta del SIM800. Verifica conexiones y señal.");
  }
}

// -----------------------------------------------------------------------------
// Activar relé 1 segundo
// -----------------------------------------------------------------------------
void handleRelayCommand() {
  digitalWrite(relayPin, HIGH);
  Serial.println("Relé activado (HIGH).");
  delay(1000);
  digitalWrite(relayPin, LOW);
  Serial.println("Relé desactivado (LOW).");
}

// -----------------------------------------------------------------------------
// Leer mensajes de Telegram para capturar /ON
// (NO enviamos respuesta a Telegram, solo procesamos el comando)
// -----------------------------------------------------------------------------
void handleTelegramMessages() {
  // Creamos cliente WiFi seguro para GET a Telegram (aunque no responderemos)
  std::unique_ptr<BearSSL::WiFiClientSecure> clienteWifi(new BearSSL::WiFiClientSecure);
  clienteWifi->setInsecure();

  HTTPClient http;
  String url = "https://api.telegram.org/bot" + String(BOT_TOKEN)
               + "/getUpdates?offset=" + String(lastUpdateId + 1);

  if (!http.begin(*clienteWifi, url)) {
    Serial.println("Error iniciando HTTP para getUpdates.");
    return;
  }

  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    String response = http.getString();
    Serial.println("Mensajes recibidos: " + response);

    DynamicJsonDocument doc(4096);
    DeserializationError error = deserializeJson(doc, response);
    if (error) {
      Serial.println("Error deserializando JSON: " + String(error.c_str()));
      http.end();
      return;
    }

    JsonArray results = doc["result"].as<JsonArray>();
    for (JsonObject message : results) {
      lastUpdateId = message["update_id"] | 0;

      String text = message["message"]["text"] | "";
      long chatId = message["message"]["chat"]["id"] | 0;

      Serial.println("Mensaje recibido: " + text);

      // Filtra si quieres chatId == tu grupo o chat personal
      // if (chatId == atol(GROUP_CHAT_ID) || chatId == atol(CHAT_ID)) {
      //   ...
      // }

      // Si es /ON
      if (text.startsWith("/ON")) {
        Serial.println("Comando /ON detectado.");
        handleRelayCommand(); // Activa el relé

        // Enviar SMS confirmando
        sendSMS(phoneNumber, "Se activó el relé (1 seg).");
        sendSMS(alternatePhoneNumber, "Se activó el relé (1 seg).");
      }
    }
  } else {
    Serial.printf("Error al obtener mensajes de Telegram: %d\n", httpCode);
  }
  http.end();
}

// -----------------------------------------------------------------------------
// checkInternetStatus
// -----------------------------------------------------------------------------
void checkInternetStatus() {
  if (millis() - lastPingTime > 2000) {
    lastPingTime = millis();
    if (WiFi.status() != WL_CONNECTED) {
      if (internetStatus == HIGH) {
        internetStatus = LOW;
        sendSMS(phoneNumber, smsInternetDown);
        sendSMS(alternatePhoneNumber, smsInternetDown);
        Serial.println("⚠️ Se perdió la conexión a internet.");
      }
    } else {
      if (internetStatus == LOW) {
        internetStatus = HIGH;
        sendSMS(phoneNumber, smsInternetBack);
        sendSMS(alternatePhoneNumber, smsInternetBack);
        Serial.println("✅ Conexión a internet restablecida.");
      }
    }
  }
}

// -----------------------------------------------------------------------------
// checkPowerStatus
// -----------------------------------------------------------------------------
void checkPowerStatus() {
  if (millis() - lastPowerCheckTime > 2000) {
    lastPowerCheckTime = millis();
    int currentPowerStatus = digitalRead(powerDetectPin);

    if (currentPowerStatus != powerStatus) {
      if (currentPowerStatus == LOW) {
        sendSMS(phoneNumber, smsPowerDown);
        sendSMS(alternatePhoneNumber, smsPowerDown);
        Serial.println("⚠️ Corte de electricidad detectado.");
      } else {
        sendSMS(phoneNumber, smsPowerBack);
        sendSMS(alternatePhoneNumber, smsPowerBack);
        Serial.println("✅ Electricidad restablecida.");
      }
      powerStatus = currentPowerStatus;
    }

    if (currentPowerStatus == HIGH) {
      Serial.println("Energía presente (3.3V detectado en el pin).");
    } else {
      Serial.println("No hay energía (0V detectado en el pin).");
    }
  }
}

// -----------------------------------------------------------------------------
// SETUP
// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  sim800.begin(9600);

  Serial.println("Inicializando...");

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);

  pinMode(powerDetectPin, INPUT);

  // Conectar WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a Wi-Fi...");
  }
  Serial.println("Conectado a Wi-Fi. IP: " + WiFi.localIP().toString());

  // Probar SIM800
  testSIM800();

  // Opcional: enviar SMS avisando que el sistema inició
  sendSMS(phoneNumber, "Sistema iniciado correctamente.");
  sendSMS(alternatePhoneNumber, "Sistema iniciado correctamente.");
}

// -----------------------------------------------------------------------------
// LOOP
// -----------------------------------------------------------------------------
void loop() {
  checkInternetStatus();
  checkPowerStatus();
  handleTelegramMessages();

  // Pequeña pausa para no saturar getUpdates
  delay(2000);
}
