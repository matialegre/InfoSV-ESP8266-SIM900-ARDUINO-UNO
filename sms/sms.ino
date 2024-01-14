#include <SoftwareSerial.h>

SoftwareSerial mySerial(7, 8); // RX, TX
const int pinDetect = 12; // El pin al que estás conectando para detectar los 3.3V
const int pinPower = 13; // El pin al que estás conectando para detectar la electricidad
int previousStateInternet = LOW;
int previousStatePower = LOW;

String phoneNumber = "+542934459101"; // Reemplaza esto con el número al que deseas enviar el mensaje
String messageInternetBack = "El internet volvió."; 
String messageInternetDown = "El internet se cayó.";
String messagePowerBack = "La electricidad volvió.";
String messagePowerDown = "La electricidad se apagó.";

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);

  pinMode(pinDetect, INPUT); // Establecer el pin como entrada
  pinMode(pinPower, INPUT); // Establecer el pin como entrada

  delay(1000);

  // Configurar el modo de texto para mensajes SMS
  sendATCommand("AT+CMGF=1", 1000);
}

void loop() {
  int currentStateInternet = digitalRead(pinDetect);
  int currentStatePower = digitalRead(pinPower);

  // Verificar si hay un cambio de estado en el pinDetect
  if (currentStateInternet != previousStateInternet) {
    Serial.println("Internet state changed to " + String(currentStateInternet));

    // Enviar un mensaje AT según el nuevo estado
    if (currentStateInternet == HIGH) {
      sendSMS(phoneNumber, messageInternetBack); // Enviar SMS cuando el estado es alto
    } else {
      sendSMS(phoneNumber, messageInternetDown); // Enviar SMS cuando el estado es bajo
    }

    previousStateInternet = currentStateInternet;
  }

  // Verificar si hay un cambio de estado en el pinPower
  if (currentStatePower != previousStatePower) {
    Serial.println("Power state changed to " + String(currentStatePower));

    // Enviar un mensaje AT según el nuevo estado
    if (currentStatePower == HIGH) {
      sendSMS(phoneNumber, messagePowerBack); // Enviar SMS cuando el estado es alto
    } else {
      sendSMS(phoneNumber, messagePowerDown); // Enviar SMS cuando el estado es bajo
    }

    previousStatePower = currentStatePower;
  }

  // Puedes agregar código adicional en el bucle si es necesario
}

void sendATCommand(String command, int timeout) {
  mySerial.println(command);
  delay(timeout);
  while (mySerial.available()) {
    Serial.write(mySerial.read());
  }
}

void sendSMS(String number, String message) {
  sendATCommand("AT+CMGS=\"" + number + "\"", 1000);
  delay(1000);
  mySerial.println(message);
  delay(1000);
  mySerial.write(26);
}
