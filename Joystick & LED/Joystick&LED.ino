#include <Adafruit_NeoPixel.h>

#define PIN_LED 7     // Pin conectado al anillo LED
#define NUM_LEDS 12   // Número de LEDs en el anillo

#define PIN_X A0      // Pin para eje X del joystick
#define PIN_Y A1      // Pin para eje Y del joystick
#define PIN_BTN 4     // Pin para el botón del joystick

#define JOYSTICK_CENTER 512  // Valor central para el eje Y del joystick
#define JOYSTICK_THRESHOLD 100  // Umbral para detectar movimiento

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN_LED, NEO_GRB + NEO_KHZ800);

int brightness = 128; // Brillo inicial a la mitad

void setup() {
  strip.begin();
  strip.show();
  pinMode(PIN_BTN, INPUT_PULLUP);
}

void loop() {
  int yVal = analogRead(PIN_Y);
  bool buttonPressed = digitalRead(PIN_BTN) == LOW;

  // Detectar movimiento del joystick en el eje Y
  if (yVal < (JOYSTICK_CENTER - JOYSTICK_THRESHOLD)) {
    brightness = max(0, brightness - 10); // Disminuir brillo
  } else if (yVal > (JOYSTICK_CENTER + JOYSTICK_THRESHOLD)) {
    brightness = min(255, brightness + 10); // Aumentar brillo
  }

  // Cambiar colores al presionar el botón
  static int currentColor = 0;
  if (buttonPressed) {
    currentColor = (currentColor + 1) % 3; // Cambia entre 3 colores
    delay(200); // Debounce delay
  }

  uint32_t newColor;
  if (currentColor == 0) {
    newColor = strip.Color(255, 0, 0); // Rojo
  } else if (currentColor == 1) {
    newColor = strip.Color(0, 255, 0); // Verde
  } else {
    newColor = strip.Color(0, 0, 255); // Azul
  }

  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, newColor);
  }
  strip.setBrightness(brightness);
  strip.show();

  // Pequeña pausa para evitar cambios demasiado rápidos
  delay(100);
}