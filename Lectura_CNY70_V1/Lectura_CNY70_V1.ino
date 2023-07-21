int PinLectura = A0;

void setup() {
  
  pinMode(PinLectura, INPUT);
  Serial.begin(9600);
}

void loop() {

  Serial.println("Lectura: ");
  Serial.println(analogRead(PinLectura));
  delay(2000);
}
