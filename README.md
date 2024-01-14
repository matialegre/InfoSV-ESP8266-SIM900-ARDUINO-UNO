# InfoSV-ESP8266-SIM900-ARDUINO-UNO

Hablarle a @INFO_SERVIDORBOT por telegram, 1 sola persona admitida

Para estar admitido en el bot, pasar ID hablandole a @JsonDumpBot


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


Si la conexión a google no da ping (no hay internet), pin GPIO2 = 4 en código, se pone en estado bajo, conectado al pin 12 del Arduino Uno, significando estado alto = hay internet, estado bajo = no hay internet. 

De acá, envía un mensaje por SMS al número registrado/números rgistrados, avisando si hay, o no hay internet.


Para el tema electricidad, se usa el pin 13 como entrada de 5V, viniendo de un adaptador para avisar si hay o no hay ELECTRICIDAD.

Se necesita una FUENTE EXTERNA potente para poder hacer andar la antena GSM (2A COMO MÍNIMO, 5V/9V/12V) A SOLUCIONAR.



Los datos son avisados vía SMS, ya que la unica forma de avisar por internet, es solo cuando hay internet.

El resto de las opciones, son una extensión del proyecto para agregar cosas.

