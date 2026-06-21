// ===========================================================================
//  notice_manager.cpp  -  Texto temporal en la OLED con ajuste de linea simple.
// ===========================================================================
#include "notice_manager.h"
#include "display_manager.h"

static String   text;
static uint32_t expiry = 0;

void noticeShow(const String& t, uint16_t ms) {
  text   = t;
  expiry = millis() + ms;
}

void noticeClear() { expiry = 0; text = ""; }

bool noticeActive(uint32_t now) {
  return text.length() > 0 && (int32_t)(expiry - now) > 0;   // wrap-safe
}

void noticeRender() {
  dispClear();
  const int  MAXC   = 21;                       // ~21 chars a fuente 6x10 en 128px
  const int  YS[]   = { 10, 22, 34, 46, 58 };
  const int  MAXL   = 5;
  int        line   = 0;
  String     cur    = "";

  String t = text;
  t.replace('\n', ' ');
  int pos = 0;
  while (pos <= (int)t.length() && line < MAXL) {
    int    sp   = t.indexOf(' ', pos);
    String word = (sp < 0) ? t.substring(pos) : t.substring(pos, sp);
    if (cur.length() == 0)                           cur = word;
    else if ((int)(cur.length() + 1 + word.length()) <= MAXC) cur += " " + word;
    else { dispTextSmall(0, YS[line++], cur.c_str()); cur = word; }
    if (sp < 0) break;
    pos = sp + 1;
  }
  if (line < MAXL && cur.length() > 0) dispTextSmall(0, YS[line], cur.c_str());

  dispFlush();
}
