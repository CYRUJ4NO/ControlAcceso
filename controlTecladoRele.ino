#include <Keypad.h>


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
  pinMode(35,OUTPUT);
  digitalWrite(35, HIGH);
  Serial.begin(9600);
  Serial.println("Control de acceso con teclado");
  Serial.println();
}
void loop() {
  
//Verifica si alguna tecla fue presionada
char tecla=teclado.getKey(); // leemos pulsacio

//Verifica si alguna tecla fue presionada
 if (tecla != NO_KEY)//si se presionó una tecla
    {
      //--- Condicionales para comprobar la clave introducida -----------
      // comparamos entrada con cada uno de los digitos, uno a uno
      if (tecla == clave[posicion]){
          posicion ++; // aumentamos posicion si es correcto el digito
        if(posicion==5){ //Cuando coinciden todos los digitos
          digitalWrite(35,LOW);  //Abre switch
          delay(2000);   //Espera un tiempo
          posicion=0;    //Reinicia la posicion
          digitalWrite(35,HIGH);  //Cierra el switch 
         }
       }else{
        //En este bloque de codigo se agrega algo cuando los digitos no van coincidiendo
        }
     }
         //FIN CODIGO APERTURA CON TECLADO
}
