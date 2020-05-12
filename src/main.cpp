#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <Ticker.h>
#include <ArduinoJson.h>
#include <WebSocketsClient.h>
#include <SocketIOclient.h>
#include <cubeControl.h>
#include <string>

//
//variables
//
int first;
int second;
const char *message;
bool connection_type = false;
const double pi = 3.14159;
String cubeId = "cube16";
//
//classes
//

WebSocketsClient web_sockets;
cubeControl cube;
WiFiMulti mwifi;

//
//functions
//
void hexdump(const void *mem, uint32_t len, uint8_t cols);
void webSocketEvent(WStype_t type, uint8_t *payload, size_t length);
//
//

void setup()
{
	Serial.begin(115200);
	mwifi.addAP("WR-Sydor5", "IRENA1978");
	mwifi.addAP("Lab_101", "LaB_1010");
	mwifi.addAP("Lab124");
	mwifi.addAP("S16", "12345678");
	while (mwifi.run() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}
	/*
  WiFi.begin(ssid);
  while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
*/
	Serial.println("");
	Serial.println("WiFi connected");
	Serial.println(WiFi.SSID());
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());

	cube.init();
	cube.sensorsInit();

	if (connection_type)
	{
		web_sockets.begin("robo-dot.akit.pp.ua", 5050, "/");
	}
	else
		web_sockets.beginSSL("robo-dot.akit.pp.ua", 5050, "/");

	web_sockets.onEvent(webSocketEvent);
	web_sockets.setReconnectInterval(5000);
}

void loop()
{
	StaticJsonDocument<2048> doc;
	DeserializationError root = deserializeJson(doc, message);
	String event = doc["event"];
	String direction = doc["data"]["command"];
	String id = doc["data"]["cubeId"];
	int x = doc["data"]["x"];
	int y = doc["data"]["y"];
	String state = doc["data"]["state"];

	if (id == "cube16")
	{
		if (event == "move_response")
		{
			if (direction == "moveFront")
			{
				cube.moveFoward();
			}
			else if (direction == "moveBack")
			{
				cube.moveBackward();
			}
			else if (direction == "turnLeft")
			{
				cube.moveLeft();
			}
			else if (direction == "turnRight")
			{
				cube.moveRight();
			}
			else
				cube.stop();
			if(cube.hc_sr04() <=8 && direction=="moveFront")
			{
				cube.stop();
			}
		}
		if (event == "set_pixel_response")
		{
			if (state == "true")
			{

				cube.setPixel(y, x);
			}
			else if (state == "false")
			{

				cube.clearOnePixel(y, x);
			}
		}
		if (event == "clear_matrix_response" || event == "user_cube_break_conn_response")
		{
			cube.clearPixel();
		}
		
	}
	web_sockets.loop();
}
void hexdump(const void *mem, uint32_t len, uint8_t cols = 16)
{
	const uint8_t *src = (const uint8_t *)mem;
	Serial.printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
	for (uint32_t i = 0; i < len; i++)
	{
		if (i % cols == 0)
		{
			Serial.printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
		}
		Serial.printf("%02X ", *src);
		src++;
	}
	Serial.printf("\n");
}
void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{

	switch (type)
	{
	case WStype_DISCONNECTED:
		Serial.printf("[WSc] Disconnected!\n");
		break;
	case WStype_CONNECTED:
		Serial.printf("[WSc] Connected to url: %s\n", payload);
		web_sockets.sendTXT("{\"event\":\"cube_connect_request\",\"data\": \""+cubeId+"\"}");
		// send message to server when Connected
		//  web_sockets.sendTXT("{\"event\":\"cube_connect_request\",\"data\":\"cube3\"}");
		break;
	case WStype_TEXT:
		Serial.printf("[WSc] get text: %s\n", payload);
		message = (char *)payload;
		// send message to server

		break;
	case WStype_BIN:
		Serial.printf("[WSc] get binary length: %u\n", length);
		hexdump(payload, length);

		// send data to server
		// web_sockets.sendBIN(payload, length);
		break;
	case WStype_ERROR:
	case WStype_FRAGMENT_TEXT_START:
	case WStype_FRAGMENT_BIN_START:
	case WStype_FRAGMENT:
	case WStype_FRAGMENT_FIN:
		break;
	}
}