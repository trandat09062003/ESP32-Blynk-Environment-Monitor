# 🌿 Dự Án Giám Sát Môi Trường Thông Minh (ESP32 - Blynk IoT - Web Server)

Chào mừng bạn đến với tài liệu hướng dẫn chi tiết của dự án **Giám sát môi trường**. Đây là một hệ thống IoT hoàn chỉnh sử dụng vi điều khiển **ESP32** để theo dõi nhiệt độ phòng và nồng độ khí gas/khói theo thời gian thực, đồng thời tự động kích hoạt quạt làm mát và hệ thống cảnh báo (còi hú, relay ngắt khẩn cấp) khi có sự cố xảy ra.

Dự án này sở hữu tính năng **Dual-Control/Dual-Monitor**:
1. **Giám sát & Điều khiển từ xa qua Blynk IoT Cloud**: Biểu đồ trực quan, đẩy dữ liệu liên tục và cho phép người dùng cấu hình ngưỡng cảnh báo bằng thanh kéo (Slider) trực tiếp trên điện thoại hoặc máy tính.
2. **Trang Web nội bộ (Local Web Server)**: Giao diện web được thiết kế theo phong cách **Glassmorphism UI** hiện đại, mượt mà, phản hồi siêu nhanh trực tiếp từ IP của ESP32 qua mạng LAN/Wi-Fi cục bộ.

---

## 📌 Các Tính Năng Nổi Bật

*   **Đo nhiệt độ chính xác**: Sử dụng cảm biến **DS18B20** giao tiếp qua chuẩn 1-Wire, dải đo rộng và cực kỳ ổn định.
*   **Đo khí gas rò rỉ nhạy bén**: Sử dụng cảm biến **MQ5** (kết nối chân Analog) để phát hiện sớm các nguy cơ rò rỉ khí gas hóa lỏng LPG, Methane (CH4), hoặc gas tự nhiên.
*   **Điều khiển tự động thông minh**:
    *   **Quạt làm mát (FAN)** tự động bật khi nhiệt độ vượt quá ngưỡng cài đặt (`tempThreshold`).
    *   **Còi báo (BUZZER)** và **Relay (thiết bị an toàn)** tự động kích hoạt ngay khi nồng độ khí gas vượt ngưỡng an toàn (`smokeThreshold`).
*   **Giao diện Glassmorphism Web hiện đại**: Hiển thị dữ liệu dạng thẻ (Card) thời thượng, hỗ trợ hiệu ứng chuyển động vi mô (micro-animations), chế độ cảnh báo nhấp nháy đỏ khi có sự cố nguy hiểm và cơ chế tự kết nối lại nếu mất mạng.
*   **Đồng bộ hóa Blynk 2-way**: Đồng bộ các thông số cấu hình ngưỡng cảnh báo giữa ESP32 và Server Blynk ngay cả khi khởi động lại (`Blynk.syncVirtual`).

---

## 🛠️ Cấu Hình Phần Cứng & Sơ Đồ Nối Chân

Dưới đây là sơ đồ kết nối các chân IO của ESP32 với các linh kiện ngoại vi:

| Thiết Bị | Loại Linh Kiện | Chân ESP32 (GPIO) | Ghi Chú |
| :--- | :--- | :---: | :--- |
| **DS18B20** | Cảm biến Nhiệt độ | **GPIO 4** | Cần trở kéo lên **4.7kΩ** nối giữa chân VCC (3.3V) và chân Data. |
| **MQ5** | Cảm biến Rò rỉ Gas | **GPIO 33** | Kết nối chân **AO (Analog Output)** vào ESP32. |
| **BUZZER** | Còi báo động | **GPIO 18** | Dùng còi báo tích cực (Active Buzzer) hoặc qua Transistor đệm. |
| **FAN** | Quạt làm mát (5V/12V) | **GPIO 25** | Điều khiển qua Transistor (C1815/IRF540) hoặc Relay Module. |
| **RELAY** | Relay ngắt điện / thiết bị phụ | **GPIO 27** | Sử dụng Module Relay cách ly quang (Optocoupler). |
| **LED STATUS**| LED hiển thị Wi-Fi | **GPIO 26** | Đèn sáng: Wi-Fi Connected | Đèn tắt/nhấp nháy: Wi-Fi Disconnected. |

> [!IMPORTANT]
> **Lưu ý lắp ráp phần cứng:**
> - Cảm biến **MQ5** cần nguồn **5V** ổn định để sấy nóng cuộn dây cảm ứng bên trong. Nếu cấp nguồn 3.3V, kết quả đo analog sẽ rất thấp và không chính xác.
> - Cảm biến **DS18B20** bắt buộc phải có trở kéo lên **4.7kΩ** từ chân Data lên VCC (3.3V). Nếu không có trở này, ESP32 sẽ báo lỗi không tìm thấy cảm biến (`-127 °C`).

---

## 📂 Cấu Trúc Thư Mục Dự Án

Thư mục dự án được tổ chức gọn gàng và dễ dàng nạp code:

```text
blink/
├── blink.ino              <-- Mã nguồn chính của dự án (Blynk IoT + Web Server)
├── code.TXT               <-- Bản lưu mã nguồn sơ cua (để tham khảo)
├── README.md              <-- Hướng dẫn này (Tài liệu dự án)
├── hardware_test/
│   └── hardware_test.ino  <-- Bản vẽ TEST PHẦN CỨNG nhanh qua Serial Monitor
└── libraries/             <-- Chứa các thư viện cục bộ (Blynk, DallasTemp, OneWire)
```

---

## 🚀 Hướng Dẫn Cấu Hình Phần Mềm

### 1. Cài đặt môi trường
*   Tải và cài đặt **Arduino IDE** phiên bản mới nhất (khuyến nghị 2.x).
*   Thêm board ESP32 vào Arduino IDE:
    1.  Vào `File` -> `Preferences`.
    2.  Tại ô `Additional Boards Manager URLs`, dán đường dẫn: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
    3.  Vào `Tools` -> `Board` -> `Boards Manager`, tìm kiếm `esp32` và ấn **Install**.

### 2. Cài đặt Thư viện cần thiết
Dự án đã tích hợp sẵn thư viện trong thư mục `libraries`. Nếu Arduino IDE không tự nhận diện, hãy đảm bảo bạn đã cài đặt các thư viện sau thông qua thư viện hệ thống (`Sketch` -> `Include Library` -> `Manage Libraries...`):
*   `Blynk` (bởi Volodymyr Shymanskyy)
*   `OneWire` (bởi Paul Stoffregen)
*   `DallasTemperature` (bởi Miles Burton)

### 3. Cấu hình Code chính (`blink.ino`)
Mở file [blink.ino](file:///c:/Users/DELL/OneDrive%20-%20Hanoi%20University%20of%20Science%20and%20Technology/Desktop/blink/blink.ino) trong Arduino IDE và điều chỉnh các thông tin sau để khớp với tài khoản Blynk và Wi-Fi nhà bạn:

```cpp
/* --- CẤU HÌNH BLYNK --- */
#define BLYNK_TEMPLATE_ID   "TMPL2378628"                  // ID Template Blynk của bạn
#define BLYNK_TEMPLATE_NAME "Giám sát môi trường"           // Tên Template Blynk của bạn
#define BLYNK_AUTH_TOKEN    "HM_Co1NJv9KJp7Phnny0tyGQBRHkO1e3" // Auth Token thiết bị Blynk

// Thông tin mạng WiFi của bạn
const char* ssid = "VIETSET_TECH";   // Tên Wi-Fi
const char* pass = "vs68686868";     // Mật khẩu Wi-Fi
```

---

## 🧪 QUY TRÌNH KIỂM TRA TOÀN DIỆN (FULL FUNCTIONAL TEST)

Để đảm bảo hệ thống hoạt động hoàn hảo 100%, hãy thực hiện quy trình test gồm 4 giai đoạn chi tiết dưới đây:

### Giai Đoạn 1: Kiểm Tra Chẩn Đoán Phần Cứng (Hardware Diagnostic)
Trước khi khởi chạy hệ thống IoT phức tạp, ta dùng công cụ chẩn đoán nhanh để xác định các linh kiện và dây nối có chạy tốt không.

1.  Mở thư mục [hardware_test/hardware_test.ino](file:///c:/Users/DELL/OneDrive%20-%20Hanoi%20University%20of%20Science%20and%20Technology/Desktop/blink/hardware_test/hardware_test.ino) bằng Arduino IDE.
2.  Kết nối ESP32 với máy tính qua cáp USB và chọn đúng cổng COM.
3.  Nạp (Upload) code `hardware_test.ino`.
4.  Mở **Serial Monitor** trong Arduino IDE, chỉnh tốc độ thành **115200 baud**, chọn chế độ gửi kèm kí tự dòng mới `Both NL & CR`.
5.  Thực hiện gõ các phím trên bàn phím máy tính để kiểm tra trực quan:
    *   Gõ `1` -> Đèn LED Trạng thái (chân 26) có sáng không? Gõ `1` lần nữa để tắt.
    *   Gõ `2` -> Quạt (chân 25) có quay không? Gõ `2` lần nữa để tắt.
    *   Gõ `3` -> Relay (chân 27) có kêu tiếng "tạch" giòn giã và cấp điện không? Gõ `3` để tắt.
    *   Gõ `4` -> Còi báo (chân 18) có hú còi liên tục không? Gõ `4` để tắt.
    *   Gõ `5` -> Màn hình hiển thị giá trị nhiệt độ bao nhiêu? (Nếu báo `[LỖI]`, kiểm tra lại điện trở kéo lên 4.7kΩ và kết nối DS18B20).
    *   Gõ `6` -> Cảm biến MQ5 trả về giá trị bao nhiêu? (Nếu dưới 100, cảm biến chưa được làm nóng đủ hoặc lỗi dây VCC 5V).
    *   Gõ `7` -> Chế độ **Auto Test**: Hệ thống sẽ tự chạy một chu trình nhấp nháy đèn -> còi kêu tít tít 3 lần -> quạt quay 3 giây -> relay đóng 3 giây -> đo liên tục 2 cảm biến trong 5 giây. Nếu tất cả đều đáp ứng tốt, phần cứng của bạn đã hoàn hảo!

---

### Giai Đoạn 2: Kiểm Tra Kết Nối Wi-Fi & Web Server Nội Bộ
Khi phần cứng đã hoạt động tốt, chúng ta tiến hành nạp mã nguồn chính và kiểm tra cổng Web Server nội bộ.

1.  Mở [blink.ino](file:///c:/Users/DELL%20-%20Hanoi%20University%20of%20Science%20and%20Technology/Desktop/blink/blink.ino) trong Arduino IDE.
2.  Nạp (Upload) code sang ESP32.
3.  Đảm bảo điện thoại hoặc máy tính test **kết nối chung một mạng Wi-Fi** với ESP32.
4.  Có 2 cách để truy cập trang web giám sát:
    *   **Cách 1 (Nhanh và tiện nhất):** Mở trình duyệt Web (Chrome, Safari, Edge...) và nhập địa chỉ tên miền cục bộ: **`http://giamsat.local`** rồi nhấn Enter.
    *   **Cách 2 (Sử dụng IP):** Mở **Serial Monitor** ở tốc độ **115200 baud**, xem dòng chữ địa chỉ IP được in ra (Ví dụ: `http://192.168.1.15`) rồi gõ địa chỉ IP đó vào trình duyệt Web.
5.  **Đánh giá Giao Diện Web:**
    *   Giao diện Glassmorphism cực đẹp xuất hiện với tiêu đề "GIÁM SÁT MÔI TRƯỜNG".
    *   Dữ liệu nhiệt độ và khí gas sẽ được cập nhật tự động liên tục mỗi 2 giây mà không cần tải lại trang (nhờ công nghệ fetch AJAX gọi API `/data`).
    *   Dấu chấm trạng thái (Status Dot) góc trên sẽ chuyển nhấp nháy màu vàng cam khi đang tải dữ liệu và giữ màu xanh lục nhạt khi kết nối tốt.

---

### Giai Đoạn 3: Cấu Hình & Kiểm Tra Blynk IoT Cloud
Tiếp theo, ta kiểm tra khả năng đẩy dữ liệu từ xa thông qua đám mây Blynk.

1.  Truy cập vào ứng dụng **Blynk App** trên điện thoại di động hoặc **Blynk Console** trên trình duyệt web máy tính.
2.  Tạo hoặc mở Template có tên trùng khớp: `Giám sát môi trường`.
3.  Thiết lập các chân ảo (**Datastreams**) trên Blynk như sau:
    *   **V0 (Virtual Pin 0)**: Kiểu dữ liệu `Double/Float`, đơn vị `°C`, dùng hiển thị Nhiệt độ hiện tại.
    *   **V1 (Virtual Pin 1)**: Kiểu dữ liệu `Integer`, dải đo `0 - 4095`, dùng hiển thị Nồng độ khí gas MQ5.
    *   **V2 (Virtual Pin 2)**: Kiểu dữ liệu `Double/Float`, dải đo `0 - 100`, chế độ Đọc/Ghi (Read/Write), dùng làm thanh kéo **Slider chỉnh Ngưỡng nhiệt độ** (`tempThreshold`).
    *   **V3 (Virtual Pin 3)**: Kiểu dữ liệu `Integer`, dải đo `0 - 4095`, chế độ Đọc/Ghi (Read/Write), dùng làm thanh kéo **Slider chỉnh Ngưỡng khí gas** (`smokeThreshold`).
4.  Thiết kế giao diện Widget:
    *   Kéo thả 2 Widget hiển thị số hoặc biểu đồ (Gauge / Chart) kết nối tới **V0** và **V1**.
    *   Kéo thả 2 Widget thanh trượt (Slider) hoặc ô nhập số (Numeric Input) kết nối tới **V2** và **V3**.
5.  **Chạy thử nghiệm:**
    *   Mở app Blynk và chỉnh thanh Slider **V2** lên `35.0 °C` và Slider **V3** lên `1500`.
    *   Quan sát Serial Monitor của ESP32, xem có in dòng chữ nhận lệnh không:
        ```text
        Ngưỡng nhiệt độ mới (Blynk): 35.00
        Ngưỡng khói mới (Blynk): 1500
        ```
    *   Nhấn nút Reset trên mạch ESP32. Sau khi khởi động và kết nối lại, ESP32 sẽ tự động gọi hàm `BLYNK_CONNECTED()` để đồng bộ hóa, kéo lại chính xác giá trị `35.00` và `1500` từ đám mây xuống bộ nhớ RAM của mạch mà bạn không cần chỉnh lại.

---

### Giai Đoạn 4: Kiểm Tra Kịch Bản Logic Cảnh Báo Tự Động (Automation Logic)
Đây là bước quan trọng nhất để chứng minh thuật toán bảo vệ hoạt động chính xác khi có sự cố.

#### Kịch Bản A: Vượt Ngưỡng Nhiệt Độ (Tự động kích hoạt Quạt)
1.  Trên giao diện Blynk, kéo Slider chỉnh ngưỡng nhiệt độ (**V2**) xuống thấp hơn nhiệt độ môi trường hiện tại (Ví dụ: Nhiệt độ phòng là `30 °C`, bạn kéo ngưỡng xuống `28 °C`).
2.  Quan sát phản ứng ngay lập tức (trong vòng tối đa 5 giây):
    *   **Quạt (GPIO 25)** lập tức kích hoạt quay mạnh để hạ nhiệt.
    *   **Trang web nội bộ**: Hiển thị bảng màu đỏ cảnh báo đỏ chói: `⚠️ CẢNH BÁO NGUY HIỂM! HỆ THỐNG PHÁT HIỆN SỰ CỐ`. Dấu chấm trạng thái nhấp nháy đỏ rực.
3.  Kéo Slider ngưỡng nhiệt độ (**V2**) lên cao trở lại (Ví dụ: `45 °C`).
    *   **Quạt (GPIO 25)** lập tức tắt.
    *   Bảng cảnh báo nguy hiểm biến mất, giao diện trở lại trạng thái xanh dịu mát bình thường.

#### Kịch Bản B: Vượt Ngưỡng Rò Rỉ Gas (Tự động còi báo và kích Relay an toàn)
1.  Trên giao diện Blynk, đặt ngưỡng khí gas MQ5 (**V3**) là `1800`.
2.  Dùng một chiếc bật lửa gas (nhấn hờ nút xì gas không đánh lửa để thổi khí gas ra trực tiếp trước đầu cảm biến MQ5).
3.  Quan sát giá trị hiển thị trên Web Server và Blynk tăng dần qua mức 1800:
    *   Ngay khi giá trị vượt ngưỡng `1800`, **Còi báo (GPIO 18)** sẽ rú vang liên hồi báo hiệu nguy hiểm.
    *   Đồng thời **Relay (GPIO 27)** sẽ đóng mạch (bật) để kích hoạt hệ thống cứu hộ hoặc tự động ngắt điện nguồn phòng chống cháy nổ.
    *   Trên Web hiển thị cảnh báo đỏ rực rỡ báo động rò rỉ gas nguy cấp.
    *   Serial Monitor hiển thị liên tục dòng chữ: `>>> CẢNH BÁO KHÓI/GAS <<<`.
4.  Khi khí gas tan đi, giá trị cảm biến MQ5 giảm xuống dưới `1800`:
    *   Còi báo tắt ngay lập tức.
    *   Relay ngắt trạng thái kích hoạt về an toàn.
    *   Hệ thống trở về trạng thái giám sát nền bình thường.

---

## 📈 Bảo Trì & Xử Lý Sự Cố (Troubleshooting)

*   **ESP32 liên tục khởi động lại (Bootloop)**: Kiểm tra nguồn điện cấp cho ESP32. Khi bật còi báo, quạt và relay đóng mở đồng thời, dòng điện tiêu thụ tăng cao đột biến. Hãy sử dụng củ sạc USB chất lượng cao (5V - 2A trở lên) cấp nguồn qua cổng sạc ESP32.
*   **Trang Web nội bộ không truy cập được**: Đảm bảo thiết bị truy cập (điện thoại/máy tính) và ESP32 đang kết nối **cùng một bộ phát Wi-Fi (SSID)**. Đồng thời kiểm tra xem địa chỉ IP trên Serial Monitor có bị thay đổi sau mỗi lần reset mạch không (cân nhắc gán IP tĩnh trong code nếu cần thiết).
*   **Cảm biến MQ5 đọc giá trị đứng im**: Cảm biến MQ5 mới mua cần được cắm nguồn liên tục trong khoảng 12 - 24 giờ đầu tiên để "lão hóa" lớp hóa chất bên trong bề mặt cảm ứng. Sau thời gian này, cảm biến sẽ hoạt động rất nhạy và chính xác.

---

Chúc các bạn vận hành hệ thống giám sát môi trường thành công! Hệ thống này rất phù hợp làm đồ án môn học, hệ thống nhà thông minh (Smart Home) tự chế hoặc giám sát an toàn cho phòng máy chủ Server.
