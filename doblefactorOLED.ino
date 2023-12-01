#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <MFRC522.h>
#include <Keypad.h>

//se declaran los pines para el funcionamiento de la tarjeta 
#define RST_PIN  48   //Pin  para el reset del RC522
#define SS_PIN  53   //Pin  para el SS (SDA) del RC522
//mega mosi 51, miso 50, sck 52, sda 53
MFRC522 mfrc522(SS_PIN, RST_PIN); ///Creamos el objeto para el RC522

// Aqui se declaran las medidas del display
#define SCREEN_WIDTH 128 // Ancho display, en pixeles
#define SCREEN_HEIGHT 32 //Alto display, en pixeles
// Aqui se declaran las conexiones I2C del display SSD1306(SDA, SCL)
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT);

int rele=35;//salida hacia rele
int ledV=44;//Led verde
int ledR=45;//Led rojo
int pass1=0;
int pass2=0;
int presiones=0;

int posicion=0;    // necesaria para la clave
const byte filas = 4; 
const byte columnas = 3;
byte pinesFilas[]  = {9,8,7,6};
byte pinesColumnas[] = {5,4,3};
char teclas[4][3] = {{'1','2','3'},
                     {'4','5','6'},
                     {'7','8','9'},
                     {'*','0','#'}};
Keypad teclado = Keypad( makeKeymap(teclas), pinesFilas, pinesColumnas, filas, columnas);  
char clave[5]={'6','6','0','9','9'};//clave de acceso 


void setup() {
  pinMode(rele,OUTPUT);
  pinMode(ledV,OUTPUT);
  pinMode(ledR,OUTPUT);
  digitalWrite(rele, HIGH);
  digitalWrite(ledV, HIGH);
  digitalWrite(ledR, HIGH);
  
  Serial.begin(9600); //Iniciamos La comunicacion serial
  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  oled.clearDisplay();
  oled.setTextColor(SSD1306_WHITE);
  SPI.begin();        //Iniciamos el Bus SPI
  mfrc522.PCD_Init(); // Iniciamos el MFRC522
  Serial.println("Control de acceso:");
  oled.setTextSize(1);
  oled.setCursor(0, 10);
  oled.print("Control de acceso 2F");
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
  if(pass1==0&&pass2==0){
    oled.clearDisplay();//Limpia la pantalla
    oled.setTextSize(2);//Tamaño letra
    oled.setCursor(40, 10);//posicio(x,y)
    oled.print("RFID");//texo
    oled.display();
    Serial.println("RFID");
    }

  
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
                    oled.clearDisplay();//Limpia la pantalla
                    oled.setTextSize(1);//Tamaño letra
                    oled.setCursor(10, 10);//posicio(x,y)
                    oled.print("INGRESE SU PASSWORD");//texo
                    oled.display();
                    Serial.println("Ingrese su password...");
                    digitalWrite(ledV, LOW);
                    pass1=7;
                    delay(1000);
             
                 // else if(compareArray(ActualUID,User2))
                 //   Serial.println("Acceso concedido...");
                  }else{
                    oled.clearDisplay();//Limpia la pantalla
                    oled.setTextSize(1);//Tamaño letra
                    oled.setCursor(0, 10);//posicio(x,y)
                    oled.print("...ACCESO DENEGADO...");//texo
                    oled.display();
                    Serial.println("Acceso denegado...");
                    digitalWrite(ledR, LOW);
                    digitalWrite(ledV, HIGH);
                    delay(2000);
                    digitalWrite(ledR, HIGH);
                    pass1=0;
                    pass2=0;
                  }
                  // Terminamos la lectura de la tarjeta tarjeta  actual
                  mfrc522.PICC_HaltA();
          
            }
      
  }
  //Verifica si alguna tecla fue presionada
char tecla=teclado.getKey(); // leemos pulsacio

//Verifica si alguna tecla fue presionada
 if (tecla != NO_KEY && pass1>0)//si se presionó una tecla
    {
      presiones=presiones+1;
      //--- Condicionales para comprobar la clave introducida -----------
      // comparamos entrada con cada uno de los digitos, uno a uno
      if (tecla == clave[posicion]){
          posicion ++; // aumentamos posicion si es correcto el digito
        if(posicion==5){ //Cuando coinciden todos los digitos
          pass2=3;
          posicion=0;    //Reinicia la posicion
          if(pass1+pass2==10){
             oled.clearDisplay();//Limpia la pantalla
             oled.setTextSize(2);//Tamaño letra
             oled.setCursor(5, 10);//posicio(x,y)
             oled.print("BIENVENIDO.");//texo
             oled.display();
             Serial.println("Bienvenido");
             digitalWrite(rele,LOW);  //Abre switch
             delay(2000);   //Espera un tiempo
             digitalWrite(rele,HIGH);  //Cierra el switch
             digitalWrite(ledV,HIGH);
             pass1=0;
             pass2=0;
             presiones=0;
           }
         }
         
       }else{
        //En este bloque de codigo se agrega algo cuando los digitos no van coincidiendo
        }
      if(presiones==5){
        oled.clearDisplay();//Limpia la pantalla
        oled.setTextSize(1);//Tamaño letra
        oled.setCursor(20, 10);//posicio(x,y)
        oled.print("CLAVE INVALIDA");//texo
        oled.display();
        Serial.println("Clave invalida");
        digitalWrite(ledR,LOW);
        Serial.println("RFID");
        digitalWrite(ledV,HIGH);
        presiones=0;
        pass1=0;
        delay(1000);
        digitalWrite(ledR,HIGH);
      }
  }
         //FIN CODIGO APERTURA CON TECLADO

  

         
  
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
