#define NUM_TLCS 2
#include <Tlc5940.h>

void setup() {
  // put your setup code here, to run once:
  Tlc.init();
  Tlc.clear();
  Tlc.update();
}

void loop() {
  for (char s = 0; s < 32; s++) {
    Tlc.clear();
    Tlc.set(s, 4095);
    Tlc.update();
    delay(100);
  } 
}
