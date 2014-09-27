#include <SPI.h>
#include <Ethernet.h>

String ports[] = {
  "null", "one", "two", 
  "three", "four", "five", 
  "six", "seven", "eight", "nine"
};

int status[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int portnum = 10;
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 
  0xEF, 0xFE, 0xED 
};

IPAddress fallback_ip(141,60,125,4);
IPAddress fallback_gateway(141,60,125,1);
IPAddress fallback_subnet(255,255,255,0);
String current_ip;

EthernetServer server(80);
boolean rcvd_msg = false;

void setup(){
  /* configure pins */
  for(int i = 0; i < portnum; i++){
    pinMode(i, OUTPUT);
  }
  
  /* configure the serial connection */
  Serial.begin(9600);

  Serial.println("[setup()] Trying to query configuration by using DHCP");
  if (Ethernet.begin(mac) == 0){
    Serial.println("[setup()] Failed to get DHCP-IP address");
    Serial.println("... using fallback configuration instead");
    Ethernet.begin(mac, fallback_ip, fallback_gateway, fallback_subnet);
  }
  
  Serial.print("[setup()] IP-address: ");
  IPAddress ip = Ethernet.localIP();
  for (byte thisByte = 0; thisByte < 4; thisByte++){
    Serial.print(ip[thisByte], DEC);
    Serial.print(".");
  }
  Serial.println();
  
  /* set the current ip */
  current_ip = String("");
  for (byte b = 0; b < 4; b ++){
    current_ip += String(ip[b], DEC);
    if (b != 3 ) {
      current_ip += ".";
    }
  }
    
  server.begin();
}

void loop(){
  /* like the accept() library function from the BSD-Socketlib */
  EthernetClient client = server.available();
  String path = "";
  
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
    Serial.print('/'); Serial.println(path);
    
    if(!path.equals("")){
      Serial.println("[server] sending webpage ...");
      client.println(generate_webpage());
    } else if (!path.equals("main.css")) {
      Serial.println("[server] sending css ...");
      client.println(generate_css());
    } else {
    
      for (int i = 0; i < portnum; i++){
        if(!path.compareTo(ports[i])){
          /* Port found! */
        
          if(status[i]){
            digitalWrite(i, LOW);
          } else {
            digitalWrite(i, HIGH);
          }
        
          status[i] = !status[i];

          break;
        }
      }
    }
    
    /* collect the irrelevant headers until the double \n is reached 
        Attention: length() also counts the null byte at the end of the string! 
    while (get_line(client).length() != 1);
    Serial.println("[server] Disconnecting from client...");
    */
    client.stop();
  }
}

String get_line(EthernetClient c){
  String result = "";
  char temp;
  while ((temp = c.read()) != '\n'){
    result += temp;
  }
  
  /* debug */
  Serial.print("[server:get_line()] ");
  Serial.println(result);

  return result;
}

String generate_css(){
  String css = String("");

  css += ".aesock a {    border-bottom: 1px solid #777777;  border-left: 1px solid #000000;  border-right: 1px solid #333333;border-top: 1px solid #000000;color: #000000;display: block;height: 2.5em;padding: 0 1em;width: 5em;       text-decoration: none; }";   
  css += ".aesock a:hover {    border-bottom: 1px solid #777777;   border-left: 1px solid #000000; border-right: 1px solid #333333; border-top: 1px solid #000000;color: #f0f;display: block;    height: 2.5em;padding: 0 1em;width: 5em;       text-decoration: none;}";
  css += ".aesock a:active {    border-bottom: 1px solid #777777;    border-left: 1px solid #000000;   border-right: 1px solid #333333;    border-top: 1px solid #000000;    color: #f00;    display: block;    height: 2.5em;    padding: 0 1em;    width: 5em;       text-decoration: none; }";
  css += ".button-link {  padding: 10px 15px;  background: #4479BA;  color: #FFF;  -webkit-border-radius: 4px;  -moz-border-radius: 4px;   border-radius: 4px;   border: solid 1px #20538D;   text-shadow: 0 -1px 0 rgba(0, 0, 0, 0.4);  -webkit-box-shadow: inset 0 1px 0 rgba(255, 255, 255, 0.4), 0 1px 1px rgba(0, 0, 0, 0.2);   -moz-box-shadow: inset 0 1px 0 rgba(255, 255, 255, 0.4), 0 1px 1px rgba(0, 0, 0, 0.2);   box-shadow: inset 0 1px 0 rgba(255, 255, 255, 0.4), 0 1px 1px rgba(0, 0, 0, 0.2);    -webkit-transition-duration: 0.2s;   -moz-transition-duration: 0.2s;   transition-duration: 0.2s;}";
  css += ".button-link:hover, .button-link:focus {   background: #356094;   border: solid 1px #2A4E77;text-decoration: none;}";
  css += ".button-link:active {    -webkit-box-shadow: inset 0 1px 4px rgba(0, 0, 0, 0.6);    -moz-box-shadow: inset 0 1px 4px rgba(0, 0, 0, 0.6);    box-shadow: inset 0 1px 4px rgba(0, 0, 0, 0.6);    background: #2E5481;    border: solid 1px #203E5F;}";

  return css;
}

String generate_webpage(){
  String result = String("");
  /* Fucking Arduino IDE doesn't know how to handle 
    strings spanning over more than one line ... */
  result += "<!DOCTYPE html>";
  result += "<html lang=\"en\">";
  result += "<head>";
  result += "<meta charset=\"utf-8\">";
  result += "<title>Electrical network socket</title>";
  result += "<link href=\"main.css\" rel=\"stylesheet\" />";
  result += "</head>";
  result += "<body>";
  result += "<h2>";
  result += "Toggle the state of the ports by pressing the buttons below";
  result += "</h2>";
  result += "<br></br>";
  result += "<p>";
  
  /* Dynamic content */
  for (int i = 1; i < 4; i++){
    result += "<a class=\"button-link\" href=\"";
    result += "http://";
    result += current_ip;
    result += "/";
    result += ports[i];
    result += "\">Toggle Socket 1</a>";
  }
  result += "<br></br></p><p>";
  
  for (int i = 4; i < 7; i++){
    result += "<a class=\"button-link\" href=\"";
    result += "http://";
    result += current_ip;
    result += "/";
    result += ports[i];
    result += "\">Toggle Socket 1</a>";
  }
  result += "<br></br></p><p>";
  
 for (int i = 7; i < 10; i++){
    result += "<a class=\"button-link\" href=\"";
    result += "http://";
    result += current_ip;
    result += "/";
    result += ports[i];
    result += "\">Toggle Socket 1</a>";
  }
  
  result += "</p></body></html>";
  
  return result;
}
