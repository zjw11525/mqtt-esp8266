//注意我这边用的是esp12e模块，淘宝16块左右，所以有16引脚，esp8266也可以烧制以下程序

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define lightPin 0
// Update these with values suitable for your network.

const char* ssid = "FAST_09F958";
const char* password = "-J3pXCwjh";
const char* mqttServer = "mq.tongxinmao.com";
const int mqttPort = 18831;
const char* mqttUserName = "esp8266";
const char* mqttPassword = "123456";
const char* lightTopic = "light";
const char* willTopic = "will";
const char* onlineTopic = "online"; 
const char* sensorTopic = "sensor"; 
const char* clientId = "13579";
char uartbuf[8];
int buflength = 0;

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println(topic);
  String command = "";
  for (int i = 0; i < length; i++) {
    command += (char)payload[i];
  }
  Serial.println(command);
  handlePayload(String(topic), command); 
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    //详细参数说明请查看文档
    if (client.connect(clientId,mqttUserName,mqttPassword,willTopic,1,0,clientId)) {
      Serial.println("connected");
      client.publish(onlineTopic, clientId);
      client.subscribe(lightTopic, 1);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(lightPin, OUTPUT);     // Initialize the lightPin pin as an output
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
}

void loop() {
  
  if (!client.connected()) {
    reconnect();
  }  
  if (Serial.available() >= 6) {
    buflength = Serial.available();
    Serial.println(buflength);
    Serial.readBytes(uartbuf,buflength);
    client.publish(sensorTopic, uartbuf);
  }
  
  client.loop();


}

//处理命令
String handlePayload(String topic, String payload) {
  if (String(lightTopic).equals(topic)) {
    //light command
    if (String("lightOn").equals(payload)) {
      digitalWrite(lightPin, LOW);
    } else if (String("lightOff").equals(payload)) {
      digitalWrite(lightPin, HIGH);
    } 
  }
}
