#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// ---- WiFi Credentials ----
const char* ssid = "xxxxx";
const char* password = "xxxx";

// ---- MQTT Broker ----
const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;

// ---- Topics ----
const char* topic_light = "NBLVPL";  // Topic to control Light
const char* topic_fan = "NBLVPF";    // Topic to control Fan

// ---- Relay Pins ----
#define RELAY_LIGHT D0
#define RELAY_FAN   D2

WiFiClient espClient;
PubSubClient client(espClient);

// ---- Connect to WiFi ----
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// ---- Callback function when message received ----
void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(message);

  // Light control
  if (String(topic) == topic_light) {
    if (message.equalsIgnoreCase("ON")) {
      digitalWrite(RELAY_LIGHT, LOW);  // Relay ON (Active LOW)
      Serial.println("Light ON");
    } else if (message.equalsIgnoreCase("OFF")) {
      digitalWrite(RELAY_LIGHT, HIGH); // Relay OFF
      Serial.println("Light OFF");
    }
  }

  // Fan control
  if (String(topic) == topic_fan) {
    if (message.equalsIgnoreCase("ON")) {
      digitalWrite(RELAY_FAN, LOW);
      Serial.println("Fan ON");
    } else if (message.equalsIgnoreCase("OFF")) {
      digitalWrite(RELAY_FAN, HIGH);
      Serial.println("Fan OFF");
    }
  }
}

// ---- Reconnect to MQTT if disconnected ----
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266_Client")) {
      Serial.println("connected");
      client.subscribe(topic_light);
      client.subscribe(topic_fan);
      Serial.println("Subscribed to topics!");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(RELAY_LIGHT, OUTPUT);
  pinMode(RELAY_FAN, OUTPUT);

  digitalWrite(RELAY_LIGHT, HIGH); // Relays OFF initially
  digitalWrite(RELAY_FAN, HIGH);

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
