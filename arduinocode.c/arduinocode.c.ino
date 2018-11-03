int LDR = A0;
int ldrValue = 0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(LDR,INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  ldrValue = analogRead(LDR);
  if(ldrValue >=200 && ldrValue<=990){
    Serial.print(ldrValue);
    Serial.print("\n");
  }
  delay(20);

}
