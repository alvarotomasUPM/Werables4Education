// Definición de los pines utilizados en el Arduino
#define TRIGGER_PIN  7  // Pin usado para enviar el pulso al sensor ultrasonido
#define ECHO_PIN     8  // Pin para recibir el eco del pulso ultrasonido
#define BUZZER_PIN   4  // Pin conectado al buzzer para emitir sonidos

// Configuración inicial para el sensor de ultrasonido
const int DISTANCIA_MAXIMA = 10; // Distancia máxima (en centímetros) en la que el buzzer reaccionará
const int TRIGGER_HIGH_MICROS = 10; // Tiempo que el pulso estará en alto (en microsegundos)
const int TRIGGER_LOW_MICROS = 2; // Tiempo de espera antes de emitir el pulso (en microsegundos)
const int BUZZER_DELAY = 100; // Intervalo entre mediciones (en milisegundos) para evitar saturación de sonidos
const double VELOCIDAD_SONIDO_CM_MICROS = 0.034; // Velocidad del sonido en cm por microsegundo

long duracion, distancia; // Variables para almacenar la duración del eco y la distancia calculada

// Configuración inicial de los pines
void setup() {
  pinMode(TRIGGER_PIN, OUTPUT); // Configura el pin del trigger como salida
  pinMode(ECHO_PIN, INPUT); // Configura el pin del eco como entrada
  pinMode(BUZZER_PIN, OUTPUT); // Configura el pin del buzzer como salida
}

// Bucle principal del programa
void loop() {
  distancia = medirDistancia(); // Llama a la función para medir la distancia
  if (distancia <= DISTANCIA_MAXIMA) { // Verifica si la distancia es menor o igual a la máxima establecida
    activarBuzzer(distancia); // Activa el buzzer basado en la distancia medida
  }
  delay(BUZZER_DELAY); // Espera un poco antes de hacer la próxima medición
}

// Función para medir la distancia utilizando el sensor de ultrasonido
long medirDistancia() {
  enviarPulso(); // Envía un pulso al sensor
  duracion = pulseIn(ECHO_PIN, HIGH); // Mide la duración del eco recibido
  return calcularDistancia(duracion); // Calcula y devuelve la distancia basada en la duración del eco
}

// Función para enviar un pulso ultrasonido
void enviarPulso() {
  digitalWrite(TRIGGER_PIN, LOW); // Asegura que el pulso comienza en bajo
  delayMicroseconds(TRIGGER_LOW_MICROS); // Espera un corto tiempo
  digitalWrite(TRIGGER_PIN, HIGH); // Envía un pulso alto
  delayMicroseconds(TRIGGER_HIGH_MICROS); // Mantiene el pulso alto por un breve momento
  digitalWrite(TRIGGER_PIN, LOW); // Vuelve el pin a bajo
}

// Función para calcular la distancia basada en la duración del eco
long calcularDistancia(long duracion) {
  // Calcula la distancia usando la fórmula de la velocidad del sonido
  // La duración se divide entre 2 porque el sonido viaja de ida y vuelta
  return duracion * VELOCIDAD_SONIDO_CM_MICROS / 2;
}

// Función para activar el buzzer basada en la distancia medida
void activarBuzzer(long distancia) {
  // Mapea la distancia a un valor de duración del sonido del buzzer
  long duracion_sonido = map(distancia, 0, DISTANCIA_MAXIMA, 300, 50);
  digitalWrite(BUZZER_PIN, HIGH); // Activa el buzzer
  delay(duracion_sonido); // Duración del sonido basada en la distancia
  digitalWrite(BUZZER_PIN, LOW); // Apaga el buzzer
}