// -----------------------------------------------------------------------------------
// Stepper driver mode control

bool _stepperModeTrack=false;

// initialize stepper drivers
void StepperModeTrackingInit() {
  _stepperModeTrack=false; 
  digitalWrite(Axis1_EN,Axis1_Enabled); axis1Enabled=true;
  digitalWrite(Axis2_EN,Axis2_Enabled); axis2Enabled=true;
  delay(100);
  StepperModeTracking();
  digitalWrite(Axis1_EN,Axis1_Disabled); axis1Enabled=false;
  digitalWrite(Axis2_EN,Axis2_Disabled); axis2Enabled=false;
}

// if stepper drive can switch decay mode, set it here
void StepperModeTracking() {
  if (_stepperModeTrack) return;
  _stepperModeTrack=true;
  cli();
#if defined(DECAY_MODE_OPEN)
  pinModeOpen(Axis1_Mode);
  pinModeOpen(Axis2_Mode);
#elif defined(DECAY_MODE_LOW)
  pinMode(Axis1_Mode,OUTPUT); digitalWrite(Axis1_Mode,LOW);
  pinMode(Axis2_Mode,OUTPUT); digitalWrite(Axis1_Mode,LOW);
#elif defined(DECAY_MODE_HIGH)
  pinMode(Axis1_Mode,OUTPUT); digitalWrite(Axis1_Mode,HIGH);
  pinMode(Axis2_Mode,OUTPUT); digitalWrite(Axis2_Mode,HIGH);
#elif defined(MODE_SWITCH_BEFORE_SLEW_ON)
  #ifdef AXIS1_MODE
    #ifdef AXIS1_MODE_GOTO 
    stepAxis1=1;
    #endif
    if ((AXIS1_MODE & 0b001000)==0) { pinMode(Axis1_M0,OUTPUT); digitalWrite(Axis1_M0,(AXIS1_MODE    & 1)); } else { pinMode(Axis1_M0,INPUT); }
    if ((AXIS1_MODE & 0b010000)==0) { pinMode(Axis1_M1,OUTPUT); digitalWrite(Axis1_M1,(AXIS1_MODE>>1 & 1)); } else { pinMode(Axis1_M1,INPUT); }
    if ((AXIS1_MODE & 0b100000)==0) { pinMode(Axis1_M2,OUTPUT); digitalWrite(Axis1_M2,(AXIS1_MODE>>2 & 1)); } else { pinMode(Axis1_M2,INPUT); }
  #endif
  #ifdef AXIS2_MODE
    #ifdef AXIS2_MODE_GOTO 
    stepAxis2=1;
    #endif
    if ((AXIS2_MODE & 0b001000)==0) { pinMode(Axis2_M0,OUTPUT); digitalWrite(Axis2_M0,(AXIS2_MODE    & 1)); } else { pinMode(Axis2_M0,INPUT); }
    if ((AXIS2_MODE & 0b010000)==0) { pinMode(Axis2_M1,OUTPUT); digitalWrite(Axis2_M1,(AXIS2_MODE>>1 & 1)); } else { pinMode(Axis2_M1,INPUT); }
    if ((AXIS2_MODE & 0b100000)==0) { pinMode(Axis2_M2,OUTPUT); digitalWrite(Axis2_M2,(AXIS2_MODE>>2 & 1)); } else { pinMode(Axis2_M2,INPUT); }
  #endif
#elif defined(MODE_SWITCH_BEFORE_SLEW_SPI)
  stepAxis1=1;
  stepAxis2=1;
  bool nintpol=((AXIS1_MODE & 0b0010000)!=0);
  bool stealth=((AXIS1_MODE & 0b0100000)!=0);
  bool lowpwr =((AXIS1_MODE & 0b1000000)!=0);
  //       SS      ,SCK     ,MISO     ,MOSI
  BBSpi.begin(Axis1_M2,Axis1_M1,Axis1_Aux,Axis1_M0);
  TMC2130_setup(!nintpol,stealth,AXIS1_MODE&0b001111,lowpwr);  // default 256x interpolation ON, stealthChop OFF (spreadCycle), micro-steps
  TMC2130_sgSetSgt(EEPROM.read(EE_sgSgtAxis1));
  BBSpi.end();
  nintpol=((AXIS2_MODE & 0b0010000)!=0);
  stealth=((AXIS2_MODE & 0b0100000)!=0);
  lowpwr =((AXIS2_MODE & 0b1000000)!=0);
  BBSpi.begin(Axis2_M2,Axis2_M1,Axis2_Aux,Axis2_M0);
  TMC2130_setup(!nintpol,stealth,AXIS2_MODE&0b001111,lowpwr);
  TMC2130_sgSetSgt(EEPROM.read(EE_sgSgtAxis2));
  BBSpi.end();

  // allow stealth chop current regulation to ramp up to the initial motor current before moving
  if ((((AXIS1_MODE & 0b0100000)!=0) || ((AXIS2_MODE & 0b0100000)!=0)) & (atHome)) delay(100);
#endif

#ifdef MODE_SWITCH_SLEEP_ON
  delay(3);
#endif
  sei();
}

void StepperModeGoto() {
  if (!_stepperModeTrack) return;
  _stepperModeTrack=false;
  cli();
#if defined(DECAY_MODE_GOTO_OPEN)
  pinModeOpen(Axis1_Mode);
  pinModeOpen(Axis2_Mode);
#elif defined(DECAY_MODE_GOTO_LOW)
  pinMode(Axis1_Mode,OUTPUT); digitalWrite(Axis1_Mode,LOW);
  pinMode(Axis2_Mode,OUTPUT); digitalWrite(Axis1_Mode,LOW);
#elif defined(DECAY_MODE_GOTO_HIGH)
  pinMode(Axis1_Mode,OUTPUT); digitalWrite(Axis1_Mode,HIGH);
  pinMode(Axis2_Mode,OUTPUT); digitalWrite(Axis2_Mode,HIGH);
#elif defined(MODE_SWITCH_BEFORE_SLEW_ON)
  #ifdef AXIS1_MODE_GOTO
    stepAxis1=AXIS1_STEP_GOTO;
    if ((AXIS1_MODE_GOTO & 0b001000)==0) { pinMode(Axis1_M0,OUTPUT); digitalWrite(Axis1_M0,(AXIS1_MODE_GOTO    & 1)); } else { pinMode(Axis1_M0,INPUT); }
    if ((AXIS1_MODE_GOTO & 0b010000)==0) { pinMode(Axis1_M1,OUTPUT); digitalWrite(Axis1_M1,(AXIS1_MODE_GOTO>>1 & 1)); } else { pinMode(Axis1_M1,INPUT); }
    if ((AXIS1_MODE_GOTO & 0b100000)==0) { pinMode(Axis1_M2,OUTPUT); digitalWrite(Axis1_M2,(AXIS1_MODE_GOTO>>2 & 1)); } else { pinMode(Axis1_M2,INPUT); }
  #endif
  #ifdef AXIS2_MODE_GOTO
    stepAxis2=AXIS2_STEP_GOTO;
    if ((AXIS2_MODE_GOTO & 0b001000)==0) { pinMode(Axis2_M0,OUTPUT); digitalWrite(Axis2_M0,(AXIS2_MODE_GOTO    & 1)); } else { pinMode(Axis2_M0,INPUT); }
    if ((AXIS2_MODE_GOTO & 0b010000)==0) { pinMode(Axis2_M1,OUTPUT); digitalWrite(Axis2_M1,(AXIS2_MODE_GOTO>>1 & 1)); } else { pinMode(Axis2_M1,INPUT); }
    if ((AXIS2_MODE_GOTO & 0b100000)==0) { pinMode(Axis2_M2,OUTPUT); digitalWrite(Axis2_M2,(AXIS2_MODE_GOTO>>2 & 1)); } else { pinMode(Axis2_M2,INPUT); }
  #endif
#elif defined(MODE_SWITCH_BEFORE_SLEW_SPI)
  stepAxis1=AXIS1_STEP_GOTO;
  stepAxis2=AXIS2_STEP_GOTO;
  bool nintpol=((AXIS1_MODE_GOTO & 0b0010000)!=0);
  bool stealth=((AXIS1_MODE_GOTO & 0b0100000)!=0);
  bool lowpwr =((AXIS1_MODE_GOTO & 0b1000000)!=0);
  //       CS      ,SCK     ,MISO     ,MOSI
  BBSpi.begin(Axis1_M2,Axis1_M1,Axis1_Aux,Axis1_M0);
  TMC2130_setup(!nintpol,stealth,AXIS1_MODE_GOTO&0b001111,lowpwr);  // default 256x interpolation ON, stealthChop OFF (spreadCycle), micro-steps
  BBSpi.end();
  nintpol=((AXIS2_MODE_GOTO & 0b0010000)!=0);
  stealth=((AXIS2_MODE_GOTO & 0b0100000)!=0);
  lowpwr =((AXIS2_MODE_GOTO & 0b1000000)!=0);
  BBSpi.begin(Axis2_M2,Axis2_M1,Axis2_Aux,Axis2_M0);
  TMC2130_setup(!nintpol,stealth,AXIS2_MODE_GOTO&0b001111,lowpwr);
  BBSpi.end();
#endif

#ifdef MODE_SWITCH_SLEEP_ON
  delay(3);
#endif
  sei();
}
 
