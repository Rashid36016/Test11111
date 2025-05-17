//Verified 1 Led sender
#include <esp_now.h>
#include <WiFi.h>

#define CHANNEL 1
#define LED_PIN 13  // LED-এর পিন

// পিয়ার তথ্য কাঠামো
esp_now_peer_info_t slave;

// ডেটা রিসিভ করার জন্য কলব্যাক ফাংশন
void OnDataReceived(const esp_now_recv_info *recv_info, const uint8_t *data, int len) {
  Serial.println("Connected successfully");
  Serial.print("ডেটা রিসিভ হয়েছে MAC: ");
  const uint8_t *mac_addr = recv_info->src_addr; // প্রেরকের MAC অ্যাড্রেস
  for (int i = 0; i < 6; i++) {
    Serial.printf("%02X", mac_addr[i]);
    if (i < 5) Serial.print(":");
  }
  Serial.print(" ডেটা: ");
  if (len == 1) { // যেহেতু সেন্ডার একটি একক uint8_t পাঠায়
    Serial.print(data[0]);
    digitalWrite(LED_PIN, data[0]); // ডেটা অনুযায়ী LED জ্বালানো/নেভানো
  } else {
    Serial.print("অপ্রত্যাশিত ডেটা দৈর্ঘ্য");
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);     // LED-এর জন্য পিন সেটআপ
  digitalWrite(LED_PIN, LOW);   // শুরুতে LED বন্ধ

  WiFi.mode(WIFI_STA);
  
  // রিসিভারের MAC অ্যাড্রেস প্রিন্ট
  Serial.print("রিসিভারের MAC অ্যাড্রেস: ");
  Serial.println(WiFi.macAddress());
  
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESP-NOW সফলভাবে ইনিশিয়ালাইজড হয়েছে");
  } else {
    Serial.println("ESP-NOW ইনিশিয়ালাইজড করতে সমস্যা হয়েছে");
    return;
  }

  // পিয়ার অ্যাড করা (সেন্ডারের MAC অ্যাড্রেস)
  memset(&slave, 0, sizeof(slave));
  uint8_t sender_mac[6] = {0x94, 0x54, 0xC5, 0x75, 0x89, 0xD4}; // সেন্ডারের MAC অ্যাড্রেস
  memcpy(slave.peer_addr, sender_mac, 6);
  slave.channel = CHANNEL;
  slave.encrypt = false;
  
  if (esp_now_add_peer(&slave) == ESP_OK) {
    Serial.println("পিয়ার সফলভাবে যোগ করা হয়েছে");
  } else {
    Serial.println("পিয়ার যোগ করতে সমস্যা হয়েছে");
    return;
  }

  // রিসিভ কলব্যাক রেজিস্টার করা
  esp_now_register_recv_cb(OnDataReceived);
}

void loop() {
  // কিছু করার প্রয়োজন নেই, ডেটা স্বয়ংক্রিয়ভাবে রিসিভ হবে
}