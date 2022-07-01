#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP_EEPROM.h>

const char *ssid     = "SSID HERE";
const char *password = "MDP HERE";

ESP8266WebServer webServer(80);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 7200); // Fuseau Paris

String hr, minut ;
int hr1, minut1;
String new_time;
int vibrate_time = 3000;
int vibrating = 0;
int reed = D2;
int BUTTONstate = 0;

/*START OF HMTL CODE*/
String style_detials =  //This String defines the style attributes for webpage
  "<style type=\"text/css\">"
  " body{"
  "  background-color: #193047;"
  "}"
  " h1{"
  "  color: white;"
  "  font-family: Tahoma,sans-serif;"
  "}"
  " h2{"
  "  color: white;"
  "  font-family: Tahoma,sans-serif;"
  "}"
  " div{"
  "  color: white;"
  "  font-family: Tahoma,sans-serif;"
  "}"

  "input[type=submit] {"
//" padding: 5px 15px;"
  " margin-top: 30px;"
  " background:#ccc;"
  " border:0 none;"
  " cursor:pointer;"
  " -webkit-border-radius: 5px;"
  " border-radius: 5px;"
  " height: 100px;"
  " width: 300px;"
  " font-size: 30px;"
  " color: white;"
  " font-weight: bold;"
  "}"
  "}"


  "</style>";


String set_alarm = "" //Page 1 - Home Screen HTML code
                   "<!DOCTYPE html><html>"
                   "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=0\">"
                   + style_detials +
                   "<body><h1>Or&eacuteveil vibrant</h1>"
                   "<div id=\"login\">"
                   "  <form action=\"confirmation.php\" method=\"get\">"
                   "  <h2 >Merci d'indiquer l'heure du reveil au format (05:45)</h2>"
                   "   <div>Heure du reveil: <input type=\"numeric\" name=\"code\" autofocus></div>"
                   "   <div id=\"submit\"><input type=\"submit\" value=\"Valider\"></div>"
                   "  </form>"
                   "</div>"
                   "</body></html>";

String confirmation = "" //Page 2 - If Alarm is Set
                      "<!DOCTYPE html><html>"
                      "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
                      + style_detials +
                      "<body><h1>Alarme regl&eacutee</h1>"
                      "</body></html>";

String error = "" //Page 2 - If Alarm is Set
               "<!DOCTYPE html><html>"
               "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
               + style_detials +
               "<body><h1>Le r&eacuteglage de l'heure n'est pas au bon format</h1>"
               "<h2>Merci de respecter le format HH:MM</h2>"
               "</body></html>";

/*END OF HMTL CODE*/

void setup() {
  Serial.begin(115200);
  EEPROM.begin(EEPROM_MIN_SIZE);
  WiFi.begin(ssid, password);

  //EEPROM.wipe();

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  webServer.on("/", []() {
    webServer.send(200, "text/html", set_alarm);
  });

  webServer.on("/confirmation.php", []() {

    new_time = webServer.arg(0);// Stores the Alarm time input
    hr = new_time.substring(0, 2); //substring is used to split the data in hour and minute form
    hr1 = hr.toInt();// toInt is used to convert string into integer
    minut = new_time.substring(3, 5);
    minut1 = minut.toInt();


    if (hr1 > 24 || minut1 > 60) {
      webServer.send(200, "text/html", error);  //when alarm time incorrect
    }

    else {
      webServer.send(200, "text/html", confirmation);  //when alarm time entered direct user to Confirmaton page
      vibrating = 1;
      EEPROM.put(0, hr1);
      EEPROM.put(4, minut1);
      EEPROM.commit();
    }
  });

  webServer.on("/error.php", []() {
    webServer.send(200, "text/html", error);  //when alarm time entered direct user to Confirmaton page
  });

  webServer.begin();
  timeClient.begin();
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(reed, INPUT_PULLUP);
  digitalWrite(LED_BUILTIN, HIGH);   // Arduino: turn the LED on (HIGH)
}

void loop() {
  timeClient.update();
  webServer.handleClient();
  BUTTONstate = digitalRead(reed);

  int EEPROMhr1;
  int EEPROMminut1;
  EEPROM.get(0, EEPROMhr1);
  EEPROM.get(4, EEPROMminut1);

  Serial.print("Heure actuelle : ");
  Serial.println(timeClient.getFormattedTime());
  Serial.print("Heure du reveil : ");
  Serial.print(EEPROMhr1);
  Serial.print(":");
  Serial.println(EEPROMminut1);

  if (EEPROMhr1 == timeClient.getHours() && EEPROMminut1 <= timeClient.getMinutes() && vibrating == 1) {
    digitalWrite(LED_BUILTIN, LOW);   // Arduino: turn the LED on (HIGH)
    Serial.println("VIBRATE");
    delay(vibrate_time);
    digitalWrite(LED_BUILTIN, HIGH);   // Arduino: turn the LED on (HIGH)
    vibrating = 0;
  }


  if (EEPROMhr1 == timeClient.getHours() && EEPROMminut1 <= timeClient.getMinutes() && vibrating == 1) {
    digitalWrite(LED_BUILTIN, LOW);   // Arduino: turn the LED on (HIGH)
    Serial.println("VIBRATE");
    delay(vibrate_time);
    digitalWrite(LED_BUILTIN, HIGH);   // Arduino: turn the LED on (HIGH)
    vibrating = 0;
  }

  delay(1000);

  if (BUTTONstate == LOW) {
    ESP.deepSleep( 60 * 1000000 );
  }

}
