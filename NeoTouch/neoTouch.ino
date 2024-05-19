#include <Adafruit_NeoPixel.h>

// Definición de pines
#define PIN_TAP 4       // Pin donde está conectado el módulo táctil
#define PIN_NEOPIXEL 7  // Pin donde está conectado el anillo de LEDs

// Configuración del Neopixel Ring
#define NUMPIXELS 12    // Número de LEDs en el anillo

// Definición de colores en formato RGB
const uint32_t RED = Adafruit_NeoPixel::Color(255, 0, 0);      // Rojo
const uint32_t GREEN = Adafruit_NeoPixel::Color(0, 255, 0);    // Verde
const uint32_t BLUE = Adafruit_NeoPixel::Color(0, 0, 255);     // Azul
const uint32_t YELLOW = Adafruit_NeoPixel::Color(255, 255, 0); // Amarillo
const uint32_t CYAN = Adafruit_NeoPixel::Color(0, 255, 255);   // Cian
const uint32_t MAGENTA = Adafruit_NeoPixel::Color(255, 0, 255);// Magenta

// Array de colores para cambiar entre ellos
const uint32_t COLORS[] = {RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA};
const uint8_t NUM_COLORS = sizeof(COLORS) / sizeof(COLORS[0]); // Número total de colores

// Inicialización del anillo de LEDs
Adafruit_NeoPixel pixels(NUMPIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

// Variables para el estado del módulo táctil
bool tapState = false;        // Estado actual del módulo táctil
bool tapPreviousState = false;// Estado anterior del módulo táctil
uint8_t colorIndex = 0;       // Índice del color actual en el array de colores

void setup() {
  // Configuración del pin del módulo táctil como entrada
  pinMode(PIN_TAP, INPUT);
  
  // Inicializar el anillo de LEDs
  pixels.begin(); 
  pixels.show();  // Inicializar todos los LEDs apagados
  
  // Inicializar la comunicación serial para depuración
  Serial.begin(9600);
  
  // Encender los LEDs con el primer color
  changeColor(colorIndex);
}

void loop() {
  // Leer el estado del módulo táctil
  tapState = digitalRead(PIN_TAP);
  
  // Detectar si el estado ha cambiado de no presionado a presionado
  if (tapState == HIGH && tapPreviousState == LOW) {
    Serial.println("Tap detected"); // Mostrar mensaje de depuración
    // Cambiar al siguiente color en la lista
    colorIndex = (colorIndex + 1) % NUM_COLORS;
    changeColor(colorIndex);
  }
  
  // Guardar el estado actual del módulo táctil para la próxima iteración
  tapPreviousState = tapState;
}

// Función para cambiar el color de los LEDs
void changeColor(uint8_t index) {
  // Obtener el color del array de colores según el índice
  uint32_t color = COLORS[index];
  
  // Encender todos los LEDs con el color seleccionado
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, color);
  }
  pixels.show(); // Actualizar el anillo de LEDs para mostrar el nuevo color
}

