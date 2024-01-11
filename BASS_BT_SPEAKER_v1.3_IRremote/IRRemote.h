#ifndef IRREMOTE_H
#define IRREMOTE_H
// PILOT IR //
    if (IrReceiver.decode()) {
      int REC = IrReceiver.decodedIRData.command;
      int RRAW = IrReceiver.decodedIRData.decodedRawData;
      //Serial.println(REC, HEX);
       if (siteSelector==1){tft.fillRect(180, 90, 120, 40, TFT_BLACK);tft.drawCentreString("R: "+String(RRAW,HEX), 230, 150-40, 2);}

      // Obudz ekran
      ledcWrite(1, 128);
      TFT_DIM_TIMER = millis();
  
      if (IRRemoteDel.del(60))
      { //  STEROWANIE POTAMI
      // VOLUME
      if (RRAW == 0x6c9a) { if (M_LOUD  < MAX_LAUDNESS) {M_LOUD ++; ledIndicator(M_LOUD,0);} }
      if (RRAW == 0x6c9b) { if (M_LOUD  > 0) {M_LOUD --; ledIndicator(M_LOUD,0);} }
      // BASS
      if (RRAW == 0x3d6) { if (M_BASAMP  < 99) {M_BASAMP ++; ledIndicator(M_BASAMP,1);} }
      if (RRAW == 0x3d7) { if (M_BASAMP  > 0)  {M_BASAMP --; ledIndicator(M_BASAMP,1);} }
      // TREBLE
      if (RRAW == 0x39c) { if (M_SPKAMP  < 99) {M_SPKAMP ++; ledIndicator(M_SPKAMP,2);} }
      if (RRAW == 0x39b) { if (M_SPKAMP  > 0)  {M_SPKAMP --; ledIndicator(M_SPKAMP,2);} }
      // LPF CUT
      if (RRAW == 0x3a3) { if (M_CUTOFF  < 99) {M_CUTOFF ++; ledIndicator(M_CUTOFF,3);} }
      if (RRAW == 0x399) { if (M_CUTOFF  > 0)  {M_CUTOFF --; ledIndicator(M_CUTOFF,3);} }

      // WL/WYL Przekazniki
      if (RRAW == 0x5c99) { 
        bool STA;
        if (digitalRead(RELAYS_SPK) != digitalRead(RELAYS_BAS) ) { digitalWrite(RELAYS_SPK, HIGH); digitalWrite(RELAYS_BAS, HIGH); STA=false; }
        else if (digitalRead(RELAYS_SPK)==LOW)
        { digitalWrite(RELAYS_SPK, HIGH); digitalWrite(RELAYS_BAS, HIGH); STA=false; }
        else
        { digitalWrite(RELAYS_SPK, LOW); digitalWrite(RELAYS_BAS, LOW); STA=true; }
        
        if (siteSelector==0)
        { SUB_REL_TOG.setState(STA); LR_REL_TOG.setState(STA); }
        delay(400);
      }

      // WL/WYL Pojed. Przekaznik
      if (RRAW == 0x3c7) { 
        bool STA;
        if (digitalRead(RELAYS_BAS)==LOW)
        { digitalWrite(RELAYS_BAS, HIGH); STA=false; }
        else
        { digitalWrite(RELAYS_BAS, LOW); STA=true; }
        if (siteSelector==0) { SUB_REL_TOG.setState(STA); }
        delay(400);
      }
      if (RRAW == 0x3da) { 
        bool STA;
        if (digitalRead(RELAYS_SPK)==LOW)
        { digitalWrite(RELAYS_SPK, HIGH); STA=false; }
        else
        { digitalWrite(RELAYS_SPK, LOW); STA=true; }
        if (siteSelector==0) { LR_REL_TOG.setState(STA); }
        delay(400);
      }

      // LED MODE ++
      if (RRAW == 0x5c95) {
        MODE_ITR++;
       if (MODE_ITR>3) MODE_ITR=0;
       switch(MODE_ITR)
       {
        case 0: { RAINBOW = false; VU = false; FFT_ON = false; STATIC_LIGHTS = true; break;}
        case 1: { RAINBOW = true; VU = false; FFT_ON = false; STATIC_LIGHTS = false; break;}
        case 2: { RAINBOW = false; VU = true; FFT_ON = false; STATIC_LIGHTS = false; break;}
        case 3: { RAINBOW = false; VU = false; FFT_ON = true; STATIC_LIGHTS = false; break;}
        default:break;
       }
       delay(400);
      }
      // VU MODE ++
      if (RRAW == 0x5cad) {
        if (VUMODE + 1 > 3) VUMODE=0;
        else VUMODE++;
        delay(400);
      }

      // LED H ++
      if (RRAW == 0x398)
      {
        BGcol.h+=2; mRing.h+=2; BGcol.s=255; mRing.s=255;
        for(int i = 0; i < 88; i++) {leds[i] = CHSV(BGcol.h, BGcol.s, BGcol.v);}
        for(int i = 88; i < 105; i++) {leds[i] = CHSV(mRing.h, mRing.s, mRing.v);}
        FastLED.show(); 
        delay(20);
      }

      // LED V ++
      if (RRAW == 0x39a)
      {
        BGcol.v+=5; mRing.v+=5; BGcol.s=255; mRing.s=255;
        for(int i = 0; i < 88; i++) {leds[i] = CHSV(BGcol.h, BGcol.s, BGcol.v);}
        for(int i = 88; i < 105; i++) {leds[i] = CHSV(mRing.h, mRing.s, mRing.v);}
        FastLED.show();
        delay(40); 
      }
      
      if (IrReceiver.decodedIRData.protocol == UNKNOWN) {/*Niepelny/Bledny sygnal, nic nie rob*/}
      IrReceiver.resume();
      }// DEL
    }
// PILOT IR END //
#endif
