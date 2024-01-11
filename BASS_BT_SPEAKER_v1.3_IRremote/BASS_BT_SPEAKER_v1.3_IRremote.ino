/*
 * AWS
 *    /update
 *    /rain                       -   switch rainbow
 *    /setpot - (nr & val)        -   usraw pot x na 0-99
 *    /givepots                   -   wartosci potow
 *    /relays - (stereo,bass)     -   ustaw relay x na 0-1
 *    /setled                     -   ustaw led (R,G,B)
 *    /ledamps - (min,max)        -   min/max dla VU MIC
 *    /vuCol
 */
#define DECODE_SONY // IR dla pilota od kamerki
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
TaskHandle_t Task1;
#include "Arduino.h"
#include "X9C10X.h"
#include "Kronos.h"
#include "JSONcreator.h"
#include "AiEsp32RotaryEncoder.h"
//tapeta bg1.jpg
#include <SPI.h>
#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite(&tft);
TFT_eSprite sprTxt = TFT_eSprite(&tft);

#include "PinDefinitionsAndMore.h"
#include <IRremote.hpp>

#include <FastLED.h>
#define NUM_LEDS 105 
#define DATA_PIN 32
CRGB leds[NUM_LEDS];

#include "WiFi.h"
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"
#include <AsyncElegantOTA.h>
AsyncWebServer server(80);

const char* ssid = "RagNet";
const char* password =  "azgard666";
// spobuje ustawic statyczne IP
IPAddress local_IP(192, 168, 1, 75);
IPAddress gateway(192, 168, 1 ,1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8); 
IPAddress secondaryDNS(8, 8, 4, 4);

X9C10X pot0(10000);  //  10 KΩ
X9C10X pot1(10000);
X9C10X pot2(10000);
X9C10X pot3(10000);

int M_LOUD = 32;
int M_BASAMP = 50;
int M_SPKAMP = 50;
int M_CUTOFF = 85;
int M_LOUDp = 32;
int M_BASAMPp = 50;
int M_SPKAMPp = 50;
int M_CUTOFFp = 85;

const int MAX_LAUDNESS = 90;

#define RELAYS_SPK 22
#define RELAYS_BAS 21

#define ENC0 35
#define ENC1 34
#define ENCSW 39
#define MIC 36
#define TFT_LED 4
#define ROTARY_ENCODER_STEPS 4
AiEsp32RotaryEncoder ENC = AiEsp32RotaryEncoder(ENC0, ENC1, ENCSW, -1, ROTARY_ENCODER_STEPS);

void IRAM_ATTR readEncoderISR() { ENC.readEncoder_ISR(); }

uint8_t direction = LOW;
uint8_t step = 1;

uint16_t tx = 0, ty = 0;
boolean pressed;

 bool RAINBOW = false;
 bool VU = false;
 bool STATIC_LIGHTS = false;
 bool FFT_ON = false;
 int  VUMODE = 0; // 0-3
 
class myLed
{
  public:
  myLed(byte H, byte S, byte V) : h(H), s(S), v(V) {};
  myLed() : h(0), s(0), v(0) {};
  ~myLed(){};

  byte h,s,v;
  byte r,g,b;
    
  void operator() (byte H, byte S, byte V) { this->h = H; this->s = S; this->v = V; }
  void operator = (myLed clone) { this->h = clone.h; this->s = clone.s; this->v = clone.v; }
  myLed operator+ (myLed clone)
  {
    myLed out;
    out.h = this->h + clone.h;
    int cha=static_cast<int>(this->s), chb=static_cast<int>(clone.s);
    if (cha + chb > 255) { out.s = 255; }
    else { out.s = this->s + clone.s; }
    cha=static_cast<int>(this->v), chb=static_cast<int>(clone.v);
    if (cha + chb > 255) { out.v = 255; }
    else { out.v = this->v + clone.v; }
    return out;
  }
  myLed operator- (myLed clone)
  {
    myLed out;
    out.h = this->h - clone.h;
    int cha=static_cast<int>(this->s), chb=static_cast<int>(clone.s);
    if (cha - chb < 0) { out.s = 0; }
    else { out.s = this->s - clone.s; }
    cha=static_cast<int>(this->v), chb=static_cast<int>(clone.v);
    if (cha - chb < 0) { out.v = 0; }
    else { out.v = this->v - clone.v; }
    return out;
  }
};

// Defaultowe paramy ledow
myLed BGcol = (myLed) {.h=0, .s=0, .v=0 };
myLed mRing = (myLed) {.h=1, .s=255, .v=0 };

myLed LeftL = (myLed) {.h=0, .s=255, .v=32 };
myLed LeftR = (myLed) {.h=0, .s=255, .v=32 };
myLed RightL = (myLed) {.h=0, .s=255, .v=32 };
myLed RightR = (myLed) {.h=0, .s=255, .v=32 };

#include "screen.h"

myLed LED[22][4];
myLed RING[17];
myLed Lmat1[22][1];
myLed Lmat2[22][1];

button      SUB_PLUS(">"), SUB_MINUS("<"), SPK_PLUS(">"), SPK_MINUS("<"), LOUD_PLUS(">"), LOUD_MINUS("<"), CUT_PLUS(">"), CUT_MINUS("<");
valDisplay  SUB_VALDISP, SPK_VALDISP, LOUD_VALDISP, CUT_VALDISP;
selectorFrame sFrame;
valBarDisp LOUD_VALBAR, SUB_VALBAR, SPK_VALBAR, CUT_VALBAR;
toggler SUB_REL_TOG("S.CH", true), LR_REL_TOG("L/R.CH", true);;

// STRONA CONFIG
radioBtn LEDFX_SELECTOR("LedFX");

// STRONY       // glowna(vol)          ustawienia                podswietlenie             oscylogram
button        SITE_MAIN("MAIN",500),    SITE_CONF("CONF",500),    SITE_LED("BLED",500),  SITE_OSC("OSC",500);
volatile bool SITE_DRAWN = false;
valBarDisp    FIRST_LOADING;

spkLedSelector virtualSpk;

int itemSelector = 0;
int siteSelector = 0;

#include "funcs.h" // startWIFI(), rgb2hsv(), hsv2hex(), fadeall(), mapled()


// Zmienne mikrofonu (VU)
uint16_t      MicOffset = 3470;
uint16_t      VU_MinAmp = 200;
uint16_t      VU_MaxAmp = 3000;
byte          VU_falloffMili = 5;
bool          isRingVU = true;

#include "arduinoFFT_f.h" // dla pojedynczej precyzji
#define SAMPLES 128
#define SAMPLING_FREQUENCY 36000

arduinoFFT FFT = arduinoFFT();
float         vReal[SAMPLES];
float         vImag[SAMPLES];

unsigned long  milliFFT = 0;
unsigned long  microsecFFT = 0x00;

unsigned int    SF = round(1000000/SAMPLING_FREQUENCY); // 26 us = ~38 000 Hz // 45 ~ 22kHz
int             sum; // wartosc z AnalogRead
byte            mapped[SAMPLES / 2];
byte            mapped_p[SAMPLES / 2] = {};
float           ADC_PRESCALAR = 1.0;
uint16_t        noise_reduction = 75;
uint16_t        fftMaxAmp = 3000;
uint16_t        fftMinAmp = 800;
float           norm_base = 0.77;
float           normalizer = norm_base;
byte            bin_reject = 3;
float           norm_gain = 0.008;  // base (0.00125) 
Kronos          FFT_FALOFF_DEL;

Kronos IRRemoteDel;
// Adresy ValElementow VU/FFT/etc.
ValElem *SETTINGS_OBJECTS[11];
//                          addr *val,         i/f vmin,  i/f vmax,      inc       const char *NAZWA
ValElem VE_VUMODE           ( &VUMODE,               0,      4,            1,       "VU: Mode");
ValElem VE_VU_MinAmp        ( &VU_MinAmp,           50,    500,           10,       "VU: MinAmp");
ValElem VE_VU_MaxAmp        ( &VU_MaxAmp,          750,   4000,           40,       "VU: MaxAmp");
ValElem VE_VU_falloffMili   ( &VU_falloffMili,       1,     50,            1,       "VU/FX: F.Of.ms");
ValElem VE_ADC_PRESCALAR    ( &ADC_PRESCALAR,      0.5,      2,        0.015,       "FFT: ADC PS.");
ValElem VE_noise_reduction  ( &noise_reduction,     25,    200,            5,       "FFT: Noise Red.");
ValElem VE_fftMinAmp        ( &fftMinAmp,          500,   1600,           20,       "FFT: MinAmp");
ValElem VE_fftMaxAmp        ( &fftMaxAmp,         1000,   4500,           40,       "FFT: MaxAmp");
ValElem VE_norm_base        ( &norm_base,          0.6,    1.2,         0.05,       "FFT: Norm.Base");
ValElem VE_norm_gain        ( &norm_gain,        0.001,  0.018,        0.002,       "FFT: Norm.Gain");
ValElem VE_bin_reject       ( &bin_reject,           0,      4,            1,       "FFT: BinReject");

int MODE_ITR = 3;

void setup() {
 #include "setup.h"
} // SETUP END
ValChngObjArrtList SETTINGS_MENU(SETTINGS_OBJECTS, 11);

uint16_t TFT_DIMTIME_SEC = 16;    // po ile sec ekran wygasa
unsigned long TFT_DIM_TIMER = 0; // tylko timer dla wygaszacza

bool wasHandRised = false;

unsigned long shortPressAfterMiliseconds = 50;
unsigned long longPressAfterMiliseconds = 300;
bool ENCclicked = false;
int ENCbtnDir = 1;

void on_button_short_click() { ENCclicked = true; ENCbtnDir = 1; }
void on_button_long_click() { ENCclicked = true; ENCbtnDir = 0; }

void handle_rotary_button() {
  static unsigned long lastTimeButtonDown = 0;
  static bool wasButtonDown = false;
  bool isEncoderButtonDown = ENC.isEncoderButtonDown();
  if (isEncoderButtonDown) {if (!wasButtonDown) {lastTimeButtonDown = millis();} wasButtonDown = true; return;}

  if (wasButtonDown) { if (millis() - lastTimeButtonDown >= longPressAfterMiliseconds) {
      on_button_long_click();
    } else if (millis() - lastTimeButtonDown >= shortPressAfterMiliseconds) { on_button_short_click(); }}
  wasButtonDown = false; }

void chkIfPotsChange()
{
  if (M_LOUDp != M_LOUD)
  { 
    pot2.setPosition(M_LOUD , true);
    // Aktywna tylko vol dla ENC kiedy widoczna inna niz 0 strona
    if (siteSelector == 0) 
    {
      LOUD_VALDISP.draw("Volume: ", " %", M_LOUD , 65, 10, 190, 15);
      LOUD_VALBAR.draw(M_LOUD, 0, MAX_LAUDNESS, 65, 25, 190, 15);
    }
    M_LOUDp = M_LOUD; 
  }
  if (M_BASAMPp != M_BASAMP)
  { 
    pot1.setPosition(M_BASAMP , true);
    if (siteSelector == 0) 
    {
    int MEDS = (50-M_BASAMP)*-2;
    if (MEDS > 0) { SUB_VALDISP.draw("Bass: +", " %", MEDS , 65, 55, 190, 15); }
    else { SUB_VALDISP.draw("Bass: ", " %", MEDS , 65, 55, 190, 15); }
    SUB_VALBAR.draw(M_BASAMP, 0, 99, 65, 70, 190, 15);
    }
    M_BASAMPp = M_BASAMP; 
  }
  if (M_SPKAMPp != M_SPKAMP)
  { 
    pot0.setPosition(M_SPKAMP , true);
    if (siteSelector == 0) 
    {
    int MEDT = (50-M_SPKAMP)*-2;
    if (MEDT > 0) { SPK_VALDISP.draw("Treble: +", " %", MEDT , 65, 100, 190, 15); }
    else { SPK_VALDISP.draw("Treble: ", " %", MEDT , 65, 100, 190, 15); }
    SPK_VALBAR.draw(M_SPKAMP, 0, 99, 65, 115, 190, 15);
    }
    M_SPKAMPp = M_SPKAMP;
  }
  if (M_CUTOFFp != M_CUTOFF)
  {
    pot3.setPosition(M_CUTOFF , true);
    if (siteSelector == 0) 
    {
    CUT_VALDISP.draw("Cutoff: ", " Hz", 20+(M_CUTOFF+1)*2 , 65, 145, 190, 15);
    CUT_VALBAR.draw(M_CUTOFF, 0, 99, 65, 160, 190, 15);
    }
    M_CUTOFFp = M_CUTOFF; 
  }
}

void siteChange()
{
  if (pressed && wasHandRised == true)
  {
  wasHandRised = false;
  switch(siteSelector)
  {
    case 0:
    {
      SITE_CONF.chkIfPress(tx,ty,[]()   // STRONA 0 -> 1
      {
        siteSelector = 1;
        SITE_DRAWN = false;
        return; });
      SITE_LED.chkIfPress(tx,ty,[]()    // STRONA 0 -> 2
      {
        siteSelector = 2;
        SITE_DRAWN = false;
        return; });
      break;
    }
    
    case 1:
    {
      SITE_MAIN.chkIfPress(tx,ty,[]()   // STRONA 1 -> 0
      {
        siteSelector = 0;
        SITE_DRAWN = false;
        return; });
      SITE_LED.chkIfPress(tx,ty,[]()    // STRONA 1 -> 2
      {
        siteSelector = 2;
        SITE_DRAWN = false;
        return; });
      LEDFX_SELECTOR.chkIfPress(tx,ty);
      break;
    }
    case 2:
    {
      SITE_MAIN.chkIfPress(tx,ty,[]()   // STRONA 2 -> 0
      {
        siteSelector = 0;
        SITE_DRAWN = false;
        return;
      });
      SITE_CONF.chkIfPress(tx,ty,[]()   // STRONA 2 -> 1
      {
        siteSelector = 1;
        SITE_DRAWN = false;
        return;
      });
      SITE_OSC.chkIfPress(tx,ty,[]()   // STRONA 2 -> 3
      {
        siteSelector = 3;
        SITE_DRAWN = false;
        return;
      });
      break;
    }
    case 3:
    {
      SITE_MAIN.chkIfPress(tx,ty,[]()   // STRONA 3 -> 0
      {
        siteSelector = 0;
        SITE_DRAWN = false;
        return;
      });
      SITE_CONF.chkIfPress(tx,ty,[]()   // STRONA 3 -> 1
      {
        siteSelector = 1;
        SITE_DRAWN = false;
        return;
      });
      break;
    }
    default: break;
  } // switch
  } // pressed
}

void chkIfTFTtouched()
{
  if (pressed){
  // Rozjasnij ekran rowniez po dot. dziekolwiek
  ledcWrite(1, 128);
  TFT_DIM_TIMER = millis();
  
  if (siteSelector==0){
  // Togglery
  if (SUB_REL_TOG.chkIfPress(tx,ty)){
    if (SUB_REL_TOG.getState()==true) { digitalWrite(RELAYS_BAS, LOW); }
    else { digitalWrite(RELAYS_BAS, HIGH); } }
  if (LR_REL_TOG.chkIfPress(tx,ty)) {
    if (LR_REL_TOG.getState()==true) { digitalWrite(RELAYS_SPK, LOW); }
    else { digitalWrite(RELAYS_SPK, HIGH); } }

  // Przyciski
  SUB_PLUS.chkIfPress(tx,ty,[]()
  { if (M_BASAMP < 99) M_BASAMP++; });
  SUB_MINUS.chkIfPress(tx,ty,[]()
  { if (M_BASAMP > 0) M_BASAMP--; });
  //////////////////////////////////
  SPK_PLUS.chkIfPress(tx,ty,[]() 
  { if (M_SPKAMP < 99) M_SPKAMP++; });
  SPK_MINUS.chkIfPress(tx,ty,[]()
  { if (M_SPKAMP > 0) M_SPKAMP--; });
  //////////////////////////////////
  LOUD_PLUS.chkIfPress(tx,ty,[]()
  { if (M_LOUD  < MAX_LAUDNESS) M_LOUD ++; });
  LOUD_MINUS.chkIfPress(tx,ty,[]()
  { if (M_LOUD  > 0) M_LOUD --; });
  //////////////////////////////////
  CUT_PLUS.chkIfPress(tx,ty,[]()
  { if (M_CUTOFF < 99) M_CUTOFF++; });
  CUT_MINUS.chkIfPress(tx,ty,[]()
  { if (M_CUTOFF > 0) M_CUTOFF--; });
  } //STRONA 0

  //STRONA 1
  if (siteSelector==1)
  {
    SETTINGS_MENU.chkIfPress(tx,ty);
  } //STRONA 1

  //STRONA 2
  if (siteSelector==2)
  {
    if (virtualSpk.chkIfPress(tx,ty)) { STATIC_LIGHTS=true; }
    virtualSpk.chkIfPressHSV(tx,ty);
  } //STRONA 2
   
  }//pressed
}

void drawMainSite()
{
  tft.fillScreen(TFT_BLACK);

  LOUD_MINUS.draw(10,5,40,40);
  LOUD_PLUS.draw(270,5,40,40);
  LOUD_VALDISP.draw("Volume: ", " %", M_LOUD , 65, 10, 190, 15);
  LOUD_VALBAR.draw(M_LOUD, 0, MAX_LAUDNESS, 65, 25, 190, 15);
  
  SUB_MINUS.draw(10,50,40,40);
  SUB_PLUS.draw(270,50,40,40);
  int MED = (50-M_BASAMP)*-2;
  if (MED > 0) { SUB_VALDISP.draw("Bass: +", " %", MED , 65, 55, 190, 15); }
  else { SUB_VALDISP.draw("Bass: ", " %", MED , 65, 55, 190, 15); }
  SUB_VALBAR.draw(M_BASAMP, 0, 99, 65, 70, 190, 15);

  SPK_MINUS.draw(10,95,40,40);
  SPK_PLUS.draw(270,95,40,40);
  MED = (50-M_SPKAMP)*-2;
  if (MED > 0) { SPK_VALDISP.draw("Treble: ", " %", MED , 65, 100, 190, 15); }
  else { SPK_VALDISP.draw("Treble: ", " %", MED , 65, 100, 190, 15); }
  SPK_VALBAR.draw(M_SPKAMP, 0, 99, 65, 115, 190, 15);

  CUT_MINUS.draw(10,140,40,40);
  CUT_PLUS.draw(270,140,40,40);
  CUT_VALDISP.draw("Cutoff: ", " Hz", 20+(M_CUTOFF+1)*2  , 65, 145, 190, 15);
  CUT_VALBAR.draw(M_CUTOFF, 0, 99, 65, 160, 190, 15);

  // Wstepne rysowanie ramki selektora
  sFrame.draw(63, 8, 194, 34, 2);

  // TOGGLER
  SUB_REL_TOG.draw(115, 180, 90, 25);
  LR_REL_TOG.draw(115, 210, 90, 25);

  // STRONY
  SITE_CONF.draw(10, 190, 90, 45);
  SITE_LED.draw(220, 190, 90, 45);
}

void drawConfSite()
{
  tft.fillScreen(TFT_BLACK);
  LEDFX_SELECTOR.draw(10,115,120,30);
  SITE_MAIN.draw(235, 190, 80, 45);
  SITE_LED.draw(140, 190, 80, 45);
  SETTINGS_MENU.draw(10, 10, 220);
  tft.drawCentreString("BIFROST", 230, 150-40, 2);
  if (WiFi.status() == WL_CONNECTED)
  {tft.drawCentreString(WiFi.localIP().toString(), 230, 150-12, 2);}
  else{tft.drawCentreString("OFFLINE", 230, 150-12, 2);}
}

void drawLedSite()
{
  tft.fillScreen(TFT_BLACK);
  virtualSpk.draw(160, 10);
  virtualSpk.drawbar_H(10, 115, 300, 20);
  virtualSpk.drawbar_S(10, 145, 300, 20);
  virtualSpk.drawbar_V(10, 175, 300, 20);
  virtualSpk.draw_RGB(5, 10, 20, 80);
  
  SITE_MAIN.draw(10, 205, 60, 30);
  SITE_CONF.draw(80, 205, 60, 30);
  SITE_OSC.draw(150, 205, 60, 30);
}

oscylogram osc(0, 210);
void drawOscSite()
{
  tft.fillScreen(TFT_BLACK);
  osc.start();
  SITE_MAIN.draw(10, 215, 60, 30);
  SITE_CONF.draw(80, 215, 60, 30);
}

// Pokazanie wielkosci V,B,T,C na LEDy //
uint32_t ledIndiTimer = 0;
uint32_t ledIndiDelay = 1000;
bool isIndicate = false;
void ledIndicator(int val, int nr)
{
  isIndicate = true;
  for(int i = 0; i < 4; i++) {
  for(int j = 0; j < 22; j++) 
  {LED[j][i]( 0,0,0 ); }}
  
  // 0 - vol, 1 - bass, 2 - treble, 3 - cutoff
  switch(nr)
  {
    case 0: // VOL
    {
      int MAG = map(val, 0, MAX_LAUDNESS, 0, 22);
      for (int i=0; i<MAG; i++)
      {
        LED[i][0](5, 255, 255);
      }
      mapled();
      FastLED.show();
      break;
    }
    case 1: // BASS
    {
      int MAG = map(val, 0, 99, 0, 22);
      for (int i=0; i<MAG; i++)
      {
        LED[i][1](35, 255, 255);
      }
      mapled();
      FastLED.show();
      break;
    }
    case 2: // TREBLE
    {
      int MAG = map(val, 0, 99, 0, 22);
      for (int i=0; i<MAG; i++)
      {
        LED[i][2](65, 255, 255);
      }
      mapled();
      FastLED.show();
      break;
    }
    case 3: // CUT
    {
      int MAG = map(val, 0, 99, 0, 22);
      for (int i=0; i<MAG; i++)
      {
        LED[i][3](110, 255, 255);
      }
      mapled();
      FastLED.show();
      break;
    }
    default: break;
  }
  ledIndiTimer = millis();
  //isIndicate = false;
}

void loop() {
// Sciemnianie ekranu
if (millis() - TFT_DIM_TIMER > TFT_DIMTIME_SEC*1000) { ledcWrite(1, 252); TFT_DIM_TIMER = millis(); }
// Wyl. LED indicatora
if (isIndicate==true && millis() - ledIndiTimer > ledIndiDelay )
{
  for(int i = 0; i < 4; i++) {
  for(int j = 0; j < 22; j++) 
  {LED[j][i]( BGcol.h, BGcol.s, BGcol.v ); }}
  mapled();
  FastLED.show();
  isIndicate = false;
}

if (siteSelector==0 && SITE_DRAWN==false) { drawMainSite(); SITE_DRAWN=true;}
if (siteSelector==1 && SITE_DRAWN==false) { drawConfSite(); SITE_DRAWN=true; itemSelector=0; ENC.setBoundaries(0, MAX_LAUDNESS, false); ENC.setEncoderValue(M_LOUD); ENC.setAcceleration(30);}
if (siteSelector==2 && SITE_DRAWN==false) { drawLedSite();  SITE_DRAWN=true; itemSelector=0; ENC.setBoundaries(0, MAX_LAUDNESS, false); ENC.setEncoderValue(M_LOUD); ENC.setAcceleration(30);}
if (siteSelector==3 && SITE_DRAWN==false) { drawOscSite(); SITE_DRAWN=true; itemSelector=0; ENC.setBoundaries(0, MAX_LAUDNESS, false); ENC.setEncoderValue(M_LOUD); ENC.setAcceleration(30);}

if (siteSelector==3) { osc.plot(); } // jak 3 str. to dziala oscylogram

chkIfPotsChange();
tx=0; ty=0;
pressed = tft.getTouch(&tx, &ty);
if (pressed==false) wasHandRised=true;
siteChange();
chkIfTFTtouched();
 
if (ENC.encoderChanged())
{
  // Rozjasnij ekran
  ledcWrite(1, 128);
  TFT_DIM_TIMER = millis();
  switch(itemSelector)
  {
    case 0:{ M_LOUD = ENC.readEncoder(); break; }
    case 1: { M_BASAMP = ENC.readEncoder(); break; }
    case 2: { M_SPKAMP = ENC.readEncoder(); break; }
    case 3: { M_CUTOFF = ENC.readEncoder(); break; }
    default: break;
  }
}

if (siteSelector==0) { handle_rotary_button(); }

if (ENCclicked == true)
{
  ledcWrite(1, 128);
  TFT_DIM_TIMER = millis();

  // Czyszczenie selektora przed zmiana
  switch(itemSelector)
  {
    case 0: { sFrame.clr(63, 8, 194, 34, 2); break; }
    case 1: { sFrame.clr(63, 53, 194, 34, 2); break; }
    case 2: { sFrame.clr(63, 98, 194, 34, 2); break; }
    case 3: { sFrame.clr(63, 143, 194, 34, 2); break; }
    default: break;
  }
  // Ustalanie przemiesz. na pod. dlg. nacisku
  if (ENCbtnDir == 1) itemSelector++;
  else itemSelector--;
  
  if (itemSelector>=4) itemSelector = 0; // zabezpieczenie
  if (itemSelector < 0) itemSelector = 3;
  
  switch(itemSelector)
  {
    case 0: // VOLUME
    {
      ENC.setBoundaries(0, MAX_LAUDNESS, false);
      ENC.setEncoderValue(M_LOUD);
      ENC.setAcceleration(30);
      sFrame.draw(63, 8, 194, 34, 2);
      break;
    }
    case 1: // BASS
    {
      ENC.setBoundaries(0, 99, false);
      ENC.setEncoderValue(M_BASAMP);
      ENC.setAcceleration(100);
      sFrame.draw(63, 53, 194, 34, 2);
      break;
    }
    case 2: // SPKR
    {
      ENC.setBoundaries(0, 99, false);
      ENC.setEncoderValue(M_SPKAMP);
      ENC.setAcceleration(100);
      sFrame.draw(63, 98, 194, 34, 2);
      break;
    }
    case 3: // CUTOFF
    {
      ENC.setBoundaries(0, 99, false);
      ENC.setEncoderValue(M_CUTOFF);
      ENC.setAcceleration(200);
      sFrame.draw(63, 143, 194, 34, 2);
      break;
    }
    default: break;
  }  
  ENCclicked = false;
} // ENC CLICKED END

// STEROWANIE PILOTEM IR
#include "IRRemote.h"

}// LOOP END

//########################################################################################## CORE 0
unsigned long VUVAL = 0, VUVALMAX = 0;
int RAINBOW_ITER = 0;
bool RAIN_DIR = 0;
Kronos RAINdel;

void coreTask( void * pvParameters ){
  while(true){
#include "core0.h"
 } // endless FOR
}//######################################################################################### CORE 0
