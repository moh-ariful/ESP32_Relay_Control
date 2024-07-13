#include <WiFi.h>
#include <WiFiManager.h>

#define RELAY_PIN 33

WiFiManager wm;
WiFiServer server(80); // Deklarasikan server di sini

bool isLive = false;
String header;

void setup() {
  pinMode(RELAY_PIN, OUTPUT);

  Serial.begin(115200);
  Serial.println();
  Serial.println("Mulai Konfigurasi");

  if (!wm.autoConnect("ESP32_AP")) {
    Serial.println("Gagal menghubungkan, ESP32 beralih ke mode AP");
    while (true) {
      delay(1000);
    }
  }

  Serial.println("WiFi terhubung.");
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  server.begin();
  Serial.println("Memulai Server");
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    Serial.println("Client Baru");
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        header += c;
        Serial.write(c);
        if (c == '\n') {
          if (currentLine.length() == 0) {
            Serial.println();

            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html; charset=UTF-8");
            client.println("Connection: close");
            client.println();
            
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<meta charset=\"UTF-8\">");
            client.println("<meta http-equiv=\"refresh\" content=\"5\">"); // Mengubah meta refresh menjadi 5 detik
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.15.4/css/all.min.css\">"); // Tambahkan link Font Awesome
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center; padding-top: 10px;}"); // Mengubah padding-top menjadi 10px
            client.println("body { margin-top: 0.5cm; }"); // Menambahkan margin-top untuk body
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}");
            client.println(".header-text { color: #00008B; }");
            client.println("</style></head>");
            
            client.println("<body><h1 class='header-text'>Next Koding</h1>");
            client.println("<h1>ESP32 Web Server</h1>");

            if (header.indexOf("GET /26/H") >= 0) {
              isLive = true;
              digitalWrite(RELAY_PIN, LOW);  // Mengaktifkan relay (aktif rendah)
            }
            else if (header.indexOf("GET /27/L") >= 0) {
              isLive = false;
              digitalWrite(RELAY_PIN, HIGH);  // Mematikan relay (aktif rendah)
            }

            if (isLive == false) {
              client.print("<p><a href=\"/26/H\"><button class=\"button\"> Hidupkan Relay </button></a></p><br>");
            }
            else {
              client.print("<p><a href=\"/27/L\"><button class=\"button button2\">Matikan Relay </button></a></p><br>");
            }

            client.println("</body></html>");
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    client.stop();
    Serial.println("Client Terputus");
    header = "";
  }
}
