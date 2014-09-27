#include <SPI.h>
#include <Ethernet.h>

String ports[] = {"null", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};
int status[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int portnum = 10;
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress fallback_ip(141,60,125,4);
IPAddress fallback_gateway(141,60,125,1);
IPAddress fallback_subnet(255,255,255,0);

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
  
  server.begin();

}

void loop(){
  /* like the accept() library function from the BSD-Socketlib */
  EthernetClient client = server.available();
  String path = "";
  
  if(client){
    Serial.println("1");

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
    Serial.print(path);
    Serial.println();
    
    for (int i = 0; i < portnum; i++){
      if(!path.compareTo(ports[i])){
        /* Port found! */
        status[i] = !status[i];        
        
        if(status){
          digitalWrite(i, HIGH);
        } else {
          digitalWrite(i, LOW);
        }
      }
    }
    
    /* collect the irrelevant headers until the double \n is reached */
    while (get_line(client).length() != 0);
  }
}

String get_line(EthernetClient c){
  String result = "";
  char temp;
  while ((temp = c.read()) != '\n'){
    Serial.print("get_line[]: ");
    Serial.print(temp);
    Serial.println();
    result += temp;
  }
  
  return result;
}
