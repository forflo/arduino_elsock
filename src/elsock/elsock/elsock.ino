#include <SPI.h>
#include <Ethernet.h>

char ports[] = {
  'a', 'b', 'c', 
  'd', 'e', 'f', 
  'g', 'h', 'i', 'j'
};

byte status[] = {
  0, 0, 0, 
  0, 0, 0, 
  0, 0, 0, 
  0
};

byte portnum = 10;

byte i;
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 
  0xEF, 0xFE, 0xED 
};

char buf[60];

IPAddress ip;
String current_ip;
EthernetServer server(80);

void setup(){
  /* configure pins */
  for(i = 0; i < portnum; i++){
    pinMode(i, OUTPUT);
  }
  
  /* configure the serial connection */
  Serial.begin(9600);

  /* Ethernet config */
  Serial.println("[setup()] Trying to query configuration by using DHCP");
  if (Ethernet.begin(mac) == 0){
    Serial.println("[setup()] Failed to get DHCP-IP address");
    exit();
  }

  ip = Ethernet.localIP();

  /* set the current ip */
  current_ip.reserve(16);
  current_ip = "";
  for (i = 0; i < 4; i++){
    current_ip += ip[i];
    if (i != 3) {
      current_ip += ".";
    }
  }
  
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

  if(client){
    char path[12];
    
    /* setting buf to the request line */
    get_line(client);
    
    if (buf[0] == 'G' && buf[1] == 'E' && buf[2] == 'T' && buf [4] == '/'){
        i = 5;
        char temp;
        
        /* Get the Path of the request */
        while ((temp = buf[i]) != ' '){
          path[i-5] = temp;
          i++;
        }
        path[i-5] = '\0';
    }

    Serial.print("[server] path: /");
    Serial.println(path);

    if(path[0] == '\0'){
      Serial.println("[server] sending index ...");
      /* static content */
      client.println("HTTP/1.1 200 OK\n");
      send_webpage(client);
    } else if (path[0] == 'm') {
      Serial.println("[server] sending css ...");
      
      client.print("HTTP/1.1 200 OK\n");
      client.println("Content-Type: text/css; charset=iso-8859-1\n");
      send_css(client);
    } else if (path[0] == 's') {
      Serial.println("[server] sending status page ...");
      client.println("HTTP/1.1 200 OK\n");
      send_status(client);
    } else {
      
      for (i = 0; i < portnum; i++){
        if(path[0] == ports[i]){
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
      
      client.print("HTTP/1.1 302 Found\n");
      client.print("Location: http://");
      client.print(current_ip); client.print("/\n");
      client.print("Connection: close\n\n");
    }
    
    client.stop();
  }
}

void get_line(EthernetClient c){
  i = 0;
  char temp;
  
  while ((temp = c.read()) != '\n'){
    buf[i++] = temp;
  }
  buf[i] = '\0';
}

void send_status(EthernetClient c){
  c.print("<!DOCTYPE html>\n");
  c.print("<html lang=\"en\">\n");
  c.print("<head>\n");
  c.print("<meta charset=\"utf-8\">\n");
  c.print("<title>Status</title>\n");

  c.print("<link href=\"main.css\" rel=\"stylesheet\" />\n");
  c.print("</head>\n");

  c.print("<body>");
  
  /* Dynamic content */
  for (i = 1; i < 10; i++){
    c.print("<p class=\"button-link\"");
    
    if(status[i]){
      c.print(" style=\"color:#00FF00\">");
    } else {
      c.print(" style=\"color:#FF0000\">");
    }
    c.print("Socket ");
    c.print(i);
    
    if(status[i]){
      c.print(" = On");
    } else {
      c.print(" = Off");
    }
    c.print("</p>");
  }

  c.print("</body>");
}

void send_css(EthernetClient c){
  c.print(".button-link{ \npadding: 10px 15px;\n");
  c.print("background: #808080; \ncolor: #FFF;\n");
  c.print("border-radius: 4px; \nborder: solid 1px #000000;\n");
  c.print("}\n\n");

  c.print(".button-link:hover, .button-link:focus {\n");
  c.print("background: #4E4E4E;\n");
  c.print("border: solid 1px #000000;\n");
  c.print("text-decoration: none;\n");
  c.print("}");
}

void send_webpage(EthernetClient c){
  c.print("<!DOCTYPE html>\n");
  c.print("<html lang=\"en\">\n");
  c.print("<head>\n");
  c.print("<meta charset=\"utf-8\">\n");
  c.print("<title>Electrical network socket</title>\n");

  c.print("<link href=\"main.css\" rel=\"stylesheet\" />\n");
  c.print("</head>\n");

  c.print("<body>\n<h2>Toggle the state of the ports by pressing the buttons below");
  c.print("</h2>\n<br><br>\n<p>");

  /* Dynamic content */
  for (i = 1; i < 10; i++){
    c.print("<a class=\"button-link\" href=\"");
    c.print("http://");
    c.print(current_ip);
    c.print("/");
    c.print(ports[i]); c.print("musch");
    if(status[i]){
      c.print("\" style=\"color:#00FF00\">");
    } else {
      c.print("\" style=\"color:#FF0000\">");
    }
    c.print("Socket ");
    c.print(i);
    
    if(status[i]){
      c.print(" = On");
    } else {
      c.print(" = Off");
    }
    
    c.print("</a>\n");

    if(i%3==0){
      c.print("<br><br>\n</p><p>\n");
    }
  }
  
  c.print("<br><br>\n</p><p>\n");
  c.print("<a class=\"button-link\" href=\"http://");
  c.print(current_ip);
  c.print("/");
  c.print("status\">Status</a>\n");

  c.print("</p>\n</body>\n</html>\n");
}
