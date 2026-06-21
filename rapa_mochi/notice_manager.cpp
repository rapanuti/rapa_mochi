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
  const int MAXC = 20, MAXL = 5;        // ~20 chars/linea a 6x10 en 128px
  String lines[MAXL];
  int    nL = 0;

  // 1) Partir el texto en lineas por palabras.
  String t = text;
  t.replace('\n', ' ');
  String cur = "";
  int pos = 0;
  while (pos <= (int)t.length() && nL < MAXL) {
    int    sp   = t.indexOf(' ', pos);
    String word = (sp < 0) ? t.substring(pos) : t.substring(pos, sp);
    if (cur.length() == 0)                                    cur = word;
    else if ((int)(cur.length() + 1 + word.length()) <= MAXC) cur += " " + word;
    else { lines[nL++] = cur; cur = word; }
    if (sp < 0) break;
    pos = sp + 1;
  }
  if (nL < MAXL && cur.length() > 0) lines[nL++] = cur;

  // 2) Dibujar centrado vertical y horizontalmente.
  if (nL > 0) {
    int startY = (64 - nL * 12) / 2 + 9;          // baseline de la 1a linea
    for (int i = 0; i < nL; i++) {
      int x = (128 - 6 * (int)lines[i].length()) / 2;
      if (x < 0) x = 0;
      dispTextSmall(x, startY + i * 12, lines[i].c_str());
    }
  }
  dispFlush();
}
