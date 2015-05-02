const int n = 768;

short values[n];
unsigned long now, old;

void setup() {
  analogReference(DEFAULT);
  Serial.begin(9600);
  
  pinMode(13, OUTPUT);
}

void loop() {
  old = micros();
  for(int i = 0; i < n; i++)
  {
    values[i] = analogRead(0);
    delayMicroseconds(600);
  }
  now = micros();
  
  digitalWrite(13, HIGH);
  delay(200);
  
  Serial.print("Total time: ");
  now -= old;
  Serial.println(now);
  Serial.print(now / n);
  Serial.print("mcs per value. ~");
  Serial.println(now % n);
  
  for(int i = 0; i < n; i++)
  {
    Serial.println(values[i]);
  }
  
  while(true)
  {
    digitalWrite(13, HIGH);
    delay(500);
    digitalWrite(13, LOW);
    delay(500);
  }
}
