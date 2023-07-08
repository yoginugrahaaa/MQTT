#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

// Konfigurasi jaringan Wi-Fi
const char* ssid = "yoogi";
const char* password = "baladaregar";

// Konfigurasi broker MQTT
const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;
const char* mqttTopic = "lenovo/esp32/test";

// Konfigurasi sensor DHT
#define DHTPIN 15        // Pin GPIO yang terhubung ke sensor DHT
#define DHTTYPE DHT22   // Jenis sensor DHT (DHT11, DHT21, DHT22)

DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  // Inisialisasi Serial Monitor
  Serial.begin(115200);

  // Menghubungkan ke jaringan Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Mengatur server MQTT
  client.setServer(mqttServer, mqttPort);

  // Inisialisasi sensor DHT
  dht.begin();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Baca suhu dari sensor DHT
  float temperature = dht.readTemperature();
  if (isnan(temperature)) {
    Serial.println("Failed to read temperature from DHT sensor!");
    return;
  }
  Serial.print("Temperature: ");
  Serial.println(temperature);

  // Mengirimkan data suhu ke broker MQTT
  String payload = String(temperature);
  client.publish(mqttTopic, payload.c_str());

  // Tunda selama 5 detik sebelum membaca suhu berikutnya
  delay(5000);
}

void reconnect() {
  // Loop hingga terhubung ke broker MQTT
  while (!client.connected()) {
    Serial.println("Connecting to MQTT Broker...");
    if (client.connect("ESP32Publisher")) {
      Serial.println("Connected to MQTT Broker");
    } else {
      Serial.print("Failed to connect to MQTT Broker. Retrying in 5 seconds...");
      delay(5000);
    }
  }
}
