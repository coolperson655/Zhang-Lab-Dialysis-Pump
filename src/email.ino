// /*
//   Rui Santos & Sara Santos - Random Nerd Tutorials
//   Complete project details at: https://RandomNerdTutorials.com/esp32-send-email-smtp-server-arduino-ide/  
//   Based on the example provided by the ReadyMail library: https://github.com/mobizt/ReadyMail/
// */
// #include <Arduino.h>
// #include <WiFi.h>
// #include <WiFiClientSecure.h>

// #define ENABLE_SMTP
// #define ENABLE_DEBUG
// #include <ReadyMail.h>

// // REPLACE WITH YOUR NETWORK CREDENTIALS
// const char* ssid = "TMOBILE-FD74";
// const char* password = "ESP32project";
// //const char* ssid = "Infinidy 5.0";
// //const char* password = "dw4p#T9m3%5m!n9ba793HAR&99";

// // Sender SMTP settings (GMAIL)
// // Change if using a different provider
// #define SMTP_HOST "smtp.gmail.com"
// #define SMTP_PORT 465

// // Sender email, app password, and name
// #define AUTHOR_EMAIL "esptempreciever655@gmail.com"
// #define AUTHOR_APP_PASS "exyp rqzc lpcy pzbq"
// #define AUTHOR_NAME "ESP32"

// //Recipient's email
// #define RECIPIENT_EMAIL "martikyl004@gmail.com"
// #define RECIPIENT_NAME "Kyle"

// WiFiClientSecure ssl_client;
// SMTPClient smtp(ssl_client);

// void setup() {
//   Serial.begin(115200);
//   WiFi.mode(WIFI_STA);
//   WiFi.begin(ssid, password);
//   Serial.println("check1");
//   Serial.println(WiFi.status());
//   if (WiFi.status() == WL_CONNECTED){
//     Serial.println("check2");
//   }

//   while (WiFi.status() != WL_CONNECTED) delay(500);

//   ssl_client.setInsecure();

//   auto statusCallback = [](SMTPStatus status) {
//     Serial.println(status.text);
//   };

//   smtp.connect(SMTP_HOST, SMTP_PORT, statusCallback);

//   if (smtp.isConnected()) {
//     smtp.authenticate(AUTHOR_EMAIL, AUTHOR_APP_PASS, readymail_auth_password);

//     SMTPMessage msg;

//     msg.headers.add(rfc822_from, String(AUTHOR_NAME) + " <" + AUTHOR_EMAIL + ">");
//     msg.headers.add(rfc822_to, String(RECIPIENT_NAME) + " <" + RECIPIENT_EMAIL + ">");
//     msg.headers.add(rfc822_subject, "Hello from the ESP32");
//     msg.text.body("This is a plain text message.");
//     //msg.html.body("<html><body><h1>Hello!</h1></body></html>");
     
//     // Set NTP config time
//     /* For times east of the Prime Meridian use 0-12
//     For times west of the Prime Meridian add 12 to the offset.
//     Ex. American/Denver GMT would be -6. 6 + 12 = 18 */
//     const int gmtOffset_sec = 18; //offset time in seconds
//     const int daylightOffset_sec = 0; //daylight saving time offset in seconds

//     configTime(gmtOffset_sec, daylightOffset_sec, "pool.ntp.org");
//     // Set timestamp for the email
//     while (time(nullptr) < 100000) delay(100);
//     msg.timestamp = time(nullptr);

//     smtp.send(msg);
//   }
// }

// void loop() {
  
// }