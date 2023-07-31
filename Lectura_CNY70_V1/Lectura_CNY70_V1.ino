//--- Pines INPUT / OUTPUT ---
// Motores
#define M2A 22
#define M2B 23
#define M1B 19
#define M1A 21
// Canales
#define LEFT_1 14
#define LEFT_2 13
#define RIGHT_1 12
#define RIGHT_2 11
// CNY70
#define PIN_CNY1 4
#define PIN_CNY2 2
#define PIN_BOTON 5

//--- Cantidad sensores CNY70 ---
#define CNY_CANT 2

// --- Estados logicos ---
#define DEBUG 1
#define LEFT 0
#define RIGHT 1
#define BACKWARD 0
#define FORWARD 1
#define QUIETO 0

// --- Velocidades ---
#define VEL_MIN_PID 110
#define VEL_MIN 80
#define VEL_WHITE_FLOOR 100

// --- Calculo Proporcional ---
#define KP 10
int error, calc_p;
float gain = 120;
float error_prom_max = (KP * 15600);
float error_prom_min = (KP * -15600);

// --- Posicion lado negro ---
bool black_side = RIGHT;
// bool black_side = LEFT;

int vel_der, vel_izq;

int pines_CNY70[CNY_CANT] = { PIN_CNY1, PIN_CNY2 };

// --- Calibracion de sensores ---
int valores_max_leidos[CNY_CANT];
int valores_min_leidos[CNY_CANT];
int valor;

// --- Lecturas de sensores ---
int valores_actuales[CNY_CANT];
bool all_white = false;

bool estado = true;

const int PWMFreq = 1000;
const int PWMResolution = 8;

// --------------------------- Funciones ---------------------------
bool boton_on_off() {
  return estado = digitalRead(PIN_BOTON);
}

void pins_and_PWM_setup() {
  // --- Pines Motores (RPWM & LPWM) ---
  pinMode(M1A, OUTPUT);
  pinMode(M1B, OUTPUT);
  pinMode(M2A, OUTPUT);
  pinMode(M2B, OUTPUT);
  // --- CONFIG. PWM ---
  ledcSetup(LEFT_1, PWMFreq, PWMResolution);
  ledcSetup(LEFT_2, PWMFreq, PWMResolution);
  ledcSetup(RIGHT_1, PWMFreq, PWMResolution);
  ledcSetup(RIGHT_2, PWMFreq, PWMResolution);
  // --- Enlace canales PWM a pines motores ---
  ledcAttachPin(M1A, LEFT_1);
  ledcAttachPin(M1B, LEFT_2);
  ledcAttachPin(M2A, RIGHT_1);
  ledcAttachPin(M2B, RIGHT_2);
}

void motor(int motor, int direccion, int velocidad) {

  if (motor == 0) {

    if (direccion == 0) {  // ---> gira en un sentido
      ledcWrite(LEFT_1, velocidad);
      ledcWrite(LEFT_2, 0);

    } else {  // direccion == 1 ---> gira en el otro sentido
      ledcWrite(LEFT_1, 0);
      ledcWrite(LEFT_2, velocidad);
    }
  }

  else if (motor == 1) {

    if (direccion == 0) {  // ---> gira en un sentido
      ledcWrite(RIGHT_1, 0);
      ledcWrite(RIGHT_2, velocidad);

    } else {  // direccion == 1 ---> gira en el otro sentido
      ledcWrite(RIGHT_1, velocidad);
      ledcWrite(RIGHT_2, 0);
    }
  }
}

void setup() {

  pinMode(PIN_CNY1, INPUT);
  pinMode(PIN_CNY2, INPUT);
  pinMode(PIN_BOTON, INPUT);
  pins_and_PWM_setup();
  Serial.begin(115200);

  for (int i = 0; i < CNY_CANT; i++) {

    valores_min_leidos[i] = 100000;
    valores_max_leidos[i] = -100000;
  }

  //calibrar lecturas minimas y maximas
  while (boton_on_off() == true) {
    Serial.println("Calibrando..");
    for (int i = 0; i < CNY_CANT; i++) {

      valor = analogRead(pines_CNY70[i]);

      valores_min_leidos[i] = constrain(valores_min_leidos[i], -100000, valor);
      valores_max_leidos[i] = constrain(valores_max_leidos[i], valor, 100000);
    }

    Serial.print("Valores mininimos CNY 1: ");
    Serial.println(valores_min_leidos[0]);
    Serial.print("Valores mininimos CNY 2: ");
    Serial.println(valores_min_leidos[1]);
    Serial.println();
    Serial.print("Valores maximos CNY 1: ");
    Serial.println(valores_max_leidos[0]);
    Serial.print("Valores maximos CNY 2: ");
    Serial.println(valores_max_leidos[1]);
    delay(1000);
  }

  Serial.print("Valores mininimos CNY 1 (FINALES): ");
  Serial.println(valores_min_leidos[0]);
  Serial.print("Valores mininimos CNY 2 (FINALES): ");
  Serial.println(valores_min_leidos[1]);

  Serial.print("Valores maximos CNY 1 (FINALES): ");
  Serial.println(valores_max_leidos[0]);
  Serial.print("Valores maximos CNY 2 (FINALES): ");
  Serial.println(valores_max_leidos[1]);
  delay(5000);
}

void loop() {
  all_white = false;
  // obtener lecturas de sensores y establecer logica para la cuenta de error
  for (int i = 0; i < CNY_CANT / 2; i++) {
    valores_actuales[i] = analogRead(pines_CNY70[i]);
    valores_actuales[i] = map(valores_actuales[i], valores_min_leidos[i], valores_max_leidos[i], 1000, 0);  // es necesario hacer esta
    valores_actuales[i] = constrain(valores_actuales[i], 0, 1000);
  }

  for (int i = CNY_CANT / 2; i < CNY_CANT; i++) {
    valores_actuales[i] = analogRead(pines_CNY70[i]);
    valores_actuales[i] = map(valores_actuales[i], valores_min_leidos[i], valores_max_leidos[i], 0, 1000);
    valores_actuales[i] = constrain(valores_actuales[i], 0, 1000);
  }

  // error = (-2 * valores_actuales[0]) + (2 * valores_actuales[1]);
  error = (-2 * valores_actuales[1]) + (2 * valores_actuales[0]);
  Serial.println();
  Serial.print("Error: ");
  Serial.println(error);

  calc_p = KP * error;
  Serial.println();
  Serial.print("Calculo proporcional (P): ");
  Serial.println(calc_p);

  calc_p = map(calc_p, error_prom_min, error_prom_max, -1 * gain, gain);
  Serial.println();
  Serial.print("Calculo proporcional (P): ");
  Serial.println(calc_p);
  Serial.println();

  if (error < -1500) all_white = true;

  if (error < error_prom_min) error_prom_min = error;
  if (error > error_prom_max) error_prom_max = error;

  if (black_side == LEFT) {
    vel_der = VEL_MIN_PID - calc_p;
    vel_izq = VEL_MIN_PID + calc_p;
  } else {
    vel_der = VEL_MIN_PID + calc_p;
    vel_izq = VEL_MIN_PID - calc_p;
  }

  vel_der = constrain(vel_der, 0, 255);
  vel_izq = constrain(vel_izq, 0, 255);

  if (all_white) {

    if (black_side == LEFT) {
      Serial.print("Motor derecho: Adelante - ");
      Serial.println(VEL_WHITE_FLOOR);
      motor(RIGHT, FORWARD, VEL_WHITE_FLOOR);

      Serial.println("Motor izquierdo: Quieto - 0");
      motor(LEFT, FORWARD, QUIETO);
    } else {
      Serial.println("Motor derecho: Quieto - 0");
      motor(RIGHT, FORWARD, QUIETO);

      Serial.print("Motor izquierdo: Adelante - ");
      Serial.println(VEL_WHITE_FLOOR);
      motor(LEFT, FORWARD, VEL_WHITE_FLOOR);
    }
  } else {

    if (vel_der < VEL_MIN) {
      int x = VEL_MIN + (VEL_MIN - vel_der);
      Serial.print("Motor derecho: Atras - ");
      Serial.println(x);
      motor(RIGHT, BACKWARD, x);
    } else {
      Serial.print("Motor derecho: Adelante - ");
      Serial.println(vel_der);
      motor(RIGHT, FORWARD, vel_der);
    }

    if (vel_izq < VEL_MIN) {
      int g = VEL_MIN + (VEL_MIN - vel_izq);
      Serial.print("Motor izquierdo: Atras - ");
      Serial.println(g);
      motor(LEFT, BACKWARD, g);
    } else {
      Serial.print("Motor izquierdo: Adelante - ");
      Serial.println(vel_izq);
      motor(LEFT, FORWARD, vel_izq);
    }
  }

  delay(1000);
}
