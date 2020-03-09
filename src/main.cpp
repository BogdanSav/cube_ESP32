#include <Arduino.h>
#include <WiFi.h>

#include<Ticker.h>
#include<ArduinoJson.h>
#include<WebSocketsClient.h>
#include<SocketIOclient.h>
#include<cubeControl.h>
#include<typeinfo>
//defines
//
//
//variables
//
int first;
int second;
const char* message;

float force=0;
//const char* ssid ="ChNU-WiFi-NET"; 
const char* ssid ="WR-Sydor5";
const char* password="IRENA1978";
//
//classes
//
WebSocketsClient web_sockets;
cubeControl cb;
//
//functions
//
void hexdump(const void *mem, uint32_t len, uint8_t cols);
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length);
//
//

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid,password);
  while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  
  cb.init();
 
 
  
  web_sockets.beginSSL("robo-dot.akit.pp.ua", 8999,"/");
	//web_sockets.begin("robo-dot.akit.pp.ua",8999,"/");
    web_sockets.onEvent(webSocketEvent);
    web_sockets.setReconnectInterval(5000);

  

}


void loop() {
  StaticJsonDocument<2048> doc;
	DeserializationError root = deserializeJson(doc,message);

	//Serial.println(root.c_str());

 
	force = doc["force"];
 	float angle =float(doc["angle"]["degree"]);
//   const char* event = doc["event"];
//   Serial.println(event);
	
  
// Serial.println(typeid(angle).name());
//Serial.println(force);
  if(force>=0.5){
	  if((angle>=45.0)&&(angle<=135.0)){
		 cb.moveFoward();
		//  Serial.println("FOw"); 
	  }
	  else if ((angle>=135.0)&&(angle<=225.0))
	  {
		  cb.moveLeft();
		//   Serial.println("left");
	  }
	  else if ((angle>=225.0)&&(angle<=315.0))
	  {
		  cb.moveBackward();
		//   Serial.println("Back");
	  }
	  else if ((angle>=315.0)||(angle<=45.0))
	  {
		  cb.moveRight();
		//   Serial.println("right");
	  }
	 
	  
	
  }
  else{
	  	cb.stop();
		// Serial.println("Stop");
  }
 
  web_sockets.loop();
  
}
void hexdump(const void *mem, uint32_t len, uint8_t cols = 16) {
	const uint8_t* src = (const uint8_t*) mem;
	Serial.printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
	for(uint32_t i = 0; i < len; i++) {
		if(i % cols == 0) {
			Serial.printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
		}
		Serial.printf("%02X ", *src);
		src++;
	}
	Serial.printf("\n");
}
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

	switch(type) {
		case WStype_DISCONNECTED:
			Serial.printf("[WSc] Disconnected!\n");
			break;
		case WStype_CONNECTED:
			Serial.printf("[WSc] Connected to url: %s\n", payload);

			// send message to server when Connected
			//  web_sockets.sendTXT("{\"event\":\"cube_connect_request\",\"data\":\"cube3\"}");
			break;
		case WStype_TEXT:
			// Serial.printf("[WSc] get text: %s\n", payload);
			message = (char*)payload;
			// send message to server
			// web_sockets.sendTXT("message here");
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