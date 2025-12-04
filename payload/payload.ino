// RFID Lese-/Schreibeinheit (13.56MHz).
// - https://docs.m5stack.com/en/unit/rfid2
// - https://github.com/OSSLibraries/Arduino_MFRC522v2/tree/master

// GROVE PORT (UNIVERSAL):
// 1 GPIO1, TWI(SCL), UART(RX)
// 2 GPIO2, TWI(SDA), UART(TX)
// 3 +5V
// 4 GND

//-------------------------------------------------------------------
#include <MFRC522v2.h>
#include <MFRC522DriverI2C.h>
#include <MFRC522Debug.h>

#include <WiFi.h>
#include <MQTT.h>

//-------------------------------------------------------------------
#define WIFI_SSID  "The Sven"  /* to change */
#define WIFI_KEY   "Muschelkong312"  /* to change */

#define MQTT_CLIENT_ID  "BBW_ESP32_SW"  /* to change */

#define MQTT_BROKER_URL            "public.cloud.shiftr.io"
#define MQTT_BROKER_USER_NAME      "public"
#define MQTT_BROKER_USER_PASSWORD  "public"
#define ENABLE_MQTT                 false

//-------------------------------------------------------------------
MFRC522DriverI2C driver;
MFRC522 mfrc522(driver);

#if ENABLE_MQTT
  WiFiClient wifinetwork;
  MQTTClient mqttclient;
#endif

//-------------------------------------------------------------------
void setup()
{
  Serial.begin(115200);
  mfrc522.PCD_Init();

#if ENABLE_MQTT
  Serial.print("Connecting to network ");
  WiFi.begin(WIFI_SSID, WIFI_KEY);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println(" Connected!");

  mqttclient.begin(wifinetwork);

  mqttclient.setHost(MQTT_BROKER_URL);
  Serial.print("Connecting to MQTT broker ");

  while (mqttclient.connect(MQTT_CLIENT_ID, MQTT_BROKER_USER_NAME, MQTT_BROKER_USER_PASSWORD) == false) {
    Serial.print(".");
    delay(500);
  }
  Serial.println(" Connected!");
#endif
}

//-------------------------------------------------------------------
void loop()
{
  if (mfrc522.PICC_IsNewCardPresent() &&  mfrc522.PICC_ReadCardSerial()) {
    Serial.println("#Start Data");
    MFRC522Debug::PICC_DumpToSerial(mfrc522, Serial, &mfrc522.uid);

    MFRC522Debug::PrintSelectedUID(mfrc522, Serial);

#if ENABLE_MQTT
    String topic = "RFID-CHIP";
    // String payload = mfrc522.uid.uidByte;
    String payload ="SVEN TEST";
    mqttclient.publish(topic, payload);
#endif

    Serial.println("\n#End Data");
  }
}
