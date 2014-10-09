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
  0, 0, 0, 0
};

byte mac[] = { 
  0xDE, 0xAD, 0xBE, 
  0xEF, 0xFE, 0xED 
};

/* number of elements in ports[] */
byte portnum = 10;

byte i;

/* used by the function get_line() */
char buf[60];
char current_ip[16];

/* Arduino will listen on port 80 */
EthernetServer server(80);

/* This function configures the
  - pins
  - the serial line
  - the network stack (using dhcp)
  - and sets the global variable current ip */
void setup(){

  int current_byte = 0, cip_cnt = 0, cip_i = 0;
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

  IPAddress ip = Ethernet.localIP();

  /* calculates and sets the current ip */
  for (i = 0; i < 4; i++){
    current_byte = ip[i];
    if(current_byte > 99){ 
      cip_i = 2 + cip_cnt;
      cip_cnt += 3;
    } 
    else if (current_byte > 9) {
      cip_i = 1 + cip_cnt;
      cip_cnt += 2;
    } 
    else {
      cip_i = 0 + cip_cnt;
      cip_cnt++;
    }

    if (i != 3) {
      cip_cnt++;
      current_ip[cip_i + 1] = '.';
    }

    while(current_byte){
      current_ip[cip_i--] = '0' + (current_byte % 10);
      current_byte /= 10;
    }
  }
  current_ip[cip_cnt] = '\0';

  Serial.print("[setup()] current IP: ");
  Serial.println(current_ip);

  server.begin();
}

/* poor man's exit */
void exit(){
  Serial.println("EXIT");
  while(true);
}

/* This function is monolitical only for one particular reason: If I create
  new functions in my current development setup based on the Arduino-Ehternet
  board, the code does strange things. I think the reason for this behaviour
  is the limited stack or RAM size, but I surely am not very confident about that.
  
  If you have a deeper knowledge of embedded systems or 
  know one plausible causal chain of my problem,
  please let me know! ;) */
void loop(){
  /* like the accept() library function from the BSD-Socketlib */
  EthernetClient client = server.available();

  /* client dispatch */
  if(client){
    char path[43]; /* will contain the path part of the url */

    /* set buf to the request line */
    get_line(client);

    if (buf[0] == 'G' && buf[1] == 'E' && buf[2] == 'T' && buf [4] == '/'){
      i = 5;
      char temp;

      /* Get the path of the request */
      while ((temp = buf[i]) != ' '){
        path[i-5] = temp;
        i++;
      }
      path[i-5] = '\0';
    }

    Serial.print("[server] path: /");
    Serial.println(path);

    /* digests the path */
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
    } else if (path[0] == 'q' && path[1] == '?') {
      i = 2;
      byte state, led, t, k;

      Serial.println("[server] Start parsing HTTP query string ...");
      client.println("HTTP/1.1 200 OK\n");

      /* Please note that only the blocks for case '1', case '0', case 's' and case 't' 
        contain the continue statement. This is because only in these cases
        the code following the switch-case-block should be executed */
      while ((t = path[i++]) != '\0'){
        switch(t){
        /* eat up variable prefix L */
        case 'L':
          continue;
          break;
        /* led codes */
        case 'a':
          led = 0; 
          continue;
          break;
        case 'b':
          led = 1; 
          continue;
          break;
        case 'c':
          led = 2; 
          continue;
          break;
        case 'd':
          led = 3; 
          continue;
          break;
        case 'e':
          led = 4; 
          continue;
          break;
        case 'f':
          led = 5; 
          continue;
          break;
        case 'g':
          led = 6; 
          continue;
          break;
        case 'h':
          led = 7; 
          continue;
          break;
        case 'i':
          led = 8; 
          continue;
          break;
        case 'j':
          led = 9;
          continue;
          break;
        case 'A':
          led = 'A';
          continue;
          break;
        /* action codes */
        case '0':
          state = LOW; 
          break;
        case '1':
          state = HIGH; 
          break;
          /* This means: "toggle all" */
        case 't':
          state = 't';
          break;
        /* s is the code for: "please give me status" */
        case 's':
          state = 's';
          break;
        default:
          continue;
          break;
        }

        if(led == 'A'){
          for(k=0; k<portnum; k++){
            /* decide if the "toggle mode" is switched on */
            if(state == 't'){
              status[k] = !status[k];
              digitalWrite(k, status[k]);
              
            /* just send all states to the client (csv) */
            } else if (state == 's'){
              client.print(status[k]);
              client.print(',');
            } else {
              digitalWrite(k, state);
              status[k] = state;
            }
          }
        } else { 
          if(state == 't'){
            status[led] = !status[led];
            digitalWrite(led, status[led]);
          } else if (state == 's') {
            client.print(status[led]);
            /* append a ',' (for csv) but only if 
              the current command isn't the last one */
            client.print(',');
            
            
          } else {
            digitalWrite(led, state);
            status[led] = state == HIGH ? 1 : 0;
          }
        }
      }
      
    /* Just normal URL-toggling. E.g.: 
      http://<ip>/j<arbitrary_string> => toggles 
      socket nr. 9 */
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
      client.print(current_ip); 
      client.print("/\n");
      client.print("Connection: close\n\n");
    }

    client.stop();
  }
}

/* queries just one line sent by the client */
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
    } 
    else {
      c.print(" style=\"color:#000000\">");
    }
    c.print("Socket ");
    c.print(i);

    if(status[i]){
      c.print(" = On");
    } 
    else {
      c.print(" = Off");
    }
    c.print("</p>");
  }

  c.print("</body>");
}

void send_css(EthernetClient c){
  c.print(".button-link{ \npadding: 10px 15px;\n");
  c.print("background: #808080; \ncolor: #FFF;\n");
  c.print("border: solid 1px #000000;\n");
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
    c.print(ports[i]); 
    c.print("musch");
    if(status[i]){
      c.print("\" style=\"color:#00FF00\">");
    } 
    else {
      c.print("\" style=\"color:#000000\">");
    }
    c.print("Socket ");
    c.print(i);

    if(status[i]){
      c.print(" = On");
    } 
    else {
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
