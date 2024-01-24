if (VU==true && isIndicate==false && VUFALLDEL.del(VU_falloffMili))
{
  RAINBOW = false; FFT_ON = false; STATIC_LIGHTS = false;
  if (VUMODEp != VUMODE)
  {
    if (VUMODE < 0) VUMODE = 0;
    else if (VUMODE > 4) VUMODE = 4;
    for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 22; j++) 
    {LED[j][i]( BGcol.h, BGcol.s, BGcol.v ); }}
    mapled();
    delay(100);
    FastLED.show();
    delay(350);
    VUMODEp = VUMODE;
  }

  /*
  if (isRingVU==true)
  {
    for (int i=0; i<17; i++) { RING[i].h = 0; RING[i].s = 0; RING[i].v = 0; }  
    byte RNGOUT = map(VUVAL, 0, VU_MaxAmp, 0, mRing.v);
    for (int i=0; i<17; i++) { RING[i].h = mRing.h;  RING[i].s = mRing.s;  RING[i].v = RNGOUT; }
  }
  */
 
  switch(VUMODE)
  {
    case 0:
    {
      VUVAL = 0;
      for (int i=0; i<VuSamples; i++)
      {
        VUVAL+=static_cast<uint64_t>(pow((analogRead(MIC) - MicOffset),2));
        //delayMicroseconds(VUsampleDel);
      }
      VUVAL/=VuSamples;
      float SQ = sqrt((float)VUVAL);
      VUVAL = static_cast<uint64_t>(SQ);
      if (VUVAL < VU_MinAmp) VUVAL = 0;
      if (VUVAL > VU_MaxAmp) VUVAL = VU_MaxAmp;
      for(int i = 0; i < 4; i++) {
      for(int j = 0; j < 22; j++) 
      {LED[j][i]( BGcol.h, BGcol.s, BGcol.v ); }}
    
      int VUOUT = map(VUVAL, 0, VU_MaxAmp, 0, 22);
      if (VUOUT > VUVALMAX) 
      {
        VUVALMAX=VUOUT;
        for(int i = 0; i < VUOUT; i++) 
        {
          LED[i][0] = LeftL;
          LED[i][1] = LeftR;
          LED[i][2] = RightL;
          LED[i][3] = RightR;
        }
      }
      else if (VUOUT <= VUVALMAX)
      {
        for(int i = 0; i < VUVALMAX; i++) 
        {
          LED[i][0] = LeftL;
          LED[i][1] = LeftR;
          LED[i][2] = RightL;
          LED[i][3] = RightR;
        }
        if (VUVALMAX > 0) VUVALMAX--;
        //delay(VU_falloffMili);
      }
      mapled();
      FastLED.show();
      break;
    }

    case 1:
    {
      VUVAL = 0;
      for (int i=0; i<VuSamples; i++)
      {
        VUVAL+=static_cast<uint64_t>(pow((analogRead(MIC) - MicOffset),2));
        //delayMicroseconds(VUsampleDel);
      }
      VUVAL/=VuSamples;
      float SQ = sqrt((float)VUVAL);
      VUVAL = static_cast<uint64_t>(SQ);
      if (VUVAL < VU_MinAmp) VUVAL = 0;
      if (VUVAL > VU_MaxAmp) VUVAL = VU_MaxAmp;
      for(int i = 0; i < 4; i++) {
      for(int j = 0; j < 22; j++) 
      {LED[j][i]( BGcol.h, BGcol.s, BGcol.v ); }}
      
      int VUOUT = map(VUVAL, 0, VU_MaxAmp, 0, 44);
      if (VUOUT > VUVALMAX) 
      {
        VUVALMAX=VUOUT;
        for(int i = 0; i < VUOUT; i++) 
        {
          if (i<22){ LED[i][0] = LeftL; LED[i][3] = RightR; }
          else{ LED[i-22][0] = LeftL; LED[i-22][3] = RightR; LED[21-(i-22)][1] = LeftR; LED[21-(i-22)][2] = RightL; }
        }
      }
      else if (VUOUT <= VUVALMAX)
      {
        for(int i = 0; i < VUVALMAX; i++) 
        {
          if (i<22){ LED[i][0] = LeftL; LED[i][3] = RightR; }
          else{ LED[i-22][0] = LeftL; LED[i-22][3] = RightR; LED[21-(i-22)][1] = LeftR; LED[21-(i-22)][2] = RightL; }
        }
        if (VUVALMAX > 0) VUVALMAX--;
        //delay(VU_falloffMili);
      }
      mapled();
      FastLED.show();
      break;
    }

    case 2:
    {
      VUVAL = 0;
      for (int i=0; i<VuSamples; i++)
      {
        VUVAL+=static_cast<uint64_t>(pow((analogRead(MIC) - MicOffset),2));
        //delayMicroseconds(VUsampleDel);
      }
      VUVAL/=VuSamples;
      float SQ = sqrt((float)VUVAL);
      VUVAL = static_cast<uint64_t>(SQ);
      if (VUVAL < VU_MinAmp) VUVAL = 0;
      if (VUVAL > VU_MaxAmp) VUVAL = VU_MaxAmp;
      for(int i = 0; i < 4; i++) {
      for(int j = 0; j < 22; j++) 
      {LED[j][i]( BGcol.h, BGcol.s, BGcol.v ); }}
      int VUOUT = map(VUVAL, 0, VU_MaxAmp, 0, 11);
      if (VUOUT > VUVALMAX) 
      {
        VUVALMAX=VUOUT;
        for(int i = 0; i < VUOUT; i++) 
        {
          LED[10-i][0] = LeftL; LED[11+i][0] = LeftL;
          LED[10-i][1] = LeftR; LED[11+i][1] = LeftR;
          LED[10-i][2] = RightL; LED[11+i][2] = RightL;
          LED[10-i][3] = RightR; LED[11+i][3] = RightR;
        }
      }
      else if (VUOUT <= VUVALMAX)
      {
        for(int i = 0; i < VUVALMAX; i++) 
        {
          LED[10-i][0] = LeftL; LED[11+i][0] = LeftL;
          LED[10-i][1] = LeftR; LED[11+i][1] = LeftR;
          LED[10-i][2] = RightL; LED[11+i][2] = RightL;
          LED[10-i][3] = RightR; LED[11+i][3] = RightR;
        }
        if (VUVALMAX > 0) VUVALMAX--;
        //delay(VU_falloffMili);
      }
      mapled();
      FastLED.show();
      break;
    }

    case 3:
    {
      VUVAL = 0;
      for (int i=0; i<VuSamples; i++)
      {
        VUVAL+=static_cast<uint64_t>(pow((analogRead(MIC) - MicOffset),2));
        //delayMicroseconds(VUsampleDel);
      }
      VUVAL/=VuSamples;
      float SQ = sqrt((float)VUVAL);
      VUVAL = static_cast<uint64_t>(SQ);
      if (VUVAL < VU_MinAmp) VUVAL = 0;
      if (VUVAL > VU_MaxAmp) VUVAL = VU_MaxAmp;
      int VUOUT = map(VUVAL, 0, VU_MaxAmp, 0, 255);
      LED[21][0](map(VUOUT,0,255,140,255), 255, map(VUOUT,0,255,0,255));
      LED[21][1](map(VUOUT,0,255,140,255), 255, map(VUOUT,0,255,0,255));
      LED[21][2](map(VUOUT,0,255,140,255), 255, map(VUOUT,0,255,0,255));
      LED[21][3](map(VUOUT,0,255,140,255), 255, map(VUOUT,0,255,0,255));
        
        for(int i = 1; i < 22; i++) 
        {
          LED[i-1][0] = LED[i][0];
          LED[i-1][1] = LED[i][0];
          LED[i-1][2] = LED[i][0];
          LED[i-1][3] = LED[i][0];
        }
        //delay(VU_falloffMili);
      mapled();
      FastLED.show();  
      break;
    }

    case 4:
    {
      VUVAL = 0;
      for (int i=0; i<VuSamples; i++)
      {
        VUVAL+=static_cast<uint64_t>(pow((analogRead(MIC) - MicOffset),2));
        //delayMicroseconds(VUsampleDel);
      }
      VUVAL/=VuSamples;
      float SQ = sqrt((float)VUVAL);
      VUVAL = static_cast<uint64_t>(SQ);
      if (VUVAL < VU_MinAmp) VUVAL = 0;
      if (VUVAL > VU_MaxAmp) VUVAL = VU_MaxAmp;
      static byte VU4minmaxMedian = 90;
      static byte VU4min = 15;
      int VUOUT = map(VUVAL, 0, VU_MaxAmp, 0, 255);
      
      // Male natezenie
      if (VUOUT <=VU4minmaxMedian)     // HUE          // SAT          // VAL
      { Lmat1[21][0]  (map(VUOUT,0,255,140,255),  255,  map(VUOUT,0,255,0,255)); 
        //Lmat2[0][0].v = map(VUOUT,0,255,0,255);
        if (Lmat2[0][0].v > 0)
        {
          Lmat2[0][0].v--;
        }
      }
      // Duze natezenie
      else
      { Lmat2[0][0].h = map(VUOUT,0,255,140,255);
        Lmat2[0][0].s = 255;
        Lmat2[0][0].v = map(VUOUT,0,255,0,255); 
      }
        
      for(int i = 0; i < 22; i++) 
      {
        LED[i][0] = Lmat1[i][0] + Lmat2[i][0];
        LED[i][1] = Lmat1[i][0] + Lmat2[i][0];
        LED[i][2] = Lmat1[i][0] + Lmat2[i][0];
        LED[i][3] = Lmat1[i][0] + Lmat2[i][0];
      }
      // Przesowanie dodatkowych macierzy
      for(int i = 1; i < 22; i++) 
      { Lmat1[i-1][0] = Lmat1[i][0]; }
      for(int i = 21; i > 0; i--) 
      { Lmat2[i][0] = Lmat2[i-1][0]; }
      
      //delay(VU_falloffMili);
     mapled();
     FastLED.show(); 
     break;
    }

    default: break;
  }

}

if (STATIC_LIGHTS==true && isIndicate==false)
{
  for(int i = 0; i < 88; i++) {leds[i] = CHSV(BGcol.h, BGcol.s, BGcol.v);}
  for(int i = 88; i < 105; i++) {leds[i] = CHSV(mRing.h, mRing.s, mRing.v);}
  FastLED.show(); 
  STATIC_LIGHTS = false;
}

if (RAINBOW == true && RAINdel.del(VU_falloffMili) && isIndicate==false){
static uint8_t hue = 0;

  if (RAIN_DIR==0 && RAINBOW_ITER < NUM_LEDS)
  {
    leds[RAINBOW_ITER] = CHSV(hue++, 255, 255);
    FastLED.show(); 
    fadeall();
    RAINBOW_ITER++;
    if (RAINBOW_ITER >= NUM_LEDS-1) RAIN_DIR=1;
  }

  if (RAIN_DIR==1 && RAINBOW_ITER > 0)
  {
    if (RAINBOW_ITER==NUM_LEDS-1) RAIN_DIR=1;
    leds[RAINBOW_ITER] = CHSV(hue++, 255, 255);
    FastLED.show(); 
    fadeall();
    RAINBOW_ITER--;
    if (RAINBOW_ITER <= 0) RAIN_DIR=0;
  }

  
}

if (FFT_ON == true && isIndicate==false)
{
  for (int i = 0; i < SAMPLES; i++)
    {
    microsecFFT = micros();
    sum = static_cast<int>((analogRead(MIC) - MicOffset)/ADC_PRESCALAR);
    if (sum > -noise_reduction && sum < noise_reduction) {
      sum = 0;
    }
    vReal[i] = sum;
    vImag[i] = 0;
    while (micros() < microsecFFT+SF){}
    }
    
  /*FFT*/
  FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
  FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);

  // Normalizacja
  normalizer = norm_base;
  for (int i = 0; i < SAMPLES / 2; i++)
  {
    normalizer+=norm_gain;
    vReal[i] = vReal[i]*normalizer;
  }
  
  /* ------------- MAPOWANIE NA mapped[256] ------------- */
    for (int i = 0; i < SAMPLES / 2; i++)
    {
      vReal[i] = round(vReal[i]);
      if (vReal[i] > fftMaxAmp) vReal[i] = fftMaxAmp;
      if (vReal[i] < fftMinAmp) vReal[i] = fftMinAmp;
      mapped[i] = map(int(vReal[i]), fftMinAmp, fftMaxAmp, 0, 255);
    }
 //FFTDoneComputing = true;
   for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 22; j++) 
    {
      LED[j][i]( BGcol.h, BGcol.s, BGcol.v );
    }}
    byte hue = 0;
    for(int j = 0; j < 22; j++) 
    { // 1 LL
      if (LED[j][0].v > mapped[bin_reject+(j*2)])
      { if (LED[j][0].v > 1)  { for (int LD=0; LD<4; LD++) { LED[j][LD].v = LED[j][0].v-=2; } } }
      else 
      { for (int LD=0; LD<4; LD++) { LED[j][LD]( hue+=1, 255, mapped[bin_reject+(j*2)] ); } }
    }
    mapled();
    FastLED.show();
    delay(VU_falloffMili);
}

vTaskDelay(1);
