const int buttonPin = 10;     // номер входа, подключенный к кнопке

// переменные
int buttonState = 0;         // переменная для хранения состояния кнопки
 
void setup() {
      Serial.begin(19200);
  Serial.print("Setup");
  // инициализируем пин, подключенный к кнопке, как вход
   pinMode(buttonPin, INPUT);   
}
 
void loop(){
  // считываем значения с входа кнопки
  buttonState = digitalRead(buttonPin);
  // проверяем нажата ли кнопка
  // если нажата, то buttonState будет HIGH:
  if (buttonState == HIGH) {   
    // включаем светодиод   
    //digitalWrite(ledPin, HIGH); 
    Serial.print("+"); 
  }
  else {
    // выключаем светодиод

     Serial.print("-");
  }
}

