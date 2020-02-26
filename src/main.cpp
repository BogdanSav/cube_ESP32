#include <Arduino.h>
#include <WiFi.h>
//#include <SPIFFS.h>
//#include <FS.h>
//#include <WebServer.h>
#include <SPI.h>
#include <LedMatrix.h>
#include<Ticker.h>
#include<ArduinoJson.h>
#include<WebSocketsClient.h>
#include<SocketIOclient.h>
//defines
//
#define NUMBER_OF_DEVICES 1
#define CS_PIN 27
#define wheel_r_1 32
#define wheel_r_2 33
#define wheel_l_1 18
#define wheel_l_2 19
//
//variables
//
int first;
int second;
const char* message;
const char* ssid ="ChNU-WiFi-NET";
//bool rst  =true;
//
//classes
//
WebSocketsClient web_sockets;
LedMatrix ledMatrix = LedMatrix(NUMBER_OF_DEVICES,CS_PIN);
//WebServer server(80);
//
//functions
//
String getContentType(String filename);
bool handleFileRead(String path);
String setpixel(int first, int second);
String clearpixel();
String clearone(int first, int second);
String move_fow();
String move_back();
String move_left();
String move_right();
String inv_left();
String inv_right();
String move_fow_left();
String move_fow_right();
//String move_back_left();
//String move_back_right();
String _stop();
void t_left();
void t_right();
void inv_t_right();
void inv_t_left();
Ticker tr(t_right, 100);
Ticker tl(t_left, 100);

//
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
			web_sockets.sendTXT("Connected");
			break;
		case WStype_TEXT:
		//	Serial.printf("[WSc] get text: %s\n", payload);
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

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid);
   while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  //SPIFFS.begin();
  
  tr.start();
  tl.start();
  ledMatrix.init();
  ledMatrix.clear();
  ledMatrix.commit();
  pinMode(wheel_l_1,OUTPUT);
  pinMode(wheel_l_2,OUTPUT);
  pinMode(wheel_r_1,OUTPUT);
  pinMode(wheel_r_2,OUTPUT);
   web_sockets.beginSSL("robo-dot.akit.pp.ua", 8999,"/");
    web_sockets.onEvent(webSocketEvent);
    web_sockets.setReconnectInterval(5000);

  
/*
   server.on("/sended", []()
  {
    server.send(200, "text/html", "message recived");
    first  = server.arg("first").toInt();
    second = server.arg("second").toInt();

    setpixel(second, first);
  });
  server.on("/clean", []()
  {

    server.send(200, "text/html", clearpixel());

  });
  server.on("/clearone", []()
  {
    first  = server.arg("first").toInt();
    second = server.arg("second").toInt();
    server.send(200, "text/html", clearone(second, first));

  });
  server.on("/fow", []() {
    server.send(200, "text/html", move_fow());

  });
  server.on("/back", []() {
    server.send(200, "text/html", move_back());

  });
  server.on("/left", []() {
    server.send(200, "text/html", move_left());

  });
  server.on("/right", []() {
    server.send(200, "text/html", move_right());

  });
  server.on("/stop", []() {
    server.send(200, "text/html", _stop());

  });
  server.on("/fow-right", []() {
    server.send(200, "text/html", move_fow_right());

  });
  server.on("/fow-left", []() {
    server.send(200, "text/html", move_fow_left());

  });



  server.onNotFound([](){
    if(!handleFileRead(server.uri())){
      server.send(404,"plain/text","not found");
    }
  });


  server.begin();*/
}


void loop() {
  StaticJsonDocument<2048> doc;
	DeserializationError root = deserializeJson(doc,message);

	//Serial.println(root.c_str());
  float angle = doc["angle"]["degree"];
  float force = doc["force"];
Serial.println(angle);
Serial.println(force);
  if(force>=0.5){
    if(45.0<angle<135){
      move_fow();
    }
    else if(135<angle<225){
      move_left();
    }
    else if(225<angle<315){
      move_back();
    }
    else if(angle<45&&315<angle){
      move_right();
    }
  }
 // delay(1000);
  web_sockets.loop();
  //server.handleClient();
}

String setpixel(int first, int second)
{


  ledMatrix.setPixel(first, second);
  ledMatrix.commit();
  return "done";
  Serial.println("pixel set");


}
String clearpixel()
{
  ledMatrix.clear();
  ledMatrix.commit();
  Serial.println("allclear  ");
  return "done";
}
String clearone(int first, int second)
{
  Serial.println(first);
  Serial.println(second);
  ledMatrix.clearPixel(first, second);
  ledMatrix.commit();
  Serial.println("pixel clear");
  return "done";

}

String move_fow()
{
  digitalWrite(wheel_r_1, HIGH);
  digitalWrite(wheel_r_2, LOW);
  digitalWrite(wheel_l_1, HIGH);
  digitalWrite(wheel_l_2, LOW);
  Serial.println("fow");
  return "fow";

}
String move_back()
{
  digitalWrite(wheel_r_1, LOW);
  digitalWrite(wheel_r_2, HIGH);
  digitalWrite(wheel_l_1, LOW);
  digitalWrite(wheel_l_2, HIGH);
  Serial.println("Back");
  return "back";
}
String move_left()
{
  digitalWrite(wheel_r_1, HIGH);
  digitalWrite(wheel_r_2, LOW);
  digitalWrite(wheel_l_1, LOW);
  digitalWrite(wheel_l_2, LOW);
  Serial.println("left");
  return "left";
}
String move_right()
{
  digitalWrite(wheel_r_1, LOW);
  digitalWrite(wheel_r_2, LOW);
  digitalWrite(wheel_l_1, HIGH);
  digitalWrite(wheel_l_2, LOW);
  Serial.println("right");
  return "right";
}
String _stop()
{
  digitalWrite(wheel_r_1, LOW);
  digitalWrite(wheel_r_2, LOW);
  digitalWrite(wheel_l_1, LOW);
  digitalWrite(wheel_l_2, LOW);
  return "stop";



}
String inv_left()
{
  digitalWrite(wheel_r_1, LOW);
  digitalWrite(wheel_r_2, HIGH);
  digitalWrite(wheel_l_1, LOW);
  digitalWrite(wheel_l_2, LOW);
  Serial.println("left");
  return"ileft";
}
String inv_right()
{
  digitalWrite(wheel_r_1, LOW);
  digitalWrite(wheel_r_2, LOW);
  digitalWrite(wheel_l_1, LOW);
  digitalWrite(wheel_l_2, HIGH);

  return "iright";


}

String move_fow_left()
{
  move_left();
  tr.update();

  //t_right();
  return "done";

}
String move_fow_right()
{
  move_right();
  tl.update();
 // t_left();
  return "done";

}
void t_left() {
  move_left();
}
void t_right() {
  move_right();
}
/*
String getContentType(String filename) { // convert the file extension to the MIME type
  if (filename.endsWith(".htm")) return "text/html";
  else if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".json")) return "application/json";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".png")) return "image/png";
  else if (filename.endsWith(".gif")) return "image/gif";
  else if (filename.endsWith(".jpg")) return "image/jpeg";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".xml")) return "text/xml";
  else if (filename.endsWith(".pdf")) return "application/x-pdf";
  else if (filename.endsWith(".zip")) return "application/x-zip";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool handleFileRead(String path) { // send the right file to the client (if it exists)
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.html";         // If a folder is requested, send the index file
  String contentType = getContentType(path);            // Get the MIME type
  if (SPIFFS.exists(path)) {                            // If the file exists
    File file = SPIFFS.open(path, "r");                 // Open it
    size_t sent = server.streamFile(file, contentType); // And send it to the client
    file.close();                                       // Then close the file again
    return true;
  }
  Serial.println("\tFile Not Found");
  return false;                                         // If the file doesn't exist, return false
}*/