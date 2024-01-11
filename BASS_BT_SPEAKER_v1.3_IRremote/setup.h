 // FAST LED
  pinMode(DATA_PIN, OUTPUT);
  FastLED.addLeds<WS2812B,DATA_PIN,GRB>(leds,NUM_LEDS);
  FastLED.setBrightness(128);
  
  // ENCODER
  ENC.begin();
  ENC.setup(readEncoderISR);
  ENC.setBoundaries(0, MAX_LAUDNESS, false);
  ENC.setEncoderValue(M_LOUD);
  ENC.setAcceleration(30);

  // TFT LCD
  tft.init();
  tft.setRotation(1);
  uint16_t calData[5] = { 462, 3343, 347, 3448, 7 };
  tft.setTouch(calData);
  tft.fillScreen(TFT_BLACK);
  tft.drawCentreString("LOADING", 160, 120-8, 2);
  FIRST_LOADING.draw(10,0,100,20,130,280,40);
  
  // POTY - pulse, direction, select
  pot0.begin(13, 14, 27); //pot0.setPosition(0);
  pot1.begin(13, 14, 26); //pot1.setPosition(0);
  pot2.begin(13, 14, 25); //pot2.setPosition(0);
  pot3.begin(13, 14, 33); //pot3.setPosition(0);
  
  Serial.begin(115200);
  Serial.println("BASS");

  pinMode(RELAYS_SPK, OUTPUT);
  pinMode(RELAYS_BAS, OUTPUT);
  digitalWrite(RELAYS_SPK, HIGH);
  digitalWrite(RELAYS_BAS, HIGH);

  pinMode(ENC0, INPUT);
  pinMode(ENC1, INPUT);
  pinMode(ENCSW, INPUT);

    // Konfiguracja PWM dla poedswietlenia TFT  kanal - 1, res - 8 bit
  ledcAttachPin(TFT_LED, 1);
  ledcSetup(1, 120, 8);
  ledcWrite(1, 128);

  analogReadResolution(12);
  adcAttachPin(MIC);
  analogSetPinAttenuation(MIC, ADC_6db);

  FIRST_LOADING.draw(24,0,100,20,130,280,40);

  delay(500);
  
  // AWS REQUESTY
  #include "awserver.h";
  
  if (digitalRead(ENCSW)==LOW){FIRST_LOADING.draw(60,0,100,20,130,280,40);}
  else 
  {
    startWIFI(6);
    AsyncElegantOTA.begin(&server);
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    server.begin();
  }
  FIRST_LOADING.draw(60,0,100,20,130,280,40);

  xTaskCreatePinnedToCore(coreTask,"Task1",10000,NULL,0,&Task1,0);     
  
  delay(10);
  pot0.setPosition(M_SPKAMP , true);
  pot1.setPosition(M_BASAMP , true);
  pot2.setPosition(M_LOUD , true);
  pot3.setPosition(M_CUTOFF , true);
  
  byte hue = 0;
  for (int i=21; i>=0; i--)
  {
    LED[i][0] = (myLed){.h=hue, .s=255, .v=200};
    LED[i][1] = (myLed){.h=hue+4, .s=255, .v=200};
    LED[i][2] = (myLed){.h=hue+8, .s=255, .v=200};
    LED[i][3] = (myLed){.h=hue+12, .s=255, .v=200};
    hue++;
    mapled();
    FastLED.show();
    fadeall();
    delay(20);
    if (i==11) FIRST_LOADING.draw(65,0,100,20,130,280,40);
  }
  for (int i=0; i<100; i++) { fadeall(); FastLED.show(); delay(5); 
  if (i==40){ FIRST_LOADING.draw(80,0,100,20,130,280,40);}
  if (i==90){ FIRST_LOADING.draw(96,0,100,20,130,280,40);}}
  digitalWrite(RELAYS_SPK, LOW);
  digitalWrite(RELAYS_BAS, LOW);

  IrReceiver.begin(IR_RECEIVE_PIN, DISABLE_LED_FEEDBACK);

  LEDFX_SELECTOR.addItem(&RAINBOW,"Rainbow");
  LEDFX_SELECTOR.addItem(&VU,"VU");
  LEDFX_SELECTOR.addItem(&STATIC_LIGHTS,"Back.Lit");
  LEDFX_SELECTOR.addItem(&FFT_ON,"FFT");

  SETTINGS_OBJECTS[0] = &VE_VUMODE;
  SETTINGS_OBJECTS[1] = &VE_VU_MinAmp;
  SETTINGS_OBJECTS[2] = &VE_VU_MaxAmp;
  SETTINGS_OBJECTS[3] = &VE_VU_falloffMili;
  SETTINGS_OBJECTS[4] = &VE_ADC_PRESCALAR;
  SETTINGS_OBJECTS[5] = &VE_noise_reduction;
  SETTINGS_OBJECTS[6] = &VE_fftMinAmp;
  SETTINGS_OBJECTS[7] = &VE_fftMaxAmp;
  SETTINGS_OBJECTS[8] = &VE_norm_base;
  SETTINGS_OBJECTS[9] = &VE_norm_gain;
  SETTINGS_OBJECTS[10] =&VE_bin_reject;
