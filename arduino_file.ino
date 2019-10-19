/*
 * LIGHT OFF  + PIR OFF  -> bagno libero
 * LIGHT OFF  + PIR ON   -> bagno libero
 * LIGHT ON   + PIR OFF  -> luce accesa
 * LIGHT ON   + PIR ON   -> bagno occupato, persona dentro
 * LIGHT ON   + PIR OFF  con iniziale movimento -> persona dentro ma immobile -> DANGER 
 * 
 */

#define pinBright A0              //SENSORE: pin A0, analogico, per sensore di luminosita' 
int pir = 12;                     //SENSORE: PIR connesso a pin 12
int pinLed = 11;                  //ATTUATORE: led connesso a pin 11

int lum = 0;                      //quantita' di luce rilevata
int calibrationTime = 15;         //tempo di calibrazione del sensore PIR
boolean firstTime = true;         //prima volta che accendo la luce del bagno
boolean firstTimeOccupied = true;         //prima volta che movimento in bagno
boolean firstTimeFree = true;         //prima volta bagno libero
int tempo = 0;                    //tempo di inizio movimento
int tempo2 = 0;                   //tempo di fine movimento
boolean occupied = false;         //boolean per identificare presenza in bagno
bool danger_wait = false;         //boolean per controllo durante fase di DANGER
//int cont = 0;

void setup(){
  Serial.begin(9600);
  pinMode(pinBright,INPUT);
  pinMode(pir, INPUT);
  pinMode(pinLed, OUTPUT);
  
  digitalWrite(pir, LOW);
  digitalWrite(pinLed, LOW);
  
  //Fase di calibrazione
  Serial.println("Calibrazione del sistema (15 secondi)");
  for(int i = 0; i < calibrationTime; i++){
    //Serial.println(".");   
    delay(1000);
  }
  Serial.print("SISTEMA PRONTO!\n");
  delay(1000);
}


//funzione di supporto per stampare la luminosita'
void brightStamp(){
  lum = analogRead(pinBright);
  Serial.print("Rilevata luminosita' -> ");
  Serial.print(lum);
  Serial.print("\n");
}


void loop(){
  brightStamp();    
  if (lum < 200){                              // LIGHT OFF  + PIR OFF  e LIGHT OFF + PIR ON
    //digitalWrite(pinLed, LOW);
    occupied=false;
    if(firstTimeFree){
      Serial.print("Bagno Libero.");
      firstTime = true;
      firstTimeFree = false;
      firstTimeOccupied = true;
    }
  }
  else{                                       // LIGHT ON  + PIR OFF  
    if(firstTime)
    {                           
      Serial.print("Luce Accesa!");
      Serial.print("\n");
      firstTime = false;
      firstTimeFree = true;
      firstTimeOccupied = true;
    }
    
    if (digitalRead(pir) == HIGH)             // LIGHT ON + PIR ON
    {             
      delay(500);
      occupied = true;
      tempo = millis()/1000;
      if(firstTimeOccupied){
        Serial.print("WARNING: Bagno Occupato.");
        Serial.print("\n");
        firstTimeOccupied = false;
        }
      firstTime = false;
    }
    else                                       // LIGHT ON + PIR OFF
    {
       if( occupied ){                         //valuto quanto tempo passa
        tempo2 = millis()/1000;
        int diff = tempo2 - tempo;
        if ( diff > 9 ){
          Serial.print("DANGER: Inattivita' da 10 secondi.");
          digitalWrite(pinLed, HIGH);          // attivo ATTUATORE, luce rossa di allarme
          danger_wait = true;
          delay(200);
          
          while( danger_wait ){                //finche' ho inattivita' dopo il "PIR ON"
            delay(10000);                      //Attendo 10 sec e rimando il messaggio, fintanto che LIGHT ON
            brightStamp();
            if( lum < 200 ){                   //se LIGHT OFF: uso il bright pin come controllo al posto del sensore di movimento cosi che se l anziano sta male e si muove un po e poi continua a stare immobile gli altri continuano a riceve iol messaggio anche se ci sono altri che mettono a posto
              danger_wait = false;
              occupied = false;
              firstTime = true;
              firstTimeOccupied = true;
              firstTimeFree = true;
              digitalWrite(pinLed, LOW);      // spegno ATTUATORE, luce rossa di allarme
              Serial.println("PERICOLO TERMINATO.\n");
            }
            else{                             //se ancora LIGHT ON                 
              Serial.print("PERICOLO CONTINUO.");
              Serial.print("\n");
            }
          }
        }
       }
     }
  }
  delay(1000);
}
      
