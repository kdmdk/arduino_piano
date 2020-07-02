#include <MozziGuts.h>
#include <Oscil.h>
#include <tables/SIN8192_int8.h>

#include <mozzi_midi.h>
#include <ADSR.h>
#include <EventDelay.h>
ADSR <AUDIO_RATE, AUDIO_RATE> envelope_A;
ADSR <AUDIO_RATE, AUDIO_RATE> envelope_B;

ADSR <AUDIO_RATE, AUDIO_RATE> envelope_C;
ADSR <AUDIO_RATE, AUDIO_RATE> envelope_D;
ADSR <AUDIO_RATE, AUDIO_RATE> envelope_E;
ADSR <AUDIO_RATE, AUDIO_RATE> envelope_F;

unsigned int Dur, Atk, Dec, Sus, Rel;//ADSRの長さを入れておく変数
unsigned int Dur2, Atk2, Dec2, Sus2, Rel2;//ADSRの長さを入れておく変数
Oscil <SIN8192_NUM_CELLS, AUDIO_RATE> aSIN(SIN8192_DATA);
Oscil <SIN8192_NUM_CELLS, AUDIO_RATE> bSIN(SIN8192_DATA);
Oscil <SIN8192_NUM_CELLS, AUDIO_RATE> cSIN(SIN8192_DATA);
Oscil <SIN8192_NUM_CELLS, AUDIO_RATE> dSIN(SIN8192_DATA);
Oscil <SIN8192_NUM_CELLS, AUDIO_RATE> eSIN(SIN8192_DATA);
Oscil <SIN8192_NUM_CELLS, AUDIO_RATE> fSIN(SIN8192_DATA);

Oscil <SIN8192_NUM_CELLS, AUDIO_RATE> gSIN(SIN8192_DATA);
Oscil <SIN8192_NUM_CELLS, AUDIO_RATE> hSIN(SIN8192_DATA);
Oscil <SIN8192_NUM_CELLS, AUDIO_RATE> iSIN(SIN8192_DATA);
Oscil <SIN8192_NUM_CELLS, AUDIO_RATE> jSIN(SIN8192_DATA);
Oscil <SIN8192_NUM_CELLS, AUDIO_RATE> kSIN(SIN8192_DATA);

#include <tables/SAW256_int8.h>

Oscil <SAW256_NUM_CELLS, AUDIO_RATE> aSAW(SAW256_DATA);
Oscil <SAW256_NUM_CELLS, AUDIO_RATE> bSAW(SAW256_DATA);
Oscil <SAW256_NUM_CELLS, AUDIO_RATE> cSAW(SAW256_DATA);
Oscil <SAW256_NUM_CELLS, AUDIO_RATE> dSAW(SAW256_DATA);
Oscil <SAW256_NUM_CELLS, AUDIO_RATE> eSAW(SAW256_DATA);
Oscil <SAW256_NUM_CELLS, AUDIO_RATE> fSAW(SAW256_DATA);

#include <tables/square_no_alias_2048_int8.h>

Oscil <SQUARE_NO_ALIAS_2048_NUM_CELLS, AUDIO_RATE> aSQ(SQUARE_NO_ALIAS_2048_DATA);
Oscil <SQUARE_NO_ALIAS_2048_NUM_CELLS, AUDIO_RATE> bSQ(SQUARE_NO_ALIAS_2048_DATA);
Oscil <SQUARE_NO_ALIAS_2048_NUM_CELLS, AUDIO_RATE> cSQ(SQUARE_NO_ALIAS_2048_DATA);
Oscil <SQUARE_NO_ALIAS_2048_NUM_CELLS, AUDIO_RATE> dSQ(SQUARE_NO_ALIAS_2048_DATA);
Oscil <SQUARE_NO_ALIAS_2048_NUM_CELLS, AUDIO_RATE> eSQ(SQUARE_NO_ALIAS_2048_DATA);
Oscil <SQUARE_NO_ALIAS_2048_NUM_CELLS, AUDIO_RATE> fSQ(SQUARE_NO_ALIAS_2048_DATA);

#define CONTROL_RATE 64
#define g 0.9

byte gain_aSIN;
byte gain_bSIN;
byte gain_cSIN;
byte gain_dSIN;
byte gain_eSIN;
byte gain_fSIN;

#include "pitches.h"

//
#define MISO 12     // データの入力ピン(74HC165-QH)
#define SCK  13     // クロック出力ピン(74HC165-CK)
#define SL   10     // レジスタロードピン(74HC165-SL)
#define MISO2 7
#define SCK2 8
#define SL2 11
byte ShiftData ;
byte ShiftData2;
// 74HC165のデータを読込む(受信する)関数
// DataPin ：データの入力ピン
// clockPin：クロック出力ピン
// loadPin ：レジスタロードピン
byte ShiftIn(int dataPin, int clockPin, int loadPin)
{
  unsigned char x ;
  int  i ;

  x = 0 ;
  digitalWrite(loadPin, LOW) ;         // 入力端子(A-H)の情報をレジスタに読込めと指示する
  digitalWrite(loadPin, HIGH) ;
  x = x | (digitalRead(dataPin) << 7) ;// H端子の内容を受信する
  for (i = 6 ; i >= 0 ; i--) {         // G端子～A端子まで繰り返す
    digitalWrite(clockPin, HIGH) ;        // 1ビットシフト指示を出す
    digitalWrite(clockPin, LOW) ;
    x = x | (digitalRead(dataPin) << i) ; // シフトされた内容を受信する
  }
  return x ;
}

int SW = 0;

EventDelay timer_A;
EventDelay timer_B;
EventDelay timer_C;

//
void setup() {
  // put your setup code here, to run once:
  startMozzi(CONTROL_RATE);
  pinMode(A0, INPUT_PULLUP);
  pinMode(A1, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);
  pinMode(A3, INPUT_PULLUP);
  pinMode(A4, INPUT_PULLUP);
  pinMode(A5, INPUT_PULLUP);
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  /*
    pinMode(3, INPUT_PULLUP);
    pinMode(4, INPUT_PULLUP);
    pinMode(5, INPUT_PULLUP);
  */
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);

  pinMode(6, INPUT_PULLUP);
  //pinMode(10, INPUT_PULLUP);
  //pinMode(11, INPUT_PULLUP);

  //pinMode(13,OUTPUT);
  //Serial.begin(9600);

  //
  // 74HC165のピン情報初期化
  pinMode(MISO, INPUT) ;
  pinMode(SCK,  OUTPUT) ;
  pinMode(SL,   OUTPUT) ;
  digitalWrite(SL, HIGH) ;
  digitalWrite(SCK, LOW) ;
  // ３秒後に開始
  //delay(3000) ;
  // 74HC165入力端子のデータを読込み表示を行う
  ShiftData = ShiftIn(MISO, SCK, SL) ;
  pinMode(MISO2, INPUT) ;
  pinMode(SCK2,  OUTPUT) ;
  pinMode(SL2,   OUTPUT) ;
  digitalWrite(SL2, HIGH) ;
  digitalWrite(SCK2, LOW) ;
  ShiftData2 = ShiftIn(MISO2, SCK2, SL2) ;
  //  Serial.println(ShiftData, BIN);
  //pinMode(9, OUTPUT);
  //Serial.begin(9600);
  //

  timer_A.set(500);
  timer_A.start();
  timer_B.set(750);
  timer_B.start();
  timer_C.set(1500);
  timer_C.start();

  Atk = 50;
  Dec = 10;
  Sus = 50;
  Rel = 50;
  Atk2 = 100;
  Dec2 = 100;
  Sus2 = 100;
  Rel2 = 100;
  envelope_A.setTimes(Atk, Dec, Sus, Rel);
  envelope_A.setADLevels(255, 128);
  envelope_B.setTimes(Atk2, Dec2, Sus2, Rel2);
  envelope_B.setADLevels(255, 128);

  envelope_C.setTimes(Atk, Dec, Sus, Rel);
  envelope_C.setADLevels(255, 128);
  envelope_D.setTimes(Atk, Dec, Sus, Rel);
  envelope_D.setADLevels(255, 128);
  envelope_E.setTimes(Atk, Dec, Sus, Rel);
  envelope_E.setADLevels(255, 128);
  envelope_F.setTimes(Atk, Dec, Sus, Rel);
  envelope_F.setADLevels(255, 128);
}
byte sound[25] = {0};
byte num = 0;
byte n = 0;

byte phase1 = 0;
byte phase2 = 0;
byte phase3 = 0;
byte phase4 = 0;
byte phase5 = 0;
byte phase6 = 0;
byte phase7 = 0;
byte phase8 = 0;
byte phase9 = 0;
byte phase10 = 0;
byte phase11 = 0;
byte phase12 = 0;

byte phA, phB, phC, phD, phE, ph1, ph2, ph3, ph4, ph5, ph6;

//きらきら星の音階
byte p1[] = {72, 72, 79, 79, 81, 81};
byte p2[] = {79, 79};
byte p3[] = {77, 77, 76, 76, 74, 74};
byte p4[] = {72, 72};
byte p5[] = {79, 79, 77, 77, 76, 76};
byte p6[] = {74, 74};

byte sub_A1[] = {64, 64, 62, 62, 64, 64};
byte sub_A2[] = {60, 60, 59, 59, 60, 60};
byte sub_A3[] = {59, 59, 55, 55, 57, 57};
byte sub_A4[] = {55, 55, 53, 53, 53, 53};

byte sub_B1[] = {64, 64};
byte sub_B2[] = {60, 60};
byte sub_B3[] = {59, 59};
byte sub_B4[] = {55, 55};

byte sub_C1[] = {64, 64, 64, 64, 62, 62};
byte sub_C2[] = {60, 60, 60, 60, 59, 59};
byte sub_C3[] = {57, 57, 59, 59, 55, 55};
byte sub_C4[] = {53, 53, 55, 55, 53, 53};

byte sub_D1[] = {64, 64};
byte sub_D2[] = {60, 60};
byte sub_D3[] = {59, 59};
byte sub_D4[] = {55, 55};

byte sub_E1[] = {64, 64, 64, 64, 64, 64};
byte sub_E2[] = {60, 60, 60, 60, 60, 60};
byte sub_E3[] = {59, 59, 57, 57, 59, 59};
byte sub_E4[] = {55, 55, 53, 53, 55, 55};

byte sub_F1[] = {62, 62};
byte sub_F2[] = {59, 59};
byte sub_F3[] = {55, 55};
byte sub_F4[] = {53, 53};


byte phase_A = 0;
byte phase_B = 0;

/*
  #define DATA_LED 5
  #define LATCH_LED 4
  #define CLOCK_LED 3
  byte Shift(byte leds){
  digitalWrite(LATCH_LED,LOW);
  shiftOut(DATA_LED,CLOCK_LED,LSBFIRST,leds);
  digitalWrite(LATCH_LED,HIGH);
  return leds;
  }
*/
void updateControl() {
  

  //オルゴールの音の処理
  if (SW == 3) {

    byte count = 6;
    if (n <= count) {
      if (timer_B.ready()) {

        byte note1 = sub_A1[phase1];
        byte note2 = sub_A2[phase1];
        byte note3 = sub_A3[phase1];
        byte note4 = sub_A4[phase1];

        phase1++;

        hSIN.setFreq(mtof(note1));
        envelope_C.noteOn();
        iSIN.setFreq(mtof(note2));
        envelope_D.noteOn();
        jSIN.setFreq(mtof(note3));
        envelope_E.noteOn();
        kSIN.setFreq(mtof(note4));
        envelope_F.noteOn();



        //byte note = pattern[phase_A];
        byte note = p1[phase_A];
        phase_A = (phase_A + 1) % 7;
        gSIN.setFreq(mtof(note));
        envelope_A.noteOn();
        Serial.print(note);
        Serial.println(n);
        n++;
        timer_B.start();
      }
    }
    if (n >= count + 1 && n <= count + 2) {
      if (timer_C.ready()) {

        byte note1 = sub_B1[phase2];
        byte note2 = sub_B2[phase2];
        byte note3 = sub_B3[phase2];
        byte note4 = sub_B4[phase2];

        phase2++;

        hSIN.setFreq(mtof(note1));
        envelope_C.noteOn();
        iSIN.setFreq(mtof(note2));
        envelope_D.noteOn();
        jSIN.setFreq(mtof(note3));
        envelope_E.noteOn();
        kSIN.setFreq(mtof(note4));
        envelope_F.noteOn();


        byte note = p2[phA];
        phA = (phA + 1) % 2;
        //int note = pattern[phase1];
        //phase1 = phase1 + 1;
        gSIN.setFreq(mtof(note));
        envelope_B.noteOn();
        Serial.print(note);
        Serial.println(n);
        n++;
        timer_C.start();
      }
      /*
        if (timer_B.ready()) {
        byte note1 = sub_B1[phase2];
        byte note2 = sub_B2[phase2];
        byte note3 = sub_B3[phase2];
        byte note4 = sub_B4[phase2];

        phase2++;

        hSIN.setFreq(mtof(note1));
        envelope_C.noteOn();
        iSIN.setFreq(mtof(note2));
        envelope_D.noteOn();
        jSIN.setFreq(mtof(note3));
        envelope_E.noteOn();
        kSIN.setFreq(mtof(note4));
        envelope_F.noteOn();
        timer_B.start();
        }
      */
    }
    if (n >= count + 3 && n <= count + 9) {
      if (timer_B.ready()) {
        byte note1 = sub_C1[phase3];
        byte note2 = sub_C2[phase3];
        byte note3 = sub_C3[phase3];
        byte note4 = sub_C4[phase3];

        phase3++;

        hSIN.setFreq(mtof(note1));
        envelope_C.noteOn();
        iSIN.setFreq(mtof(note2));
        envelope_D.noteOn();
        jSIN.setFreq(mtof(note3));
        envelope_E.noteOn();
        kSIN.setFreq(mtof(note4));
        envelope_F.noteOn();


        byte note = p3[phB];
        phB = (phB + 1) % 6;
        //int note = pattern[phase2];
        //phase2 = phase2 + 1;
        gSIN.setFreq(mtof(note));
        envelope_A.noteOn();
        Serial.print(note);
        Serial.println(n);
        n++;
        timer_B.start();
      }
    }
    if (n >= count + 10 && n <= count + 11) {
      if (timer_C.ready()) {

        byte note1 = sub_D1[phase4];
        byte note2 = sub_D2[phase4];
        byte note3 = sub_D3[phase4];
        byte note4 = sub_D4[phase4];

        phase4++;

        hSIN.setFreq(mtof(note1));
        envelope_C.noteOn();
        iSIN.setFreq(mtof(note2));
        envelope_D.noteOn();
        jSIN.setFreq(mtof(note3));
        envelope_E.noteOn();
        kSIN.setFreq(mtof(note4));
        envelope_F.noteOn();


        byte note = p4[phC];
        phC = (phase_A + 1) % 2;
        //int note = pattern[phase3];
        //phase3 = phase3 + 1;
        gSIN.setFreq(mtof(note));
        envelope_B.noteOn();
        Serial.print(note);
        Serial.println(n);
        n++;
        timer_C.start();
      }
      /*
        if (timer_B.ready()) {

        byte note1 = sub_D1[phase4];
        byte note2 = sub_D2[phase4];
        byte note3 = sub_D3[phase4];
        byte note4 = sub_D4[phase4];

        phase4++;

        hSIN.setFreq(mtof(note1));
        envelope_C.noteOn();
        iSIN.setFreq(mtof(note2));
        envelope_D.noteOn();
        jSIN.setFreq(mtof(note3));
        envelope_E.noteOn();
        kSIN.setFreq(mtof(note4));
        envelope_F.noteOn();

        timer_B.start();
        }
      */
    }
    if (n >= count + 12 && n <= count + 18) {
      if (timer_B.ready()) {
        byte note1 = sub_E1[phase5];
        byte note2 = sub_E2[phase5];
        byte note3 = sub_E3[phase5];
        byte note4 = sub_E4[phase5];

        phase5++;

        hSIN.setFreq(mtof(note1));
        envelope_C.noteOn();
        iSIN.setFreq(mtof(note2));
        envelope_D.noteOn();
        jSIN.setFreq(mtof(note3));
        envelope_E.noteOn();
        kSIN.setFreq(mtof(note4));
        envelope_F.noteOn();


        byte note = p5[phD];
        phD = (phD + 1) % 6;
        //int note = pattern[phase4];
        //phase4 = phase4 + 1;
        gSIN.setFreq(mtof(note));
        envelope_A.noteOn();
        Serial.print(note);
        Serial.println(n);
        n++;
        timer_B.start();
      }
    }
    if (n >= count + 19 && n <= count + 20) {
      if (timer_C.ready()) {

        byte note1 = sub_F1[phase6];
        byte note2 = sub_F2[phase6];
        byte note3 = sub_F3[phase6];
        byte note4 = sub_F4[phase6];

        phase6++;

        hSIN.setFreq(mtof(note1));
        envelope_C.noteOn();
        iSIN.setFreq(mtof(note2));
        envelope_D.noteOn();
        jSIN.setFreq(mtof(note3));
        envelope_E.noteOn();
        kSIN.setFreq(mtof(note4));
        envelope_F.noteOn();

        byte note = p6[phE];
        phE = (phE + 1) % 2;
        //int note = pattern[phase5];
        //phase5 = phase5 + 1;
        gSIN.setFreq(mtof(note));
        envelope_B.noteOn();
        Serial.print(note);
        Serial.println(n);
        n++;
        timer_C.start();
      }/*
      if (timer_B.ready()) {
        byte note1 = sub_F1[phase6];
        byte note2 = sub_F2[phase6];
        byte note3 = sub_F3[phase6];
        byte note4 = sub_F4[phase6];

        phase6++;

        hSIN.setFreq(mtof(note1));
        envelope_C.noteOn();
        iSIN.setFreq(mtof(note2));
        envelope_D.noteOn();
        jSIN.setFreq(mtof(note3));
        envelope_E.noteOn();
        kSIN.setFreq(mtof(note4));
        envelope_F.noteOn();
        timer_B.start();
      }
*/
    }
    // ココカラ

    if (n >= count + 21 && n <= count + 27) {
      if (timer_B.ready()) {
        byte note1 = sub_E1[phase7];
        byte note2 = sub_E2[phase7];
        byte note3 = sub_E3[phase7];
        byte note4 = sub_E4[phase7];

        phase7++;

        hSIN.setFreq(mtof(note1));
        envelope_C.noteOn();
        iSIN.setFreq(mtof(note2));
        envelope_D.noteOn();
        jSIN.setFreq(mtof(note3));
        envelope_E.noteOn();
        kSIN.setFreq(mtof(note4));
        envelope_F.noteOn();


        byte note = p5[ph1];
        ph1 = (ph1 + 1) % 7;
        //int note = pattern[phase6];
        //phase6 = phase6 + 1;
        gSIN.setFreq(mtof(note));
        envelope_A.noteOn();
        Serial.print(note);
        Serial.println(n);
        n++;
        timer_B.start();
      }
    }
    if (n >= count + 28 && n <= count + 29) {
      if (timer_C.ready()) {

        byte note1 = sub_F1[phase8];
        byte note2 = sub_F2[phase8];
        byte note3 = sub_F3[phase8];
        byte note4 = sub_F4[phase8];

        phase8++;

        hSIN.setFreq(mtof(note1));
        envelope_C.noteOn();
        iSIN.setFreq(mtof(note2));
        envelope_D.noteOn();
        jSIN.setFreq(mtof(note3));
        envelope_E.noteOn();
        kSIN.setFreq(mtof(note4));
        envelope_F.noteOn();


        byte note = p6[ph2];
        ph2 = (ph2 + 1) % 2;
        //int note = pattern[phase7];
        //phase7 = phase7 + 1;
        gSIN.setFreq(mtof(note));
        envelope_B.noteOn();
        Serial.print(note);
        Serial.println(n);
        n++;
        timer_C.start();
      }/*
      if (timer_B.ready()) {
        byte note1 = sub_F1[phase8];
        byte note2 = sub_F2[phase8];
        byte note3 = sub_F3[phase8];
        byte note4 = sub_F4[phase8];

        phase8++;

        hSIN.setFreq(mtof(note1));
        envelope_C.noteOn();
        iSIN.setFreq(mtof(note2));
        envelope_D.noteOn();
        jSIN.setFreq(mtof(note3));
        envelope_E.noteOn();
        kSIN.setFreq(mtof(note4));
        envelope_F.noteOn();
        timer_B.start();
      }
*/
    }
    if (n >= count + 30 && n <= count + 36) {
      if (timer_B.ready()) {
        byte note1 = sub_A1[phase9];
        byte note2 = sub_A2[phase9];
        byte note3 = sub_A3[phase9];
        byte note4 = sub_A4[phase9];

        phase9++;

        hSIN.setFreq(mtof(note1));
        envelope_C.noteOn();
        iSIN.setFreq(mtof(note2));
        envelope_D.noteOn();
        jSIN.setFreq(mtof(note3));
        envelope_E.noteOn();
        kSIN.setFreq(mtof(note4));
        envelope_F.noteOn();

        byte note = p1[ph3];
        ph3 = (ph3 + 1) % 7;
        //int note = pattern[phase8];
        //phase8 = phase8 + 1;
        gSIN.setFreq(mtof(note));
        envelope_A.noteOn();
        Serial.print(note);
        Serial.println(n);
        n++;
        timer_B.start();
      }
    }
    if (n >= count + 37 && n <= count + 38) {
      if (timer_C.ready()) {

        byte note1 = sub_B1[phase10];
        byte note2 = sub_B2[phase10];
        byte note3 = sub_B3[phase10];
        byte note4 = sub_B4[phase10];

        phase10++;

        hSIN.setFreq(mtof(note1));
        envelope_C.noteOn();
        iSIN.setFreq(mtof(note2));
        envelope_D.noteOn();
        jSIN.setFreq(mtof(note3));
        envelope_E.noteOn();
        kSIN.setFreq(mtof(note4));
        envelope_F.noteOn();


        byte note = p2[ph4];
        ph4 = (ph4 + 1) % 2;
        //int note = pattern[phase7];
        //phase7 = phase7 + 1;
        gSIN.setFreq(mtof(note));
        envelope_B.noteOn();
        Serial.print(note);
        Serial.println(n);
        n++;
        timer_C.start();
      }/*
      if (timer_B.ready()) {
        byte note1 = sub_B1[phase10];
        byte note2 = sub_B2[phase10];
        byte note3 = sub_B3[phase10];
        byte note4 = sub_B4[phase10];

        phase10++;

        hSIN.setFreq(mtof(note1));
        envelope_C.noteOn();
        iSIN.setFreq(mtof(note2));
        envelope_D.noteOn();
        jSIN.setFreq(mtof(note3));
        envelope_E.noteOn();
        kSIN.setFreq(mtof(note4));
        envelope_F.noteOn();
        timer_B.start();
      }
*/
    }
    if (n >= count + 39 && n <= count + 45) {
      if (timer_B.ready()) {
        byte note1 = sub_C1[phase11];
        byte note2 = sub_C2[phase11];
        byte note3 = sub_C3[phase11];
        byte note4 = sub_C4[phase11];

        phase11++;

        hSIN.setFreq(mtof(note1));
        envelope_C.noteOn();
        iSIN.setFreq(mtof(note2));
        envelope_D.noteOn();
        jSIN.setFreq(mtof(note3));
        envelope_E.noteOn();
        kSIN.setFreq(mtof(note4));
        envelope_F.noteOn();

        byte note = p3[ph5];
        ph5 = (ph5 + 1) % 7;
        //int note = pattern[phase9];
        //phase9 = phase9 + 1;
        gSIN.setFreq(mtof(note));
        envelope_A.noteOn();
        Serial.print(note);
        Serial.println(n);
        n++;
        timer_B.start();
      }
    }
    if (n >= count + 46 && n <= count + 47) {
      if (timer_C.ready()) {

        byte note1 = sub_D1[phase12];
        byte note2 = sub_D2[phase12];
        byte note3 = sub_D3[phase12];
        byte note4 = sub_D4[phase12];

        phase12++;

        hSIN.setFreq(mtof(note1));
        envelope_C.noteOn();
        iSIN.setFreq(mtof(note2));
        envelope_D.noteOn();
        jSIN.setFreq(mtof(note3));
        envelope_E.noteOn();
        kSIN.setFreq(mtof(note4));
        envelope_F.noteOn();


        byte note = p4[ph6];
        ph6 = (ph6 + 1) % 2;
        //int note = pattern[phase10];
        //phase10 = phase10 + 1;
        gSIN.setFreq(mtof(note));
        envelope_B.noteOn();
        Serial.print(note);
        Serial.println(n);
        n++;
        timer_C.start();
      }/*
      if (timer_B.ready()) {
        byte note1 = sub_D1[phase12];
        byte note2 = sub_D2[phase12];
        byte note3 = sub_D3[phase12];
        byte note4 = sub_D4[phase12];

        phase12++;

        hSIN.setFreq(mtof(note1));
        envelope_C.noteOn();
        iSIN.setFreq(mtof(note2));
        envelope_D.noteOn();
        jSIN.setFreq(mtof(note3));
        envelope_E.noteOn();
        kSIN.setFreq(mtof(note4));
        envelope_F.noteOn();
        timer_B.start();
      }
*/
    }
    //54音鳴らした後の処理
    if (n >= 54) {
      n = 0;
      phase_A = 0;

      phase1 = 0;
      phase2 = 0;
      phase3 = 0;
      phase4 = 0;
      phase5 = 0;
      phase6 = 0;
      phase7 = 0;
      phase8 = 0;
      phase9 = 0;
      phase10 = 0;
      phase11 = 0;
      phase12 = 0;

      ph1 = 0;
      ph2 = 0;
      ph3 = 0;
      ph4 = 0;
      ph5 = 0;
      ph6 = 0;
      phA = 0;
      phB = 0;
      phC = 0;
      phD = 0;
      phD = 0;
      phE = 0;
    }

    //timer_B.start();

  }
  //隠しコマンドでオルゴール
  if (digitalRead(6) == LOW && digitalRead(2) == LOW)
  {
    SW = 3;
    digitalWrite(3, HIGH);
    digitalWrite(4, HIGH);
    digitalWrite(5, HIGH);
  }

  //切り替えスイッチ
  //byte leds = 0;
  //leds = Shift(leds);
  if (digitalRead(6) == LOW) {
    if (timer_A.ready()) {
      if (SW == 2) {
        SW = 0;
        //bitSet(leds,0);
        //bitWrite(leds,1,1);
        //bitWrite(leds,1,0);
        //bitWrite(leds,2,0);
        //leds = Shift(leds);
        digitalWrite(3, HIGH);
        digitalWrite(4, LOW);
        digitalWrite(5, LOW);
      } else if (SW == 1) {
        SW = 2;
        //bitSet(leds,1);
        //bitWrite(leds,0,0);
        //bitWrite(leds,1,1);
        //bitWrite(leds,2,0);
        //leds = Shift(leds);
        digitalWrite(3, LOW);
        digitalWrite(4, HIGH);
        digitalWrite(5, LOW);
      } else {
        SW = 1;
        //bitSet(leds,2);
        //bitWrite(leds,0,0);
        //bitWrite(leds,1,0);
        //bitWrite(leds,2,1);
        //leds = Shift(leds);
        digitalWrite(3, LOW);
        digitalWrite(4, LOW);
        digitalWrite(5, HIGH);
      } 
      timer_A.start();
    }
    //delay(500);
  }
  //Serial.println(mozziAnalogRead(A0));

  byte dt ;
  byte dt2;
  // 74HC165入力端子のデータを読込む
  dt = ShiftIn(MISO, SCK, SL) ;
  dt2 = ShiftIn(MISO2, SCK2, SL2);

  if ( dt != ShiftData ) {
    // データに変化が有れば表示する
    ShiftData = dt ;
    //Serial.println(dt, BIN) ;

  }
  if ( dt2 != ShiftData2 ) {
    // データに変化が有れば表示する
    ShiftData2 = dt2 ;
    //Serial.println(dt2, BIN) ;
  }

  int ary[8] = {0};
  int ary2[8] = {0};
  //aryはdtを2進数にして1か0の入った配列を作る
  for (int j = 0; j < 8; j++)
  {
    ary[j] = dt % 2;
    dt /= 2;
    ary2[j] = dt2 % 2;
    dt2 /= 2;
  }


  //ピアノのキーを配列に格納

  //A 音C5
  if (ary[0] == 0)
  {
    sound[0] = 1;

  } else {
    sound[0] = 0;

  }
  //B 音D5
  if (ary[1] == 0)
  {
    sound[1] = 2;

  } else {
    sound[1] = 0;

  }
  //C 音E5
  if (ary[2] == 0)
  {
    sound[2] = 3;

  } else {
    sound[2] = 0;

  }
  //D 音F5
  if (ary[3] == 0)
  {
    sound[3] = 4;

  } else {
    sound[3] = 0;

  }
  //E G5
  if (ary[4] == 0)
  {
    sound[4] = 5;

  } else {
    sound[4] = 0;

  }
  //F A5
  if (ary[5] == 0)
  {
    sound[5] = 6;

  } else {
    sound[5] = 0;

  }
  //G B5
  if (ary[6] == 0)
  {
    sound[6] = 7;

  } else {
    sound[6] = 0;

  }
  //H C6
  if (ary[7] == 0)
  {
    sound[7] = 8;

  } else {
    sound[7] = 0;

  }
  //レジスタ2 A D6
  if (ary2[0] == 0)
  {
    sound[8] = 9;

  } else {
    sound[8] = 0;

  }

  //レジスタ2 B E6
  if (ary2[1] == 0)
  {
    sound[9] = 10;

  } else {
    sound[9] = 0;

  }
  //レジスタ2 C F6
  if (ary2[2] == 0)
  {
    sound[10] = 11;

  } else {
    sound[10] = 0;

  }
  //レジスタ2 D G6
  if (ary2[3] == 0)
  {
    sound[11] = 12;

  } else {
    sound[11] = 0;

  }

  //音がおかしい

  //レジスタ2 E A6
  if (ary2[4] == 0)
  {
    sound[12] = 13;

  } else {
    sound[12] = 0;

  }

  //レジスタ2 F B6
  if (ary2[5] == 0)
  {
    sound[13] = 14;

  } else {
    sound[13] = 0;

  }

  //レジスタ2 G C7
  if (ary2[6] == 0)
  {
    sound[14] = 15;

  } else {
    sound[14] = 0;

  }

  //レジスタ2 H CS5
  if (ary2[7] == 0)
  {
    sound[15] = 16;

  } else {
    sound[15] = 0;

  }
  if (mozziAnalogRead(A0) < 800) {
    sound[16] = 17;
  } else {
    sound[16] = 0;
  }

  if (mozziAnalogRead(A1) < 800) {
    sound[17] = 18;
  } else {
    sound[17] = 0;
  }
  if (mozziAnalogRead(A2) < 800) {
    sound[18] = 19;
  } else {
    sound[18] = 0;
  }
  if (mozziAnalogRead(A3) < 800) {
    sound[19] = 20;
  } else {
    sound[19] = 0;
  }
  if (mozziAnalogRead(A4) < 800) {
    sound[20] = 21;
  } else {
    sound[20] = 0;
  }
  if (mozziAnalogRead(A5) < 800) {
    sound[21] = 22;
  } else {
    sound[21] = 0;
  }
  if (digitalRead(0) == LOW) {
    sound[22] = 23;
  } else {
    sound[22] = 0;
  }
  if (digitalRead(1) == LOW) {
    sound[23] = 24;
  } else {
    sound[23] = 0;
  }
  if (digitalRead(2) == LOW) {
    sound[24] = 25;
  } else {
    sound[24] = 0;
  }


  int soundCount = 0;
  int setScale;

  //キーに対応した音をセット
  for (int i = 0; i < 25; i++)
  {
    if (sound[i] != 0 && soundCount != 6)
    {
      switch (sound[i])
      {
        case 1:
          {
            setScale = NOTE_C5;
            break;
          }
        case 2:
          {
            setScale = NOTE_D5;
            break;
          }
        case 3:
          {
            setScale = NOTE_E5;
            break;
          }
        case 4:
          {
            setScale = NOTE_F5;
            break;
          }
        case 5:
          {
            setScale = NOTE_G5;
            break;
          }
        case 6:
          {
            setScale = NOTE_A5;
            break;
          }
        case 7:
          {
            setScale = NOTE_B5;
            break;
          }
        case 8:
          {
            setScale = NOTE_C6;
            break;
          }
        case 9:
          {
            setScale = NOTE_D6;
            break;
          }
        case 10:
          {
            setScale = NOTE_E6;
            break;
          }
        case 11:
          {
            setScale = NOTE_F6;
            break;
          }
        case 12:
          {
            setScale = NOTE_G6;
            break;
          }
        case 13:
          {
            setScale = NOTE_A6;
            break;
          }
        case 14:
          {
            setScale = NOTE_B6;
            break;
          }
        case 15:
          {
            setScale = NOTE_C7;
            break;
          }

        case 16:
          {
            setScale = NOTE_CS5;
            break;
          }
        case 17:
          {
            setScale = NOTE_DS5;
            break;
          }
        case 18:
          {
            setScale = NOTE_FS5;
            break;
          }
        case 19:
          {
            setScale = NOTE_GS5;
            break;
          }
        case 20:
          {
            setScale = NOTE_AS5;
            break;
          }
        case 21:
          {
            setScale = NOTE_CS6;
            break;
          }
        case 22:
          {
            setScale = NOTE_DS6;
            break;
          }
        case 23:
          {
            setScale = NOTE_FS6;
            break;
          }
        case 24:
          {
            setScale = NOTE_GS6;
            break;
          }
        case 25:
          {
            setScale = NOTE_AS6;
            break;
          }
        default:
          {
            break;
          }
      }
      soundCount++;

      //6音(a~f)
      switch (soundCount)
      {
        case 1:
          {
            aSIN.setFreq(setScale);
            aSAW.setFreq(setScale);
            aSQ.setFreq(setScale);
            gain_aSIN = 255;
            break;
          }
        case 2:
          {
            bSIN.setFreq(setScale);
            bSAW.setFreq(setScale);
            bSQ.setFreq(setScale);

            gain_bSIN = 255;
            break;
          }
        case 3:
          {
            cSIN.setFreq(setScale);
            cSAW.setFreq(setScale);
            cSQ.setFreq(setScale);
            gain_cSIN = 255;
            break;
          }
        case 4:
          {
            dSIN.setFreq(setScale);
            dSAW.setFreq(setScale);
            cSQ.setFreq(setScale);

            gain_dSIN = 255;
            break;
          }
        case 5:
          {
            eSIN.setFreq(setScale);
            eSAW.setFreq(setScale);
            eSQ.setFreq(setScale);

            gain_eSIN = 255;
            break;
          }
        case 6:
          {
            fSIN.setFreq(setScale);
            fSAW.setFreq(setScale);
            fSQ.setFreq(setScale);

            gain_fSIN = 255;
            break;
          }
        default:
          {
            break;
          }
      }
    }
  }

  num = soundCount;

  gain_aSIN *= g;
  gain_bSIN *= g;
  gain_cSIN *= g;
  gain_dSIN *= g;
  gain_eSIN *= g;
  gain_fSIN *= g;


}
int updateAudio() {

  //スイッチごとの音の処理
  if (SW == 0) {
    return (((aSIN.next() * gain_aSIN) >> 8) +
            ((bSIN.next() * gain_bSIN) >> 8) +
            ((cSIN.next() * gain_cSIN) >> 8) +
            ((dSIN.next() * gain_dSIN) >> 8) +
            ((eSIN.next() * gain_eSIN) >> 8) +
            ((fSIN.next() * gain_fSIN) >> 8) ) / 6;

  } else if (SW == 1) {
    return (((aSQ.next() * gain_aSIN) >> 8) +
            ((bSQ.next() * gain_bSIN) >> 8) +
            ((cSQ.next() * gain_cSIN) >> 8) +
            ((dSQ.next() * gain_dSIN) >> 8) +
            ((eSQ.next() * gain_eSIN) >> 8) +
            ((fSQ.next() * gain_fSIN) >> 8) ) / 6;
  } else if (SW == 2) {
    return (((aSAW.next() * gain_aSIN) >> 8) +
            ((bSAW.next() * gain_bSIN) >> 8) +
            ((cSAW.next() * gain_cSIN) >> 8) +
            ((dSAW.next() * gain_dSIN) >> 8) +
            ((eSAW.next() * gain_eSIN) >> 8) +
            ((fSAW.next() * gain_fSIN) >> 8) ) / 6;
  } else if (SW == 3) {

    //すべての音の処理をすると音が汚くなるので3音までにしている
    envelope_A.update();
    int A = (envelope_A.next() * gSIN.next()) >> 8;
    /*
      envelope_B.update();
      int B = (envelope_B.next() * gSIN.next()) >> 8;
    */

    envelope_C.update();
    int C = (envelope_C.next() * hSIN.next()) >> 8;

    envelope_D.update();
    int D = (envelope_D.next() * iSIN.next()) >> 8;

    /*
      envelope_E.update();
      int E = (envelope_E.next() * jSIN.next()) >> 8;
      /*
      envelope_F.update();
      int F = (envelope_F.next() * kSIN.next()) >> 8;
    */
    return (A + C + D) / 6;
  }

}
void loop() {
  // put your main code here, to run repeatedly:
  audioHook();
}
