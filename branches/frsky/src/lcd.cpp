/*
 * gruvin9x Author Bryan J.Rentoul (Gruvin) <gruvin@gmail.com>
 *
 * gruvin9x is based on code named er9x by
 * Author - Erez Raviv <erezraviv@gmail.com>, which is in turn
 * based on th9x -> http://code.google.com/p/th9x/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include "gruvin9x.h"


uint8_t displayBuf[DISPLAY_W*DISPLAY_H/8];
#define DISPLAY_END (displayBuf+sizeof(displayBuf))
#include "font.lbm"
#define font_5x8_x20_x7f (font+3)

#include "font_dblsize.lbm"
#define font_10x16_x20_x7f (font_dblsize+3)

void lcd_clear()
{
  memset(displayBuf, 0, sizeof(displayBuf));
}


void lcd_img(uint8_t i_x,uint8_t i_y,const prog_uchar * imgdat,uint8_t idx,uint8_t mode)
{
  const prog_uchar  *q = imgdat;
  uint8_t w    = pgm_read_byte(q++);
  uint8_t hb   = (pgm_read_byte(q++)+7)/8;
  uint8_t sze1 = pgm_read_byte(q++);
  q += idx*sze1;
  bool    inv  = (mode & INVERS) ? true : (mode & BLINK ? BLINK_ON_PHASE : false);
  for(uint8_t yb = 0; yb < hb; yb++){
    uint8_t   *p = &displayBuf[ (i_y / 8 + yb) * DISPLAY_W + i_x ];
    for(uint8_t x=0; x < w; x++){
      uint8_t b = pgm_read_byte(q++);
      *p++ = inv ? ~b : b;
    }
  }
}

void lcd_putcAtt(uint8_t x,uint8_t y,const char c,uint8_t mode)
{
  uint8_t *p    = &displayBuf[ y / 8 * DISPLAY_W + x ];
  //uint8_t *pmax = &displayBuf[ DISPLAY_H/8 * DISPLAY_W ];

  prog_uchar    *q = &font_5x8_x20_x7f[ + (c-0x20)*5];
  bool         inv = (mode & INVERS) ? true : (mode & BLINK ? BLINK_ON_PHASE : false);
  if(mode&DBLSIZE)
  {
    /* each letter consists of ten top bytes followed by
     * five bottom by ten bottom bytes (20 bytes per 
     * char) */
    q = &font_10x16_x20_x7f[(c-0x20)*10 + ((c-0x20)/16)*160];
    for(char i=5; i>=0; i--){
      /*top byte*/
      uint8_t b1 = i>0 ? pgm_read_byte(q) : 0;
      /*bottom byte*/
      uint8_t b3 = i>0 ? pgm_read_byte(160+q) : 0;
      /*top byte*/
      uint8_t b2 = i>0 ? pgm_read_byte(++q) : 0;
      /*bottom byte*/
      uint8_t b4 = i>0 ? pgm_read_byte(160+q) : 0;
      q++;
      if(inv) {
        b1=~b1;
        b2=~b2;
        b3=~b3;
        b4=~b4;
      }   

      if(&p[DISPLAY_W+1] < DISPLAY_END){
        p[0]=b1;
        p[1]=b2;
        p[DISPLAY_W] = b3; 
        p[DISPLAY_W+1] = b4; 
        p+=2;
      }   
    }   

//    for(char i=5; i>=0; i--){
//      uint8_t b = i ? pgm_read_byte(q++) : 0;
//      if(inv) b=~b;
//      static uint8_t dbl[]={0x00,0x03,0x0c,0x0f, 0x30,0x33,0x3c,0x3f,
//                            0xc0,0xc3,0xcc,0xcf, 0xf0,0xf3,0xfc,0xff};
//      if(&p[DISPLAY_W+1] < DISPLAY_END){
//        p[0] = p[1] = dbl[b&0xf];
//        p[DISPLAY_W]=p[DISPLAY_W+1] = dbl[b>>4];
//        p+=2;
//      }
//    }
  }
  else {
    uint8_t condense=0;

    if (mode & CONDENSED) {
        *p++ = inv ? ~0 : 0;
        condense=1;
    }

    for (char i=5; i!=0; i--) {
        uint8_t b = pgm_read_byte(q++);
        if (condense && i==4) {
            /*condense the letter by skipping column 4 */
            continue;
        }
        if(p<DISPLAY_END) *p++ = inv ? ~b : b;
    }
    if(p<DISPLAY_END) *p++ = inv ? ~0 : 0;
  }
}

void lcd_putc(uint8_t x,uint8_t y,const char c)
{
  lcd_putcAtt(x,y,c,0);
}

void lcd_putsnAtt(uint8_t x,uint8_t y,const prog_char * s,uint8_t len,uint8_t mode)
{
  while(len!=0) {
    char c = (mode & BSS) ? *s++ : pgm_read_byte(s++);
    lcd_putcAtt(x,y,c,mode);
    x+=FW;
    len--;
  }
}
void lcd_putsn_P(uint8_t x,uint8_t y,const prog_char * s,uint8_t len)
{
  lcd_putsnAtt( x,y,s,len,0);
}
uint8_t lcd_putsAtt(uint8_t x,uint8_t y,const prog_char * s,uint8_t mode)
{
  //while(char c=pgm_read_byte(s++)) {
  while(1) {
    char c = (mode & BSS) ? *s++ : pgm_read_byte(s++);
    if(!c) break;
    lcd_putcAtt(x,y,c,mode);
    x+=FW;
    if(mode&DBLSIZE) x+=FW;
  }
  return x;
}
void lcd_puts_P(uint8_t x,uint8_t y,const prog_char * s)
{
  lcd_putsAtt( x, y, s, 0);
}
void lcd_outhex4(uint8_t x,uint8_t y,uint16_t val)
{
  x+=FWNUM*4;
  for(int i=0; i<4; i++)
  {
    x-=FWNUM;
    char c = val & 0xf;
    c = c>9 ? c+'A'-10 : c+'0';
    lcd_putcAtt(x,y,c,c>='A'?CONDENSED:0);
    val>>=4;
  }
}
void lcd_outdez(uint8_t x,uint8_t y,int16_t val)
{
  lcd_outdezAtt(x,y,val,0);
}

void lcd_outdezAtt(uint8_t x,uint8_t y,int16_t val,uint8_t mode)
{
  lcd_outdezNAtt( x,y,val,mode,5);
}

uint8_t lcd_lastPos;
#define PREC(n) ((n&0x20) ? ((n&0x10) ? 2 : 1) : 0)
void lcd_outdezNAtt(uint8_t x,uint8_t y,int16_t val,uint8_t mode,uint8_t len)
{
  uint8_t fw = FWNUM;
  uint8_t prec = PREC(mode);
  int16_t tmp = abs(val);
  uint8_t xn = 0;
  uint8_t ln = 2;
  char c;

  if (mode & DBLSIZE) {
    fw += FWNUM;
    if (mode & LEFT) {
      if (tmp >= 100 || prec==2)
        x += 2*FW;
      if (tmp >= 10 || prec>0)
        x += 2*FW;
    }
    else {
      x -= 2*FW;
    }
    lcd_lastPos = x + 2*FW;
  }
  else {
    if (mode & LEFT) {
      if (prec)
        x += 2;
      if (val < 0)
        x += FWNUM;
      if (tmp >= 100 || prec==2)
        x += FWNUM;
      if (tmp >= 10 || prec>0)
        x += FWNUM;
    }
    else {
      x -= FW;
    }
    lcd_lastPos = x + FW;
  }

  for (uint8_t i=1; i<=len; i++) {
    c = (tmp % 10) + '0';
    lcd_putcAtt(x, y, c, mode);
    if (prec==i) {
      mode &= ~PREC2;
      if (mode & DBLSIZE) {
        xn = x;
        if(c=='2' || c=='3' || c=='1') ln++;
        uint8_t tn = (tmp/10) % 10;
        if(tn==2 || tn==4) {
          if (c=='4') {
            xn++;
          }
          else {
            xn--; ln++;
          }
        }
      }
      else {
        x -= 2;
        if (mode & INVERS)
          lcd_vline(x+1, y, 7);
        else
          lcd_plot(x+1, y+6);
      }
      if (tmp >= 10)
        prec = 0;
    }
    tmp /= 10;
    if (!tmp) {
      if (prec) {
        if (i >= prec)
          prec = 0;
      }
      else if (mode & LEADING0)
        mode -= LEADING0;
      else
        break;
    }
    x-=fw;
  }
  if (xn) {
    lcd_hline(xn, y+2*FH-4, ln);
    lcd_hline(xn, y+2*FH-3, ln);
  }
  if(val<0) lcd_putcAtt(x-fw,y,'-',mode);
}

void lcd_plot(uint8_t x,uint8_t y)
{
  uint8_t *p   = &displayBuf[ y / 8 * DISPLAY_W + x ];
  if(p<DISPLAY_END) *p ^= BITMASK(y%8);
}
void lcd_hlineStip(unsigned char x,unsigned char y, signed char w,uint8_t pat)
{
  if(w<0) {x+=w; w=-w;}
  uint8_t *p  = &displayBuf[ y / 8 * DISPLAY_W + x ];
  uint8_t msk = BITMASK(y%8);
  while(w){
    if(pat&1) {
      //lcd_plot(x,y);
      *p ^= msk;
      pat = (pat >> 1) | 0x80;
    }else{
      pat = pat >> 1;
    }
    w--;
    p++;
  }
}

void lcd_hline(uint8_t x,uint8_t y, int8_t w)
{
  lcd_hlineStip(x,y,w,0xff);
}

void lcd_vlineStip(uint8_t x, uint8_t y, int8_t h, uint8_t pat)
{
  uint8_t *p  = &displayBuf[ y / 8 * DISPLAY_W + x ];
  y = y % 8;
  if (y) {
    assert(p < DISPLAY_END);
    *p ^= ~(BITMASK(y)-1) & pat;
    p += DISPLAY_W;
    h -= 8-y;
  }
  while (h>0) {
    assert(p < DISPLAY_END);
    *p ^= pat;
    p += DISPLAY_W;
    h -= 8;
  }
  h = (h+8) % 8;
  if (h) {
    p -= DISPLAY_W;
    assert(p < DISPLAY_END);
    *p ^= ~(BITMASK(h)-1) & pat;
  }
}

void lcd_vline(uint8_t x, uint8_t y, int8_t h)
{
  lcd_vlineStip(x, y, h, 0xff);
}

void lcd_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t pat)
{
  lcd_vlineStip(x, y, h, pat);
  lcd_hlineStip(x, y+h-1, w, pat);
  lcd_vlineStip(x+w-1, y, h, pat);
  lcd_hlineStip(x, y, w, pat);
}

void lcd_filled_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
  for (uint8_t i=y+h-1; i>=y; i--)
    lcd_hline(x, i, w);
}

void lcdSendCtl(uint8_t val)
{
  PORTC_LCD_CTRL &= ~(1<<OUT_C_LCD_CS1);
#ifdef LCD_MULTIPLEX
  DDRA = 0xFF; // set LCD_DAT pins to output
#endif
  PORTC_LCD_CTRL &= ~(1<<OUT_C_LCD_A0);
  PORTC_LCD_CTRL &= ~(1<<OUT_C_LCD_RnW);
  PORTA_LCD_DAT = val;
  PORTC_LCD_CTRL |=  (1<<OUT_C_LCD_E);
  PORTC_LCD_CTRL &= ~(1<<OUT_C_LCD_E);
  PORTC_LCD_CTRL |=  (1<<OUT_C_LCD_A0);
#ifdef LCD_MULTIPLEX
  DDRA = 0x00; // set LCD_DAT pins to input
#endif
  PORTC_LCD_CTRL |=  (1<<OUT_C_LCD_CS1);
}


#define delay_1us() _delay_us(1)
void delay_1_5us(int ms)
{
  for(int i=0; i<ms; i++) delay_1us();
}


void lcd_init()
{
  // /home/thus/txt/datasheets/lcd/KS0713.pdf
  // ~/txt/flieger/ST7565RV17.pdf  from http://www.glyn.de/content.asp?wdid=132&sid=

  PORTC_LCD_CTRL &= ~(1<<OUT_C_LCD_RES);  //LCD_RES
  delay_1us();
  delay_1us();//    f520  call  0xf4ce  delay_1us() ; 0x0xf4ce
  PORTC_LCD_CTRL |= (1<<OUT_C_LCD_RES); //  f524  sbi 0x15, 2 IOADR-PORTC_LCD_CTRL; 21           1
  delay_1_5us(1500);
  lcdSendCtl(0xe2); //Initialize the internal functions
  lcdSendCtl(0xae); //DON = 0: display OFF
  lcdSendCtl(0xa1); //ADC = 1: reverse direction(SEG132->SEG1)
  lcdSendCtl(0xA6); //REV = 0: non-reverse display
  lcdSendCtl(0xA4); //EON = 0: normal display. non-entire
  lcdSendCtl(0xA2); // Select LCD bias=0
  lcdSendCtl(0xC0); //SHL = 0: normal direction (COM1->COM64)
  lcdSendCtl(0x2F); //Control power circuit operation VC=VR=VF=1
  lcdSendCtl(0x25); //Select int resistance ratio R2 R1 R0 =5
  lcdSendCtl(0x81); //Set reference voltage Mode
  lcdSendCtl(0x22); // 24 SV5 SV4 SV3 SV2 SV1 SV0 = 0x18
  lcdSendCtl(0xAF); //DON = 1: display ON
  g_eeGeneral.contrast = 0x22;
}
void lcdSetRefVolt(uint8_t val)
{
  lcdSendCtl(0x81);
  lcdSendCtl(val);
}

void refreshDiplay()
{
  uint8_t *p=displayBuf;
  for(uint8_t y=0; y < 8; y++) {
    lcdSendCtl(0x04);
    lcdSendCtl(0x10); //column addr 0
    lcdSendCtl( y | 0xB0); //page addr y
    PORTC_LCD_CTRL &= ~(1<<OUT_C_LCD_CS1);
#ifdef LCD_MULTIPLEX
    DDRA = 0xFF; // set LCD_DAT pins to output
#endif
    PORTC_LCD_CTRL |=  (1<<OUT_C_LCD_A0);
    PORTC_LCD_CTRL &= ~(1<<OUT_C_LCD_RnW);
    for(uint8_t x=128; x>0; --x) {
      PORTA_LCD_DAT = *p++;
      PORTC_LCD_CTRL |= (1<<OUT_C_LCD_E);
      PORTC_LCD_CTRL &= ~(1<<OUT_C_LCD_E);
    }
    PORTC_LCD_CTRL |=  (1<<OUT_C_LCD_A0);
    PORTC_LCD_CTRL |=  (1<<OUT_C_LCD_CS1);
  }
}
