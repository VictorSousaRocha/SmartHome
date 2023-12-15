#include<WiFi.h>
#include<Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"


#define WIFI_SSID ""
#define WIFI_PASSWORD ""
#define API_KEY ""
#define DATABASE_URL ""

#define LED1_PIN 23

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis =0;
bool signupOK = false;
int ldrData = 0;
float voltage = 0.0;

void setup() {
  pinMode(LED1_PIN, OUTPUT);
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("conetando ao wifi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.println("ainda conectando"); delay(300);
  }
  Serial.println();
  Serial.print("Conectado ao ip: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  if(Firebase.signUp(&config, &auth, "", "")){
    Serial.println("signUp OK");
    signupOK =true;
  }
  else { 
    Serial.printf("%s/n", config.signer.signupError.message.c_str());
  }
  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 5000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
    ldrData = analogRead (LED1_PIN);
    voltage = (float)analogReadMilliVolts(LED1_PIN)/1000;
    // area de teste

     if(fbdo.stringData()=="a"){
        digitalWrite (LED1_PIN, LOW);
      }

      if(fbdo.stringData()=="A"){
        digitalWrite (LED1_PIN, LOW);
      }



    // fim da area de teste
    if(Firebase.RTDB.setInt(&fbdo, "Escrever/led", ldrData)){
      Serial.println(); Serial.print(ldrData);
      Serial.println("salvo em ");
      Serial.println(fbdo.dataPath());
      Serial.println( fbdo.dataType());
    }
    else {
      Serial.println("Falhou ");
      Serial.println(fbdo.errorReason());
    }
    if (Firebase.RTDB.getString(&fbdo, "Sensor/mensagem")) {

      String mensagem = fbdo.stringData();

      if (mensagem.indexOf("A") != -1) {
        Serial.println("entrou ");
        digitalWrite(LED1_PIN, HIGH);
      } else if (mensagem.indexOf("a") != -1) {
        Serial.println("entrou 2222222");
        digitalWrite(LED1_PIN, LOW);
      }
    }
    

    //a partir daqui n sei oq eu to fazendo
     if(Firebase.RTDB.setFloat(&fbdo, "Sensor/voltage", voltage)){
      Serial.println(); Serial.print(voltage);
      Serial.print("salvo em " );
      Serial.print(fbdo.dataPath());
      Serial.println(" fbdo.dataType()");
    }
    else {
      Serial.println("Falhou ");
    }

  }
}