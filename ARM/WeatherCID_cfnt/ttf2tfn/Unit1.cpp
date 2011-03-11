//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
  int n = 0;
  for (int i = 0; i < Screen->Fonts->Count; i++)
  {
    ComboBox1->Items->Add(Screen->Fonts->Strings[i]);
    if(Screen->Fonts->Strings[i] == "MS Sans Serif") n = i;
 }
 ComboBox1->ItemIndex = n;
 ComboBox2->Items->Add("Normal");
 ComboBox2->Items->Add("Bold");
 ComboBox2->Items->Add("Italic");
 ComboBox2->Items->Add("Bold Italic");

}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button1Click(TObject *Sender)
{
  int height = Edit1->Text.ToInt();
  Image1->Canvas->Font->Name = ComboBox1->Text;
  Image1->Canvas->Font->Height = height;

  switch(ComboBox2->ItemIndex)
  {
    case 0:
      Image1->Canvas->Font->Style = TFontStyles();
      break;
    case 1:
      Image1->Canvas->Font->Style = TFontStyles()<<fsBold;
      break;
    case 2:
      Image1->Canvas->Font->Style = TFontStyles()<<fsItalic;
      break;
    case 3:
      Image1->Canvas->Font->Style = TFontStyles()<<fsBold<<fsItalic;
      break;
  }

  Image1->Canvas->Brush->Style = bsSolid;

  DeleteFile(ComboBox1->Text+".tfn");
  int f = FileCreate(ComboBox1->Text+".tfn");
  char font_name[256];
  strcpy(font_name, Edit2->Text.c_str());
  font_name[15] = 0;

  unsigned char buf[65536];
  unsigned char widths[256];

  for(int i = 0; i < 65536; buf[i++] = 0);

  int ww = 0, cw, w, h;
  char c[2];

  for(int i = 32; i < 128; i++)
  {
    c[0] = i;
    c[1] = 0;
    Image1->Canvas->Pen->Color = clWhite;
    Image1->Canvas->Rectangle(0,0,Image1->Width,Image1->Height);
    Image1->Canvas->Pen->Color = clBlack;

    h = height;
    cw = 0;
    w = Image1->Canvas->TextWidth(c);
    Image1->Canvas->TextOutA(0,0,c);
    for(int y = 0; y < h; y++)
      for(int x = 0; x < w; x++)
        if(Image1->Canvas->Pixels[x][y]<clGray)
          cw = x>cw?x:cw;
    if(i == 32)
      cw = w-1;
    widths[i] = cw+1;
    if(ww < cw) ww = cw;

  }
  for(int i = 0xC0; i < 0x100; i++)
  {
    c[0] = i;
    c[1] = 0;
    Image1->Canvas->Pen->Color = clWhite;
    Image1->Canvas->Rectangle(0,0,Image1->Width,Image1->Height);
    Image1->Canvas->Pen->Color = clBlack;

    h = height;
    cw = 0;
    w = Image1->Canvas->TextWidth(c);
    Image1->Canvas->TextOutA(0,0,c);
    for(int y = 0; y < h; y++)
      for(int x = 0; x < w; x++)
        if(Image1->Canvas->Pixels[x][y]<clGray)
          cw = x>cw?x:cw;
    widths[i] = cw+1;
    if(ww < cw) ww = cw;

  }
  Image1->Canvas->Pen->Color = clWhite;
  Image1->Canvas->Rectangle(0,0,Image1->Width,Image1->Height);
  Image1->Canvas->Pen->Color = clBlack;

  h = height;
  cw = 0;
  w = Image1->Canvas->TextWidth("¨");
  Image1->Canvas->TextOutA(0,0,"¨");
  for(int y = 0; y < h; y++)
    for(int x = 0; x < w; x++)
      if(Image1->Canvas->Pixels[x][y]<clGray)
        cw = x>cw?x:cw;
  widths[0xA8] = cw+1;
  if(ww < cw) ww = cw;

  Image1->Canvas->Pen->Color = clWhite;
  Image1->Canvas->Rectangle(0,0,Image1->Width,Image1->Height);
  Image1->Canvas->Pen->Color = clBlack;

  h = height;
  cw = 0;
  w = Image1->Canvas->TextWidth("¸");
  Image1->Canvas->TextOutA(0,0,"¸");
  for(int y = 0; y < h; y++)
    for(int x = 0; x < w; x++)
      if(Image1->Canvas->Pixels[x][y]<clGray)
        cw = x>cw?x:cw;
  widths[0xB8] = cw+1;
  if(ww < cw) ww = cw;

  int bw = (ww>>3)+((ww&7)?1:0);

  for(int i = 0; i < 0x100*(height*bw+1); buf[i] = widths[32], i += (height*bw+1));

  for(int i = 32; i < 128; i++)
  {
    c[0] = i;
    c[1] = 0;
    Image1->Canvas->Pen->Color = clWhite;
    Image1->Canvas->Rectangle(0,0,Image1->Width,Image1->Height);
    Image1->Canvas->Pen->Color = clBlack;

    h = height;
    w = Image1->Canvas->TextWidth(c);
    Image1->Canvas->TextOutA(0,0,c);
    buf[i*(height*bw+1)] = widths[i];
    for(int y = 0, dy = i*(height*bw+1) + 1; y < h; y++)
    {
      unsigned char l = 0;
      for(int x = 0, bx = 0; x < (bw<<3); x++, bx++)
      {
        l<<=1;
        int b = (Image1->Canvas->Pixels[x][y]>clGray)?0:1;
        l |= b;
        if(bx == 7)
        {
          bx = -1;
          buf[dy++] = l;
          l = 0;
       }
      }
    }
  }
  for(int i = 0xC0; i < 0xF0; i++)
  {
    c[0] = i;
    c[1] = 0;
    Image1->Canvas->Pen->Color = clWhite;
    Image1->Canvas->Rectangle(0,0,Image1->Width,Image1->Height);
    Image1->Canvas->Pen->Color = clBlack;

    h = height;
    w = Image1->Canvas->TextWidth(c);
    Image1->Canvas->TextOutA(0,0,c);
    buf[(i-0x40)*(height*bw+1)] = widths[i];
    for(int y = 0, dy = (i-0x40)*(height*bw+1)+1; y < h; y++)
    {
      unsigned char l = 0;
      for(int x = 0, bx = 0; x < (bw<<3); x++, bx++)
      {
        l<<=1;
        int b = (Image1->Canvas->Pixels[x][y]>clGray)?0:1;
        l |= b;
        if(bx == 7)
        {
          bx = -1;
          buf[dy++] = l;
          l = 0;
       }
      }
    }
  }
  for(int i = 0xF0; i < 0x100; i++)
  {
    c[0] = i;
    c[1] = 0;
    Image1->Canvas->Pen->Color = clWhite;
    Image1->Canvas->Rectangle(0,0,Image1->Width,Image1->Height);
    Image1->Canvas->Pen->Color = clBlack;

    h = height;
    w = Image1->Canvas->TextWidth(c);
    Image1->Canvas->TextOutA(0,0,c);
    buf[(i-0x10)*(height*bw+1)] = widths[i];
    for(int y = 0, dy = (i-0x10)*(height*bw+1)+1; y < h; y++)
    {
      int l = 0;
      for(int x = 0, bx = 0; x < (bw<<3); x++, bx++)
      {
        l<<=1;
        int b = (Image1->Canvas->Pixels[x][y]>clGray)?0:1;
        l |= b;
        if(bx == 7)
        {
          bx = -1;
          buf[dy++] = l;
          l = 0;
       }
      }
    }
  }

  Image1->Canvas->Pen->Color = clWhite;
  Image1->Canvas->Rectangle(0,0,Image1->Width,Image1->Height);
  Image1->Canvas->Pen->Color = clBlack;

  h = height;
  w = Image1->Canvas->TextWidth("¨");
  Image1->Canvas->TextOutA(0,0,"¨");
  buf[0xF0*(height*bw+1)] = widths[0xA8];
  for(int y = 0, dy = 0xF0*(height*bw+1)+1; y < h; y++)
  {
    int l = 0;
    for(int x = 0, bx = 0; x < (bw<<3); x++, bx++)
    {
      l<<=1;
      int b = (Image1->Canvas->Pixels[x][y]>clGray)?0:1;
      l |= b;
      if(bx == 7)
      {
        bx = -1;
        buf[dy++] = l;
        l = 0;
     }
    }
  }

  Image1->Canvas->Pen->Color = clWhite;
  Image1->Canvas->Rectangle(0,0,Image1->Width,Image1->Height);
  Image1->Canvas->Pen->Color = clBlack;

  h = height;
  w = Image1->Canvas->TextWidth("¸");
  Image1->Canvas->TextOutA(0,0,"¸");
  buf[0xF1*(height*bw+1)] = widths[0xB8];
  for(int y = 0, dy = 0xF1*(height*bw+1)+1; y < h; y++)
  {
    int l = 0;
    for(int x = 0, bx = 0; x < (bw<<3); x++, bx++)
    {
      l<<=1;
      int b = (Image1->Canvas->Pixels[x][y]>clGray)?0:1;
      l |= b;
      if(bx == 7)
      {
        bx = -1;
        buf[dy++] = l;
        l = 0;
     }
    }
  }

  FileWrite(f, &bw, 1);
  FileWrite(f, &height, 1);
  FileWrite(f, &font_name, 16);

  FileWrite(f, buf, 0x100*(height*bw+1));
  FileClose(f);

  char FH, FW, fname[16];
  int fh, fw;
  unsigned char l, cc;
  int xp = 0, yp = 0;
  f = FileOpen(ComboBox1->Items->Strings[ComboBox1->ItemIndex]+".tfn", fmOpenRead);

  FileRead(f, &FW, 1);
  FileRead(f, &FH, 1);
  fh = FH; fw = FW;
  FileRead(f, &fname, 16);

  unsigned char *font = new unsigned char[(fw*fh+1) * 256];
  FileRead(f, font, (fw*fh+1) * 256);
  FileClose(f);

  Form1->Image1->Canvas->Brush->Color = clWhite;
  Form1->Image1->Canvas->FillRect(Rect(0,0,Form1->Image1->Width,Form1->Image1->Height));

  for(int i = 0, row = 0; i < 256; i++, row++)
  {
    cc = i;
    w = font[(fw*fh+1) * cc];

    xp += (w + 1);
    if(row == 16) {xp = w+1; yp += fh + 1; row = 0;}
    for(int y = 0; y < fh; y++)
    {
      for(int x = 0, dx = 0; x < fw; x++)
      {
        l = font[(fw*fh+1) * cc + y*fw + x + 1];
        for(int bx = 0; dx < w && bx < 8 ; dx++, bx++)
        {
          Image1->Canvas->Pixels[xp - w - 1 + dx][y + yp] = (l&0x80)?clBlack:clWhite;
          l <<= 1;
        }
      }
    }

  }

  delete []font;
  Image1->Canvas->Pen->Color = clLtGray;
  Image1->Canvas->Brush->Style = bsClear;
  Image1->Canvas->Rectangle(0,0,240,320);
  Image1->Canvas->Rectangle(0,0,132,176);
  Image1->Canvas->Rectangle(0,0,132,132);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ComboBox1Change(TObject *Sender)
{
  char font_name[512];
  strcpy(font_name, ComboBox1->Text.c_str());
  font_name[15] = 0;
  Edit2->Text = font_name;
}
//---------------------------------------------------------------------------


void __fastcall TForm1::Button2Click(TObject *Sender)
{
  RECT r, d = {0,0,Image1->Width,Image1->Height};
  HDC hdc;
  HWND hwnd;
  int weight = FW_NORMAL, italic = 0;

  hwnd = Form1->Handle;
  switch(ComboBox2->ItemIndex)
  {
    case 1:
      weight = FW_BOLD;
      break;
    case 2:
      italic = 1;
      break;
    case 3:
      weight = FW_BOLD;
      italic = 1;
      break;
    default:
      break;
  }

  ::GetClientRect(hwnd, &r);

  hdc = GetDC(hwnd);

//  d = RECT(0,0,Image1->Width,Image1->Height);
  FillRect(hdc, &d, GetStockObject(LTGRAY_BRUSH));

HFONT hfnt, hOldFont;

PGLYPHSET glset;

    d.left = 10; d.top = 10; d.right = 11; d.bottom = 101;
    int height = Edit1->Text.ToInt();

    wchar_t s;
//    s[0] = 0x480;
//    for(int i = 0; i < 0x1000; i++)
//      s[i] = i;
    hfnt=CreateFont(height,0,0,0,weight,italic,0,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,ComboBox1->Text.c_str());// גמע עאך סמחהאורü סגמי רנטפע
    //hfnt = GetStockObject(ANSI_VAR_FONT);
    if (hOldFont = SelectObject(hdc, hfnt))
    {
        int n;
        n = GetFontUnicodeRanges(hdc, 0);
        glset = new GLYPHSET[n];
        n = GetFontUnicodeRanges(hdc, glset);
        SetBkMode(hdc, TRANSPARENT);
        //TextOutW(hdc, 10, 50, s, 100);
        for(int i = 0; i < glset->cRanges; i++)
        {
          for(int j = glset->ranges[i].wcLow; j < glset->ranges[i].wcLow+glset->ranges[i].cGlyphs; j++)
          {
            s = j;
            DrawTextW(hdc, &s, 1, &d, DT_CALCRECT);
            FillRect(hdc, &d, GetStockObject(LTGRAY_BRUSH));
            DrawTextW(hdc, &s, 1, &d, DT_NOCLIP);
            d.left = d.right + 1;
            if(d.left > Form1->Width - 20) {d.left = 10; d.top = d.bottom + 5;}
            if(d.bottom > Form1->Height) break;
          }
          d.left = d.right + 15;
          if(d.left > Form1->Width - 20) {d.left = 10; d.top = d.bottom + 5;}
          if(d.bottom > Form1->Height) break;
        }
        SelectObject(hdc, hOldFont);
        delete []glset;
    }

  /*

  BeginPath(hdc);

  TCHAR text[ ] = "Defenestration can be hazardous";
  TextOut(hdc,r.left,r.top,text, ARRAYSIZE(text));

  EndPath(hdc);

  SelectClipPath(hdc, RGN_AND);
    */
  /*
  for(int y = 0; y < Form1->Height; y++)
    for(int x = 0; x < Form1->Width/2; x++)
    {
      COLORREF c = GetPixel(hdc, x, y);
      COLORREF c1 = GetPixel(hdc, Form1->Width-x, y);
      SetPixel(hdc, x, y, c1);
      SetPixel(hdc, Form1->Width-x, y, c);
    }
   */

  //FillRect(hdc, &r, GetStockObject(GRAY_BRUSH));


}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button3Click(TObject *Sender)
{
  struct _region
  {
    unsigned reg_start;
    unsigned reg_end;
  } region, *regions;
  if(!OpenDialog1->Execute()) return;
  int f = FileOpen(OpenDialog1->FileName, fmOpenRead);
  FileSeek(f, 8, 0);
  __int8 fw, fh;
  __int16 regionscount;
  FileRead(f, &fh, 1);
  FileRead(f, &fw, 1);

  FileSeek(f, 13, 0);
  FileRead(f, &regionscount, 2);

  regions = new _region[regionscount];

  int pos = FileSeek(f, 32, 0);

  int charscount = 0;
  for(int i = 0; i < regionscount; i++)
  {
    FileRead(f, &regions[i].reg_start, 4);
    FileRead(f, &regions[i].reg_end, 4);
    charscount += (regions[i].reg_end - regions[i].reg_start)+1;
  }

  __int8 *charswidths = new __int8[charscount];
  pos = FileSeek(f, 4, 1);

  for(int i = 0; i < charscount; i++)
    FileRead(f, &charswidths[i], 1);

  pos = FileSeek(f, 4, 1);

  RECT r, d = {0,0,Image1->Width,Image1->Height};
  HDC hdc;
  HWND hwnd;
  hwnd = Form1->Handle;
  ::GetClientRect(hwnd, &r);
  hdc = GetDC(hwnd);
  FillRect(hdc, &d, GetStockObject(LTGRAY_BRUSH));

/*      COLORREF c = GetPixel(hdc, x, y);
      COLORREF c1 = GetPixel(hdc, Form1->Width-x, y);
      SetPixel(hdc, x, y, c1);
      SetPixel(hdc, Form1->Width-x, y, c);
*/
  int bits = 0;
  unsigned char byte = 0;
  long black = 0x00;
  int xx = 0, yy = 0;

  for(int i = 0, chr = 0; i < regionscount; i++)
  {
    for(int j = regions[i].reg_start; j <= regions[i].reg_end; j++, chr++)
    {
      bits = 0;
      for(int y = 0; y < fh; y++)
        for(int x = 0; x < charswidths[chr]; x++)
        {
          if(!bits)
          {
            FileRead(f, &byte, 1);
            bits = 8;
          }
          if(byte&1)
            SetPixel(hdc, xx+x, yy+y, black);
          byte >>= 1;
          bits--;
        }
      xx += charswidths[chr] + 1;
      if(xx > Form1->Width - 20)
      {
        xx = 0; yy += fh+3;
      }
    }
      xx = 0; yy += fh+3;
  }

  FileClose(f);
  delete []regions;
  delete []charswidths;

}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button4Click(TObject *Sender)
{
  RECT r, d = {0,0,Image1->Width,Image1->Height};
  HDC hdc;
  HWND hwnd;
  int weight = FW_NORMAL, italic = 0;

  hwnd = Form1->Handle;
  switch(ComboBox2->ItemIndex)
  {
    case 1:
      weight = FW_BOLD;
      break;
    case 2:
      italic = 1;
      break;
    case 3:
      weight = FW_BOLD;
      italic = 1;
      break;
    default:
      break;
  }

  ::GetClientRect(hwnd, &r);

  hdc = GetDC(hwnd);

  FillRect(hdc, &d, GetStockObject(LTGRAY_BRUSH));

  HFONT hfnt, hOldFont;
  PGLYPHSET glset;

  __int8 height = Edit1->Text.ToInt();
  wchar_t s;

  hfnt=CreateFont(height,0,0,0,weight,italic,0,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,ComboBox1->Text.c_str());// גמע עאך סמחהאורü סגמי רנטפע
  if (hOldFont = SelectObject(hdc, hfnt))
  {
    __int16 n, charscount = 0;
    n = GetFontUnicodeRanges(hdc, 0);
    glset = new GLYPHSET[n];
    n = GetFontUnicodeRanges(hdc, glset);
    SetBkMode(hdc, TRANSPARENT);

    for(int i = 0; i < glset->cRanges; i++)
      charscount += glset->ranges[i].cGlyphs;

    __int8 *charwidths = new __int8[charscount];
    __int8 *charsize = new __int8[charscount];
    __int8 **symbols = new __int8*[charscount];
    for(int i = 0; i < charscount; i++)
      symbols[i] = 0;

    for(int i = 0, k = 0; i < glset->cRanges; i++)
    {
      for(int j = glset->ranges[i].wcLow; j < glset->ranges[i].wcLow+glset->ranges[i].cGlyphs; j++, k++)
      {
        s = j;
        d.left = d.top = d.right = d.bottom = 0;
        DrawTextW(hdc, &s, 1, &d, DT_CALCRECT);
        FillRect(hdc, &d, GetStockObject(LTGRAY_BRUSH));
        charwidths[k] = d.right - d.left;
        charsize[k] = height * ((charwidths[k]>>3) + ((charwidths[k]&7)?1:0));
        symbols[k] = new __int8[charsize[k]];
        DrawTextW(hdc, &s, 1, &d, DT_NOCLIP);

        unsigned char byte = 0;
        int bits = 0;
        int bitsize = height*charwidths[k];
        int byteidx = 0;
        for(int x = 0, y = 0; bitsize; bitsize--, x++)
        {
          if(x>charwidths[k]) {x=0; y++;}
          COLORREF c = GetPixel(hdc, x, y);
          if(c == 0)
            byte |= (1<<bits);
          bits++;
          if(bits == 8)
          {
            symbols[k][byteidx++] = byte;
            byte = 0;
            bits = 0;
            charsize[k] = byteidx-1;
          }
        }
        if(bits)
        {
          byte <<= (8-bits);
          symbols[k][byteidx++] = byte;
          byte = 0;
          bits = 0;
          charsize[k] = byteidx-1;
        }
/*        for(int y = 0, byteidx = 0; y < height; y++)
        {
          for(int x = 0; x < charwidths[k]; x++)
          {
            COLORREF c = GetPixel(hdc, x, y);
            if(c == 0)
              byte |= (1<<bits);
            bits++;
            if(bits == 8)
            {
              symbols[k][byteidx++] = byte;
              byte = 0;
              bits = 0;
              charsize[k] = byteidx;
            }
            //byte <<= 1;
          }
//          if(bits)
//            symbols[k][byteidx++] = byte;
        }  */
      }
    }
    SelectObject(hdc, hOldFont);

    __int8 width = 0;
    for(int i = 0; i < charscount; i++)
      if(charwidths[i] > width) width = charwidths[i];

    DeleteFile(ComboBox1->Text+".cfnt");
    int f = FileCreate(ComboBox1->Text+".cfnt");
    char *sign = "CFNTv1#";
    __int8 zero = 0;
    __int8 dash = 0x23;
    __int8 dash4[] = {0x23, 0x23, 0x23, 0x23};
    __int8 caps[] = {0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0, 0, 0, 0, 0, 0};
    __int16 ranges = glset->cRanges;
    FileWrite(f, sign, 8);
    FileWrite(f, &height, 1);
    FileWrite(f, &width, 1);
    FileWrite(f, &height, 1);
    FileWrite(f, &zero, 1);
    FileWrite(f, &dash, 1);

    FileWrite(f, &ranges, 2);
    FileWrite(f, &dash, 1);
    FileWrite(f, &dash4, 4);

    FileWrite(f, &caps, 12);

    for(int i = 0; i < glset->cRanges; i++)
    {
      unsigned min = glset->ranges[i].wcLow,
               max = glset->ranges[i].wcLow+glset->ranges[i].cGlyphs-1;
      FileWrite(f, &min, 4);
      FileWrite(f, &max, 4);
    }
    FileWrite(f, &dash4, 4);

    for(int i = 0; i < charscount; i++)
      FileWrite(f, &charwidths[i], 1);

    FileWrite(f, &dash4, 4);

    for(int i = 0; i < charscount; i++)
      FileWrite(f, symbols[i], charsize[i]);


    FileClose(f);

    for(int i = 0; i < charscount; i++)
      delete []symbols[i];
    delete []charwidths;
    delete []charsize;
    delete []symbols;
    delete []glset;
  }


}
//---------------------------------------------------------------------------

