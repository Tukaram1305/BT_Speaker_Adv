#ifndef FUNCS_H
#define FUNCS_H
void startWIFI(byte ret)
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  int LITER = 25;
  if (WiFi.status() != WL_CONNECTED)
  {
    for (byte i=0; i<ret; i++)  {
      if (digitalRead(ENCSW)==LOW)
      {
        tft.drawCentreString("OFFLINE", 160, 190, 2);
      }
      else tft.drawCentreString("ONLINE MODE", 160, 190, 2);
      delay(1000);
      FIRST_LOADING.draw(LITER,0,100,20,130,280,40);
      LITER+=10;
      Serial.println("Lacze z WiFi..");
      if (WiFi.status() == WL_CONNECTED) break;
      }
  }
  
  if (WiFi.status() == WL_CONNECTED)
  {
    if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {Serial.println("Nie udalo sie skonfigurowac WiFi STA");}
    String IP = WiFi.localIP().toString().c_str();
    int RSI = WiFi.RSSI();
    Serial.println("IP ESP: "+IP+" , sygnal: "+String(RSI));
    //tft.drawCentreString("BIFROST",tft.width()/2, tft.height()-40, 2);
    //tft.drawCentreString(WiFi.localIP().toString(),tft.width()/2, tft.height()-2, 2);
  }
  else
  {
    //tft.drawCentreString("BIFROST",tft.width()/2, tft.height()-40, 2);
    //tft.drawCentreString("OFFLINE",tft.width()/2, tft.height()-2, 2);
  }
  
}

struct RGB_set {
 unsigned char r;
 unsigned char g;
 unsigned char b;
} RGB_set;
 
struct HSV_set {
 signed int h;
 unsigned char s;
 unsigned char v;
} HSV_set;

void rgb2hsv(struct RGB_set RGB, struct HSV_set *HSV){
 unsigned char min, max, delta;
 
 if(RGB.r<RGB.g)min=RGB.r; else min=RGB.g;
 if(RGB.b<min)min=RGB.b;
 
 if(RGB.r>RGB.g)max=RGB.r; else max=RGB.g;
 if(RGB.b>max)max=RGB.b;
 
 HSV->v = max;                // v, 0..255
 
 delta = max - min;                      // 0..255, < v
 
 if( max != 0 )
 HSV->s = (int)(delta)*255 / max;        // s, 0..255
 else {
 // r = g = b = 0        // s = 0, v is undefined
 HSV->s = 0;
 HSV->h = 0;
 return;
 }
 
 if( RGB.r == max )
 HSV->h = (RGB.g - RGB.b)*60/delta;        // between yellow & magenta
 else if( RGB.g == max )
 HSV->h = 120 + (RGB.b - RGB.r)*60/delta;    // between cyan & yellow
 else
 HSV->h = 240 + (RGB.r - RGB.g)*60/delta;    // between magenta & cyan
 
 if( HSV->h < 0 )
 HSV->h += 360;
}

word hsv2hex(byte h, byte s, byte v)
{
    double  H, S, V,
            P, Q, T,
            RC,GC,BC,
            fract;


    H = 1.4117647058823529411764705882353*h; // mapuj 0-360 -> 0-255
    S = 0.0039215686274509803921568627451*s; //       0-1   -> 0-255
    V = 0.0039215686274509803921568627451*v; //       0-1   -> 0-255

    byte  bR, bG, bB;

    (H == 360.)?(H = 0.):(H /= 60.);
    fract = H - floor(H);

    P = V*(1. - S);
    Q = V*(1. - S*fract);
    T = V*(1. - S*(1. - fract));

    if      (0. <= H && H < 1.)
    {
        RC = V;
        GC = T;
        BC = P;
    }
    else if (1. <= H && H < 2.)
    {
        RC = Q;
        GC = V;
        BC = P;
    }
    else if (2. <= H && H < 3.)
    {
        RC = P;
        GC = V;
        BC = T;
    }
    else if (3. <= H && H < 4.)
    {
        RC = P;
        GC = Q;
        BC = V;
    }
    else if (4. <= H && H < 5.)
    {
        RC = T;
        GC = P;
        BC = V;
    }
    else if (5. <= H && H < 6.)
    {
        RC = V;
        GC = P;
        BC = Q;
    }
    else
    {
        RC = 0;
        GC = 0;
        BC = 0;
    }

    bR = byte(RC*255); bG = byte(GC*255); bB = byte(BC*255);
    
    return ( ((bR & 0xF8) << 8) | ((bG & 0xFC) << 3) | (bB >> 3) );
}

void fadeall() { for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(250); } }
void mapled()
{
  for (int i=0; i<22; i++)
  {
    leds[21-i] = CHSV(LED[i][0].h, LED[i][0].s, LED[i][0].v);
  }
  for (int i=0; i<22; i++)
  {
    leds[22+i] = CHSV(LED[i][1].h, LED[i][1].s, LED[i][1].v);
  }
  for (int i=0; i<22; i++)
  {
    leds[65-i] = CHSV(LED[i][2].h, LED[i][2].s, LED[i][2].v);
  }
  for (int i=0; i<22; i++)
  {
    leds[66+i] = CHSV(LED[i][3].h, LED[i][3].s, LED[i][3].v);
  }
  for (int i=0; i<17; i++)
  {
    leds[88+i] = CHSV(RING[i].h, RING[i].s, RING[i].v);
  }
}

void tftBaclitSet(int val)
{
  
  ledcWrite(1, val);
}
#endif
