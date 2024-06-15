#include <Wire.h> // Biblioteca para la comunicación I2C
#include <LiquidCrystal_I2C.h> // Biblioteca para controlar la pantalla LCD con I2C
#include <Adafruit_NeoPixel.h> // Biblioteca para controlar los LEDs NeoPixel

// Definición de pines
#define PIXEL_PIN 7 // Pin digital al que está conectado el anillo de LEDs NeoPixel
#define NUM_PIXELS 12 // Número de LEDs en el anillo NeoPixel
#define TEMP_ANALOG_PIN A0 // Pin analógico para leer el valor del sensor de temperatura
#define TEMP_DIGITAL_PIN 2 // Pin digital para el sensor de temperatura (no se usa en este caso)

// Definición de colores en formato RGB (Rojo, Verde, Azul)
const uint32_t YELLOW = Adafruit_NeoPixel::Color(255, 255, 0); // Amarillo
const uint32_t CYAN = Adafruit_NeoPixel::Color(0, 255, 255); // Cian
const uint32_t MAGENTA = Adafruit_NeoPixel::Color(255, 0, 255); // Magenta

// Inicialización del anillo de LEDs NeoPixel
Adafruit_NeoPixel pixels(NUM_PIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Inicialización del LCD con la dirección I2C (0x27) y tamaño de la pantalla (16 columnas, 2 filas)
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  // Inicializar el LCD
  lcd.init(); // Inicializa la pantalla LCD
  lcd.backlight(); // Enciende la luz de fondo del LCD

  // Inicializar el anillo de LEDs
  pixels.begin(); // Inicializa el anillo de LEDs NeoPixel
  pixels.show(); // Apaga todos los LEDs inicialmente

  // Configurar el pin del sensor de temperatura como entrada
  pinMode(TEMP_DIGITAL_PIN, INPUT); // Configura el pin digital del sensor de temperatura como entrada
}

void loop() {
  // Leer el valor del sensor de temperatura
  int tempReading = analogRead(TEMP_ANALOG_PIN); // Lee el valor analógico del sensor de temperatura
  float voltage = tempReading * 5.0 / 1024.0; // Convierte el valor leído a voltaje
  float temperatureC = (voltage - 0.5) * 100.0; // Convierte el voltaje a grados Celsius

  // Mostrar la temperatura en el LCD
  lcd.setCursor(0, 0); // Coloca el cursor en la primera columna, primera fila
  lcd.print("Temp: "); // Muestra el texto "Temp: " en la pantalla LCD
  lcd.print(temperatureC); // Muestra el valor de la temperatura en la pantalla LCD
  lcd.print(" C"); // Muestra el símbolo de grados Celsius en la pantalla LCD

  // Actualizar el color del anillo de LEDs según la temperatura
  if (temperatureC < 20) { // Si la temperatura es menor a 20 grados Celsius
    setRingColor(CYAN); // Cambia el color del anillo de LEDs a cian
  } else if (temperatureC >= 20 && temperatureC < 30) { // Si la temperatura está entre 20 y 30 grados Celsius
    setRingColor(YELLOW); // Cambia el color del anillo de LEDs a amarillo
  } else { // Si la temperatura es mayor o igual a 30 grados Celsius
    setRingColor(MAGENTA); // Cambia el color del anillo de LEDs a magenta
  }

  delay(1000); // Espera 1 segundo antes de actualizar nuevamente
}

// Función para cambiar el color del anillo de LEDs
void setRingColor(uint32_t color) {
  for (int i = 0; i < NUM_PIXELS; i++) { // Recorre todos los LEDs en el anillo
    pixels.setPixelColor(i, color); // Establece el color del LED actual
  }
  pixels.show(); // Actualiza el anillo de LEDs para mostrar el nuevo color
}
