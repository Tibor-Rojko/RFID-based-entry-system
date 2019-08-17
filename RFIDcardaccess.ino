/********************************
* RFID alapú beléptető rendszer *
* Verzió:     1.0               *
* Készítette: Rojkó Tibor       *
* Dátum:      2017.10.19        *
********************************/

#include <SPI.h>                              //Az SPI busz működéséhez szükséges könyvtár
#include <RFID.h>                             //Az RFID modul működéséhez szükséges könyvtár

//kimenetek definiálása
#define SS_PIN    10                          //Slave kiválasztó pin
#define RST_PIN   9                           //RESET pin
#define rgbRED       4                           //RGB vörös pin
#define rgbBLUE      5                           //RGB kék pin
#define rgbGREEN     6                           //RGB zöld pin
#define doorLock 13                           //Ajtó zárját nyitó relé vezérlő pinje

RFID rfid(SS_PIN,RST_PIN);                    //RFID modul beállítás

int serNum[5];                                //A kiolvasott kártyaszám tárolására szolgáló tömb
int cards[][5] = {{231, 204, 214, 43, 214}};  //Érvényes kártyaszámokat tartalmazó tömb

bool access = false;                          //Engedély

//Tag ID-t kiolvasó függvény
int tagReader(){
    if(rfid.isCard()){                                            //Ha van a közelben kártya...
        if(rfid.readCardSerial()){                                //Akkor kiolvassuk az ID-ját
            for(int i = 0; i<5; i++){
              serNum[i] = rfid.serNum[0];                         //A kártyaszámokat egy tömbbe töltjük
            }
        }
    }
    rfid.halt();                                                  //Nyugalmi állpotba helyezzük az RFID olvasót         
    return serNum;
}

//Tag ID-t ellenőrző függvény
bool checkCard(){
    for(int x = 0; x < sizeof(cards); x++){
      for(int i = 0; i < sizeof(rfid.serNum); i++ ){
        if(rfid.serNum[i] != cards[x][i]) {
          access = false;
          break;
        }else{
          access = true;
        }
      }
      if(access) break;
    }
    return access;  
}

//RGB LED-et működtető függvény
void setRGB(int stat){
  //1 = zöld
  //2 = vörös
  switch(stat){
    case 1:
      digitalWrite(rgbGREEN, LOW);
      digitalWrite(rgbBLUE, HIGH);
      digitalWrite(rgbRED, HIGH);
    break;
    case 2:
      digitalWrite(rgbRED, LOW);
      digitalWrite(rgbGREEN, HIGH);
      digitalWrite(rgbBLUE, HIGH);
    break;
    default:
      digitalWrite(rgbBLUE, LOW);
      digitalWrite(rgbGREEN, HIGH);
      digitalWrite(rgbRED, HIGH);
  }
}

//Ajtónyitást vezérelő függvény
void door(int delayTime){
  digitalWrite(doorLock, HIGH);                     //Kinyitom az ajtózárat
  delay(delayTime);                                 //Várok delayTime ms-ig
  digitalWrite(doorLock, LOW);                      //Bezárom az ajtózárat
}

void tagWriter(){
  for(int i=0; i<5; i++){
    Serial.println("Az uj kartya szama:");
    Serial.print(serNum[i]);
    Serial.print(" ");
    Serial.println();  
  }
}

//Kezdeti beállítások
void setup(){                                 //Egyszer fut le...

    Serial.begin(9600);                       //Soros monitor indítása
    
    //buszok, és modulok aktivizálása
    SPI.begin();                              //SPI busz inicializálás
    rfid.init();                              //RFID olvasó inicializálás
    
    //kimenetek beállítása
    pinMode(doorLock, OUTPUT);                //13-as pin kimenetként való definiálása
    
}

void loop(){                                  //Végtelenszer fut le...
    setRGB(0);                                //RGB -> kékre
    tagReader();                              //Olvassuk ki a tag ID-t
    //tagWriter();                            //Ha új kártyát teszek hozzá, ezzel tudom kiolvasni az ID-jét
    access = checkCard();                     //Ellenőrizzük le a tag ID-t
    
    if(access){                               //Ha érvényes volt a kártya, akkor az access = TRUE
          setRGB(1);                          //RGB -> zöldre
          door(5000);                         //Ajtónyitás
    }else{                                    //Ha nem volt érvényes a kártya
          setRGB(2);                          //RGB -> vörösre;
    }
              
}

 
