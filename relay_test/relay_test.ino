/* 
  ================================================================================
  CHƯƠNG TRÌNH KIỂM TRA RELAY ĐƠN GIẢN (SIMPLE RELAY TEST)
  ================================================================================
  Mục đích: Chỉ kiểm tra hoạt động đóng/cắt của duy nhất Relay cắm ở GPIO 27.
  Chương trình này cực kỳ nhẹ, không cần Wi-Fi, không cần Blynk, không cần cảm biến.
  Giúp bạn cách ly hoàn toàn lỗi phần cứng của Relay.
  
  Hoạt động:
  - Bật Relay trong 2 giây (In chữ "Relay: BẬT (ON)" lên Serial Monitor)
  - Tắt Relay trong 2 giây (In chữ "Relay: TẮT (OFF)" lên Serial Monitor)
  ================================================================================
*/

#include <Arduino.h>

// Chân điều khiển Relay
#define RELAY_PIN 27

void setup() {
  // Khởi động cổng Serial Monitor
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n========================================");
  Serial.println("BẮT ĐẦU CHƯƠNG TRÌNH KIỂM TRA RELAY");
  Serial.println("========================================");
  
  // Cấu hình chân GPIO 27 là ĐẦU RA (OUTPUT)
  pinMode(RELAY_PIN, OUTPUT);
  
  // Tắt Relay lúc khởi động
  digitalWrite(RELAY_PIN, LOW);
  Serial.println("Chân GPIO 27 đã được cấu hình làm đầu ra.");
  Serial.println("Hệ thống sẽ bật/tắt Relay mỗi 2 giây...");
}

void loop() {
  // 1. KÍCH HOẠT RELAY (BẬT)
  Serial.println("-> [RELAY: BẬT] - Gửi tín hiệu mức CAO (HIGH) tới chân 27");
  digitalWrite(RELAY_PIN, HIGH); 
  delay(2000); // Chờ 2 giây để bạn quan sát đèn LED trên Relay và nghe tiếng "tạch"

  // 2. TẮT RELAY
  Serial.println("-> [RELAY: TẮT] - Gửi tín hiệu mức THẤP (LOW) tới chân 27");
  digitalWrite(RELAY_PIN, LOW);  
  delay(2000); // Chờ 2 giây
}
