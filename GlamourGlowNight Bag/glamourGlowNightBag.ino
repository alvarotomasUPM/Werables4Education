#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include <RTClib.h>
#include "PinChangeInterrupt.h"

// Definición de pines
const uint8_t PIXEL_PIN = 7; // Pin digital al que está conectado el anillo de LEDs NeoPixel
const uint8_t NUM_PIXELS = 12; // Número de LEDs en el anillo NeoPixel
const uint8_t CLK_PIN = 2; // Pin del reloj del encoder rotativo
const uint8_t DT_PIN = 3; // Pin de datos del encoder rotativo
const uint8_t SW_PIN = 4; // Pin del botón del encoder rotativo

// Definición de rangos de tiempo
const uint8_t LEDS_ON_START_HOUR = 22; // Hora de inicio para encender LEDs (10 PM)
const uint8_t LEDS_ON_END_HOUR = 6; // Hora de fin para apagar LEDs (6 AM)

// Inicialización del anillo de LEDs
Adafruit_NeoPixel pixels(NUM_PIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Inicialización del módulo RTC (reloj en tiempo real)
RTC_DS1307 rtc;

// Variables para el encoder rotativo
volatile int lastClk = LOW; // Almacena el último estado del pin del reloj del encoder
volatile int currentClk; // Almacena el estado actual del pin del reloj del encoder
volatile int brightness = 128; // Valor inicial del brillo (0 a 255)
uint32_t currentColor = pixels.Color(255, 255, 255); // Color inicial de los LEDs (blanco)

// Variables para controlar el estado de los LEDs según la hora
bool ledsActive = false; // Indica si los LEDs están activos o no

void setup() {
  // Inicializa los componentes
  initializeComponents();
  // Verifica el estado del RTC
  checkRTC();
  // Configura las interrupciones para el encoder rotativo
  configureEncoderInterrupts();
}

void loop() {
  // Gestiona el encendido y apagado de los LEDs según la hora
  manageLEDs();
  // Maneja el botón del encoder rotativo para cambiar el color de los LEDs
  handleEncoderButton();
  delay(100); // Espera un breve período antes de la siguiente lectura para no sobrecargar el sistema
}

// Inicialización de los componentes
void initializeComponents() {
  pixels.begin(); // Inicializa el anillo de LEDs
  pixels.show(); // Apaga todos los LEDs inicialmente

  Wire.begin(); // Inicializa la comunicación I2C para el RTC
  
  // Configura los pines del encoder rotativo
  pinMode(CLK_PIN, INPUT);
  pinMode(DT_PIN, INPUT);
  pinMode(SW_PIN, INPUT_PULLUP); // Configura el botón del encoder con resistencia pull-up interna
}

// Verificación del estado del RTC
void checkRTC() {
  if (!rtc.begin()) { // Verifica si el RTC está conectado correctamente
    Serial.println("Couldn't find RTC"); // Si no, muestra un mensaje de error
    while (1); // Detiene el programa
  }

  if (!rtc.isrunning()) { // Verifica si el RTC está funcionando
    Serial.println("RTC is NOT running!"); // Si no, muestra un mensaje de error
    rtc.adjust(DateTime(__DATE__, __TIME__)); // Ajusta el RTC con la fecha y hora de compilación del código
  }
}

// Configuración de interrupciones para el encoder rotativo
void configureEncoderInterrupts() {
  // Asigna una función (updateEncoder) para ser llamada cuando el estado del pin del reloj del encoder cambie
  attachPCINT(digitalPinToPCINT(CLK_PIN), updateEncoder, CHANGE);
}

// Gestión de los LEDs según la hora
void manageLEDs() {
  DateTime now = rtc.now(); // Obtiene la hora actual del RTC
  int hour = now.hour(); // Obtiene solo la hora actual

  // Verifica si estamos dentro del rango de tiempo para encender los LEDs
  if (hour >= LEDS_ON_START_HOUR || hour < LEDS_ON_END_HOUR) {
    ledsActive = true; // Activa los LEDs
    setRingColor(currentColor, brightness); // Establece el color y brillo de los LEDs
  } else {
    ledsActive = false; // Desactiva los LEDs
    setRingColor(pixels.Color(0, 0, 0), brightness); // Apaga los LEDs
  }
}

// Manejo del botón del encoder rotativo
void handleEncoderButton() {
  if (ledsActive && digitalRead(SW_PIN) == LOW) { // Verifica si los LEDs están activos y si el botón está presionado
    currentColor = getNewColor(currentColor); // Genera un nuevo color aleatorio
    setRingColor(currentColor, brightness); // Establece el nuevo color con el brillo actual
    delay(500); // Evita el rebote del botón (espera antes de permitir otra lectura del botón)
  }
}

// Función para cambiar el color y brillo del anillo de LEDs
void setRingColor(uint32_t color, int brightness) {
  for (int i = 0; i < NUM_PIXELS; i++) { // Recorre todos los LEDs del anillo
    pixels.setPixelColor(i, color); // Establece el color para cada LED
  }
  pixels.setBrightness(brightness); // Establece el brillo para todos los LEDs
  pixels.show(); // Actualiza el anillo de LEDs para mostrar el nuevo color y brillo
}

// Función de interrupción para actualizar el encoder rotativo
void updateEncoder() {
  if (ledsActive) { // Solo actualiza si los LEDs están activos
    currentClk = digitalRead(CLK_PIN); // Lee el estado actual del pin del reloj del encoder
    if (currentClk != lastClk && currentClk == HIGH) { // Verifica si hubo un cambio en el estado del reloj
      if (digitalRead(DT_PIN) != currentClk) { // Verifica la dirección de rotación del encoder
        adjustBrightness(15); // Aumenta el brillo
      } else {
        adjustBrightness(-15); // Disminuye el brillo
      }
    }
    lastClk = currentClk; // Actualiza el último estado del reloj
  }
}

// Ajuste del brillo
void adjustBrightness(int amount) {
  brightness = constrain(brightness + amount, 0, 255); // Ajusta el brillo asegurándose de que se mantenga entre 0 y 255
  setRingColor(currentColor, brightness); // Actualiza el color con el nuevo brillo
}

// Función para generar un nuevo color aleatorio que no sea muy parecido al anterior
uint32_t getNewColor(uint32_t oldColor) {
  uint8_t oldR = (oldColor >> 16) & 0xFF; // Extrae el componente rojo del color anterior
  uint8_t oldG = (oldColor >> 8) & 0xFF; // Extrae el componente verde del color anterior
  uint8_t oldB = oldColor & 0xFF; // Extrae el componente azul del color anterior
  uint32_t newColor;
  uint8_t newR, newG, newB;
  do {
    newR = random(0, 256); // Genera un componente rojo aleatorio
    newG = random(0, 256); // Genera un componente verde aleatorio
    newB = random(0, 256); // Genera un componente azul aleatorio
    newColor = pixels.Color(newR, newG, newB); // Combina los componentes para crear un nuevo color
  } while (colorDistance(oldR, oldG, oldB, newR, newG, newB) < 100); // Asegura que el nuevo color sea suficientemente diferente al anterior
  return newColor; // Devuelve el nuevo color
}

// Función para calcular la distancia de color (cuán diferente es un color de otro)
float colorDistance(uint8_t r1, uint8_t g1, uint8_t b1, uint8_t r2, uint8_t g2, uint8_t b2) {
  return sqrt(sq(r2 - r1) + sq(g2 - g1) + sq(b2 - b1)); // Calcula la distancia entre dos colores usando la fórmula de distancia euclidiana
}
