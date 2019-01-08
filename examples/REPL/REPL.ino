#include <stdio.h>
#include <stdlib.h>
#include "tinyscript.h"


// compute a function of two variables
// used for testing scripts.

//0 to 4 variables are accepted.
static Val testfunc(Val x, Val y)
{
  return x * x + y * y;
}


void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  //Give the interpreter 768 bytes
  TinyScript.begin(768);
  TinyScript.addFunc("test", testfunc);
}

void loop() {
  // put your main code here, to run repeatedly:
  TinyScript.pollREPL();
}
