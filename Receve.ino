//Verified LED Recever
#include <esp_now.h>
#include <WiFi.h>

#define CHANNEL 1
#define BUTTON_PIN 14  // বাটনের পিন
#define LED_PIN 13     // LED-এর পিন

// পিয়ার তথ্য কাঠামো
esp_now_peer_info_t slave;

// ডেটা পাঠানোর স্থিতি কলব্যাক ফাংশন
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  if (status == ESP_NOW_SEND_SUCCESS) {
    Serial.println("Connected successfully");
  } else {
    Serial.println("ডেটা পাঠানো ব্যর্থ হয়েছে");
  }
}

// ডেটা পাঠানোর জন্য ফাংশন
void sendData(uint8_t *data, size_t len) {
  esp_err_t result = esp_now_send(slave.peer_addr, data, len);
  if (result != ESP_OK) {
    Serial.println("ডেটা পাঠাতে সমস্যা হয়েছে");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLDOWN);  // বাটনের জন্য পিন সেটআপ
  pinMode(LED_PIN, OUTPUT);             // LED-এর জন্য পিন সেটআপ
  digitalWrite(LED_PIN, LOW);           // শুরুতে LED বন্ধ

  WiFi.mode(WIFI_STA);
  
  // সেন্ডারের MAC অ্যাড্রেস প্রিন্ট
  Serial.print("সেন্ডারের MAC অ্যাড্রেস: ");
  Serial.println(WiFi.macAddress());
  
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESP-NOW সফলভাবে ইনিশিয়ালাইজড হয়েছে");
  } else {
    Serial.println("ESP-NOW ইনিশিয়ালাইজড করতে সমস্যা হয়েছে");
    return;
  }

  // পিয়ার অ্যাড্রেস নির্ধারণ এবং যোগ করা
  memset(&slave, 0, sizeof(slave));
  uint8_t receiver_mac[6] = {0x78, 0x42, 0x1C, 0x6d, 0x3f, 0xb4}; // রিসিভারের MAC অ্যাড্রেস
  memcpy(slave.peer_addr, receiver_mac, 6);
  slave.channel = CHANNEL;
  slave.encrypt = false;

  if (esp_now_add_peer(&slave) == ESP_OK) {
    Serial.println("পিয়ার সফলভাবে যোগ করা হয়েছে");
  } else {
    Serial.println("পিয়ার যোগ করতে সমস্যা হয়েছে");
    return;
  }

  // সেন্ড কলব্যাক রেজিস্টার করা
  esp_now_register_send_cb(OnDataSent);
}

void loop() {
  static uint8_t lastButtonState = LOW;  // বাটনের আগের অবস্থা
  uint8_t buttonState = digitalRead(BUTTON_PIN);  // বাটনের বর্তমান অবস্থা

  // বাটনের অবস্থা পরিবর্তন হলে ডেটা পাঠানো
  if (buttonState != lastButtonState) {
    lastButtonState = buttonState;
    digitalWrite(LED_PIN, buttonState);  // বাটনের অবস্থা অনুযায়ী LED জ্বালানো/নেভানো
    sendData(&buttonState, sizeof(buttonState));  // বাটনের অবস্থা পাঠানো
    Serial.print("বাটনের অবস্থা পাঠানো হয়েছে: ");
    Serial.println(buttonState);
  }

  delay(50);  // ডিবাউন্সিং এবং স্থিতিশীলতার জন্য
}
