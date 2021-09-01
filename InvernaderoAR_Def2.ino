
#include <DHT.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>    //lcd
#include <Wire.h>                //lcd

#define Ventilador 12 //
#define Calefaccion 6 //
const int sensorDHT = A0;
int temp;
int hum;
int ctrl = 0;
const int bomba = 13; //Pin 13 para la bomba
const int lectura = A1; //A1 para la humedad del suelo
int TRIG = 10;      // trigger en pin 10
int ECO = 9;      // echo en pin 9
int DURACION;
int DISTANCIA;
int lecturaPorcentaje = map(lectura, 1023, 0, 0, 100);//Convirtiendo a Porcentaje

SoftwareSerial BT1(4,3); //Harán de RX Y TX, respectivamente.
DHT dht (sensorDHT,DHT11);
LiquidCrystal_I2C lcd(0x27,16,2);

//Dibujo caracter grado
//byte sw;
byte grado[8] =
 {
 0b00001100,
 0b00010010,
 0b00010010,
 0b00001100,
 0b00000000,
 0b00000000,
 0b00000000,
 0b00000000
 };
 
void setup() {
  BT1.begin(38400); //Comunicacion con modulo BT
  Serial.begin(9600); // Comunicacion con la PC
  
  pinMode(Ventilador, OUTPUT);
  pinMode(Calefaccion, OUTPUT);
  digitalWrite(Ventilador,LOW);
  digitalWrite(Calefaccion, LOW); // pinMode(in,INPUT);   

  pinMode(bomba, OUTPUT);
  pinMode(lectura, INPUT);
  pinMode(TRIG, OUTPUT);  // trigger ultrasonico como salida
  pinMode(ECO, INPUT);    // echo como ultrasonico entrada
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.createChar(1, grado);// agregamos el caracter grado
  }
void loop() {
  // -----------------------------------------DATOS------------------------------------
  //  HUMEDAD Y TEMPERATURA
  temp=dht.readTemperature();
  hum=dht.readHumidity();
  int lectura = analogRead(A1); 
  if (ctrl == 0){                
    BT1.print("Recibiendo Datos...");
    lcd.setCursor(0,0);
    lcd.print ("Recibiendo Datos");
    lcd.setCursor(0,1);
    lcd.print ("Espere...");
    delay(5000);
    //ctrl=1;
    lcd.clear();
    }
    temp=dht.readTemperature();
    hum=dht.readHumidity();

  
  //Manejo manual de Calefaccion y Ventilacion
  switch(BT1.read()| Serial.read()){ 
    case 'C':
      digitalWrite(Calefaccion, !digitalRead(Calefaccion)); 
    break;
    case 'V':
    digitalWrite(Ventilador, !digitalRead(Ventilador));
    break;
  }
    //  VENTILACION
   if((temp>=30) && (digitalRead(Ventilador) == LOW)){                                                 
   digitalWrite(Ventilador,HIGH);                      
  }
    // CALEFACCION
    if((temp <= 15) && (digitalRead(Calefaccion) == LOW)){                                                       
    digitalWrite(Calefaccion, HIGH);
    }
 
     // ULTRASONICO (NIVEL DE AGUA)
   digitalWrite(TRIG, HIGH);     
   delay(1);       
   digitalWrite(TRIG, LOW);    
   DURACION = pulseIn(ECO, HIGH);   
   DISTANCIA = DURACION / 58.2;    
   
    //BOMBA
    if(lectura >= 600 && DISTANCIA<10 ) 
  {                                     
   digitalWrite(bomba, HIGH);
   delay(500);
  }
   digitalWrite(bomba, LOW);
   delay(500);
  
// ----------------------------------IMPRESION DE DATOS--------------------------------
  // HUMEDAD Y TMPERATURA BT
  BT1.print("Temperatura ");
  BT1.print(temp);
  BT1.println("ºC");
  BT1.print("Humedad ");
  BT1.print(hum);
  BT1.println("%");
  
  //Humedad y temperatura en PC
  Serial.print("Temperatura ");
  Serial.print(temp);
  Serial.println("ºC");
  Serial.print("Humedad ");
  Serial.print(hum);
  Serial.println("%");

  // HUMEDAD Y TMPERATURA LCD 
  lcd.setCursor(0,0);
  lcd.print ("TEMP:");
  lcd.setCursor(5,0);
  lcd.print (temp);
  lcd.setCursor(7,0);
  lcd.write(1); //Imprimo el caracter Grados
  lcd.print("C");
  lcd.setCursor (0,1);
  lcd.print ("HUMEDAD:");
  lcd.setCursor (8,1);
  lcd.print (hum);
  lcd.setCursor(10,1);
  lcd.print("%");
 
 //Ventilador
  if (digitalRead(Ventilador) == HIGH){
    BT1.println("VENTILACION ON");
    Serial.println("VENTILACION ON");
 }
   //CALEFACCION
  if (digitalRead(Calefaccion)== HIGH){
    BT1.println("CALEFACCION ON");
    Serial.println("CALEFACCION ON");
  }
// Humedad del suelo
   if (lectura >= 1000)
    {
    BT1.println("EL SENSOR DE HUMEDAD ESTA DECONECTADO O FUERA DEL SUELO");
    Serial.println("EL SENSOR DE HUMEDAD ESTA DESCONECTADO O FUERA DEL SUELO");
    }
      else if (lectura <1000 && lectura >= 600){
      BT1.println("EL SUELO ESTA SECO");
      Serial.println("EL SUELO ESTA SECO");
    }
      else if (lectura < 600 && lectura >= 370){
    BT1.println("EL SUELO ESTA HUMEDO");
    Serial.println("EL SUELO ESTA HUMEDO");
    }
      else if (lectura < 370){
    BT1.println("EL SENSOR DE HUMEDAD ESTA PRACTICAMENTE EN AGUA");
    Serial.println("EL SENSOR DE HUMEDAD ESTA PRACTICAMENTE EN AGUA");
    }
  
   BT1.print("La Humedad del suelo es del: ");
   BT1.print(lecturaPorcentaje);
   BT1.println("%");
   Serial.println("La humedad del suelo es del:");
   Serial.println(lecturaPorcentaje);
   Serial.println("%");


//ULTRASONICO
   if (DISTANCIA >=10)
   {
   BT1.println ("NO HAY AGUA EN EL TANQUE");
   BT1.print(DISTANCIA);   
   BT1.println ("cm");
   Serial.println ("NO HAY AGUA EN EL TANQUE");
   Serial.print(DISTANCIA);    
   Serial.println ("cm");
      
  }
//BOMBA
 if(lectura >= 600 && DISTANCIA<10 ) 
  {
   BT1.println("La tierra está seca, comienza el riego");
   Serial.println("La tierra está seca, comienza el riego");
  }
if(ctrl=1){
delay(1000);
}
ctrl=1; 
}
