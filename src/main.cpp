#include <Arduino.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <FS.h>
#include <WebServer.h>
#include <SPI.h>
#include <LedMatrix.h>
#include<Ticker.h>
//defines
//
#define NUMBER_OF_DEVICES 1
#define CS_PIN 27
#define wheel_r_1 18
#define wheel_r_2 19
#define wheel_l_1 32
#define wheel_l_2 33
//
//variables
//
int first;
int second;
//bool rst  =true;
//
//classes
//
LedMatrix ledMatrix = LedMatrix(NUMBER_OF_DEVICES,CS_PIN);
WebServer server(80);
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


void setup() {
  WiFi.softAP("Cube_Controls"); //wifi enable
  SPIFFS.begin();
  tr.start();
  tl.start();
  ledMatrix.init();
  ledMatrix.commit();
  ledMatrix.clear();

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


  server.begin();
}


void loop() {
  server.handleClient();
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

String getContentType(String filename) { // convert the file extension to the MIME type
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
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
}