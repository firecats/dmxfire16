#define NUM_TLCS 2
#include <Tlc5940.h>

// A=0 B=1 C=2 D=3 E=4 F=5 G=6 H=7
#define BOARD 3
#define RE 5
#define BAUD 115200

#define START 0x7f
#define STOP 0x7e
#define ESCAPE 0x20

void setup() {
  // put your setup code here, to run once:
  Tlc.init();
  Tlc.clear();
  Tlc.update();
 
  pinMode(RE, OUTPUT);
  digitalWrite(RE, LOW);
 
  Serial.begin(BAUD);
}

char buf[1024];
char packet[1024];
int buf_pos = 0;
bool in_packet = false;
bool reading = false;
bool to_fire = false;
int n = 0;
int offset = BOARD * 4;

void loop() {
  
  if(Serial.available() > 0) {
  /*
    int n_read = Serial.readBytes(buf, 1024);
    for (int i = 0; i < n_read; i++) {
      // copy packet to buffer
      
      
    }
   */ 
    
    
    int in = Serial.read();
    //Serial.print("read 0x");
    //Serial.println(in, HEX);

    
    if (in == START) {
      reading = true;
      n = 0;
      Serial.println("start");
    } else if (in == STOP) {
      reading = false;
      to_fire = true;
      Serial.println("stop");
    } else if (reading) {
      if (in == ESCAPE) {
        in = Serial.read() ^ ESCAPE;
        Serial.println("escape");
      }   
      buf[n++] = in;
    }
  }
  
  if (!reading) {
    for (int i = 0; i < 33; i++) {
      Serial.println(buf[i], HEX);
    }
  }
  
  if (!reading && n == 33) {
    // first 4 bytes (0-3) are ours
    // last byte (32) is checksum
    
    char csum = 0;
    for (int i = 0; i < 32; i++) {
      csum ^= buf[i];
    }
    
    if (csum == buf[32] && to_fire) {
      // send 4 bytes starting at "offset"
      Tlc.clear();
      
      Serial.println("checksum OK, firing!");
      for (char s = 0; s < 32; s++) {
        char by = s / 8 + offset;
        char bi = s % 8;
        char on = (buf[by] >> bi) & 0x01;
        int val = on ? 4095 : 0;
        Tlc.set(s, val);
        Serial.print(s);
        Serial.print(": ");
        Serial.println(val);
      }
      Tlc.update();
      to_fire = false;
    }   
  }
}
