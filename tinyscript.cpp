#include "Arduino.h"
#include "tinyscript.h"
class _TinyScript TinyScript;



static char replbuffer[128];
static int replpointer=0;
static int startofline = 0;
static int string = 0;
static int blocks = 0;
static char dotprompt = 0;
static char retval = 0;
static char esc = 0;

int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}



static void replChar(char c)
{
    int r;
  if (c == '\n') {

    if (blocks)
    {
      Serial.print("\n...");
    }
  }
  else
  {
    Serial.write(c);
  }

  if (c == '\n') {
    if (replpointer > 0 && replbuffer[replpointer - 1] == '\\')
    {
      replbuffer[replpointer - 1] = '\n';

    }
    else if (blocks == 0)goto doing;
    replbuffer[replpointer++] = '\n';
  }

  else if (c == '\\')
  {
    esc = 1;
  }
  else if (string && esc)
  {
    replbuffer[replpointer++] = c;
  }
  else if (c == '}' && !string) {
    blocks--;
    replbuffer[replpointer++] = c;
  }
  else if (c == '{' && !string) {
    blocks++;
    replbuffer[replpointer++] = c;
  }
  else if (c == '"' || c == '\'') {
    string = !string;
    replbuffer[replpointer++] = c;
  }
  else if (replpointer >= 125) {
    Serial.print(F("ts: input line too long\n"));
    goto resetting;
  }
  else {
    replbuffer[replpointer++] = c;
  }

  esc = 0;
  return;
doing:
  replbuffer[replpointer] = 0;
  Serial.println("");
  r = TinyScript_Run(replbuffer, 1, 1);
  if (r != 0) {
    Serial.print("error :");
    Serial.println(r);
  }
resetting:
  //Done with that, make the replbuffer go to the default len if it was expanded
  replpointer = 0;
  replbuffer[replpointer] = 0;
  blocks = 0;
  string = 0;
  retval = 0;
  Serial.print("\n>>>");
}


void _TinyScript::pollREPL()
{
  while (Serial.available())
  {
    char x = Serial.read();
    replChar(x);
  }
}

void hdeferr(int x)
{
  if(x!=TS_ERR_OK)
  {
    Serial.println(F("Could not define sym. Out of mem?"));
  }
}
void _TinyScript::addFunc(const char *name,  Val (*Cfunc)(Val, Val, Val, Val))
{
  hdeferr(TinyScript_Define(name, CFUNC(4), (intptr_t)Cfunc));
}
void _TinyScript::addFunc(const char *name,  Val (*Cfunc)(Val, Val, Val))
{
  hdeferr(TinyScript_Define(name, CFUNC(3), (intptr_t)Cfunc));
}
void _TinyScript::addFunc(const char *name,  Val (*Cfunc)(Val, Val))
{
  hdeferr(TinyScript_Define(name, CFUNC(2), (intptr_t)Cfunc));
}
void _TinyScript::addFunc(const char *name, Val (*Cfunc)(Val))
{
  hdeferr(TinyScript_Define(name, CFUNC(1), (intptr_t)Cfunc));
}
void _TinyScript::addFunc(const char *name, Val (*Cfunc)())
{
  hdeferr(TinyScript_Define(name, CFUNC(0), (intptr_t)Cfunc));
}

void _TinyScript::addIntVar(const char *name, int x){
      hdeferr(TinyScript_Define(name, INT, x));
}

/*void _TinyScript::getIntVar(const char *name, int x){
}*/


// compute a function of two variables
// used for testing scripts
static Val tspinMode(Val x, Val y)
{
  pinMode(x, y);
  return 0;
}


static Val tsdigiwrite(Val x, Val y)
{
  digitalWrite(x, y);
  return 0;
}


static Val tsdigiread(Val x)
{
  return digitalRead(x);
}



static Val tsdelay(Val x)
{
  delay(x);
  return 0;
}

static Val tsmillis()
{
  return millis();
}


static Val tsmemfree()
{
 return TinyScript_memfree();
}


static char * arena = 0;

int inchar() {
  while (Serial.available() == 0)
  {
  }
  return Serial.read();
}
void outchar(int c) {
  Serial.write(c);
}


void _TinyScript::begin(int sz)
{
  if (arena)
  {
    return;
  }
  arena = (char*) malloc(sz);
  if (arena == 0) {
    Serial.print("Could not allocate memory for TinyScript interpreter!\n");
    return;
  }
  int err = TinyScript_Init(arena, sz);
  if (err != 0) {
    Serial.print("Initialization of interpreter failed!\n");
    return;
  }

  addFunc("memfree", tsmemfree);
  addFunc("digitalRead", tsdigiread);
  addFunc("digitalWrite", tsdigiwrite);
  addFunc("pinMode", tspinMode);
  addFunc("delay", tsdelay);
  addFunc("millis", tsmillis);

  addIntVar("HIGH",HIGH);
  addIntVar("LOW",LOW);
  addIntVar("INPUT",INPUT);
  addIntVar("OUTPUT", OUTPUT);
  addIntVar("INPUT_PULLUP", INPUT_PULLUP);

    
}
