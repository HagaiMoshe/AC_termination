#include <ESP8266WiFi.h>

//consts

const char* SSID_NAME = "";
const char* SSID_PASS = "";
const int LISTEN_PORT = 80;
const int RELAY_PIN = 3;
const int LOOP_DELAY = 500; //in ms
const unsigned int periodicResetTime = 86400000;



// assamble the server page string - this is the http code to send to the client
const String HEADER_1     = "HTTP/1.1 200 OK\n";
const String HEADER_2     = "Content-Type: text/html\n";
const String HEADER_3     = "Connnection: close\n";
const String HEADER_4     = "<!DOCTYPE HTML>\n";
const String HEADER_5     = "\n<HTML>\n<HEAD>\n";
const String SERVER_STYLE = "<link rel='stylesheet' type='text/css' href='http://randomnerdtutorials.com/ethernetcss.css' />\n";
const String BACKGROUNG_1 = "<body background=";
const String BACKGROUNG_2 = "http://i.redd.it/ne990adh7ld61.jpg"; 
const String BACKGROUNG_3 = " dir=\"rtl\">\n";
const String BACKGROUNG_4 = "</HEAD>\n<BODY>\n";
const String TITLE_H1     = "<font face=\"Courier\" size=\"10\" color=\"blue\"><H1>AC TERMINATION CONTROL</H1></font>\n";
const String TITLE_H2     = "<font size=\"5\" color=\"white\"><H2>?Is it cold in here, or is just me</H2></font>\n";
const String BUTTON_ON    = "<a {font-size: 150px} href=\"/?button1on\"  style=\"color:red\" >PUSH THE BUTTON</a>\n";
const String BUTTON_OFF   = "<a {font-size: 150px} href=\"/?button1off\" style=\"color:orange\">RELEASE THE BUTTON</a>\n";   
const String FOOTER       = "</BODY>\n</HTML>";

const String STATUS_ON    = "<p><font size=\"6\" color=\"red\" >button pressed</font></p>";
const String STATUS_OFF   = "<p><font size=\"6\" color=\"orange\">button released</font></p>";


const String response_on = HEADER_1 + HEADER_2 + HEADER_3 + HEADER_4 + HEADER_5 +
                                   SERVER_STYLE + BACKGROUNG_1 + BACKGROUNG_2 + BACKGROUNG_3 + 
                                   BACKGROUNG_4 + TITLE_H1 + TITLE_H2 + BUTTON_ON + BUTTON_OFF +
                                   STATUS_ON + FOOTER;

const String response_off = HEADER_1 + HEADER_2 + HEADER_3 + HEADER_4 + HEADER_5 +
                                   SERVER_STYLE + BACKGROUNG_1 + BACKGROUNG_2 + BACKGROUNG_3 + 
                                   BACKGROUNG_4 + TITLE_H1 + TITLE_H2 + BUTTON_ON + BUTTON_OFF +
                                   STATUS_OFF + FOOTER;


// -----------------------------------------------------------------
WiFiServer server(LISTEN_PORT);

void HandleRequest(WiFiClient& client, String &request){
  static int stat = LOW; // this is the status of the relay

  // find the type of the request
  if(request.indexOf("GET /?button1on HTTP" ) >=0) // button set to on
    stat = HIGH;
  else if(request.indexOf("GET /?button1off HTTP") >=0) // button set to off
    stat = LOW;
  else if (request.indexOf("GET / HTTP") >=0) // empty GET - no button set
    {}
  else { // any other request will not be handled - stoping client
    client.stop();
    return;
  }
 
  digitalWrite(RELAY_PIN, stat); // toggeling the relay's pin
  Serial.print("pin status ");
  Serial.println(stat);

  if(stat)// sending response to client
    client.println(response_on);
  else
    client.println(response_off);

  client.stop(); // ending communication
}//HandleRequest


bool connectToWifi(void)
{
  WiFi.begin(SSID_NAME, SSID_PASS);
  int timeout_itr = 30;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (timeout_itr-- < 1){
      delay(500);
      return false;
    }
  }
  Serial.println("\nWiFi connected");
  return true;
}

void setup(void)
{  
  pinMode(RELAY_PIN, INPUT);
 
  // Start Serial
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
  delay(1000);
  Serial.println("bootup");
  
  if(connectToWifi() == false)
    ESP.restart();
 
  // Start the server
  server.begin();
  Serial.println("Server started");
  
  // Print the IP address
  Serial.println(WiFi.localIP());

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
}// setup




void loop() {
  WiFiClient client = server.available();
  static String readString = "";
  static String request = "";

  delay(LOOP_DELAY);

  if(WiFi.status() != WL_CONNECTED)
    connectToWifi();

  if(millis() > periodicResetTime) //reset the board every 24 hours
    ESP.restart();

  if (client){
    while (client.connected()) {
      if (client.available()) {
        
        //read line HTTP request
        char c = client.read();
        readString += c;
        if(c == '\n'){
          request += readString;
          
          if(readString == "\r\n"){
            //Serial.println(request);
            HandleRequest(client, request);
            request = "";
          }
            
          readString="";
        }// if(c == '\n')
      }// if(client.available())
    }// while
  }// if(client)
}// loop
