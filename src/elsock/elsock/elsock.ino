#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>

String ports[] = {
  "nul", "one", "two", 
  "thr", "fou", "fiv", 
  "six", "sev", "eig", "nin"
};

byte status[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

int portnum = 10;
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 
  0xEF, 0xFE, 0xED 
};

IPAddress fallback_ip(141,60,125,4);

IPAddress fallback_gateway(141,60,125,1);
IPAddress fallback_subnet(255,255,255,0);

String current_ip;

File myFile;
EthernetServer server(80);

void setup(){
  /* configure pins */
  for(int i = 0; i < portnum; i++){
    pinMode(i, OUTPUT);
  }
  
  /* configure the serial connection */
  Serial.begin(9600);
  
  Serial.println("[setup()] Begin SD init ...");
  pinMode(10, OUTPUT);
  if (!SD.begin(4)){
    Serial.println("[setup()] SD init failed");
    exit();
  }
  Serial.println("[setup()] finished!");

  /* Ethernet config */
  Serial.println("[setup()] Trying to query configuration by using DHCP");
  if (Ethernet.begin(mac) == 0){
    Serial.println("[setup()] Failed to get DHCP-IP address");
    Serial.println("... using fallback configuration instead");
    Ethernet.begin(mac, fallback_ip, fallback_gateway, fallback_subnet);
  }

  fallback_ip = Ethernet.localIP();

  /* set the current ip */
  current_ip = String("");
  for (byte b = 0; b < 4; b ++){
    current_ip += String(fallback_ip[b], DEC);
    if (b != 3 ) {
      current_ip += ".";
    }
  }
  
  exit();
  Serial.print("[setup()] current IP: ");
  Serial.println(current_ip);

  server.begin();
}

void exit(){
  Serial.println("EXIT");
  while(true);
}

void loop(){
  /* like the accept() library function from the BSD-Socketlib */
  EthernetClient client = server.available();
  String path = String("");

  if(client){
    String line = get_line(client);
    Serial.println(line);
    if (line.charAt(0) == 'G' && line.charAt(1) == 'E' && line.charAt(2) == 'T'){
      if (line.charAt(4) == '/'){

        int index = 5;
        char temp;
        /* Get the Path of the request */
        while((temp = line.charAt(index++)) != ' '){
          path += temp;
        }
      }
    }

    Serial.print("[server] path: ");
    Serial.print('/'); 
    Serial.println(path);

    if(!path.equals("")){
      Serial.println("[server] sending webpage ...");
      /* static content */
      myFile = SD.open("index.htm");
      if(myFile){
        while(myFile.available()){
          client.println(myFile.read());
        }
        myFile.close();
      } 
      else {
        Serial.println("[server] Error opening index.css");
      }
      /* sending dynamic content */
      send_webpage(client);
    } 
    else if (!path.equals("main.css")) {
      Serial.println("[server] sending css ...");

      /* send main.css to client */
      myFile = SD.open("main.css");
      if(myFile){
        while(myFile.available()){
          client.println(myFile.read());
        }
        myFile.close();
      } 
      else {
        Serial.println("[server] Error opening main.css");
      }

    } 
    else {

      for (int i = 0; i < portnum; i++){
        if(!path.compareTo(ports[i])){
          /* Port found! */

          if(status[i]){
            digitalWrite(i, LOW);
          } 
          else {
            digitalWrite(i, HIGH);
          }

          status[i] = !status[i];

          break;
        }
      }
    }

    client.stop();
  }
}

String get_line(EthernetClient c){
  String result = String("");
  char temp;
  while ((temp = c.read()) != '\n'){
    result += temp;
  }

  /* debug */
  Serial.print("[server:get_line()] ");
  Serial.println(result);

  return result;
}

void send_webpage(EthernetClient c){
  c.print("<p>");

  /* Dynamic content */
  for (int i = 1; i < 4; i++){
    c.print("<a class=\"button-link\" href=\"");
    c.print("http://");
    c.print(current_ip);
    c.print("/");
    c.print(ports[i]);
    c.print("\">Socket 1</a>");
  }
  c.print("<br></br></p><p>");

  for (int i = 4; i < 7; i++){
    c.print("<a class=\"button-link\" href=\"");
    c.print("http://");
    c.print(current_ip);
    c.print("/");
    c.print(ports[i]);
    c.print("\">Socket 1</a>");
  }
  c.print("<br></br></p><p>");

  for (int i = 7; i < 10; i++){
    c.print( "<a class=\"button-link\" href=\"");
    c.print("http://");
    c.print(current_ip);
    c.print("/");
    c.print(ports[i]);
    c.print("\">Socket 1</a>");
  }

  c.print("</p></body></html>");
}



