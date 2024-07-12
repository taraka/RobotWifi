#include <WiFi.h>
#include <WebServer.h>

const char *ssid = "ssid";
const char *password = "password";

WebServer server(80);

int leftMotorSpeed;
int rightMotorSpeed;
int left_motor_direction;
int right_motor_direction;
int adjustedRightSpeed;
int adjustedLeftSpeed;

float speedTrim = 0.0;




void setup() {
  Serial.begin(115200);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(26, OUTPUT);
  pinMode(33, OUTPUT);


  pinMode(34, INPUT);
  delay(10);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  
  server.on("/set", set_state);
  server.on("/get", get_state);
  server.onNotFound(handle_NotFound);
  
  server.begin();
  Serial.println("HTTP server started");

}

void loop(){
  server.handleClient();
  speedTrim = float(analogRead(34));
  
  // Serial.print("Turn pin 34:");
  // Serial.println(analogRead(34));

  // delay(1000);
  float trimPercent = (speedTrim - 2048) / 2048.0;
  
  adjustedRightSpeed = rightMotorSpeed;
  adjustedLeftSpeed = leftMotorSpeed;
  
  if (trimPercent > 0) {
    adjustedLeftSpeed = leftMotorSpeed * (1.0 - trimPercent);
  } else {
    adjustedRightSpeed = rightMotorSpeed * (1.0 + trimPercent);
  }

  analogWrite(12, adjustedLeftSpeed);
  analogWrite(13, adjustedRightSpeed);
  digitalWrite(26, left_motor_direction);
  digitalWrite(33, right_motor_direction);
}

void set_state() {
  leftMotorSpeed = server.hasArg("ls") ? server.arg("ls").toInt() : 0;
  rightMotorSpeed = server.hasArg("rs") ? server.arg("rs").toInt() : 0;
  left_motor_direction = (server.hasArg("ld") && server.arg("ld") == "f") ? LOW : HIGH;
  right_motor_direction = (server.hasArg("rd") && server.arg("rd") == "f") ? HIGH : LOW;
  server.send(200, "text/plain", "Request received!!");
}

void get_state() {
  server.send(200, "text/plain", "speed trim %: " + String((speedTrim - 2048) / 2048.0) + "speed trim: " + String(speedTrim) + " left: " + String(adjustedLeftSpeed) + " right: " + String(adjustedRightSpeed) + " pre left: " + String(leftMotorSpeed) + " pre right: " + String(rightMotorSpeed));
}



void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}
