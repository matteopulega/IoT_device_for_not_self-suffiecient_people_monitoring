import http.requests.*;
import processing.serial.*;

Serial myPort;  // Creo oggetto della classe Serial


void send_msg(String message){
  //String url = "http://notify.moosa.it:88/a08ab1c6-a4ec-4183-b13d-a68ed1805e1d/";
  String url = "https://api.telegram.org/bot761484947:AAHUxodkz2iHkVKZueseuWx35E5jEQ23ILU/sendMessage?chat_id=-361136352&text=";
  url = url + message;
  println("\n");
  System.out.println(url);    //stampo a video il link + messaggio
  GetRequest get = new GetRequest(url);
  get.send();
}

void setup()
{
  //
  String portName = Serial.list()[0]; //se avessi piu porte dovrei cambiare a 1 o 2
  myPort = new Serial(this, portName, 9600);

  String message = "AVVIO+DEL+SISTEMA:+Calibrazione...";
  send_msg(message);

  println("Fine setup\n");
}

int sec = 10;
boolean free_bool = true;                //variabile per inviare a Telegram 1 sola volta il msg se accendo e spengo luce ogni volta
    

void draw()
{
  String val="";
  if ( myPort.available() > 0){                  // Se ricevo dei dati, leggi e salvali in val
    val = myPort.readString();
    print(val);                                 //Stampo tutto cio' che mi arriva da Serial.print di Arduino
    
    //-----controllo dei pacchetti in arrivo da ARDUINO-----
    String msg; 
    String msg1; 
    
    if( val.contains("PRONTO") ){
      msg1 = "SISTEMA+PRONTO.";
      send_msg(msg1);
    }
    
    if( val.contains("Libero") & free_bool){
      msg1 = "Bagno+Libero.";
      send_msg(msg1);
      free_bool = false;
    }
    
    if( val.contains("WARNING") ){  
      msg1 = "WARNING:+Bagno+Occupato.";
      send_msg(msg1);
      free_bool = true;
    }
    
    msg = "CONTINUO";
    if( val.contains(msg) ){
      msg1 = "****DANGER:+Inattivita'+da+";
      sec += 10;
      msg1 = msg1 + sec + "+secondi.";
      send_msg(msg1);
    }
    
    //msg = "TERMINATO";
    //if( val.contains(msg) ){
    if( val.contains("TERMINATO") ){
      msg1 = "PERICOLO+TERMINATO+_______________";
      send_msg(msg1);
      free_bool = true;
    }    
  }
 }
