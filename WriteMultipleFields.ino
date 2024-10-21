#include <WiFiNINA.h>
#include <PubSubClient.h>

// WiFi credentials
const char* ssid = "Jio AirFiber";        
const char* password = "9416730670";      

// MQTT broker settings
const char* mqtt_server = "broker.mqtt.cool";  
const int mqtt_port = 1883;                    
const char* mqtt_topic = "SIT210/wave";        

// Ultrasonic Sensor Pins
const int trigPin = 2;  
const int echoPin = 3;  

// LED Pin
const int ledPin = 4;   

// WiFi and MQTT client objects
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(9600);           
  pinMode(trigPin, OUTPUT);      // Set trigger pin as output
  pinMode(echoPin, INPUT);       // Set echo pin as input
  pinMode(ledPin, OUTPUT);       // Set LED pin as output

  connectToWiFi();               // Connect to WiFi
  client.setServer(mqtt_server, mqtt_port);  
  client.setCallback(callback);  
  reconnectMQTT();               // Connect to MQTT broker
}

void loop() {
  // Check if the client is connected to the MQTT broker
  if (!client.connected()) {
    reconnectMQTT();  // Reconnect if disconnected
  }
  client.loop();  

  long duration, distance;
  
  // Send a pulse to the ultrasonic sensor to trigger a measurement
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Measure the duration of the echo pulse
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate distance in cm
  distance = (duration / 2) / 29.1;

  // If distance is less than 10 cm, assume a wave is detected
  if (distance < 10) {
    client.publish(mqtt_topic, "Yashika");  // Publish a message to the MQTT topic
    delay(1000);  
  }
}

// Callback function that is called when a message is received from the subscribed topic
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);  
  }
  Serial.println();

  // Blink the LED 3 times to indicate message receipt
  for (int i = 0; i < 3; i++) {
    digitalWrite(ledPin, HIGH);  
    delay(500);                  
    digitalWrite(ledPin, LOW);   
    delay(500);                  
  }
}

// Function to connect the device to WiFi
void connectToWiFi() {
  WiFi.begin(ssid, password);  // Start the WiFi connection with the given credentials
  while (WiFi.status() != WL_CONNECTED) {  
    delay(1000);  
    Serial.println("Connecting to WiFi...");  
  }
  Serial.println("Connected to WiFi"); 
}

// Function to reconnect to the MQTT broker if the connection is lost
void reconnectMQTT() {
  while (!client.connected()) {  
    Serial.println("Connecting to MQTT...");
    if (client.connect("ArduinoClient")) {  
      Serial.println("Connected to MQTT");  
      client.subscribe(mqtt_topic);  
    } else {
      delay(5000); 
      
    }
  }
}
