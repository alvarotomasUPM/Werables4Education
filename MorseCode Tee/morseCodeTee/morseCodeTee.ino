#include <Wire.h>  // Incluye la librería Wire para comunicación I2C
#include <LiquidCrystal_I2C.h>  // Incluye la librería para controlar el LCD I2C

// Configura el LCD con dirección I2C en 0x27 y especifica las dimensiones (16 columnas x 2 filas)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Mensaje que se mostrará y se enviará en código Morse
const String mensaje = "Hola Mundo";
String codigoMorse = "";  // String para almacenar el código Morse generado

// Estructura para relacionar cada carácter alfabético con su correspondiente código Morse
const struct {
  char caracter;
  const char* morse;
} morseMap[] = {
  {'a', ".-"}, {'b', "-..."}, {'c', "-.-."}, {'d', "-.."}, {'e', "."},
  {'f', "..-."}, {'g', "--."}, {'h', "...."}, {'i', ".."}, {'j', ".---"},
  {'k', "-.-"}, {'l', ".-.."}, {'m', "--"}, {'n', "-."}, {'o', "---"},
  {'p', ".--."}, {'q', "--.-"}, {'r', ".-."}, {'s', "..."}, {'t', "-"},
  {'u', "..-"}, {'v', "...-"}, {'w', ".--"}, {'x', "-..-"}, {'y', "-.--"},
  {'z', "--.."}, {'0', "-----"}, {'1', ".----"}, {'2', "..---"},
  {'3', "...--"}, {'4', "....-"}, {'5', "....."}, {'6', "-...."},
  {'7', "--..."}, {'8', "---.."}, {'9', "----."}, {' ', "/"}
};

void setup() {
  lcd.init();  // Inicializa el LCD
  lcd.backlight();  // Enciende la retroiluminación del LCD
  lcd.setCursor(0, 0);  // Posiciona el cursor en la primera línea
  lcd.print("Mensaje:");  // Imprime "Mensaje:" en la pantalla
  lcd.setCursor(0, 1);  // Posiciona el cursor en la segunda línea
  lcd.print(mensaje);  // Imprime el mensaje en la pantalla

  pinMode(13, OUTPUT); // Establece el pin 13 como salida para controlar el LED
  generarMorse(mensaje);  // Convierte el mensaje a código Morse
}

void loop() {
  enviarMorse();  // Envía el mensaje en código Morse usando el LED
  delay(5000);  // Espera 5 segundos antes de repetir el proceso
}

// Función para generar código Morse a partir del texto proporcionado
void generarMorse(const String &texto) {
  for (char c : texto) {
    c = toLowerCase(c);  // Convierte el carácter a minúscula
    for (auto &m : morseMap) {  // Recorre la tabla de Morse
      if (c == m.caracter) {  // Si el carácter coincide
        codigoMorse += m.morse;  // Añade el código Morse correspondiente
        codigoMorse += ' ';  // Añade un espacio para separar los símbolos Morse
        break;
      }
    }
  }
}

// Función para enviar el código Morse a través de un LED
void enviarMorse() {
  for (char c : codigoMorse) {  // Recorre la cadena de código Morse
    switch (c) {
      case '.':  // Para puntos en Morse
        toggleLED(250);  // Enciende el LED por 250 ms
        break;
      case '-':  // Para rayas en Morse
        toggleLED(750);  // Enciende el LED por 750 ms
        break;
      case ' ':  // Para espacios entre letras
        delay(250);  // Espera 250 ms
        break;
    }
  }
  delay(5000);  // Pausa de 5 segundos al final de un ciclo completo
}

// Función para activar y desactivar el LED
void toggleLED(unsigned int duration) {
  digitalWrite(13, HIGH);  // Enciende el LED
  delay(duration);  // Mantiene el LED encendido por la duración especificada
  digitalWrite(13, LOW);  // Apaga el LED
  delay(250);  // Espera 250 ms antes de la siguiente señal
}
