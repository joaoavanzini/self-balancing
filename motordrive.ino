#define OUTA 3
#define OUTB 9
#define OUTC 10
#define OUTD 11

void init_motores(){
  pinMode(OUTA, OUTPUT);
  pinMode(OUTB, OUTPUT);
  pinMode(OUTC, OUTPUT);
  pinMode(OUTD, OUTPUT);
  
}

void PMWMotorControl(double manager){
  if(manager > 0){
    analogWrite(OUTA, 0); // RIGHT - BACK
    analogWrite(OUTB, abs(manager)); // RIGHT - FRONT
    analogWrite(OUTC, abs(manager)); // LEFT - FRONT
    analogWrite(OUTD, 0); // LEFT - BACK
  }else{
    analogWrite(OUTA, abs(manager)); // RIGHT - BACK
    analogWrite(OUTB, 0); // RIGHT - FRONT
    analogWrite(OUTC, 0); // LEFT - FRONT
    analogWrite(OUTD, abs(manager)); // LEFT - BACK
    
  }
}
