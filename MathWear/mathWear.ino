#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <math.h>

// Configuración de la pantalla LCD (donde se mostrarán los números y resultados)
const int LCD_ADDRESS = 0x27; // Dirección I2C de la pantalla LCD
const int LCD_COLUMNS = 16;   // Número de columnas del LCD
const int LCD_ROWS = 2;       // Número de filas del LCD

LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);

// Configuración del teclado de membrana (el teclado en la camiseta)
const byte ROWS = 4;          // Número de filas en el teclado
const byte COLS = 4;          // Número de columnas en el teclado
char keys[ROWS][COLS] = {     // Asignación de caracteres a cada tecla
  {'1', '2', '3', '+'},
  {'4', '5', '6', '-'},
  {'7', '8', '9', '*'},
  {'C', '0', '=', '/'}
};

// Pines del Arduino conectados a las filas y columnas del teclado
byte colPins[ROWS] = {5, 4, 3, 2}; 
byte rowPins[COLS] = {9, 8, 7, 6};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Variables globales para almacenar los datos ingresados y los resultados
String inputString = ""; // Cadena para almacenar la entrada del usuario
char operatorChar;       // Carácter del operador (+, -, *, /)
float num1, num2, result; // Números y resultado de las operaciones
bool newInput = true;    // Indica si se está ingresando un nuevo número
bool resultDisplayed = false; // Indica si se está mostrando un resultado
bool operatorEntered = false; // Indica si se ha ingresado un operador
bool errorState = false; // Indica si hay un estado de error

// Declaración de funciones
void setup();
void loop();
void resetCalculator();
void showError();
void handleKey(char key);
void handleNumber(char key);
void handleOperator(char key);
void handleEquals();

void setup() {
  lcd.init();       // Inicializar la pantalla LCD
  lcd.backlight();  // Encender la luz de fondo de la pantalla
  resetCalculator(); // Reiniciar la calculadora al iniciar
}

void loop() {
  char key = keypad.getKey(); // Leer la tecla presionada
  if (key) {                  // Si se presionó una tecla
    handleKey(key);           // Manejar la tecla presionada
  }
}

// Función para reiniciar la calculadora
void resetCalculator() {
  inputString = "";          // Limpiar la cadena de entrada
  operatorChar = '\0';       // Reiniciar el operador
  num1 = num2 = result = 0.0; // Reiniciar los números y el resultado
  newInput = true;           // Permitir nueva entrada
  resultDisplayed = false;   // Indicar que no se está mostrando un resultado
  operatorEntered = false;   // Indicar que no se ha ingresado un operador
  errorState = false;        // Indicar que no hay un estado de error
  lcd.clear();               // Limpiar la pantalla LCD
  lcd.setCursor(0, 0);
  lcd.print("MathWear :)");  // Mostrar mensaje de bienvenida en la pantalla
}

// Función para mostrar un mensaje de error y reiniciar la calculadora
void showError() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Error de Calculo"); // Mostrar mensaje de error
  delay(2000);                   // Esperar 2 segundos
  resetCalculator();             // Reiniciar la calculadora
}

// Función para manejar la tecla presionada
void handleKey(char key) {
  if (errorState && key != 'C') { // Ignorar todas las entradas excepto 'C' si hay un estado de error
    return;
  }
  
  if (key >= '0' && key <= '9') { // Si la tecla es un número
    handleNumber(key);
  } else if (key == 'C') {        // Si la tecla es 'C'
    resetCalculator(); // Reiniciar el estado
  } else if (key == '+' || key == '-' || key == '*' || key == '/') { // Si la tecla es un operador
    handleOperator(key);
  } else if (key == '=') {        // Si la tecla es '='
    handleEquals();
  }
}

// Función para manejar la entrada de números
void handleNumber(char key) {
  if (newInput || resultDisplayed || errorState) {
    inputString = ""; // Limpiar la cadena de entrada
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("MathWear :)");
    newInput = false;         // Reiniciar la bandera
    resultDisplayed = false;  // Reiniciar la bandera de resultado
    errorState = false;       // Reiniciar la bandera de error
  }
  inputString += key;         // Añadir el número a la cadena de entrada
  lcd.setCursor(0, 1);
  lcd.print(inputString);     // Mostrar el número en la pantalla
  operatorEntered = false;    // Permitir la entrada de un operador después del número
}

// Función para manejar la entrada de operadores
void handleOperator(char key) {
  if (inputString.length() == 0) { // No hacer nada si no hay un número antes del operador
    return;
  }
  if (resultDisplayed) {
    resultDisplayed = false;
  }
  num1 = inputString.toFloat(); // Convertir la cadena de entrada a un número
  operatorChar = key;           // Guardar el operador
  inputString = "";             // Limpiar la cadena de entrada
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MathWear :)");
  lcd.setCursor(0, 1);
  lcd.print(operatorChar);      // Mostrar el operador en la pantalla
  newInput = false;             // Reiniciar la bandera
  operatorEntered = true;       // Indicar que se ingresó un operador
}

// Función para manejar la tecla '='
void handleEquals() {
  if (operatorEntered) {        // Mostrar error si '=' se presiona después de un operador
    showError();
    errorState = true;
    resetCalculator();
    return;
  }
  if (resultDisplayed) {
    num1 = result; // Usar el resultado actual como num1
  } else {
    num2 = inputString.toFloat(); // Convertir la cadena de entrada a un número
  }

  bool error = false;
  switch (operatorChar) { // Realizar la operación correspondiente
    case '+':
      result = num1 + num2;
      break;
    case '-':
      result = num1 - num2;
      break;
    case '*':
      result = num1 * num2;
      break;
    case '/':
      if (num2 != 0) {
        result = num1 / num2;
      } else {
        error = true; // Establecer error si se intenta dividir por cero
      }
      break;
    default:
      error = true;
  }

  if (error || isinf(result) || isnan(result) || abs(result) > 9.9E+10) { // Verificación de desbordamiento y errores
    showError(); // Mostrar error y reiniciar la calculadora automáticamente en caso de error
    return;
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Resultado ="); // Mostrar el resultado en la pantalla
    lcd.setCursor(0, 1);
    lcd.print(result);
    inputString = String(result); // Guardar el resultado en la cadena de entrada
    newInput = true; // Configurar la bandera para una nueva entrada
    resultDisplayed = true; // Indicar que se muestra el resultado
    operatorEntered = false; // Reiniciar la bandera de operador
  }
}
