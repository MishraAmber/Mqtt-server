#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>




#define echoPin 13
#define trigPin 12

//#define pinA 25

#define MQ3Pin 26

DHT dht(4,11);


//WiFi credentials
const char*ssid = "Real Me Narzo 30 Pro";
const char*pass = "12345678";

// IP address of the broker
const char*mqtt_server = "";

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;

void setupwifi(){
  delay(50);
  Serial.println("Connecting to");
  Serial.println(ssid);

  WiFi.begin(ssid,pass);

  int a = 0;
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(1000);
    a = a+1;
    if(a>10){
      ESP.restart();  // esp restart after 10 sec
      Serial.print("Restarting esp");
    }
  }
  Serial.print("Wifi conneted");
}

void connect_mqttServer(){
  while(!client.connected()){
    if(WiFi.status() != WL_CONNECTED){
      setupwifi();
    }
    Serial.print("Trying to connect to mqtt server");
    if (client.connect("ESP32")){
      Serial.println("COnnected");
    }
    else{
      Serial.print("Failed to connect, rc = ");
      Serial.print(client.state());
      Serial.print("Retrying in 2 seconds");
      delay(2000);
    }
  }
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Check if a message is received on the topic "rpi/broadcast"
  if (String(topic) == "rpi/broadcast") {
      if(messageTemp == "10"){
        Serial.println("Action: blink LED");
      }
  }
  //Similarly add more if statements to check for other subscribed topics 
}


void setup() {
  Serial.begin(115200);
//  GPS.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  dht.begin();
  setupwifi();
  client.setServer(mqtt_server,1883); //1883 is default port for MQTT server
  client.setCallback(callback);
}

void loop() {
  if(!client.connected()){
    connect_mqttServer();
  }
  client.loop();
  long now = millis();
  if (now - lastMsg > 4000){
    lastMsg = now;
    temperature();
    distance();
    alcohol();
   // vooc();
  }
}

void temperature(){
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  char str_t[10];
  char str_h[10];
  
 
 
    dtostrf(t,4,2,str_t);
    dtostrf(h,4,2,str_h);
    
    Serial.println(str_t);
    Serial.println(str_h);
    client.publish("temperature",str_t); // Publishing to the topic;
    client.publish("humidity", str_h);
  
}

void distance(){
  digitalWrite(trigPin,LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin,LOW);

  float time = pulseIn(echoPin,HIGH);
  float distance = time *0.034/2;
  char str_d[10];

    dtostrf( distance,4,2,str_d);
 
    Serial.println("Distance is : ");
    Serial.print(str_d);
    client.publish("distance",str_d);
}

void alcohol(){
  float a = analogRead(MQ3Pin);
  char str_a[10];
  
  dtostrf(a,4,2,str_a);
  Serial.println("Alcohol Value is : ");
  Serial.print(str_a);
  client.publish("alcohol",str_a);
}
