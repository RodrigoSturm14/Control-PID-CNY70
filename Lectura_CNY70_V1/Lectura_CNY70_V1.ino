#define PIN_CNY1 4
#define PIN_CNY2 2

void setup() {
  pinMode(PIN_CNY1, INPUT);
  pinMode(PIN_CNY2, INPUT);
  Serial.begin(115200);
}

void loop() {
  int CNY1 = analogRead(PIN_CNY1);
  int CNY2 = analogRead(PIN_CNY2);

  Serial.print("Lectura CNY1: ");
  Serial.println(CNY1);

  Serial.print("Lectura CNY2: ");
  Serial.println(CNY2);

  delay(1500);
}
