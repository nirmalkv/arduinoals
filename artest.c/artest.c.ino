int i;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  i = 0;

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print(i);
  Serial.print("\n");
  i++;
  delay(500);

}
