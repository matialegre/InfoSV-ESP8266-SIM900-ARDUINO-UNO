#include <SoftwareSerial.h>

SoftwareSerial mySerial(7, 8); // RX, TX
const int pinDetect = 12; // El pin al que estás conectando para detectar los 3.3V
int previousState = LOW;

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);

  pinMode(pinDetect, INPUT); // Establecer el pin como entrada

  delay(1000);

  // Configurar el modo de texto para mensajes SMS
  sendATCommand("AT+CMGF=1", 1000);
}

void loop() {
  int currentState = digitalRead(pinDetect);

  // Verificar si hay un cambio de estado en el pinDetect
  if (currentState != previousState) {
    Serial.println("Pin state changed to " + String(currentState));

    // Enviar un mensaje AT según el nuevo estado
    if (currentState == HIGH) {
      sendATCommand("AT+CPIN=1", 1000); // Enviar AT cuando el estado es alto
    } else {
      sendATCommand("AT+CPIN=0", 1000); // Enviar AT cuando el estado es bajo
    }

    previousState = currentState;
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
