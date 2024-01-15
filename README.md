# InfoSV-ESP8266-SIM900-ARDUINO-UNO

Hablarle a @INFO_SERVIDORBOT por telegram, 1 sola persona admitida

Para estar admitido en el bot, pasar ID hablandole a @JsonDumpBot


![8795ac77c535eff24812598928eec8cf](https://github.com/matialegre/InfoSV-ESP8266-SIM900-ARDUINO-UNO/assets/127926199/c29a7ac1-0944-4f68-a969-faaa97c62b70)



Si la conexión a google no da ping (no hay internet), pin GPIO2 = 4 en código, se pone en estado bajo, conectado al pin 12 del Arduino Uno, significando estado alto = hay internet, estado bajo = no hay internet. 

De acá, envía un mensaje por SMS al número registrado/números rgistrados, avisando si hay, o no hay internet.
![3cb4d0a475d53698ee8d39753001186d](https://github.com/matialegre/InfoSV-ESP8266-SIM900-ARDUINO-UNO/assets/127926199/894124df-13c7-48e4-9bf0-0d5d891808b9)



Para el tema electricidad, se usa el pin 13 como entrada de 5V, viniendo de un adaptador para avisar si hay o no hay ELECTRICIDAD.

Se necesita una FUENTE EXTERNA potente para poder hacer andar la antena GSM (2A COMO MÍNIMO, 5V/9V/12V) A SOLUCIONAR.



Los datos son avisados vía SMS, ya que la unica forma de avisar por internet, es solo cuando hay internet.

El resto de las opciones, son una extensión del proyecto para agregar cosas.


Comunicación UART Arduino UNO - PC

![444838a83d72a0292252ffddf6e083af](https://github.com/matialegre/InfoSV-ESP8266-SIM900-ARDUINO-UNO/assets/127926199/8895c4ff-5978-49ba-bfd8-6c74d84fb41f)


Todos los comandos enviados al SIM900 son comandos AT por Serial UAR

