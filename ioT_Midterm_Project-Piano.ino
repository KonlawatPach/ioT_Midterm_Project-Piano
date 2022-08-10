#include "pitches.h"
#include <Wire.h>
//วิธีแต่งเพลงใหม่ เพลงระยะเวลา stepup ทีละ 50 เท่านั้น

//SET UP I2C LCD
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F, 16, 2);
byte state0[] =   {B11111, B00000, B00000, B00000, B00000, B00000, B00000, B00000};
byte state1[] =   {B00000, B11111, B00000, B00000, B00000, B00000, B00000, B00000};
byte state2[] =   {B00000, B00000, B11111, B00000, B00000, B00000, B00000, B00000};
byte state3[] =   {B00000, B00000, B00000, B11111, B00000, B00000, B00000, B00000};
byte state4[] =   {B00000, B00000, B00000, B00000, B11111, B00000, B00000, B00000};
byte state5[] =   {B00000, B00000, B00000, B00000, B00000, B11111, B00000, B00000};
byte state6[] =   {B00000, B00000, B00000, B00000, B00000, B00000, B11111, B00000};
byte state7[] =   {B00000, B00000, B00000, B00000, B00000, B00000, B00000, B11111};
byte block[]  =   {B11111, B11111, B11111, B11111, B11111, B11111, B11111, B11111};
byte arrowup[] =  {B00000, B00000, B00100, B01110, B11111, B00000, B00000, B00000};
byte arrowdown[]= {B00000, B00000, B00000, B00000, B11111, B01110, B00100, B00000};

//SET UP LED PIN
const int RLED = 10;
const int GLED = 11;
const int BLED = 12;

//SET UP BUTTON PIN
const int DO = 2;
const int RE = 3;
const int ME = 4;
const int FA = 5;
const int SOL= 6;
const int LA = 7;
const int TE = 8;
const int DOO= 9;

boolean lastDO = LOW;
boolean lastRE = LOW;
boolean lastME = LOW;
boolean lastFA = LOW;
boolean lastSOL= LOW;
boolean lastLA = LOW;
boolean lastTE = LOW;
boolean lastDOO= LOW;

boolean curDO;
boolean curRE;
boolean curME;
boolean curFA;
boolean curSOL;
boolean curLA;
boolean curTE;
boolean curDOO;

const int SPEAKER = 13;


//----------------PIANO TILE----------------
//SET UP SONG
int notes[70];
int times[70];
int tap[120];
int notelength = 0;
int taplength = 0;

//8 BIT TEST Song
int bit_test_note[] = {
  NOTE_A4, NOTE_E3, NOTE_A4, 0, 
  NOTE_A4, NOTE_E3, NOTE_A4, 0,
  NOTE_E4, NOTE_D4, NOTE_C4, NOTE_B4, NOTE_A4, NOTE_B4, NOTE_C4, NOTE_D4,
  NOTE_E4, NOTE_E3, NOTE_A4, 0
};
int bit_test_time[] = {
  250, 250, 250, 250, 
  250, 250, 250, 250,
  125, 125, 125, 125, 125, 125, 125, 125,
  250, 250, 250, 250
};
int bit_test_tap[] = {
  0, RE, 250, DO, 300, RE, 
  450, RE, 250, DO, 250 , RE,
  350, ME, 125, RE, 125 , DO, 125, SOL, 125, FA, 125, ME,
  250, RE, 250, DO, 300, RE
};

//Jingle bell Song
int bell_note[] = {
  NOTE_E5, NOTE_E5, NOTE_E5, 
  NOTE_E5, NOTE_E5, NOTE_E5, 
  NOTE_E5, NOTE_G5, NOTE_C5, NOTE_D5,
  NOTE_E5,
  NOTE_F5, NOTE_F5, NOTE_F5, NOTE_F5,
  NOTE_F5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5,
  NOTE_E5, NOTE_D5, NOTE_D5, NOTE_E5,
  NOTE_D5, NOTE_G5, 0
};
int bell_time[] = {
  200, 200, 200, 
  400, 200, 200, 
  400, 200, 200, 200, 
  200,
  800, 200, 200, 200,
  200, 200, 200, 200, 100,
  100, 200, 200, 200,
  200, 400, 400
};
int bell_tap[] = {
  0, RE,
  800, RE,
  800, RE, 200, FA, 200, DO,
  400, RE,
  800, ME,
  800, ME, 200, RE, 500, RE,
  300, DO, 400, ME,
  200, DO, 400, FA
};

//Birth Day Song
int birth_note[] = {
  NOTE_C4, NOTE_C4,
  NOTE_D4, NOTE_C4, NOTE_F4,
  NOTE_E4, 
  
  NOTE_C4, NOTE_C4,
  NOTE_D4, NOTE_C4, NOTE_G4,
  NOTE_F4, NOTE_C4, NOTE_C4,

  NOTE_C5, NOTE_A4, NOTE_F4,
  NOTE_E4, NOTE_D4, NOTE_AS4, NOTE_AS4,
  NOTE_A4, NOTE_F4, NOTE_G4,
  NOTE_F4, NOTE_F4, 0
};
int birth_time[] = {
  200, 200,
  200, 700, 400, 
  400,

  800, 200,
  200, 700, 400, 
  400,

  800, 400, 300,
  800, 400, 800, 400,
  300, 800, 800,
  400, 800, 800
};
int birth_tap[] = {
  0, DO, 
  400, RE, 900, DO, 400, FA,
  400, RE,

  800, DO, 
  400, RE, 900, DO, 400, SOL,
  400, FA,

  800, DO, 700, SOL,
  800, FA, 400, RE, 800, SOL, 
  700, FA, 800, ME, 1400, SOL,
  400, ME 
};



//SET UP STATE
int allIndex = 0;   //clock all
int score = 0;
int fullscore = 0;
int noteIndex = 0;
int musicIndex = 0;   //like clock all but reset when play note.
int beforemusictime = 800;   //when tap are not already finish.
int aftermusictime = 0;   //when tap are not already finish.

int tapcomingIndex = 0;
int tapcomingTime = 0;
int comingstate[]  = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

int tapIndex = 0;
int tapTime = 0;
int tapstate[]     = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

int ledtime = 0;


//----------------NORMAL PIANO----------------
//SET UP STATE
boolean isToning = false;


//----------------PIANO MENU----------------
//SET UP STATE
//menu state
int menustate = 0;     // 0-menu, 1-piano, 2-tile

char *menuchoice[] = {"> Normal Piano", "> Tile Piano"};
int menucur = 0;       // 0-piano, 1-tile

//tile state
int tilestate = 0;     // 0-select music, 1-play music
char *tilechoice[] = {
  "8Bit TEST",
  "Jingle Bell",
  "Happy Birthday"
  };
int tilecur = 0;

//BACK BUTTON
int holdDOTime = 0;   //check time to back to menu

//LCD STATE
boolean lcdclear = false;
boolean backfromtile = false;
boolean backfromtilemode = false;





void setup() {
  //Serial Monitor
  Serial.begin(9600);

  //LCD Start
  lcd.init();
  lcd.backlight();
  
  lcd.createChar(0, state0);
  lcd.createChar(1, state1);
  lcd.createChar(2, state2);
  lcd.createChar(3, state3);
  lcd.createChar(4, state4);
  lcd.createChar(5, state5);
  // lcd.createChar(6, state6);
  // lcd.createChar(7, state7);
  lcd.createChar(6, arrowup);
  lcd.createChar(7, arrowdown);
  lcd.createChar(8, block);
  // lcd.createChar(9, arrowup);
  // lcd.createChar(10, arrowdown);

  //Pinmode Setting
  pinMode(DO, INPUT);
  pinMode(RE, INPUT);
  pinMode(ME, INPUT);
  pinMode(FA, INPUT);
  pinMode(SOL, INPUT);
  pinMode(LA, INPUT);
  pinMode(TE, INPUT);
  pinMode(DOO, INPUT);

  pinMode(RLED, OUTPUT);
  pinMode(GLED, OUTPUT);
  pinMode(BLED, OUTPUT);
}

boolean debounce(boolean last, int pin)
{
  boolean current = digitalRead(pin);
  if (last != current)
  {
    delay(5);
    current = digitalRead(pin);
  }
  return current;
}

//Normal Piano Function
void normalplay(){
  curDO = debounce(lastDO, DO);
  curRE = debounce(lastRE, RE);
  curME = debounce(lastME, ME);
  curFA = debounce(lastFA, FA);
  curSOL = debounce(lastSOL, SOL);
  curLA = debounce(lastLA, LA);
  curTE = debounce(lastTE, TE);
  curDOO = debounce(lastDOO, DOO);
  
  if(lastDO == LOW && curDO == HIGH){
    tone(SPEAKER, NOTE_C4, 200);
    isToning = true;
  }
  else if(lastRE == LOW && curRE == HIGH){
    tone(SPEAKER, NOTE_D4, 200);
    isToning = true;
  }
  else if(lastME == LOW && curME == HIGH){
    tone(SPEAKER, NOTE_E4, 200);
    isToning = true;
  }
  else if(lastFA == LOW && curFA == HIGH){
    tone(SPEAKER, NOTE_F4, 200);
    isToning = true;
  }
  else if(lastSOL == LOW && curSOL == HIGH){
    tone(SPEAKER, NOTE_G4, 200);
    isToning = true;
  }
  else if(lastLA == LOW && curLA == HIGH){
    tone(SPEAKER, NOTE_A4, 200);
    isToning = true;
  }
  else if(lastTE == LOW && curTE == HIGH){
    tone(SPEAKER, NOTE_B4, 200);
    isToning = true;
  }
  else if(lastDOO == LOW && curDOO == HIGH){
    tone(SPEAKER, NOTE_C5, 200);
    isToning = true;
  }
  else{
    isToning = false;
  }

  if(isToning){
    ledblue();
  }else{
    ledoff();
  }

  lastDO = curDO;
  lastRE = curRE;
  lastME = curME;
  lastFA = curFA;
  lastSOL = curSOL;
  lastLA = curLA;
  lastTE = curTE;
  lastDOO = curDOO;

  normalDisplaylcd();
  if(digitalRead(DO) == HIGH && holdDOTime >= 800){
    tone(SPEAKER, NOTE_B6, 200);
    delay(200);
    tone(SPEAKER, NOTE_B4, 250);
    delay(250);
    menustate = 0;
    holdDOTime = 0;
  }
  else if(digitalRead(DO) == HIGH && holdDOTime < 800){
    holdDOTime+=20;
  }
  else{
    holdDOTime = 0;
  }
}

void normalDisplaylcd(){
  for (int i = 0; i < 8; i++){
    if(digitalRead(i+2) == HIGH){
      for (int j = 0; j < 2; j++){
        lcd.setCursor(i*2+j, 0);
        lcd.write(8);
        lcd.setCursor(i*2+j, 1);
        lcd.write(8); 
      }
    }
    else{
      for (int j = 0; j < 2; j++){
        lcd.setCursor(i*2+j, 0);
        lcd.print((char)32);
        lcd.setCursor(i*2+j, 1);
        lcd.print((char)32); 
      }
    }
  }
}

//Piano Tile Function
void tileplay(){
  delay(50);

  //led function runing
  if(ledtime >= 100){
    ledoff();
  }
  else if(ledtime != 0){
    ledtime += 50;
  }

  //music function running
  if(noteIndex < notelength){
    stepMusic();
    clickCheck();
  }
  else{
    if(aftermusictime < 5000){
      aftermusictime+=50;
      clickCheck();
      noTone(SPEAKER);
    }
    else{
      conclusionScore();
    }
  }

  //state index step
  allIndex+=50;
}

//reset tile play
void resettilestate(){
  allIndex = 0;   //clock all
  score = 0;
  fullscore = 0;
  noteIndex = 0;
  musicIndex = 0;   //like clock all but reset when play note.
  beforemusictime = 800;   //when tap are not already finish.
  aftermusictime = 0;   //when tap are not already finish.
  
  tapcomingIndex = 0;
  tapcomingTime = 0;
//  comingstate[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  
  tapIndex = 0;
  tapTime = 0;
//  tapstate[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  ledtime = 0;
}


//music running and note tap
void stepMusic(){

  //check coming tap
  if(tapcomingIndex < taplength){
    if(tapcomingTime >= tap[tapcomingIndex]){
      boolean gotonextcomingtap = false;
      
      while(!gotonextcomingtap){
        tapcomingIndex += 2;
        tapcomingTime = 0;
        comingstate[(tap[tapcomingIndex-1]-2)*2] = 1;
        comingstate[((tap[tapcomingIndex-1]-2)*2)+1] = 0;
        if(tapcomingTime <= tap[tapcomingIndex] || tapcomingIndex < taplength){
          gotonextcomingtap = true;
        }
      }
    }
    else{
      tapcomingTime += 50;
    }
  }
  
  if(beforemusictime == 0){
    if(tapIndex < taplength){
      if(tapTime >= tap[tapIndex]){
        boolean gotonexttap = false;
        
        while(!gotonexttap){          //check note same time
          tapIndex += 2;
          tapTime = 0;
          lcdLightup(tap[tapIndex-1]);
          if(tapTime <= tap[tapIndex] || tapIndex < taplength){
            gotonexttap = true;
          }
        }
      }
      else{
        tapTime += 50;
      }
    }
    if(musicIndex >= times[noteIndex]){
      tone(SPEAKER, notes[noteIndex], times[noteIndex]);
  
      //check tap create
      musicIndex = 0;
      noteIndex++; 
    }
    else{
      musicIndex += 50;
    }
  }
  else{
    beforemusictime -= 50;
  }
}


//LCD display
void lcdcomingtap(int index, int timepass){
  if(timepass>0 && timepass<800){
    if(timepass%100 == 50) timepass-=50;

    lcd.setCursor(index, 0);
    lcd.write(timepass/100);
    lcd.setCursor(index+1, 0);
    lcd.write(timepass/100);
  }
  else if(timepass>=800){
    lcd.setCursor(index, 0);
    lcd.write(7);
    lcd.setCursor(index+1, 0);
    lcd.write(7);
  }
  else{
    lcd.setCursor(index, 0);
    lcd.print((char)32);
    lcd.setCursor(index+1, 0);
    lcd.print((char)32);
  }
}
void lcdLightup(int index){
  lcd.setCursor((index-2)*2, 1);
  lcd.write(8);
  lcd.setCursor(((index-2)*2)+1, 1);
  lcd.write(8);

  fullscore += 10;
  tapstate[(index-2)*2] = 1;
}
void lcdLightdown(int index){
  lcd.setCursor(index, 1);
  lcd.print((char)32);
  lcd.setCursor(index+1, 1);
  lcd.print((char)32);
}
void conclusionScore(){
  lcd.setCursor(0, 0);
  lcd.print("Your score: ");
  lcd.setCursor(12, 0);
  lcd.print(score);

  lcd.setCursor(0, 1);
  lcd.print("Full score: ");
  lcd.setCursor(12, 1);
  lcd.print(fullscore);

  //change custom byte back
  lcd.createChar(6, arrowup);
  lcd.createChar(7, arrowdown);

  //press any button
  for(int i = 2; i < 10; i++){
    if(digitalRead(i) == HIGH){
      resettilestate();
      backfromtile = true;
      tone(SPEAKER, NOTE_B6, 200);
      delay(200);
      tone(SPEAKER, NOTE_B4, 250);
      delay(250);
      tilestate = 0;
    }
  }
}


//Tap check
void clickCheck(){
  curDO = debounce(lastDO, DO);
  curRE = debounce(lastRE, RE);
  curME = debounce(lastME, ME);
  curFA = debounce(lastFA, FA);
  curSOL = debounce(lastSOL, SOL);
  curLA = debounce(lastLA, LA);
  curTE = debounce(lastTE, TE);
  curDOO = debounce(lastDOO, DOO);
  
  for(int i = 2; i < 10; i++){

    //active tap
    if(tapstate[(i-2)*2] == 1 && i == 2 && (lastDO == LOW && curDO == HIGH)){
      tapintime(i);
    }
    else if(tapstate[(i-2)*2] == 1 && i == 3 && (lastRE == LOW && curRE == HIGH)){
      tapintime(i);
    }
    else if(tapstate[(i-2)*2] == 1 && i == 4 && (lastME == LOW && curME == HIGH)){
      tapintime(i);
    }
    else if(tapstate[(i-2)*2] == 1 && i == 5 && (lastFA == LOW && curFA == HIGH)){
      tapintime(i);
    }
    else if(tapstate[(i-2)*2] == 1 && i == 6 && (lastSOL == LOW && curSOL == HIGH)){
      tapintime(i);
    }
    else if(tapstate[(i-2)*2] == 1 && i == 7 && (lastLA == LOW && curLA == HIGH)){
      tapintime(i);
    }
    else if(tapstate[(i-2)*2] == 1 && i == 8 && (lastTE == LOW && curTE == HIGH)){
      tapintime(i);
    }
    else if(tapstate[(i-2)*2] == 1 && i == 8 && (lastDOO == LOW && curDOO == HIGH)){
      tapintime(i);
    }
    else if(tapstate[(i-2)*2] == 1){
      nottap(i);
    }

    //coming tap
    if(comingstate[(i-2)*2] == 1 && comingstate[((i-2)*2)+1] > 800){
      comingstate[(i-2)*2] = 0;
      comingstate[((i-2)*2)+1] = 0;
      lcdcomingtap((i-2)*2, 0);
    }
    else if(comingstate[(i-2)*2] == 1){
      lcdcomingtap((i-2)*2, comingstate[((i-2)*2)+1]);
      comingstate[((i-2)*2)+1] += 50;
    }
  }

  lastDO = curDO;
  lastRE = curRE;
  lastME = curME;
  lastFA = curFA;
  lastSOL = curSOL;
  lastLA = curLA;
  lastTE = curTE;
  lastDOO = curDOO;
}

void tapintime(int note){          //tap in time
  tapstate[(note-2)*2] = 0;
  tapstate[((note-2)*2)+1] = 0;
  score += 10;
  
  ledcorrect();
  lcdLightdown((note-2)*2);
}
void nottap(int note){
  if(tapstate[((note-2)*2)+1] < 800){   //not tap but not late
    tapstate[((note-2)*2)+1] += 50;
  }
  else{                            //not tap and late
    tapstate[(note-2)*2] = 0;
    tapstate[((note-2)*2)+1] = 0;
    
    lcdLightdown((note-2)*2);
    ledmiss();
  }
}


//LED display
void ledcorrect(){
  analogWrite(RLED, 0);
  analogWrite(GLED, 255);
  analogWrite(BLED, 0);
  ledtime = 1;
}
void ledmiss(){
  analogWrite(RLED, 255);
  analogWrite(GLED, 0);
  analogWrite(BLED, 0);
  ledtime = 1; 
}
void ledblue(){
  analogWrite(RLED, 0);
  analogWrite(GLED, 0);
  analogWrite(BLED, 255);
}
void ledoff(){
  analogWrite(RLED, 0);
  analogWrite(GLED, 0);
  analogWrite(BLED, 0);
  ledtime = 0;
}


//MENU GAME
void menudisplay(){
  if(!lcdclear){
    lcdclear = true;
    lcd.clear();
  }
  lcd.setCursor(0, 0);
  lcd.print("Choose Game:");
  
  lcd.setCursor(0, 1);
  lcd.print(menuchoice[menucur]);

  if(menucur == 0){
    lcd.setCursor(15, 1);
    lcd.write(7);
  }else{
    lcd.setCursor(15, 0);
    lcd.write(6);
  }

  if(backfromtilemode){
    delay(1000);
    backfromtilemode = false;
  }
  
  if(digitalRead(DO) == HIGH && menucur == 1){
    menucur = 0;
    tone(SPEAKER, NOTE_C4, 250);
    delay(250);
    tone(SPEAKER, NOTE_B6, 200);
    lcdclear = false;
  }
  else if(digitalRead(RE) == HIGH && menucur == 0){
    menucur = 1;
    tone(SPEAKER, NOTE_C4, 250);
    delay(250);
    tone(SPEAKER, NOTE_B6, 200);
    lcdclear = false;
  }
  else if(digitalRead(DOO) == HIGH){
    tone(SPEAKER, NOTE_C4, 250);
    delay(250);
    tone(SPEAKER, NOTE_B6, 200);
    delay(200);
    
    menustate = menucur+1;
    lcd.clear();
    lcdclear = false;
  }
}

void tilemenu(){
  if(!lcdclear){
    lcdclear = true;
    lcd.clear();
  }
  lcd.setCursor(0, 0);
  lcd.print("Choose Music:");

  lcd.setCursor(0, 1);
  lcd.print("> ");
  lcd.setCursor(2, 1);
  lcd.print(tilechoice[tilecur]);

  if(tilecur > 0){
    lcd.setCursor(15, 0);
    lcd.write(6);
  }
  if(tilecur < (sizeof(tilechoice)/sizeof(*tilechoice))-1){
    lcd.setCursor(15, 1);
    lcd.write(7);
  }

  //don't click anything when back from music play
  if(backfromtile){
    backfromtile = false;
    delay(1000);
  }
  
  //select menu
  if(digitalRead(DO) == HIGH && tilecur > 0){
    tilecur--;
    tone(SPEAKER, NOTE_C4, 250);
    delay(250);
    tone(SPEAKER, NOTE_B6, 200);
    lcdclear = false;
  }
  else if(digitalRead(RE) == HIGH && tilecur < (sizeof(tilechoice)/sizeof(*tilechoice))-1){
    tilecur++;
    tone(SPEAKER, NOTE_C4, 250);
    delay(250);
    tone(SPEAKER, NOTE_B6, 200);
    lcdclear = false;
  }
  else if(digitalRead(DOO) == HIGH){
    tone(SPEAKER, NOTE_C4, 250);
    delay(250);
    tone(SPEAKER, NOTE_B6, 200);
    delay(200);

    lcd.createChar(6, state6);
    lcd.createChar(7, state7);

    lcd.clear();
    lcd.setCursor(7, 0);
    lcd.print("03");
    delay(1000);
    lcd.setCursor(7, 0);
    lcd.print("02");
    delay(1000);
    lcd.setCursor(7, 0);
    lcd.print("01");
    delay(1000);
    
    loadmusic();
    
    tilestate = 1;
    lcd.clear();
    lcdclear = false;
  }

  //back button
  if(digitalRead(DO) == HIGH && holdDOTime >= 800){
    tone(SPEAKER, NOTE_B6, 200);
    delay(200);
    tone(SPEAKER, NOTE_B4, 250);
    delay(250);
    menustate = 0;
    holdDOTime = 0;
    backfromtilemode = true;
  }
  else if(digitalRead(DO) == HIGH && holdDOTime < 800){
    holdDOTime+=20;
  }
  else{
    holdDOTime = 0;
  }
}

void loadmusic(){
  if(tilechoice[tilecur] == "8Bit TEST"){
    if(sizeof(bit_test_note)/sizeof(*bit_test_note) <= 100){
      notelength = sizeof(bit_test_note)/sizeof(*bit_test_note);
    }
    else{
      notelength = 100;
    }
    int timelength;
    if(sizeof(bit_test_time)/sizeof(*bit_test_time) <= 100){
      timelength = sizeof(bit_test_time)/sizeof(*bit_test_time);
    }
    else{
      timelength = 100;
    }
    if(sizeof(bit_test_tap)/sizeof(*bit_test_tap) <= 70){
      taplength = sizeof(bit_test_tap)/sizeof(*bit_test_tap);
    }
    else{
      taplength = 70;
    }
    for(int i = 0; i < notelength; i++){ notes[i] = bit_test_note[i]; }
    for(int i = 0; i < timelength; i++){ times[i] = bit_test_time[i]; }
    for(int i = 0; i < taplength; i++){ tap[i] = bit_test_tap[i]; }
  }
  
  else if(tilechoice[tilecur] == "Jingle Bell"){
    if(sizeof(bell_note)/sizeof(*bell_note) <= 100){
      notelength = sizeof(bell_note)/sizeof(*bell_note);
    }
    else{
      notelength = 100;
    }
    int timelength;
    if(sizeof(bell_time)/sizeof(*bell_time) <= 100){
      timelength = sizeof(bell_time)/sizeof(*bell_time);
    }
    else{
      timelength = 100;
    }
    if(sizeof(bell_tap)/sizeof(*bell_tap) <= 70){
      taplength = sizeof(bell_tap)/sizeof(*bell_tap);
    }
    else{
      taplength = 120;
    }
    for(int i = 0; i < notelength; i++){ notes[i] = bell_note[i]; }
    for(int i = 0; i < timelength; i++){ times[i] = bell_time[i]; }
    for(int i = 0; i < taplength; i++){ tap[i] = bell_tap[i]; }
  }
  
  else if(tilechoice[tilecur] == "Happy Birthday"){
    if(sizeof(birth_note)/sizeof(*birth_note) <= 100){
      notelength = sizeof(birth_note)/sizeof(*birth_note);
    }
    else{
      notelength = 100;
    }
    int timelength;
    if(sizeof(birth_time)/sizeof(*birth_time) <= 100){
      timelength = sizeof(birth_time)/sizeof(*birth_time);
    }
    else{
      timelength = 100;
    }
    if(sizeof(birth_tap)/sizeof(*birth_tap) <= 70){
      taplength = sizeof(birth_tap)/sizeof(*birth_tap);
    }
    else{
      taplength = 120;
    }
    for(int i = 0; i < notelength; i++){ notes[i] = birth_note[i]; }
    for(int i = 0; i < timelength; i++){ times[i] = birth_time[i]; }
    for(int i = 0; i < taplength; i++){ tap[i] = birth_tap[i]; }
  }
}

void loop() {
  if(menustate == 0){
    menudisplay();
  }
  if(menustate == 1){
    normalplay();
  }
  if(menustate == 2){
    if(tilestate == 0){
      tilemenu();
    }
    if(tilestate == 1){
      tileplay();
    }
  }

//  if(digitalRead(DOO) == HIGH && holdDOTime >= 6000){
//    menustate = 0;
//    holdDOTime = 0;
//  }
//  else if(digitalRead(DOO) == HIGH && holdDOTime < 6000){
//    holdDOTime+=20;
//  }
//  else{
//    holdDOTime = 0;
//  }
}
