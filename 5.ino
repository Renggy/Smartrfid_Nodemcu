#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <WiFiClient.h>
#include <MFRC522.h>
#include <SH1106.h>
#include <Wire.h>
#include <SPI.h>

#define SS_PIN  D4
#define RST_PIN D3

MFRC522 Mfrc522(SS_PIN, RST_PIN);
SH1106 display(0x3C, D2, D1);

const char* Status     = "5";
const char* Ssid       = "BARISTAND"     ;
const char* Password   = "aspirasi165"  ;
const char* GetsName   = "http://192.168.43.37/smart_rfid/gets_data.php?RFID=";
const char* PostName   = "http://192.168.43.37/smart_rfid/post_data.php"      ;

void Post_Data(String Paimin) {
  HTTPClient Http;
  Http.begin(PostName);
  Http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String HttpRequestData = "RFID=" + Paimin+
                           "&Status=" + Status;
  int HttpResponseCode = Http.POST(HttpRequestData);   
  if(HttpResponseCode > 0){
    String Data_Json = Http.getString(); 
    Parsing_Json_Post(Data_Json);
    digitalWrite(D8,HIGH);
    delay(100);
    digitalWrite(D8,LOW);
  }
  else{
    Serial.println(HttpResponseCode);
  }
  delay(100);
  Http.end(); 
  delay(5000);
  display.clear  ();
  display.display();      
}
void Gets_Data(String Paimin) {
  HTTPClient Http; 
  String HttpRequestData = GetsName + String(Paimin);
  Http.begin(HttpRequestData);
  Serial.println(HttpRequestData);
  int HttpResponseCode = Http.GET();
  if(HttpResponseCode > 0){ 
    String Data_Json = Http.getString(); 
    Parsing_Json_Gets(Data_Json);   
    digitalWrite(D8,HIGH);
    delay(100);
    digitalWrite(D8,LOW);
  }
  else{

  }
  delay(100);
  Http.end(); 
  delay(5000); 
  display.clear  ();
  display.display();
}
void Parsing_Json_Post(String URL) {
  char JSON[200];
  String Kondisi;
  StaticJsonDocument<200> DOC;  
  URL.toCharArray(JSON,200);
  DeserializationError error = deserializeJson(DOC,JSON);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }
  const int Status          = DOC["Status"];
  const int Last            = DOC["Last"];
  Serial.print("Status");Serial.print(Status);
  Serial.print("Last");  Serial.print(Last);
  if (Status == 200){
    display.setFont(ArialMT_Plain_10);
    display.drawString(47,10 ,String("Status"))         ;
    display.drawString(37,25 ,String("Selesai Uji"))    ;
    display.display();
  }
  else if(Status == 100){
    if(Last == 1){
      display.setFont(ArialMT_Plain_10);
      display.drawString(35,5 ,String("Gagal Scan"))            ;
      display.drawString(27,20 ,String("Status Terakhir"))      ;
      display.drawString(10,35 ,String("Sampel Diterima Loket"));
      display.display();
    }
    else if(Last == 2){
      display.setFont(ArialMT_Plain_10);
      display.drawString(35,5 ,String("Gagal Scan"))            ;
      display.drawString(27,20 ,String("Status Terakhir"))      ;
      display.drawString(10,35 ,String("Sampel Belum Dibayar")) ;
      display.display();
    }
    else if(Last == 3){
      display.setFont(ArialMT_Plain_10);
      display.drawString(35,5 ,String("Gagal Scan"))            ;
      display.drawString(27,20 ,String("Status Terakhir"))      ;
      display.drawString(0,35 ,String("Sampel Sudah Masuk Lab"));
      display.display();
    }
    else if(Last == 4){
      display.setFont(ArialMT_Plain_10);
      display.drawString(35,5 ,String("Gagal Scan"))            ;
      display.drawString(27,20 ,String("Status Terakhir"))      ;
      display.drawString(10,35 ,String("Sampel Dalam Proses"))  ;
      display.display();
    }
    else if(Last == 5){
      display.setFont(ArialMT_Plain_10);
      display.drawString(35,5 ,String("Gagal Scan"))            ;
      display.drawString(27,20 ,String("Status Terakhir"))      ;
      display.drawString(20,35 ,String("Sampel Selesai Uji"))   ;
      display.display();
    }
  }
  else{
    display.setFont(ArialMT_Plain_10);
    display.drawString(50,10 ,String("Status"))         ;
    display.drawString(30,25 ,String("Sudah Di Scan"))  ;
    display.drawString(35,40 ,String("Sebelumnya"))  ;
    display.display();
  }
}
void Parsing_Json_Gets(String URL) {
  char JSON[200];
  StaticJsonDocument<200> DOC;  
  URL.toCharArray(JSON,200);
  DeserializationError error = deserializeJson(DOC,JSON);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }
  const char* Kode_P          = DOC["kode_p"];
  const char* Lab             = DOC["lab"];
  const char* Nama_Perusahaan = DOC["nama_perusahaan"];
  const char* Komoditi        = DOC["komoditi"];
  if(Kode_P          == NULL){Kode_P          = "No Data";}else{}
  if(Lab             == NULL){Lab             = "No Data";}else{}
  if(Nama_Perusahaan == NULL){Nama_Perusahaan = "No Data";}else{}
  if(Komoditi        == NULL){Komoditi        = "No Data";}else{}
  display.setFont(ArialMT_Plain_10);
  display.drawString(0 , 10, String("KodeP")) ; display.drawString(37 , 10 , String(":"));display.drawString(42 , 10 , String(Kode_P         ));
  display.drawString(0 , 20, String("Lab"))   ; display.drawString(37 , 20 , String(":"));display.drawString(42 , 20 , String(Lab            ));
  display.drawString(0 , 30, String("Kantor")); display.drawString(37 , 30 , String(":"));display.drawString(42 , 30 , String(Nama_Perusahaan));
  display.drawString(0 , 40, String("Barang")); display.drawString(37 , 40 , String(":"));display.drawString(42 , 40 , String(Komoditi       ));
  display.display();
}
void Scanning_RFID_Gets_Data(){
  if(Mfrc522.PICC_IsNewCardPresent()){
    if(Mfrc522.PICC_ReadCardSerial()){
      String ID_RFID = "";
      int Data[5];             
      for (byte i = 0; i < Mfrc522.uid.size; i++){
        Data[i] = Mfrc522.uid.uidByte[i];
      }    
      for(int a = 0 ; a < Mfrc522.uid.size; a++){
        ID_RFID += String(Data[a],HEX);
      }
      ID_RFID.toUpperCase();
      Gets_Data(ID_RFID);     
      Mfrc522.PICC_HaltA();       
    }      
  } 
}
void Scanning_RFID_Post_Data(){
  if(Mfrc522.PICC_IsNewCardPresent()){
    if(Mfrc522.PICC_ReadCardSerial()){
      String ID_RFID = "";
      int Data[5];             
      for (byte i = 0; i < Mfrc522.uid.size; i++){
        Data[i] = Mfrc522.uid.uidByte[i];
      }    
      for(int a = 0 ; a < Mfrc522.uid.size; a++){
        ID_RFID += String(Data[a],HEX);
      }
      ID_RFID.toUpperCase();
      Post_Data(ID_RFID);     
      Mfrc522.PICC_HaltA();       
    }      
  }
}
void setup(){
  SPI.begin();
  display.init();
  Mfrc522.PCD_Init();
  Serial.begin(9600);  
  WiFi.disconnect();
  display.clear  ();
  delay(2000);
  display.setContrast(255);
  display.setFont(ArialMT_Plain_10);
  pinMode(D8,OUTPUT);
}
void loop (){
  if(WiFi.status()== WL_CONNECTED){
    if(digitalRead(D0) == LOW){
      Scanning_RFID_Gets_Data();
      display.setFont(ArialMT_Plain_16);
      display.drawString(25 , 25, String("Mode Baca "));
      display.display();
      display.clear  ();
    }
    else{
      Scanning_RFID_Post_Data();
      display.setFont(ArialMT_Plain_16);
      display.drawString(25 , 25, String("Mode Kirim"));
      display.display();
      display.clear  ();
    }
  }
  else{
    display.setFont(ArialMT_Plain_10);
    display.drawString(13 , 15, String("Searching Connection"));
    display.drawString(40 , 40, String("Baristand"));
    display.display();
    WiFi.begin(Ssid, Password);
    while(WiFi.status() != WL_CONNECTED){ 
      delay(1000);
      Serial.print(".");
    }
    display.clear();
    display.display();
  }
}
