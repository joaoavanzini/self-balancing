
double kp = 22.0;
double kd = 2.5; 
double ki = 4;

int OUTMAX = 200;
int OUTMIN = -200;

double SetPoint = 2.5;

float lastInput = 0.0;

double ITerm = 0.0;

double Compute(double input){

  // Error Calculation
  double erro = SetPoint - input;

  ITerm+= (ki * erro);

  if(ITerm > OUTMAX){
    ITerm = OUTMAX;
  }else if(ITerm <= OUTMIN){
    ITerm = OUTMIN;
  }


  // Difference from current - previous entry
  double dInput = input - lastInput;

  // PID Calculation
  double output = kp * erro + ITerm + kd * dInput;


  if(output > OUTMAX){
    output = OUTMAX;
  }else if(output <= OUTMIN){
    output = OUTMIN;
  }

  lastInput = input;

  return output;
  
}
