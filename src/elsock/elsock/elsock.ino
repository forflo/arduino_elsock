#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }
IPAddress fallback_ip(141,60,125,4)
IPAddress fallback_gateway(141,60,125,1)
IPAddress fallback_subnet(255,255,255,0)

EthernetServer server(7878);
boolean rcvd_msg = false;

void setup(){
  Serial.begin(9600);

  Serial.println("[setup()] Trying to query configuration by using DHCP")
  if (Ethernet.begin(mac) == 0){
    Serial.println("[setup()] Failed to get DHCP-IP address");
    Serial.println("... using fallback configuration instead");
    Ethernet.begin(mac, fallback_ip, fallback_gateway, fallback_subnet);
  }
  
  Serial.print("[setup()] IP-address: ");
  ip = Ethernet.localIP();
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
  
  if(client){
    String line = get_line(client);
    if (line.charAt(0) == 'G' && line.charAt(1) == 'E' && line.charAt(2) == 'T'){
      if (line.charAt(4) == '/'){
        String path = "";
        int index = 5;
        char temp;
        /* Get the Path of the request */
        while((temp = line.charAt(index++)) != ' '){
          path += temp;
        }
      }
    }
  }
}

String get_line(EthernetClient c){
  String result = "";
  char temp;
  while ((temp = client.read()) != '\n'){
    result += temp;
  }
  
  return result;
}
