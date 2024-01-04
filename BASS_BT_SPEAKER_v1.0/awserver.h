 #ifndef AWSERVER_H
 #define AWSERVER_H

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
    Serial.println("GET: podaje index.html");
  });

  server.on("/vumode", HTTP_GET, [] (AsyncWebServerRequest *request) {

    if (request->hasParam("mode")) 
    {
      String inputMessage = request->getParam("mode")->value();
      int val = inputMessage.toInt();
      VUMODE = val;
    }
    if (request->hasParam("fall")) 
    {
      String inputMessage = request->getParam("fall")->value();
      int val = inputMessage.toInt();
      VU_falloffMili = val;
    }
    String form = "Tryby";  
    request->send(200, "application/json",form.c_str());
    });  

  server.on("/vuCol", HTTP_GET, [] (AsyncWebServerRequest *request) {

    if (request->hasParam("LLh") && request->hasParam("LLs") && request->hasParam("LLv")) 
    {
      String inputMessage = request->getParam("LLh")->value();
      int val = inputMessage.toInt();
      LeftL.h = static_cast<byte>(val);
      inputMessage = request->getParam("LLs")->value();
      val = inputMessage.toInt();
      LeftL.s = static_cast<byte>(val);
      inputMessage = request->getParam("LLv")->value();
      val = inputMessage.toInt();
      LeftL.v = static_cast<byte>(val);
    }
    if (request->hasParam("LRh") && request->hasParam("LRs") && request->hasParam("LRv")) 
    {
      String inputMessage = request->getParam("LRh")->value();
      int val = inputMessage.toInt();
      LeftR.h = static_cast<byte>(val);
      inputMessage = request->getParam("LRs")->value();
      val = inputMessage.toInt();
      LeftR.s = static_cast<byte>(val);
      inputMessage = request->getParam("LRv")->value();
      val = inputMessage.toInt();
      LeftR.v = static_cast<byte>(val);
    }
    if (request->hasParam("RLh") && request->hasParam("RLs") && request->hasParam("RLv")) 
    {
      String inputMessage = request->getParam("RLh")->value();
      int val = inputMessage.toInt();
      RightL.h = static_cast<byte>(val);
      inputMessage = request->getParam("RLs")->value();
      val = inputMessage.toInt();
      RightL.s = static_cast<byte>(val);
      inputMessage = request->getParam("RLv")->value();
      val = inputMessage.toInt();
      RightL.v = static_cast<byte>(val);
    }
    if (request->hasParam("RRh") && request->hasParam("RRs") && request->hasParam("RRv")) 
    {
      String inputMessage = request->getParam("RRh")->value();
      int val = inputMessage.toInt();
      RightR.h = static_cast<byte>(val);
      inputMessage = request->getParam("RRs")->value();
      val = inputMessage.toInt();
      RightR.s = static_cast<byte>(val);
      inputMessage = request->getParam("RRv")->value();
      val = inputMessage.toInt();
      RightR.v = static_cast<byte>(val);
    }
    if (request->hasParam("MRh") && request->hasParam("MRs") && request->hasParam("MRv")) 
    {
      String inputMessage = request->getParam("MRh")->value();
      int val = inputMessage.toInt();
      mRing.h = static_cast<byte>(val);
      inputMessage = request->getParam("MRs")->value();
      val = inputMessage.toInt();
      mRing.s = static_cast<byte>(val);
      inputMessage = request->getParam("MRv")->value();
      val = inputMessage.toInt();
      mRing.v = static_cast<byte>(val);

      if (mRing.v == 0) isRingVU = false;
      else isRingVU = true;
    }
    
    String form = "Ustawienia kanalu VU COL";  
    request->send(200, "application/json",form.c_str());
    });  
    
  server.on("/ledamps", HTTP_GET, [] (AsyncWebServerRequest *request) {

    if (request->hasParam("min")) 
    {
      String inputMessage = request->getParam("min")->value();
      int val = inputMessage.toInt();
      VU_MinAmp = val;
    }
    if (request->hasParam("max")) 
    {
      String inputMessage = request->getParam("max")->value();
      int val = inputMessage.toInt();
      VU_MaxAmp = val;
    }
    String form = "Zmieniam czulosc MIC";  
    request->send(200, "application/json",form.c_str());
    });  
    
  server.on("/rain", HTTP_GET, [](AsyncWebServerRequest *request){
    int MODE = 0;
    if (request->hasParam("mode")) 
    {
      String inputMessage = request->getParam("mode")->value();
      MODE = inputMessage.toInt();
    }
    switch(MODE)
    {
      case 0: // sam kolor
      {
        RAINBOW = false; VU = false; FFT_ON = false; STATIC_LIGHTS = true;
        break;
      }
      case 1: // rainbow
      {
        RAINBOW = true; VU = false; FFT_ON = false; STATIC_LIGHTS = false;
        break;
      }
      case 2: // vu
      {
        RAINBOW = false; VU = true; FFT_ON = false; STATIC_LIGHTS = false;
        break;
      }
      case 3: // fft
      {
        RAINBOW = false; VU = false; FFT_ON = true; STATIC_LIGHTS = false;
        break;
      }
      default: break;
    }
    request->send(200, "application/json","Tecza");
    //Serial.println("GET: podaje index.html");
  });
  
  server.on("/setpot", HTTP_GET, [] (AsyncWebServerRequest *request) {
    int PNR = 0;
    int PVAL = 0;
    if (request->hasParam("nr")) 
    {
      String inputMessage = request->getParam("nr")->value();
      int val = inputMessage.toInt();
      PNR = val;
    }
    if (request->hasParam("val")) 
    {
      String inputMessage = request->getParam("val")->value();
      int val = inputMessage.toInt();
      PVAL = val;

      if (PNR==0) { M_SPKAMP  = PVAL; }
      if (PNR==1) { M_BASAMP  = PVAL; }
      if (PNR==2) { M_LOUD    = PVAL; }
      if (PNR==3) { M_CUTOFF  = PVAL; }
    }
    Serial.println("Server sie odezwal, ustawiam pot");
    String form = "Ustawiam pot nr: "+String(PNR)+" na val: "+String(PVAL);  
    request->send(200, "application/json",form.c_str());
    });  

  server.on("/givepots", HTTP_GET, [] (AsyncWebServerRequest *request) {

    int P0, P1, P2, P3;
    P0 = static_cast<int>(pot0.getPosition());
    P1 = static_cast<int>(pot1.getPosition());
    P2 = static_cast<int>(pot2.getPosition());
    P3 = static_cast<int>(pot3.getPosition());
    //String form = "POTS: P0 = "+String(P0)+" ,P1 = "+String(P1)+" ,P2 = "+String(P2)+" ,P3 = "+String(P3);  

    JSONcreator js1;
    js1.clearJson();
    // duze serva
    js1.strVal("P0",P0);
    js1.strVal("P1",P1);
    js1.strVal("P2",P2);
    js1.strVal("P3",P3);
    js1.endJson();
            
    request->send(200, "application/json",js1.giveCstr());
    });  

  server.on("/relays", HTTP_GET, [] (AsyncWebServerRequest *request) {

    if (request->hasParam("stereo")) 
    {
      String inputMessage = request->getParam("stereo")->value();
      int val = inputMessage.toInt();
      int STATE = val;
      if (STATE == 0) { digitalWrite(RELAYS_SPK, HIGH); }
      else digitalWrite(RELAYS_SPK, LOW);
    }
    if (request->hasParam("bass")) 
    {
      String inputMessage = request->getParam("bass")->value();
      int val = inputMessage.toInt();
      int STATE = val;
      if (STATE == 0) { digitalWrite(RELAYS_BAS, HIGH); }
      else digitalWrite(RELAYS_BAS, LOW);
    }
    String form = "Przelaczam POTy";  
    request->send(200, "application/json",form.c_str());
    });  
    
  server.on("/setled", HTTP_GET, [] (AsyncWebServerRequest *request) {
    int H,S,V;
    if (request->hasParam("h") && request->hasParam("s") && request->hasParam("v")) 
    {
      String inputMessage = request->getParam("h")->value();
      int val = inputMessage.toInt();
      H = val;
      inputMessage = request->getParam("s")->value();
      val = inputMessage.toInt();
      S = val;
      inputMessage = request->getParam("v")->value();
      val = inputMessage.toInt();
      V = val;
      BGcol.h = H; BGcol.s = S; BGcol.v = V;

     for(int i = 0; i < NUM_LEDS; i++) {leds[i] = CHSV(H, S, V);}
      FastLED.show(); 
    }

    Serial.println("Server sie odezwal, ustawiam led");
    String form = "OK";  
    request->send(200, "application/json",form.c_str());
    });  
  // REQ END

  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  server.begin();

  #endif
