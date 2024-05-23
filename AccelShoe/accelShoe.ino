#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include <MPU6050.h>

// Definir pines
#define PIN_NEOPIXEL 4  // Pin del Neopixel Ring
#define NUMPIXELS 12    // Número de LEDs en el anillo
#define PIN_INT 7       // Pin de interrupción del MPU-6050

// Inicializar el Neopixel
Adafruit_NeoPixel pixels(NUMPIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

// Inicializar el MPU-6050
MPU6050 mpu;

// Variable para indicar si se ha producido una interrupción
volatile bool mpuInterrupt = false;

// Prototipos de funciones
void setupNeopixel();
void setupMPU6050();
void updateLEDsBasedOnMotion();
void changeColor(uint32_t color);

// Función de interrupción para el MPU-6050
void dmpDataReady() {
  mpuInterrupt = true;
}

void setup() {
  // Inicializar comunicación serial para depuración
  Serial.begin(9600);

  // Configurar Neopixel y MPU-6050
  setupNeopixel();
  setupMPU6050();
}

void loop() {
  if (mpuInterrupt) {
    mpuInterrupt = false;
    updateLEDsBasedOnMotion();
  }
}

// Configurar el anillo de LEDs Neopixel
void setupNeopixel() {
  pixels.begin();
  pixels.show(); // Inicializar todos los LEDs apagados
}

// Configurar el sensor MPU-6050
void setupMPU6050() {
  Wire.begin();
  mpu.initialize();

  // Verificar conexión con el MPU-6050
  if (!mpu.testConnection()) {
    Serial.println("Error de conexión con el MPU-6050");
    while (1);
  } else {
    Serial.println("MPU-6050 conectado correctamente");
  }

  // Configurar la interrupción
  attachInterrupt(digitalPinToInterrupt(PIN_INT), dmpDataReady, RISING);

  // Habilitar la interrupción de datos de movimiento
  mpu.setIntMotionEnabled(true);
}

// Actualizar los LEDs en función del movimiento detectado
void updateLEDsBasedOnMotion() {
  int16_t ax, ay, az;
  int16_t gx, gy, gz;

  // Obtener datos del acelerómetro
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // Convertir los valores del acelerómetro a g's
  float ax_g = ax / 16384.0;
  float ay_g = ay / 16384.0;
  float az_g = az / 16384.0;

  // Calcular la magnitud del vector de aceleración
  float magnitude = sqrt(ax_g * ax_g + ay_g * ay_g + az_g * az_g);

  // Definir umbrales para el cambio de color
  if (magnitude > 2.0) {
    changeColor(pixels.Color(255, 0, 0)); // Rojo para movimiento fuerte
  } else if (magnitude > 1.0) {
    changeColor(pixels.Color(0, 255, 0)); // Verde para movimiento moderado
  } else {
    changeColor(pixels.Color(0, 0, 255)); // Azul para poco o ningún movimiento
  }
}

// Función para cambiar el color de los LEDs
void changeColor(uint32_t color) {
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, color);
  }
  pixels.show();
}
