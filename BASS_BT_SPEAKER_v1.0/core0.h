if (VU==true)
{
  //Serial.print("Jade z VU: ");
  VUVAL = 0;

  if (false) // ARYTMETYCZNE
  {
    for (int i=0; i<50; i++)
    {
      VUVAL+=static_cast<unsigned long>(abs((analogRead(MIC) - MicOffset)));
      delayMicroseconds(30);
    }
    VUVAL/=50;
  }

  if (true) // RMS
  {
    for (int i=0; i<50; i++)
    {
      VUVAL+=static_cast<unsigned long>(pow((analogRead(MIC) - MicOffset),2));
      delayMicroseconds(20);
    }
    VUVAL/=50;
    VUVAL = static_cast<unsigned long>(sqrt(VUVAL));
  }

  //////////////// WSTEPNE CZYSZCZENIE TABLIC
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 22; j++) 
    {LED[j][i]( BGcol.h, BGcol.s, BGcol.v ); }}

  if (isRingVU==true)
  {
    for (int i=0; i<17; i++) { RING[i].h = 0; RING[i].s = 0; RING[i].v = 0; }  
    byte RNGOUT = map(VUVAL, 0, VU_MaxAmp, 0, mRing.v);
    for (int i=0; i<17; i++) { RING[i].h = mRing.h;  RING[i].s = mRing.s;  RING[i].v = RNGOUT; }
  }

  if (VUVAL < VU_MinAmp) VUVAL = 0;
  if (VUVAL > VU_MaxAmp) VUVAL = VU_MaxAmp;

  if (VUMODE==0)
  {
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
      delay(VU_falloffMili);
    }
  } // MODE 0
  
  if (VUMODE==1)
  {
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
      delay(VU_falloffMili);
    }
  } // MODE 1

  if (VUMODE==2)
  {
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
      delay(VU_falloffMili);
    }
  }
  
  mapled();
  FastLED.show();
}

if (STATIC_LIGHTS==true)
{
  for(int i = 0; i < 88; i++) {leds[i] = CHSV(BGcol.h, BGcol.s, BGcol.v);}
  for(int i = 88; i < 105; i++) {leds[i] = CHSV(mRing.h, mRing.s, mRing.v);}
  FastLED.show(); 
  STATIC_LIGHTS = false;
}

if (RAINBOW == true && RAINdel.del(30)){
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

if (FFT_ON == true)
{
  for (int i = 0; i < SAMPLES; i++)
    {
    microsecFFT = micros();
    sum = static_cast<unsigned long>(abs((analogRead(MIC) - MicOffset)/ADC_PRESCALAR));
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
      if (LED[j][0].v > mapped[bin_reject+j*3])
      { if (LED[j][0].v > 0 && LED[j][0].v - 4 < 250)  { for (int LD=0; LD<4; LD++) { LED[j][LD].v = LED[j][0].v-=4; } } }
      else 
      { for (int LD=0; LD<4; LD++) { LED[j][LD]( hue+=1, 255, mapped[bin_reject+j*3] ); } }
    }
    mapled();
    FastLED.show();
    delay(5);
}

vTaskDelay(1);
