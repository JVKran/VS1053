#include <WiFi.h>
#include <VS1053.h>

const char *host = "icecast.omroep.nl";
const char *path = "/radio1-bb-mp3";
int httpPort = 80;
uint8_t mp3buff[128];
int lastUpdate = 0;

const char* ssid     = "SSID";
const char* password = "WPA2";
WiFiClient client;

VS1053 player = VS1053(5, 16, 4);

void connect(){
	WiFi.mode(WIFI_STA);
	WiFi.setHostname("Radio");
	WiFi.begin(ssid, password);
	
	while (WiFi.status() != WL_CONNECTED) {
		delay(50);
	}

	Serial.println("Connected!");
}

void setup() {
	connect();

	SPI.begin();
	Serial.begin(9600);
	player.begin();

	player.switchToMp3Mode();
    player.setVolume(90);
    player.setBass(4);
    player.setTreble(0);

    client.print(String("GET ") + path + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");

	Serial.println(String("GET ") + path + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
}

void loop() {
	if (!client.connected()) {
        Serial.println("Reconnecting...");
        if (client.connect(host, httpPort)) {
            client.print(String("GET ") + path + " HTTP/1.1\r\n" +
                         "Host: " + host + "\r\n" +
                         "Connection: close\r\n\r\n");
            Serial.println(String("GET ") + path + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
        }
    }

    if (client.available() > 0) {
        uint8_t bytesread = client.read(mp3buff, 128);
        player.playChunk(mp3buff, bytesread);
    }

    if(millis() - lastUpdate > 1000){
    	lastUpdate = millis();
    	Serial.println(player.getSeconds());
    }
}