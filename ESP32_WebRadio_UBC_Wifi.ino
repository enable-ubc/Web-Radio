#include <VS1053.h>  //https://github.com/baldram/ESP_VS1053_Library
#include <WiFi.h>
#include <HTTPClient.h>
#include <esp_wifi.h>
#include "esp_wpa2.h" 
#define buffer_size 32
#define EAP_IDENTITY "wifi_username" //login //if connecting from another corporation, use identity@organisation.domain in Eduroam 
#define EAP_PASSWORD "wifi_password" //pass //your Eduroam or School password
const char* ssid = "ubcsecure"; // Eduroam/UBC WIFI SSID (name of Wifi connection)
int counter = 0;
#define VS1053_CS    32 
#define VS1053_DCS   33  
#define VS1053_DREQ  35 
#define VOLUME  95 // volume level 0-100
// Few Radio Stations
//const char *host = "radiostreaming.ert.gr";
//const char *path = "/ert-kosmos";
int port[] = {80,1,8,254,21, 8062,0 ,1953, 2545, 2565, 443, 53, 5353, 25565,56535,9328,8266,8371,8297,8000,8226};   //21 ports
//const char *host = "cms.stream.publicradio.org";
//const char *path = "/cms.mp3";
const char *host = "ice24.securenetsystems.net";
const char *path = "/CKYE?playSessionID=3968E480-CC34-5807-610F84475F27F4DD";



//int port[4] = {8062,80,80,80};
//const char *host = "realfm.live24.gr";
//const char *path = "/realfm";
int portNum = 0;
int status = WL_IDLE_STATUS;
WiFiClient  client;
uint8_t mp3buff[buffer_size];   // vs1053 likes 32 bytes at a time
VS1053 player(VS1053_CS, VS1053_DCS, VS1053_DREQ);
void initMP3Decoder()
  {
    player.begin();
    player.switchToMp3Mode(); // optional, some boards require this
    player.setVolume(VOLUME);
  }
  
void connectToWIFI()
  {
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.disconnect(true);  //disconnect form wifi to set new wifi connection
    WiFi.mode(WIFI_STA); //init wifi mode
    esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY)); //provide identity
    esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY)); //provide username --> identity and username is same
    esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD)); //provide password
    esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT(); //set config settings to default
    esp_wifi_sta_wpa2_ent_enable(&config); //set config settings to enable function
    WiFi.begin(ssid); //connect to wifi
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
      counter++;
      if(counter>=60){ //after 30 seconds timeout - reset board
        ESP.restart();
      }
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address set: "); 
    Serial.println(WiFi.localIP()); //print LAN IP
//
//    Serial.println("");
//    Serial.println("WiFi connected.");
//    Serial.println("IP address: ");
//    Serial.println(WiFi.localIP());
//    Serial.println("connectToWifi loop");
  }
void IRAM_ATTR resetModule(){
    ets_printf("reboot\n");
    esp_restart();
}
void station_connect () {
  
    while (portNum<21){
      if (client.connect(host,port[portNum])) {
        Serial.println("Connected now to port ");
        Serial.println(port[portNum]);
        break;
      } else { 
      Serial.println();
      Serial.print("Trying Port: ");
      Serial.println(port[portNum]);
      //Serial.rp
      Serial.println(WiFi.localIP());
      delay(500);
      portNum++;
      }    
    }
    
    client.print(String("GET ") + path + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");  
//    const char *hardpath = "radiostreaming.ert.gr/ert-kosmos";
//    client.print(String(hardpath)); 
    Serial.println(String("GET ") + path + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n"); 
    Serial.println(host); 
    Serial.println(path);
  }
  
void setup () {
   Serial.begin(9600);
   delay(500);
   SPI.begin();
   initMP3Decoder();
   connectToWIFI();
   delay(500);
   station_connect();
}
void loop() {
//    station_connect();
//  Serial.println("client_avaiable");
  if (client.available() > 0) {
//        Serial.println();
//        Serial.println("InitMP3Decoder");
//        initMP3Decoder();
//        Serial.println("Initi Complete");
//        Serial.println("Start Playin");
        uint8_t bytesread = client.read(mp3buff, buffer_size);
        player.playChunk(mp3buff, bytesread);
      }
  else Serial.println("client not available");
}    
