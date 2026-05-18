/* --- CẤU HÌNH BLYNK --- */
#define BLYNK_TEMPLATE_ID   "TMPL2378628"
#define BLYNK_TEMPLATE_NAME "Giám sát môi trường"
#define BLYNK_AUTH_TOKEN    "HM_Co1NJv9KJp7Phnny0tyGQBRHkO1e3"

#define BLYNK_PRINT Serial

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <BlynkSimpleEsp32.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESPmDNS.h> // Thư viện tạo tên miền cục bộ (.local)

// Thông tin WiFi
const char* ssid = "VIETSET_TECH";
const char* pass = "vs68686868";

// Khởi tạo Web Server ở cổng 80
WebServer server(80);

// Biến lưu dữ liệu hiển thị Web và Blynk
float currentTemp = 0.0;
int currentSmoke = 0;

// Biến lưu ngưỡng cảnh báo (Có thể thay đổi từ Blynk)
float tempThreshold = 50.0; 
int smokeThreshold = 2000;  

// Biến theo dõi sự thay đổi trạng thái của Relay
bool lastRelayState = false;

// Biến theo dõi sự thay đổi trạng thái của Nhiệt độ
bool lastTempState = false;

// Khai báo chân
#define DS18B20_PIN 4
#define BUZZER_PIN 18
#define MQ5_PIN 33
#define FAN_PIN 25
#define RELAY_PIN 27
#define LED_STATUS 26

OneWire oneWire(DS18B20_PIN);
DallasTemperature sensors(&oneWire);

unsigned long previousMillis = 0;
const long interval = 5000; // Cập nhật dữ liệu mỗi 5 giây

/* --- GIAO DIỆN WEB SERVER NỘI BỘ (AESTHETIC GLASSMORPHISM UI) --- */
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="vi">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Hệ Thống Giám Sát Môi Trường</title>
    <link href="https://fonts.googleapis.com/css2?family=Outfit:wght@300;400;600;800&display=swap" rel="stylesheet">
    <style>
        :root {
            --bg-gradient: linear-gradient(135deg, #090d16 0%, #15102a 100%);
            --card-bg: rgba(30, 41, 59, 0.45);
            --card-border: rgba(255, 255, 255, 0.08);
            --primary: #38bdf8;
            --accent: #818cf8;
            --warning: #f43f5e;
            --success: #34d399;
            --text-main: #f8fafc;
            --text-sub: #94a3b8;
        }
        body { 
            font-family: 'Outfit', sans-serif; 
            background: var(--bg-gradient); 
            color: var(--text-main);
            min-height: 100vh;
            margin: 0;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            padding: 20px;
            box-sizing: border-box;
        }
        .container {
            width: 100%;
            max-width: 420px;
            background: rgba(15, 23, 42, 0.3);
            backdrop-filter: blur(15px);
            -webkit-backdrop-filter: blur(15px);
            border: 1px solid rgba(255, 255, 255, 0.05);
            border-radius: 28px;
            padding: 35px 25px;
            box-sizing: border-box;
            text-align: center;
            box-shadow: 0 25px 50px -12px rgba(0, 0, 0, 0.5);
        }
        h1 { 
            font-weight: 800;
            font-size: 26px;
            background: linear-gradient(to right, #38bdf8, #818cf8);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
            margin: 0 0 8px 0;
            letter-spacing: 0.5px;
            text-transform: uppercase;
        }
        .subtitle {
            color: var(--text-sub);
            font-size: 13px;
            margin-bottom: 30px;
            display: flex;
            align-items: center;
            justify-content: center;
            gap: 8px;
        }
        .card { 
            background: var(--card-bg); 
            backdrop-filter: blur(10px);
            -webkit-backdrop-filter: blur(10px);
            border: 1px solid var(--card-border);
            padding: 22px; 
            margin: 16px 0; 
            border-radius: 20px; 
            box-shadow: 0 10px 25px -5px rgba(0, 0, 0, 0.3);
            transition: all 0.4s cubic-bezier(0.4, 0, 0.2, 1);
            position: relative;
            overflow: hidden;
        }
        .card:hover {
            transform: translateY(-5px);
            border-color: rgba(56, 189, 248, 0.25);
            box-shadow: 0 20px 30px -10px rgba(56, 189, 248, 0.15);
        }
        h3 {
            color: var(--text-sub);
            font-size: 13px;
            font-weight: 600;
            margin: 0 0 12px 0;
            text-transform: uppercase;
            letter-spacing: 1.2px;
        }
        .value { 
            font-size: 42px; 
            font-weight: 700; 
            display: inline-block;
        }
        .temp-card .value {
            background: linear-gradient(135deg, #ff7e5f 0%, #feb47b 100%);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
        }
        .smoke-card .value {
            background: linear-gradient(135deg, #38bdf8 0%, #818cf8 100%);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
        }
        .warning-banner { 
            background: rgba(244, 63, 94, 0.12);
            border: 1px solid rgba(244, 63, 94, 0.25);
            color: #fda4af;
            padding: 16px;
            border-radius: 16px;
            font-weight: 600; 
            font-size: 13px;
            margin-top: 24px; 
            display: none; 
            align-items: center;
            justify-content: center;
            gap: 8px;
            animation: pulse 1.8s infinite alternate;
            box-shadow: 0 0 15px rgba(244, 63, 94, 0.15);
        }
        @keyframes pulse {
            0% { transform: scale(1); opacity: 0.9; }
            100% { transform: scale(1.02); opacity: 1; }
        }
        .dot {
            width: 8px;
            height: 8px;
            background-color: var(--success);
            border-radius: 50%;
            display: inline-block;
            box-shadow: 0 0 8px var(--success);
        }
        .dot.loading {
            background-color: #f59e0b;
            box-shadow: 0 0 8px #f59e0b;
            animation: blink 1s infinite alternate;
        }
        @keyframes blink {
            0% { opacity: 0.3; }
            100% { opacity: 1; }
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Giám Sát Môi Trường</h1>
        <div class="subtitle"><span class="dot" id="status-dot"></span>Cập nhật: <span id="time">Đang tải...</span></div>
        
        <div class="card temp-card">
            <h3>Nhiệt độ DS18B20</h3>
            <div class="value" id="temp">-- °C</div>
        </div>
        
        <div class="card smoke-card">
            <h3>Rò rỉ Gas (MQ5)</h3>
            <div class="value" id="smoke">--</div>
        </div>
        
        <div id="alarm" class="warning-banner">
            <span>⚠️</span> <span>CẢNH BÁO NGUY HIỂM! HỆ THỐNG PHÁT HIỆN SỰ CỐ</span>
        </div>
    </div>
    <script>
        const statusDot = document.getElementById('status-dot');
        function fetchData() {
            statusDot.className = 'dot loading';
            fetch('/data')
            .then(response => response.json())
            .then(data => {
                document.getElementById('temp').innerText = data.temperature.toFixed(1) + " °C";
                document.getElementById('smoke').innerText = data.smoke;
                document.getElementById('time').innerText = data.time;
                statusDot.className = 'dot';
                statusDot.style.backgroundColor = '#34d399';
                statusDot.style.boxShadow = '0 0 8px #34d399';
                if(data.alarm == 1) {
                    document.getElementById('alarm').style.display = "flex";
                    statusDot.style.backgroundColor = '#f43f5e';
                    statusDot.style.boxShadow = '0 0 8px #f43f5e';
                } else {
                    document.getElementById('alarm').style.display = "none";
                }
            })
            .catch(error => {
                console.log("Mất kết nối với ESP32");
                statusDot.className = 'dot loading';
                statusDot.style.backgroundColor = '#f59e0b';
                statusDot.style.boxShadow = '0 0 8px #f59e0b';
            });
        }
        setInterval(fetchData, 2000); 
        fetchData(); 
    </script>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", index_html);
}

void handleData() {
  int isAlarm = (currentTemp > tempThreshold || currentSmoke > smokeThreshold) ? 1 : 0;
  String json = "{\"temperature\": " + String(currentTemp) + ", \"smoke\": " + String(currentSmoke) + ", \"time\": \"Vừa xong\", \"alarm\": " + String(isAlarm) + "}";
  server.send(200, "application/json", json);
}

/* --- HÀM NHẬN DỮ LIỆU TỪ BLYNK (SLIDER / NUMERIC INPUT) --- */
// V2: Slider chỉnh ngưỡng nhiệt độ
BLYNK_WRITE(V2) {
  tempThreshold = param.asFloat();
  Serial.print("Ngưỡng nhiệt độ mới (Blynk): ");
  Serial.println(tempThreshold);
}

// V3: Slider chỉnh ngưỡng khói/gas
BLYNK_WRITE(V3) {
  smokeThreshold = param.asInt();
  Serial.print("Ngưỡng khói mới (Blynk): ");
  Serial.println(smokeThreshold);
}

// Bật đồng bộ giá trị Threshold từ server Blynk xuống ESP32 mỗi khi khởi động/mất mạng có lại
BLYNK_CONNECTED() {
  Blynk.syncVirtual(V2);
  Blynk.syncVirtual(V3);
}

/* --- SETUP & LOOP --- */
void setup() {
  Serial.begin(115200);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_STATUS, OUTPUT);
  pinMode(MQ5_PIN, INPUT);

  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(FAN_PIN, LOW);
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(LED_STATUS, LOW);

  sensors.begin();

  Serial.print("Đang kết nối WiFi");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nĐã kết nối WiFi!");
  Serial.print("ĐỊA CHỈ TRANG WEB NỘI BỘ: http://");
  Serial.println(WiFi.localIP());
  digitalWrite(LED_STATUS, HIGH);

  // Khởi động Web Server
  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();

  // Khởi động tên miền cục bộ giamsat.local
  if (MDNS.begin("giamsat")) {
    Serial.println("Đã cấu hình tên miền cục bộ: http://giamsat.local");
  }

  // Khởi động Blynk không đồng bộ
  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connect();
}

void loop() {
  unsigned long currentMillis = millis();

  // Chạy nền Web Server và Blynk
  server.handleClient();
  Blynk.run();

  // Xử lý mất mạng WiFi
  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_STATUS, LOW);
    WiFi.reconnect();
  } else {
    digitalWrite(LED_STATUS, HIGH);
  }

  // Đọc và xử lý chu kỳ 5 giây
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Đọc cảm biến
    sensors.requestTemperatures(); 
    float tempValue = sensors.getTempCByIndex(0);
    int smokeValue = analogRead(MQ5_PIN);

    if (tempValue == DEVICE_DISCONNECTED_C) {
      Serial.println("Lỗi: Không tìm thấy cảm biến nhiệt độ DS18B20!");
      return; 
    }

    currentTemp = tempValue;
    currentSmoke = smokeValue;

    Serial.print("Nhiệt độ: "); Serial.print(currentTemp); Serial.println(" *C");
    Serial.print("Khí gas MQ5: "); Serial.println(currentSmoke);

    // Đẩy dữ liệu lên biểu đồ/số đo trên Blynk
    Blynk.virtualWrite(V0, currentTemp);
    Blynk.virtualWrite(V1, currentSmoke);

    // 1. Logic Quạt (Theo nhiệt độ)
    bool newTempState = (currentTemp > tempThreshold);
    if (newTempState != lastTempState) {
      lastTempState = newTempState;
      if (newTempState) {
        digitalWrite(FAN_PIN, HIGH);
        Serial.println("\n[LOG TEMP] -> CẢNH BÁO: Nhiệt độ vượt ngưỡng! Bật quạt.");
        Blynk.logEvent("canh_bao_nhiet", "CẢNH BÁO: Nhiệt độ vượt ngưỡng an toàn!");
      } else {
        digitalWrite(FAN_PIN, LOW);
        Serial.println("\n[LOG TEMP] -> AN TOÀN: Nhiệt độ đã hạ xuống mức bình thường. Tắt quạt.");
      }
    }

    // 2. Logic Relay & Còi (Theo khói/gas)
    bool newRelayState = (currentSmoke > smokeThreshold);
    if (newRelayState != lastRelayState) {
      lastRelayState = newRelayState;
      if (newRelayState) {
        digitalWrite(RELAY_PIN, HIGH);
        Serial.println("\n[LOG RELAY] -> TRẠNG THÁI THAY ĐỔI: BẬT (ON) - Phát hiện rò rỉ Gas vượt ngưỡng!");
        Blynk.logEvent("canh_bao_gas", "CẢNH BÁO: Phát hiện rò rỉ khí Gas vượt ngưỡng!");
      } else {
        digitalWrite(RELAY_PIN, LOW);
        Serial.println("\n[LOG RELAY] -> TRẠNG THÁI THAY ĐỔI: TẮT (OFF) - Nồng độ gas trở về mức an toàn.");
      }
    }

    if (currentSmoke > smokeThreshold) {
      digitalWrite(BUZZER_PIN, HIGH); 
      Serial.println(">>> CẢNH BÁO KHÓI/GAS <<<");
    } else {
      digitalWrite(BUZZER_PIN, LOW);
    }
  }
}
