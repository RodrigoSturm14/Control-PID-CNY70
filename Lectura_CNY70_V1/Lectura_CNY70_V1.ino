#define PIN_CNY1 4
#define PIN_CNY2 2

#define PIN_BOTON 22

#define CNY_CANT 2

int pines_CNY70[CNY_CANT] = { PIN_CNY1, PIN_CNY2 };

int valores_max_leidos[CNY_CANT];
int valores_min_leidos[CNY_CANT];
int valores_actuales[CNY_CANT];

int valor;
bool estado = true;

#define KP 10
int lecturas, error, calc_p;
float gain = 120;
float error_prom_max = (KP * 15600);
float error_prom_min = (KP * -15600);

#define VEL_MIN_PID 110
#define VEL_MIN 80
#define VEL_WHITE_FLOOR 100

int vel_der, vel_izq;
#define LEFT 0
#define RIGHT 1
bool black_side = RIGHT;
// bool black_side = LEFT;
bool all_white = false;

// int valores_leidos[CNY_CANT];

//int sensor_1, sensor_2, sensor_5, sensor_6, error, PID_calc;

bool boton_on_off() {
  return estado = digitalRead(PIN_BOTON);
}

void setup() {

  pinMode(PIN_CNY1, INPUT);
  pinMode(PIN_CNY2, INPUT);
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


  /*
  sensor_1 = map(2900, 160, 3200, 1000, 0);
  Serial.print("Sensor 1 hola = ");
  Serial.println(sensor_1);
  sensor_1 = constrain(sensor_1, 0, 1000);


  sensor_6 = map(2900, 160, 3200, 0, 1000);
  Serial.print("Sensor 6 hola= ");
  Serial.println(sensor_6);
  sensor_6 = constrain(sensor_6, 0, 1000);

  while(boton_on_off() == true){
    int valor_cny1 = analogRead(PIN_CNY1);
    int valor_cny1 = analogRead(PIN_CNY1);
    // valores_min son los valores minnimos del sensor 
    valores_min[0] = constrain(min_value_read[0], -100000, valor_cny1); 
    valores_min[1] = constrain(min_value_read[1], -100000, valor_cny2);
    // valores_max son los valores maximos del sensor q dejo q el sensor tome 
    valores_max[0] = constrain(max_value_read[0], valor_cny1, 100000);
    valores_max[1] = constrain(max_value_read[1], valor_cny2, 100000);
  }

  error = (-16 * sensor_1) + (-8 * sensor_2) + (-4 * sensor_2) + (-2 * sensor_2) + (2 * sensor_5) + (4 * sensor_6) + (8 * sensor_6) + (16 * sensor_6);

  Serial.print("Error = ");
  Serial.println(error);


  PID_calc = (kp * -29220);
  calc_pid_min_promedio = -156000;
  calc_pid_max_promedio = 156000;
  PID_calc = map(PID_calc, calc_pid_min_promedio, calc_pid_max_promedio, (-1 * gain), gain);  // lo q le voy a restar a la velocidad de los motores

  Serial.print("Calculo P(ID) = ");
  Serial.println(PID_calc);
  */
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
      Serial.println("Motor izquierdo: Quieto - 0");
    } else {
      Serial.println("Motor derecho: Quieto - 0");
      Serial.print("Motor izquierdo: Adelante - ");
      Serial.println(VEL_WHITE_FLOOR);
    }
  } else {
    
    if (vel_der < VEL_MIN) {
      int x = VEL_MIN + (VEL_MIN - vel_der);
      Serial.print("Motor derecho: Atras - ");
      Serial.println(x);
    }
    else{
      Serial.print("Motor derecho: Adelante - ");
      Serial.println(vel_der);
    }

    if (vel_izq < VEL_MIN) {
      int g = VEL_MIN + (VEL_MIN - vel_izq);
      Serial.print("Motor izquierdo: Atras - ");
      Serial.println(g);
    }
    else{
      Serial.print("Motor izquierdo: Adelante - ");
      Serial.println(vel_izq);
    }
  }

  delay(1000);

  /*
  int CNY1 = analogRead(PIN_CNY1);
  int CNY2 = analogRead(PIN_CNY2);

  Serial.print("Lectura CNY1: ");
  Serial.println(CNY1);

  Serial.print("Lectura CNY2: ");
  Serial.println(CNY2);

  delay(1500);
  */
}
