 #ifndef SCREEN_H
 #define SCREEN_H

word hsv2hex(byte h, byte s, byte v);

 class button
 {
  public:
  button(const char *label, int DEL){
    lab = new char[strlen(label)];
    strcpy(lab,label);
    this->DELAY = DEL; };
    
  button(const char *label){
    lab = new char[strlen(label)];
    strcpy(lab,label);
    this->DELAY = 2; };
    
  ~button(){ delete [] this->lab; };

// @ DRAW ( START_X , START_Y , WIDTH , HEIGHT , LABEL )
  void draw(uint16_t bx, uint16_t by,uint16_t bw, uint16_t bh)
  {
    x = bx; y = by; w = bw; h = bh;
    for (int i=0; i<h; i++) { tft.drawFastHLine(x,y+i,w,hsv2hex(140,255-2*i,64) ); }
    tft.drawRect(x,y,w,h,hsv2hex(0,0,255) );
    tft.drawRect(x+1,y+1,w-2,h-2,hsv2hex(0,0,40) );
    tft.drawFastHLine(x+1,y+1,w-2,hsv2hex(0,0,160) );
    tft.drawFastVLine(x+1,y+1,h-2,hsv2hex(0,0,180) );
    tft.setTextColor(TFT_WHITE);
    tft.drawCentreString(lab, x+w/2, h/2+y-8, 2);
  }

  bool chkIfPress(uint16_t X, uint16_t Y, void(*foo)())
  {
    //this->pressOn = tft.getTouch(&this->TX, &this->TY);
    if (X>x && X<x+w && Y>y && Y<y+h && togDel.del(DELAY)) 
    { foo(); return true; }
    else return false;
  }
  private:
  uint16_t x,y,w,h;
  char* lab;
  Kronos togDel;
  int DELAY = 5;
 };

class valDisplay
{
  public:
  valDisplay(){}
  ~valDisplay(){}

  void draw(const char* prefix, const char* afix, int val, int x, int y, int w, int h)
  {
    tft.fillRect(x, y, w, h, TFT_BLACK);
    tft.drawRect(x, y, w, h, TFT_BLUE);
    String form = prefix+String(val)+afix;
    tft.setTextColor(TFT_WHITE);
    tft.drawCentreString(form, w/2+x, h/2+y-8, 2);
  }
};

class valBarDisp
{
  public:
  valBarDisp(){}
  ~valBarDisp(){}

  void draw(int val, int MIN, int MAX, int x, int y, int w, int h)
  {
    tft.fillRect(x, y, w, h, TFT_BLACK);
    tft.drawRect(x, y, w, h, TFT_BLUE);
    float DELTA = MAX-MIN;
    float coef = float(w/DELTA);
    float colCoef = float(100/DELTA);
    int BARCOL = int(colCoef*val);
    int BAR = round(coef*val);
    tft.fillRect(x+1, y+1, BAR-2, h-2, hsv2hex(100-BARCOL, 180, 250));
    //podzialki
    int DIV = w/5;
    for (int i=1; i<DIV-1; i++)
    {
      tft.drawFastVLine( x+i*5, y+1, h-2 ,TFT_BLACK );
      tft.drawFastVLine( x+1+i*5, y+1, h-2 ,TFT_BLACK );
    }
  }
};

class selectorFrame
{
  public:
  selectorFrame(){};
  ~selectorFrame(){};

  void draw(int x, int y, int w, int h, int thic)
  { for (int i=0; i<thic; i++) { tft.drawRect(x+i, y+i, w-i*2, h-i*2, hsv2hex(5,0,255) ); } }
    
  void clr(int x, int y, int w, int h, int thic)
  { for (int i=0; i<thic; i++) { tft.drawRect(x+i, y+i, w-i*2, h-i*2, TFT_BLACK); } }
};

class toggler
{
  public:
  toggler(const char *label, bool defState) { 
    this->state = (bool)defState;
    lab = new char[strlen(label)];
    strcpy(lab,label);
    }
  ~toggler(){ delete [] this->lab; }

  
  void draw(uint16_t bx, uint16_t by,uint16_t bw, uint16_t bh)
  {
    x = bx; y = by; w = bw; h = bh;
    // CALOSC
    tft.fillRect(x, y, w, h, TFT_BLACK);
    tft.drawRect(x, y, w, h, TFT_WHITE);
    // MALE OKIENKO
    tft.drawRect(x, y, h, h, TFT_WHITE);
    tft.drawRect(x+1, y+1, h-2, h-2, TFT_WHITE);
    tft.drawCentreString(lab, x+10+w/2, h/2+y-8, 2);
    if (this->state==true) { tft.fillRect(x+5, y+5, h-10, h-10, TFT_WHITE); }
  }
  
  void setState(bool st)
  {
    if (st==true) { 
      this->state = true;
      tft.fillRect(x+5, y+5, h-10, h-10, TFT_WHITE); }
    else {
      this->state = false;
      tft.fillRect(x+5, y+5, h-10, h-10, TFT_BLACK); }
  }
  
  bool chkIfPress(uint16_t X, uint16_t Y)
  {
    if (X>x && X<x+w && Y>y && Y<y+h && this->togDel.del(500)) 
    {
      if (this->state==false) { this->state = true; tft.fillRect(x+5, y+5, h-10, h-10, TFT_WHITE); }
      else { this->state = false; tft.fillRect(x+5, y+5, h-10, h-10, TFT_BLACK); }
      return true;
    }
    else return false;
  }

  bool getState(){ return this->state;  }

  private:
  uint16_t x,y,w,h;
  bool state;
  char *lab;
  Kronos togDel;
};

class radioBtn
{
  public:
  radioBtn(const char *label) { 
    lab = new char[strlen(label)];
    strcpy(lab,label);
    for (int i=0; i<10; i++) { this->iarr[i]=NULL; }
    }
  ~radioBtn(){ delete [] this->lab; }

  bool addItem( bool *item, const char* label)
  {
    for (int i=0; i<10; i++)
    {
      if (iarr[i]!=NULL) continue;
      else { iarr[i]=item;
            itemLabel[i] = new char[strlen(label)];
            strcpy(itemLabel[i],label);
            return true;
           }
    }
    return false;
  }
  
  void draw(uint16_t bx, uint16_t by,uint16_t bw, uint16_t bh)
  {
    x = bx; y = by; w = bw; h = bh;
    // CALOSC
    for (int i=0; i<10; i++)
    {
      if (this->iarr[i]!=NULL)
      {
        tft.fillRect(x, y+(i*h), w, h, TFT_BLACK);
        tft.drawRect(x, y+(i*h), w, h, TFT_WHITE);
        // MALE OKIENKO
        tft.drawRect(x, y+(i*h), h, h, TFT_WHITE);
        //tft.drawRect(x+1, y+(i*h)+1, h-2, h-2, TFT_WHITE);
        tft.drawCentreString(itemLabel[i], x+10+w/2, h/2+y+(i*h)-4, 1);
      } 
    }
  if (itemON >= 0)  { tft.fillRect(x+2, y+(itemON*h)+2, h-4, h-4, TFT_WHITE); }
  }

  bool chkIfPress(uint16_t X, uint16_t Y)
  {
    for (int i=0; i<10; i++)
    {
      if (X>x && X<x+w && Y>y+(i*h) && Y<y+(i*h)+h && this->togDel.del(500) && iarr[i]!=NULL)
      { itemON = i; for (int j=0; j<10; j++) { if (j==i) 
          { tft.fillRect(x+2, y+(i*h)+2, h-4, h-4, TFT_WHITE); }
          else { tft.fillRect(x+2, y+(j*h)+2, h-4, h-4, TFT_BLACK); } }
        this->selected(); return true;
      } } return false;
  }
  void selected()
  { for (int i=0; i<10; i++) {if (iarr[i]!=NULL) { *iarr[i] = false;} }
    if (itemON>=0 && iarr[itemON]!=NULL) { *iarr[itemON] = true; tft.fillRect(x+2, y+(itemON*h)+2, h-4, h-4, TFT_WHITE);  } }

  bool *iarr[10];
  char *itemLabel[10];
  int itemON = -1;
  
  private:
  uint16_t x,y,w,h;
  char *lab;
  Kronos togDel;
};

class ValElem
{
public:
  ValElem(int *val, int vmin, int vmax, int INC, const char *NAZWA) : iVAL(val) , VMIN(vmin), VMAX(vmax), vType('i'), iINC(INC)
  {
    uint16_t len = strlen(NAZWA);
    nazwa = new char [len];
    strcpy(nazwa,NAZWA);
  }

  ValElem(byte *val, int vmin, int vmax, int INC, const char *NAZWA) : bVAL(val) , VMIN(vmin), VMAX(vmax), vType('b'), iINC(INC)
  {
    uint16_t len = strlen(NAZWA);
    nazwa = new char [len];
    strcpy(nazwa,NAZWA);
  }

  ValElem(word *val, int vmin, int vmax, int INC, const char *NAZWA) : wVAL(val) , VMIN(vmin), VMAX(vmax), vType('w'), iINC(INC)
  {
    uint16_t len = strlen(NAZWA);
    nazwa = new char [len];
    strcpy(nazwa,NAZWA);
  }

  ValElem(uint16_t *val, int vmin, int vmax, int INC, const char *NAZWA) : u16VAL(val) , VMIN(vmin), VMAX(vmax), vType('l'), iINC(INC)
  {
    uint16_t len = strlen(NAZWA);
    nazwa = new char [len];
    strcpy(nazwa,NAZWA);
  }

  ValElem(float *val, float vmin, float vmax, float INC, const char *NAZWA) : fVAL(val) , fVMIN(vmin), fVMAX(vmax), vType('f'), fINC(INC)
  {
    uint16_t len = strlen(NAZWA);
    nazwa = new char [len];
    strcpy(nazwa,NAZWA);
  }

  ValElem(double *val, float vmin, float vmax, float INC, const char *NAZWA) : dVAL(val) , fVMIN(vmin), fVMAX(vmax), vType('d'), fINC(INC)
  {
    uint16_t len = strlen(NAZWA);
    nazwa = new char [len];
    strcpy(nazwa,NAZWA);
  }
  ~ValElem(){}

char *nazwa;
char vType = 'i';

int *iVAL = NULL;
byte *bVAL = NULL;
float *fVAL = NULL;
word *wVAL = NULL;
uint16_t *u16VAL = NULL;
double *dVAL = NULL;

int iINC = 1;
float fINC = 0.1;

int VMIN=0, VMAX=100;  
float fVMIN=0.0, fVMAX=100.0;  
private:

};

class ValChngObjArrtList
{
public:
  ValChngObjArrtList(ValElem **tab, byte ELEM ) : vElem(tab), elements(ELEM), listIter(0) {}
  ~ValChngObjArrtList(){}

  Kronos ValChngDel;
  int VCDdel = 75;
  void draw(uint16_t vX, uint16_t vY, uint16_t textW)
  {
    sx = vX; sy = vY; t_width = textW;
    // Gorne okno
    tft.setTextColor(TFT_WHITE);
    tft.fillRect(sx,sy,40,40,hsv2hex(140,240,255) );
      tft.drawCentreString("<", sx+20, sy+6, 4);
    tft.fillRect(sx+40,sy, t_width, 40,TFT_BLACK );
    tft.fillRect(sx+40+t_width,sy,40,40,hsv2hex(5,240,255) );
      tft.drawCentreString(">", sx+60+t_width, sy+6, 4);
    
    // Dolne okno
    tft.fillRect(sx,sy+40,40,40,hsv2hex(140,120,255) );
      tft.drawCentreString("-", sx+20, sy+46, 4);
    tft.fillRect(sx+40,sy+40, t_width ,40,hsv2hex(0,0,64) );
    tft.fillRect(sx+40+t_width ,sy+40,40,40,hsv2hex(5,120,255) );
      tft.drawCentreString("+", sx+60+t_width, sy+46, 4);

    tft.setTextFont(4);
    tft.drawCentreString(vElem[0]->nazwa, sx+((80+t_width)/2), sy+6, 4);
    switch(vElem[0]->vType)
    {
      case 'i': {tft.drawCentreString(String(*vElem[0]->iVAL), sx+((80+t_width)/2), sy+46, 4); break; }
      case 'b': {tft.drawCentreString(String(*vElem[0]->bVAL), sx+((80+t_width)/2), sy+46, 4); break; }
      case 'w': {tft.drawCentreString(String(*vElem[0]->wVAL), sx+((80+t_width)/2), sy+46, 4); break; }
      case 'l': {tft.drawCentreString(String(*vElem[0]->u16VAL), sx+((80+t_width)/2), sy+46, 4); break; }
      case 'f': { tft.drawFloat((*vElem[0]->fVAL), 5, sx+((80+t_width)/2), sy+46, 4); break; }
      case 'd': { tft.drawFloat((*vElem[0]->dVAL), 7, sx+((80+t_width)/2), sy+46, 4); break; }
      default: break;
    }
  }

  void chkIfPress(uint16_t X, uint16_t Y)
  {
    // Typ wartosci ' - '
    if (X>sx && X<sx+40 && Y>sy && Y<sy+40 && listIter > 0 && ValChngDel.del(VCDdel*5))
    {
      listIter--;
      tft.fillRect(sx+40,sy, t_width, 40,TFT_BLACK );
      tft.setTextColor(TFT_WHITE);
      tft.setTextFont(4);
      tft.drawCentreString(vElem[listIter]->nazwa, sx+((80+t_width)/2), sy+6, 4);  

      tft.fillRect(sx+40,sy+40,t_width,40,hsv2hex(0,0,64) );
      tft.setTextColor(TFT_WHITE);
      tft.setTextFont(4);
      //tft.drawCentreString(String(*vElem[listIter]->iVAL), sx+((80+t_width)/2), sy+46, 4);
        switch(vElem[listIter]->vType)
        {
          case 'i': {tft.drawCentreString(String(*vElem[listIter]->iVAL), sx+((80+t_width)/2), sy+46, 4); break; }
          //case 'f': {tft.drawCentreString(String(*vElem[listIter]->fVAL), sx+((80+t_width)/2), sy+46, 4); break; }
          case 'b': {tft.drawCentreString(String(*vElem[listIter]->bVAL), sx+((80+t_width)/2), sy+46, 4); break; }
          case 'w': {tft.drawCentreString(String(*vElem[listIter]->wVAL), sx+((80+t_width)/2), sy+46, 4); break; }
          case 'l': {tft.drawCentreString(String(*vElem[listIter]->u16VAL), sx+((80+t_width)/2), sy+46, 4); break; }
          case 'f': { tft.drawFloat((*vElem[listIter]->fVAL), 5, sx+((80+t_width)/2), sy+46, 4); break; }
          case 'd': { tft.drawFloat((*vElem[listIter]->dVAL), 7, sx+((80+t_width)/2), sy+46, 4); break; }
          default: break;
        }
    }
    // Typ wartosci ' + '
    else if (X>sx+40+t_width && X<sx+80+t_width && Y>sy && Y<sy+40 && listIter < elements-1 && ValChngDel.del(VCDdel*5))
    {
      listIter++;
      tft.fillRect(sx+40,sy,t_width,40,TFT_BLACK );
      tft.setTextColor(TFT_WHITE);
      tft.setTextFont(4);
      tft.drawCentreString(vElem[listIter]->nazwa, sx+((80+t_width)/2), sy+6, 4); 

      tft.fillRect(sx+40,sy+40,t_width,40,hsv2hex(0,0,64) );
      tft.setTextColor(TFT_WHITE);
      tft.setTextFont(4);
        switch(vElem[listIter]->vType)
        {
          case 'i': {tft.drawCentreString(String(*vElem[listIter]->iVAL), sx+((80+t_width)/2), sy+46, 4); break; }
          case 'b': {tft.drawCentreString(String(*vElem[listIter]->bVAL), sx+((80+t_width)/2), sy+46, 4); break; }
          case 'w': {tft.drawCentreString(String(*vElem[listIter]->wVAL), sx+((80+t_width)/2), sy+46, 4); break; }
          case 'l': {tft.drawCentreString(String(*vElem[listIter]->u16VAL), sx+((80+t_width)/2), sy+46, 4); break; }
          case 'f': { tft.drawFloat((*vElem[listIter]->fVAL), 5, sx+((80+t_width)/2), sy+46, 4); break; }
          case 'd': { tft.drawFloat((*vElem[listIter]->dVAL), 7, sx+((80+t_width)/2), sy+46, 4); break; }
          default: break;
        }      
    }
    // Val --
    else if (X>sx && X<sx+40 && Y>sy+40 && Y<sy+80 && ValChngDel.del(VCDdel))
    {
      switch(vElem[listIter]->vType)
      {
      case 'i': { if ((*vElem[listIter]->iVAL) > vElem[listIter]->VMIN) 
      {if ((*vElem[listIter]->iVAL) - (vElem[listIter]->iINC) < vElem[listIter]->VMIN) {(*vElem[listIter]->iVAL)=vElem[listIter]->VMIN;}
        else {(*vElem[listIter]->iVAL)-= (vElem[listIter]->iINC);}} break; }
      case 'f': { if ((*vElem[listIter]->fVAL) > vElem[listIter]->fVMIN)
      {if ((*vElem[listIter]->fVAL) - (vElem[listIter]->fINC) < vElem[listIter]->fVMIN) {(*vElem[listIter]->fVAL)=vElem[listIter]->fVMIN;}
        else {(*vElem[listIter]->fVAL)-= (vElem[listIter]->fINC);}} break; }
      case 'b': { if ((*vElem[listIter]->bVAL) > vElem[listIter]->VMIN)
      {if ((*vElem[listIter]->bVAL) - (vElem[listIter]->iINC) < vElem[listIter]->VMIN) {(*vElem[listIter]->bVAL)=vElem[listIter]->VMIN;}
        else {(*vElem[listIter]->bVAL)-= (vElem[listIter]->iINC);}} break; }
      case 'w': { if ((*vElem[listIter]->wVAL) > vElem[listIter]->VMIN)
      {if ((*vElem[listIter]->wVAL) - (vElem[listIter]->iINC) < vElem[listIter]->VMIN) {(*vElem[listIter]->wVAL)=vElem[listIter]->VMIN;}
        else {(*vElem[listIter]->wVAL)-= (vElem[listIter]->iINC);}} break; }
      case 'l': { if ((*vElem[listIter]->u16VAL) > vElem[listIter]->VMIN)
      {if ((*vElem[listIter]->u16VAL) - (vElem[listIter]->iINC) < vElem[listIter]->VMIN) {(*vElem[listIter]->u16VAL)=vElem[listIter]->VMIN;}
        else {(*vElem[listIter]->u16VAL)-= (vElem[listIter]->iINC);}} break; }
      case 'd': { if ((*vElem[listIter]->dVAL) > vElem[listIter]->fVMIN)
      {if ((*vElem[listIter]->dVAL) - (vElem[listIter]->fINC) < vElem[listIter]->fVMIN) {(*vElem[listIter]->dVAL)=vElem[listIter]->fVMIN;}
        else {(*vElem[listIter]->dVAL)-= (vElem[listIter]->fINC);}} break; }
      default: break;
      }
      //(*vElem[listIter]->iVAL)--;
      tft.fillRect(sx+40,sy+40,t_width,40,hsv2hex(0,0,64) );
      tft.setTextColor(TFT_WHITE);
      tft.setTextFont(4);
      switch(vElem[listIter]->vType)
      {
      case 'i': {tft.drawCentreString(String(*vElem[listIter]->iVAL), sx+((80+t_width)/2), sy+46, 4); break; }
      case 'b': {tft.drawCentreString(String(*vElem[listIter]->bVAL), sx+((80+t_width)/2), sy+46, 4); break; }
      case 'w': {tft.drawCentreString(String(*vElem[listIter]->wVAL), sx+((80+t_width)/2), sy+46, 4); break; }
      case 'l': {tft.drawCentreString(String(*vElem[listIter]->u16VAL), sx+((80+t_width)/2), sy+46, 4); break; }
      case 'f': { tft.drawFloat((*vElem[listIter]->fVAL), 5, sx+((80+t_width)/2), sy+46, 4); break; }
      case 'd': { tft.drawFloat((*vElem[listIter]->dVAL), 7, sx+((80+t_width)/2), sy+46, 4); break; }
      default: break;
      }
    }
    // Val ++
    else if (X>sx+40+t_width && X<sx+80+t_width && Y>sy+40 && Y<sy+80 && ValChngDel.del(VCDdel))
    {
      switch(vElem[listIter]->vType)
      {
      case 'i': { if((*vElem[listIter]->iVAL) < vElem[listIter]->VMAX)
      {if ((*vElem[listIter]->iVAL)+vElem[listIter]->iINC > vElem[listIter]->VMAX) {(*vElem[listIter]->iVAL)=vElem[listIter]->VMAX;}
        else {(*vElem[listIter]->iVAL)+= (vElem[listIter]->iINC);}} break; }
      case 'f': { if((*vElem[listIter]->fVAL) < vElem[listIter]->fVMAX)
      {if ((*vElem[listIter]->fVAL)+vElem[listIter]->fINC > vElem[listIter]->fVMAX) {(*vElem[listIter]->fVAL)=vElem[listIter]->fVMAX;}
        else {(*vElem[listIter]->fVAL)+= (vElem[listIter]->fINC);}} break; }
      case 'b': { if((*vElem[listIter]->bVAL) < vElem[listIter]->VMAX)
      {if ((*vElem[listIter]->bVAL)+vElem[listIter]->iINC > vElem[listIter]->VMAX) {(*vElem[listIter]->bVAL)=vElem[listIter]->VMAX;}
        else {(*vElem[listIter]->bVAL)+= (vElem[listIter]->iINC);}} break; }
      case 'w': { if((*vElem[listIter]->wVAL) < vElem[listIter]->VMAX)
      {if ((*vElem[listIter]->wVAL)+vElem[listIter]->iINC > vElem[listIter]->VMAX) {(*vElem[listIter]->wVAL)=vElem[listIter]->VMAX;}
        else {(*vElem[listIter]->wVAL)+= (vElem[listIter]->iINC);}} break; }
      case 'l': { if((*vElem[listIter]->u16VAL) < vElem[listIter]->VMAX)
      {if ((*vElem[listIter]->u16VAL)+vElem[listIter]->iINC > vElem[listIter]->VMAX) {(*vElem[listIter]->u16VAL)=vElem[listIter]->VMAX;}
        else {(*vElem[listIter]->u16VAL)+= (vElem[listIter]->iINC);}} break; }
      case 'd': { if((*vElem[listIter]->dVAL) < vElem[listIter]->fVMAX)
      {if ((*vElem[listIter]->dVAL)+vElem[listIter]->fINC > vElem[listIter]->fVMAX) {(*vElem[listIter]->dVAL)=vElem[listIter]->fVMAX;}
        else {(*vElem[listIter]->dVAL)+= (vElem[listIter]->fINC);}} break; }
      default: break;
      }
      tft.fillRect(sx+40,sy+40,t_width,40,hsv2hex(0,0,64) );
      tft.setTextColor(TFT_WHITE);
      tft.setTextFont(4);
      switch(vElem[listIter]->vType)
      {
      case 'i': {tft.drawCentreString(String(*vElem[listIter]->iVAL), sx+((80+t_width)/2), sy+46, 4); break; }
      case 'b': {tft.drawCentreString(String(*vElem[listIter]->bVAL), sx+((80+t_width)/2), sy+46, 4); break; }
      case 'w': {tft.drawCentreString(String(*vElem[listIter]->wVAL), sx+((80+t_width)/2), sy+46, 4); break; }
      case 'l': {tft.drawCentreString(String(*vElem[listIter]->u16VAL), sx+((80+t_width)/2), sy+46, 4); break; }
      case 'f': { tft.drawFloat((*vElem[listIter]->fVAL), 5, sx+((80+t_width)/2), sy+46, 4); break; }
      case 'd': { tft.drawFloat((*vElem[listIter]->dVAL), 7, sx+((80+t_width)/2), sy+46, 4); break; }
      default: break;
      }
    }
  }

private:
  uint16_t sx, sy, t_width;
  ValElem **vElem;
  int elements;
  word bg1, bg2;
  byte listIter;

};

class spkLedSelector
{
  public:
    spkLedSelector() : WID(30) , H(255), S(255), V(255) {}
    ~spkLedSelector(){}
    
  
  void draw(uint16_t vX, uint16_t vY)
  {
    this->cX = vX; this->cY = vY;
    // WYPELNIONE KOLORAMI
    tft.fillRect(cX-WID-80, cY, 40, 80, hsv2hex(cLLh,cLLs,cLLv));      // LL
    tft.fillRect(cX-WID-40, cY, 40, 80, hsv2hex(cLRh,cLRs,cLRv));      // LR
    tft.fillRect(cX+WID, cY, 40, 80, hsv2hex(cRLh,cRLs,cRLv));         // RL
    tft.fillRect(cX+WID+40, cY, 40, 80, hsv2hex(cRRh,cRRs,cRRv));      // RR
    tft.fillRect(cX-WID, cY, 60, 95, hsv2hex(cBGh,cBGs,cBGv));         // CENTER    
    tft.fillCircle(cX, cY+66, 20, hsv2hex(cRNGh,cRNGs,cRNGv));         // RING
      
    // Center SUB
    tft.drawRect(cX-WID, cY, 60, 95, TFT_WHITE);
    //tft.drawCircle(cX, cY+18, 8, TFT_WHITE);
    tft.drawCircle(cX, cY+66, 20, TFT_WHITE);
    // LEFT
    tft.drawRect(cX-WID-80, cY, 40, 80, TFT_WHITE);
    tft.drawRect(cX-WID-40, cY, 40, 80, TFT_WHITE);
    // RIGHT
    tft.drawRect(cX+WID, cY, 40, 80, TFT_WHITE);
    tft.drawRect(cX+WID+40, cY, 40, 80, TFT_WHITE);
    // PODPIS
    tft.drawCentreString("Bg.Lit", cX, cY+8, 1);
  }

  void drawbar_H (uint16_t H_SX, uint16_t H_SY, uint16_t H_W, uint16_t H_H)
  {
    // Wstepne przypisanie wartosci
    hp_sx = H_SX;
    hp_sy = H_SY;
    hp_w  = H_W;
    hp_h  = H_H;
    hp_indi = hp_sx;

    tft.drawRect(hp_sx-1, hp_sy-1, hp_w+2, hp_h+2,TFT_WHITE);
    // Rysuj H bar
    hCoef = float(255./hp_w);
    for (int i=0; i<hp_w; i++)
    {
      tft.drawFastVLine(hp_sx+i, hp_sy, hp_h, hsv2hex(byte(i*hCoef),S,V) );
    }
  }

  void drawbar_S (uint16_t S_SX, uint16_t S_SY, uint16_t S_W, uint16_t S_H)
  {
    // Wstepne przypisanie wartosci
    sp_sx = S_SX;
    sp_sy = S_SY;
    sp_w  = S_W;
    sp_h  = S_H;
    sp_indi = sp_sx;

    tft.drawRect(sp_sx-1, sp_sy-1, sp_w+2, sp_h+2,TFT_WHITE);
    // Rysuj S bar
    sCoef = float(255./sp_w);
    for (int i=0; i<sp_w; i++)
    {
      tft.drawFastVLine(sp_sx+i, sp_sy, sp_h, hsv2hex(H,byte(i*sCoef),V) );
    }
  }

  void drawbar_V (uint16_t V_SX, uint16_t V_SY, uint16_t V_W, uint16_t V_H)
  {
    // Wstepne przypisanie wartosci
    vp_sx = V_SX;
    vp_sy = V_SY;
    vp_w  = V_W;
    vp_h  = V_H;
    vp_indi = vp_sx;

    tft.drawRect(vp_sx-1, vp_sy-1, vp_w+2, vp_h+2,TFT_WHITE);
    // Rysuj V bar
    vCoef = float(255./vp_w);
    for (int i=0; i<vp_w; i++)
    {
      tft.drawFastVLine(vp_sx+i, vp_sy, vp_h, hsv2hex(H,S,byte(i*vCoef)) );
    }
  }

  void draw_RGB(uint16_t rgb_SX, uint16_t rgb_SY, uint16_t rgb_W, uint16_t rgb_H)
  {
    rgb_sx = rgb_SX;
    rgb_sy = rgb_SY;
    rgb_w = rgb_W;
    rgb_h = rgb_H;

    tft.fillRect(rgb_sx, rgb_sy, rgb_w, rgb_h, hsv2hex(H,S,V) );
    tft.drawRect(rgb_sx-1, rgb_sy-1, rgb_w+2, rgb_h+2, TFT_BLACK);
    tft.drawRect(rgb_sx-2, rgb_sy-2, rgb_w+4, rgb_h+4, TFT_WHITE);
  }
//tft.drawCircle(x, y, radius, color);
//tft.drawRect(cx - i2, cy - i2, i, i, color);
  bool chkIfPress(uint16_t X, uint16_t Y)
  {
    // LEFT LEFT
    if (X>cX-WID-80 && X<cX-WID-40 && Y>cY && Y<cY+80 )
    {
      cLLh = H; cLLs = S; cLLv = V;
      tft.fillRect(cX-WID-80, cY, 40, 80, hsv2hex(cLLh,cLLs,cLLv)); // LL FILLED
      tft.drawRect(cX-WID-80, cY, 40, 80, TFT_RED);                 // LL
      tft.drawRect(cX-WID-40, cY, 40, 80, TFT_WHITE);               // LR
      tft.drawRect(cX+WID, cY, 40, 80, TFT_WHITE);                  // RL
      tft.drawRect(cX+WID+40, cY, 40, 80, TFT_WHITE);               // RR
      tft.drawRect(cX-WID, cY, 60, 95, TFT_WHITE);                  // CENTER
      tft.drawCircle(cX, cY+66, 20, TFT_WHITE);                     // RING
      PART = 1;
      LeftL = (myLed) {.h=cLLh, .s=cLLs, .v=cLLv };
      return true;
    }
    // LEFT RIGHT
    if (X>cX-WID-40 && X<cX-WID && Y>cY && Y<cY+80 )
    {
      cLRh = H; cLRs = S; cLRv = V;
      tft.fillRect(cX-WID-40, cY, 40, 80, hsv2hex(cLRh,cLRs,cLRv)); // LR FILLED
      tft.drawRect(cX-WID-80, cY, 40, 80, TFT_WHITE);               // LL
      tft.drawRect(cX-WID-40, cY, 40, 80, TFT_RED);                 // LR
      tft.drawRect(cX+WID, cY, 40, 80, TFT_WHITE);                  // RL
      tft.drawRect(cX+WID+40, cY, 40, 80, TFT_WHITE);               // RR
      tft.drawRect(cX-WID, cY, 60, 95, TFT_WHITE);                  // CENTER
      tft.drawCircle(cX, cY+66, 20, TFT_WHITE);                     // RING
      PART = 2;
      LeftR = (myLed) {.h=cLRh, .s=cLRs, .v=cLRv };
      return true;
    }

    // RIGHT LEFT
    if (X>cX+WID && X<cX+WID+40 && Y>cY && Y<cY+80 )
    {
      cRLh = H; cRLs = S; cRLv = V;
      tft.fillRect(cX+WID, cY, 40, 80, hsv2hex(cRLh,cRLs,cRLv)); // RL FILLED
      tft.drawRect(cX-WID-80, cY, 40, 80, TFT_WHITE);            // LL
      tft.drawRect(cX-WID-40, cY, 40, 80, TFT_WHITE);            // LR
      tft.drawRect(cX+WID, cY, 40, 80, TFT_RED);                 // RL
      tft.drawRect(cX+WID+40, cY, 40, 80, TFT_WHITE);            // RR
      tft.drawRect(cX-WID, cY, 60, 95, TFT_WHITE);               // CENTER
      tft.drawCircle(cX, cY+66, 20, TFT_WHITE);                  // RING
      PART = 3;
      RightL = (myLed) {.h=cRLh, .s=cRLs, .v=cRLv };
      return true;
    }
    // RIGHT RIGHT
    if (X>cX+WID+40 && X<cX+WID+80 && Y>cY && Y<cY+80 )
    {
      cRRh = H; cRRs = S; cRRv = V;
      tft.fillRect(cX+WID+40, cY, 40, 80, hsv2hex(cRRh,cRRs,cRRv)); // RR FILLED
      tft.drawRect(cX-WID-80, cY, 40, 80, TFT_WHITE);               // LL
      tft.drawRect(cX-WID-40, cY, 40, 80, TFT_WHITE);               // LR
      tft.drawRect(cX+WID, cY, 40, 80, TFT_WHITE);                  // RL
      tft.drawRect(cX+WID+40, cY, 40, 80, TFT_RED);                 // RR
      tft.drawRect(cX-WID, cY, 60, 95, TFT_WHITE);                  // CENTER
      tft.drawCircle(cX, cY+66, 20, TFT_WHITE);                     // RING
      PART = 4;
      RightR = (myLed) {.h=cRRh, .s=cRRs, .v=cRRv };
      return true;
    }
    // CENTER - BG
    if (X>cX-WID && X<cX+WID && Y>cY && Y<cY+50 )
    {
      cBGh = H; cBGs = S; cBGv = V;
      tft.fillRect(cX-WID, cY, 60, 95, hsv2hex(cBGh,cBGs,cBGv));  // CENTER FILLED
      tft.drawRect(cX-WID-80, cY, 40, 80, TFT_WHITE);             // LL
      tft.drawRect(cX-WID-40, cY, 40, 80, TFT_WHITE);             // LR
      tft.drawRect(cX+WID, cY, 40, 80, TFT_WHITE);                // RL
      tft.drawRect(cX+WID+40, cY, 40, 80, TFT_WHITE);             // RR
      tft.drawRect(cX-WID, cY, 60, 95, TFT_RED);                  // CENTER
      
      tft.fillCircle(cX, cY+66, 20, hsv2hex(cRNGh,cRNGs,cRNGv));  // RING TLO
      tft.drawCircle(cX, cY+66, 20, TFT_WHITE);                   // RING
      tft.drawCentreString("Bg.Lit", cX, cY+8, 1);
      PART = 5;
      BGcol = (myLed) {.h=cBGh, .s=cBGs, .v=cBGv };
      return true;
    }
    // RING
    if (X>cX-WID && X<cX+WID && Y>cY+50 && Y<cY+95 )
    {
      cRNGh = H; cRNGs = S; cRNGv = V;
      tft.fillCircle(cX, cY+66, 20, hsv2hex(cRNGh,cRNGs,cRNGv));  // RING FILLED
      tft.drawRect(cX-WID-80, cY, 40, 80, TFT_WHITE);             // LL
      tft.drawRect(cX-WID-40, cY, 40, 80, TFT_WHITE);             // LR
      tft.drawRect(cX+WID, cY, 40, 80, TFT_WHITE);                // RL
      tft.drawRect(cX+WID+40, cY, 40, 80, TFT_WHITE);             // RR
      tft.drawRect(cX-WID, cY, 60, 95, TFT_WHITE);                // CENTER
      tft.drawCircle(cX, cY+66, 20, TFT_RED);                     // RING
      PART = 6;
      mRing = (myLed) {.h=cRNGh, .s=cRNGs, .v=cRNGv };
      return true;
    }
    
    return false;
  }
  
  bool chkIfPressHSV(uint16_t X, uint16_t Y)
  {
    // Dotyk na H barze
    if (X>hp_sx && X<hp_sx+hp_w && Y>hp_sy && Y<hp_sy+hp_h)
    {
      H = byte((X-hp_sx)*hCoef);
      tft.fillRect(rgb_sx, rgb_sy, rgb_w, rgb_h, hsv2hex(H,S,V) );

      // Aktualizuj S i V
      for (int i=0; i<sp_w; i++) {tft.drawFastVLine(sp_sx+i, sp_sy, sp_h, hsv2hex(H,byte(i*sCoef),V) );}
      for (int i=0; i<vp_w; i++) {tft.drawFastVLine(vp_sx+i, vp_sy, vp_h, hsv2hex(H,S,byte(i*vCoef)) );}
      return true;
    }// Dotyk na H barze

    // Dotyk na S barze
    if (X>sp_sx && X<sp_sx+sp_w && Y>sp_sy && Y<sp_sy+sp_h)
    {
      S = byte((X-sp_sx)*sCoef);
      tft.fillRect(rgb_sx, rgb_sy, rgb_w, rgb_h, hsv2hex(H,S,V) );

      // Aktualizuj H i V
      for (int i=0; i<hp_w; i++) {tft.drawFastVLine(hp_sx+i, hp_sy, hp_h, hsv2hex(byte(i*hCoef),S,V) );}
      for (int i=0; i<vp_w; i++) {tft.drawFastVLine(vp_sx+i, vp_sy, vp_h, hsv2hex(H,S,byte(i*vCoef)) );}
      return true;
    }// Dotyk na S barze

    // Dotyk na V barze
    if (X>vp_sx && X<vp_sx+vp_w && Y>vp_sy && Y<vp_sy+vp_h)
    {
      V = byte((X-vp_sx)*vCoef);
      tft.fillRect(rgb_sx, rgb_sy, rgb_w, rgb_h, hsv2hex(H,S,V) );

      // Aktualizuj H i S
      for (int i=0; i<hp_w; i++) {tft.drawFastVLine(hp_sx+i, hp_sy, hp_h, hsv2hex(byte(i*hCoef),S,V) );}
      for (int i=0; i<sp_w; i++) {tft.drawFastVLine(sp_sx+i, sp_sy, sp_h, hsv2hex(H,byte(i*sCoef),V) );}
      return true;
    }// Dotyk na V barze
    return false;
  }
    
    private:
    
    int WID = 30;
    uint16_t cX, cY;
    Kronos togDel;
    int PART = -1;

    // HSV Bary
    uint16_t hp_sx,  hp_sy,  hp_w,  hp_h,  hp_indi;
    uint16_t sp_sx,  sp_sy,  sp_w,  sp_h,  sp_indi;
    uint16_t vp_sx,  vp_sy,  vp_w,  vp_h,  vp_indi;
    uint16_t rgb_sx=10, rgb_sy=10, rgb_w=10, rgb_h=10;
    
    byte H, S, V; // aktualne HSV
    float hCoef, sCoef, vCoef;

    // Kolory dla segmentow speakera
    byte cLLh=0,cLLs=0,cLLv=0;
    byte cLRh=0,cLRs=0,cLRv=0;
    byte cRLh=0,cRLs=0,cRLv=0;
    byte cRRh=0,cRRs=0,cRRv=0;
    byte cBGh=0,cBGs=0,cBGv=0;
    byte cRNGh=0,cRNGs=0,cRNGv=0;
};

 #endif
