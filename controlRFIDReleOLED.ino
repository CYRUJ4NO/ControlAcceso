#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MFRC522.h>

#define RST_PIN  48   //Pin 9 para el reset del RC522
#define SS_PIN  53   //Pin 10 para el SS (SDA) del RC522
MFRC522 mfrc522(SS_PIN, RST_PIN); ///Creamos el objeto para el RC522
//miso 50    mosi 51

// Aqui se declaran las medidas del display
#define SCREEN_WIDTH 128 // Ancho display, en pixeles
#define SCREEN_HEIGHT 32 //Alto display, en pixeles
// Aqui se declaran las conexiones I2C del display SSD1306(SDA, SCL)
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT);

int rele=35;//salida hacia rele
int ledV=44;//Led verde
int ledR=45;//Led rojo

void setup() {
  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  oled.clearDisplay();
  oled.setTextColor(SSD1306_WHITE);
  
  pinMode(rele,OUTPUT);
  pinMode(ledV,OUTPUT);
  pinMode(ledR,OUTPUT);
  digitalWrite(rele, HIGH);
  digitalWrite(ledV, HIGH);
  digitalWrite(ledR, HIGH);
  Serial.begin(9600); //Iniciamos La comunicacion serial
  SPI.begin();        //Iniciamos el Bus SPI
  mfrc522.PCD_Init(); // Iniciamos el MFRC522
  Serial.println("Control de acceso:");
  oled.setTextSize(1);
  oled.setCursor(0, 10);
  oled.print("Control de acceso");
  oled.setCursor(100, 20);
  oled.print("v1.1");
  oled.display();
  delay(2000);
  oled.clearDisplay();
}

byte ActualUID[4]; //almacenará el código del Tag leído
byte User1[4]= {0xC3, 0xD2, 0x48, 0xDD} ; //código del usuario 1
//byte User2[4]= {0xC1, 0x2F, 0xD6, 0x0E} ; //código del usuario 2


void loop() {
  oled.clearDisplay();//Limpia la pantalla
  oled.setTextSize(2);//Tamaño letra
  oled.setCursor(40, 10);//posicio(x,y)
  oled.print("RFID");//texo
  oled.display();
  // Revisamos si hay nuevas tarjetas  presentes
  if ( mfrc522.PICC_IsNewCardPresent()) 
        {  
      //Seleccionamos una tarjeta
            if ( mfrc522.PICC_ReadCardSerial()) 
            {
                  // Enviamos serialemente su UID
                  Serial.print(F("Card UID:"));
                  for (byte i = 0; i < mfrc522.uid.size; i++) {
                          Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
                          Serial.print(mfrc522.uid.uidByte[i], HEX);   
                          ActualUID[i]=mfrc522.uid.uidByte[i];          
                  } 
                  Serial.print("     ");  
                                 
                  //comparamos los UID para determinar si es uno de nuestros usuarios  
                  if(compareArray(ActualUID,User1)){
                    Serial.println("Acceso concedido...");
                    oled.clearDisplay();//Limpia la pantalla
                    oled.setTextSize(1);//Tamaño letra
                    oled.setCursor(0, 10);//posicio(x,y)
                    oled.print(".. ACCESO CONCEDIDO ..");//texo
                    oled.display();
                    digitalWrite(ledV, LOW);
                    delay(1000);
                    digitalWrite(rele, LOW);
                    delay(2000);
                    digitalWrite(rele, HIGH);
                    digitalWrite(ledV, HIGH);
                    oled.clearDisplay();//Limpia la pantalla
                 // else if(compareArray(ActualUID,User2))
                 //   Serial.println("Acceso concedido...");
                  }else{
                    Serial.println("Acceso denegado...");
                    oled.clearDisplay();//Limpia la pantalla
                    oled.setTextSize(1);//Tamaño letra
                    oled.setCursor(0, 10);//posicio(x,y)
                    oled.print("...ACCESO DENEGADO...");//texo
                    oled.display();//Envia info al display
                    digitalWrite(ledR, LOW);
                    delay(2000);
                    digitalWrite(ledR, HIGH);
                    oled.clearDisplay();//Limpia la pantalla
                  }
                  // Terminamos la lectura de la tarjeta tarjeta  actual
                  mfrc522.PICC_HaltA();
          
            }
      
  }
  
}

//Función para comparar dos vectores
 boolean compareArray(byte array1[],byte array2[])
{
  if(array1[0] != array2[0])return(false);
  if(array1[1] != array2[1])return(false);
  if(array1[2] != array2[2])return(false);
  if(array1[3] != array2[3])return(false);
  return(true);
}
