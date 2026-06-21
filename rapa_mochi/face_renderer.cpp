// ===========================================================================
//  face_renderer.cpp  -  Caras procedurales estilo Dasai Mochi (Gen 3), 128x64.
//  NO hace flush: emotion_manager dibuja la transicion (parpados) y luego flushea.
//  Varias caras son animadas (usan 'now'): sakura, bee, gtr_rain, pong, rotation...
// ===========================================================================
#include "face_renderer.h"
#include "display_manager.h"
#include <math.h>

// Geometria base de la cara.
static const int ELX = 44, ERX = 84, EY = 27;
static const int EW  = 22, EH  = 26, ER = 8;
static const int MX  = 64, MY  = 48;

// Tabla de desplazamientos para pupilas girando (rotation), 8 direcciones.
static const int8_t OX8[8] = { 0, 4, 6, 4, 0, -4, -6, -4 };
static const int8_t OY8[8] = { -6, -4, 0, 4, 6, 4, 0, -4 };

// --- Helpers ---------------------------------------------------------------
static void eyeR(int cx, int h, int dy) {        // ojo redondeado (radio acotado)
  int r = ER;
  if (r > h / 2)  r = h / 2;
  if (r > EW / 2) r = EW / 2;
  if (r < 0)      r = 0;
  dispRBox(cx - EW / 2, EY - h / 2 + dy, EW, h, r);
}

static void caretEye(int cx) {                   // ojo "^"
  for (int o = 0; o < 2; o++) {
    dispLine(cx - 10, EY + 5 + o, cx,      EY - 5 + o);
    dispLine(cx,      EY - 5 + o, cx + 10, EY + 5 + o);
  }
}

static void halfEye(int cx, int coverPct) {      // ojo medio cerrado
  eyeR(cx, EH, 0);
  dispSetColor(0);
  dispBox(cx - EW / 2 - 1, EY - EH / 2 - 1, EW + 2, (EH * coverPct) / 100 + 1);
  dispSetColor(1);
}

static void xEye(int cx) {                        // ojo en "X"
  dispLine(cx - 10, EY - 10, cx + 10, EY + 10);
  dispLine(cx - 10, EY - 9,  cx + 10, EY + 11);
  dispLine(cx + 10, EY - 10, cx - 10, EY + 10);
  dispLine(cx + 10, EY - 9,  cx - 10, EY + 11);
}

static void mouthArc(int cx, int cy, int halfW, int amp, bool up) {
  for (int x = -halfW; x <= halfW; x++) {
    float t = (float)x / (float)halfW;
    int   d = (int)(amp * (1.0f - t * t));
    int   y = up ? (cy + d) : (cy - d);
    dispPixel(cx + x, y);
    dispPixel(cx + x, y + 1);
  }
}

static void brow(int xi, int yi, int xo, int yo) {
  dispLine(xi, yi, xo, yo);
  dispLine(xi, yi + 1, xo, yo + 1);
}

static void tear(int cx, int topY) {
  dispTriangle(cx, topY, cx - 3, topY + 6, cx + 3, topY + 6);
  dispDisc(cx, topY + 7, 3);
}

static void heartEye(int cx, int cy, int s) {     // corazon (ojos de amor)
  dispDisc(cx - s / 2, cy, (s + 1) / 2);
  dispDisc(cx + s / 2, cy, (s + 1) / 2);
  dispTriangle(cx - s, cy + 1, cx + s, cy + 1, cx, cy + s + 2);
}

static void tongue(int cx, int topY, int w, int h) {
  dispRBox(cx - w / 2, topY, w, h, 3);
}

static void openSmile(int cx, int cy, int rx, int ry) {   // boca abierta (media elipse)
  dispEllipseFilled(cx, cy, rx, ry);
  dispSetColor(0);
  dispBox(cx - rx, cy - ry - 1, 2 * rx + 1, ry + 1);
  dispSetColor(1);
}

static void topArc(int cx, int cy, int r) {       // solo el arco superior (rainbow)
  for (int x = -r; x <= r; x++) {
    int xx = cx + x;
    int yy = cy - (int)sqrtf((float)(r * r - x * x));
    if (xx >= 0 && xx < 128 && yy >= 0 && yy < 64) dispPixel(xx, yy);
  }
}

static void eyesOpen(int oh) { eyeR(ELX, oh, 0); eyeR(ERX, oh, 0); }

// ===========================================================================
void faceRender(Emotion e, uint32_t now) {
  dispClear();
  dispSetColor(1);

  bool blink = (now % 3500UL) < 140UL;
  int  oh    = blink ? 4 : EH;

  switch (e) {

    // ---- Set base (13) ----
    case Emotion::HAPPY:
      caretEye(ELX); caretEye(ERX); mouthArc(MX, 44, 22, 9, true); break;

    case Emotion::SAD:
      eyeR(ELX, blink ? 4 : 16, 4); eyeR(ERX, blink ? 4 : 16, 4);
      tear(ERX, EY + 13); mouthArc(MX, 51, 16, 6, false); break;

    case Emotion::SURPRISED:
      dispRBox(ELX - 13, EY - 15, 26, 30, 11); dispRBox(ERX - 13, EY - 15, 26, 30, 11);
      dispSetColor(0); dispDisc(ELX, EY, 4); dispDisc(ERX, EY, 4); dispSetColor(1);
      dispCircle(MX, 50, 5); break;

    case Emotion::ANGRY:
      eyeR(ELX, blink ? 4 : 14, 5); eyeR(ERX, blink ? 4 : 14, 5);
      brow(ELX - 12, EY - 13, ELX + 11, EY - 4); brow(ERX + 12, EY - 13, ERX - 11, EY - 4);
      mouthArc(MX, 50, 16, 5, false); break;

    case Emotion::SLEEPY:
      halfEye(ELX, 62); halfEye(ERX, 62); dispLine(MX - 6, MY, MX + 6, MY);
      dispTextSmall(98, 16, "z"); dispTextSmall(106, 9, "z"); break;

    case Emotion::HOT:
      eyesOpen(oh); dispEllipseFilled(MX, MY + 1, 11, 7);
      dispSetColor(0); dispBox(MX - 11, MY - 6, 22, 5); dispSetColor(1);
      dispDisc(ERX + 15, EY - 8, 3); break;

    case Emotion::COLD:
      eyeR(ELX, blink ? 4 : 15, 0); eyeR(ERX, blink ? 4 : 15, 0);
      for (int i = -3; i <= 2; i++)
        dispLine(MX + i * 6, MY + (i % 2 ? 3 : -3), MX + (i + 1) * 6, MY + ((i + 1) % 2 ? 3 : -3));
      dispTextSmall(2, 14, "*"); dispTextSmall(118, 14, "*"); break;

    case Emotion::THINKING:
      eyeR(ELX, oh, -4); eyeR(ERX, oh, -4); dispLine(MX - 11, MY, MX + 11, MY);
      dispTextSmall(100, 18, "?"); break;

    case Emotion::NOTIFICATION:
      eyesOpen(oh); dispBox(MX - 2, MY - 7, 4, 9); dispDisc(MX, MY + 7, 2); break;

    case Emotion::ERROR:
      xEye(ELX); xEye(ERX); dispLine(MX - 12, MY, MX + 12, MY); break;

    case Emotion::WIFI_OK:
      eyesOpen(oh); mouthArc(MX, 45, 14, 6, true);
      dispLine(MX - 22, MY + 6, MX - 16, MY + 11); dispLine(MX - 16, MY + 11, MX - 6, MY + 1);
      dispTextSmall(82, MY + 10, "WiFi"); break;

    case Emotion::WIFI_FAIL:
      eyeR(ELX, blink ? 4 : 16, 4); eyeR(ERX, blink ? 4 : 16, 4); mouthArc(MX, 51, 14, 6, false);
      dispLine(MX - 24, MY + 2, MX - 14, MY + 12); dispLine(MX - 14, MY + 2, MX - 24, MY + 12);
      dispTextSmall(82, MY + 10, "WiFi"); break;

    // ---- Catalogo Dasai (20) ----
    case Emotion::SMILE:
      eyesOpen(oh); openSmile(MX, MY - 1, 14, 8); break;

    case Emotion::LAUGH:
      caretEye(ELX); caretEye(ERX); openSmile(MX, MY, 13, 9); break;

    case Emotion::UWU:
      caretEye(ELX); caretEye(ERX);
      for (int o = 0; o < 2; o++) {
        dispLine(MX - 12, MY - 2 + o, MX - 6, MY + 4 + o); dispLine(MX - 6, MY + 4 + o, MX, MY - 2 + o);
        dispLine(MX, MY - 2 + o, MX + 6, MY + 4 + o);      dispLine(MX + 6, MY + 4 + o, MX + 12, MY - 2 + o);
      }
      break;

    case Emotion::RASPBERRY:
      caretEye(ELX); caretEye(ERX); dispLine(MX - 8, MY - 3, MX + 8, MY - 3);
      tongue(MX, MY - 2, 12, 9); break;

    case Emotion::SAKURA: {
      for (int k = 0; k < 6; k++) {
        int x = 8 + k * 21;
        int y = (int)((now / 22 + (uint32_t)k * 23) % 72);
        if (y < 63) { dispDisc(x, y, 2); dispPixel(x, y - 2); }
      }
      eyesOpen(oh); mouthArc(MX, 46, 9, 4, true);
      break;
    }

    case Emotion::RAINBOW:
      topArc(64, 20, 12); topArc(64, 20, 16); topArc(64, 20, 20);
      dispDisc(52, 42, 4); dispDisc(76, 42, 4); mouthArc(64, 52, 7, 3, true); break;

    case Emotion::DUMB_LOVE:
      heartEye(ELX, EY, 7); heartEye(ERX, EY, 7); tongue(MX, MY - 2, 10, 8); break;

    case Emotion::LOVE:
      heartEye(ELX, EY, 8); heartEye(ERX, EY, 8); mouthArc(MX, 47, 8, 4, true); break;

    case Emotion::DISTRACTED: {
      int dx = (int)((now / 350) % 3) - 1;
      eyesOpen(EH);
      dispSetColor(0); dispDisc(ELX + dx * 4, EY, 4); dispDisc(ERX + dx * 4, EY, 4); dispSetColor(1);
      dispLine(MX - 7, MY, MX + 7, MY);
      break;
    }

    case Emotion::SNEEZE:
      for (int o = 0; o < 2; o++) {
        dispLine(ELX - 8, EY - 6 + o, ELX + 3, EY + o); dispLine(ELX + 3, EY + o, ELX - 8, EY + 6 + o);
        dispLine(ERX + 8, EY - 6 + o, ERX - 3, EY + o); dispLine(ERX - 3, EY + o, ERX + 8, EY + 6 + o);
      }
      dispEllipseFilled(MX, MY, 7, 6); break;

    case Emotion::SQUINT:
      dispBox(ELX - 10, EY - 1, 20, 3); dispBox(ERX - 10, EY - 1, 20, 3);
      dispLine(MX - 8, MY, MX + 8, MY); break;

    case Emotion::LOOK_DOWN:
      eyesOpen(EH);
      dispSetColor(0); dispDisc(ELX, EY + 5, 4); dispDisc(ERX, EY + 5, 4); dispSetColor(1);
      dispLine(MX - 7, MY, MX + 7, MY); break;

    case Emotion::SHIFT: {
      int s = (int)((now / 60) % 12);
      for (int j = 0; j < 4; j++) dispLine(2 + s, 12 + j * 12, 20 + s, 12 + j * 12);
      eyesOpen(oh); mouthArc(MX, 46, 8, 3, true);
      break;
    }

    case Emotion::BEE: {
      eyesOpen(oh); mouthArc(MX, 46, 8, 4, true);
      int bx = 14 + (int)((now / 9) % 100);
      int by = 8 + (int)(((now / 250) % 2) * 4);
      dispDisc(bx, by, 3);                          // cuerpo
      dispLine(bx - 4, by - 4, bx - 1, by - 1);     // ala
      dispLine(bx + 4, by - 4, bx + 1, by - 1);
      break;
    }

    case Emotion::GTR_RAIN: {
      for (int k = 0; k < 9; k++) {
        int x = 6 + k * 14;
        int y = (int)((now / 14 + (uint32_t)k * 11) % 72);
        if (y < 60) dispLine(x, y, x, y + 5);
      }
      eyesOpen(oh); mouthArc(MX, 47, 7, 3, false);
      break;
    }

    case Emotion::NEON_TUBE:
      dispCircle(ELX, EY, 11); dispCircle(ERX, EY, 11);
      dispCircle(ELX, EY, 4);  dispCircle(ERX, EY, 4);
      mouthArc(MX, 46, 12, 5, true); break;

    case Emotion::PONG: {
      for (int y = 2; y < 62; y += 7) dispBox(63, y, 2, 4);
      long ph = (long)(now % 1800UL);
      int  bx = (ph < 900) ? (int)(10 + ph * 104 / 900) : (int)(114 - (ph - 900) * 104 / 900);
      long qh = (long)(now % 1300UL);
      int  by = (qh < 650) ? (int)(6 + qh * 50 / 650) : (int)(56 - (qh - 650) * 50 / 650);
      dispBox(bx, by, 3, 3);
      int p = by - 7; if (p < 2) p = 2; if (p > 48) p = 48;
      dispBox(5, p, 3, 14); dispBox(120, p, 3, 14);
      break;
    }

    case Emotion::ROTATION: {
      int a = (int)((now / 90) % 8);
      dispCircle(ELX, EY, 11); dispCircle(ERX, EY, 11);
      dispDisc(ELX + OX8[a], EY + OY8[a], 3); dispDisc(ERX + OX8[a], EY + OY8[a], 3);
      dispLine(MX - 6, MY, MX + 6, MY);
      break;
    }

    case Emotion::GFORCE: {
      int w = (int)((now / 120) % 2);
      dispEllipseFilled(ELX + 7 + w, EY, 12, 7); dispEllipseFilled(ERX + 7 + w, EY, 12, 7);
      dispSetColor(0); dispDisc(ELX + 11 + w, EY, 3); dispDisc(ERX + 11 + w, EY, 3); dispSetColor(1);
      for (int j = 0; j < 3; j++) dispLine(2, EY - 8 + j * 8, 16, EY - 8 + j * 8);
      dispLine(MX - 4, MY - 2, MX + 12, MY + 2);
      break;
    }

    case Emotion::SPLAT:
      dispDisc(64, 32, 17); dispDisc(48, 26, 10); dispDisc(82, 30, 12);
      dispDisc(60, 46, 9);  dispDisc(74, 18, 7);
      dispDisc(26, 16, 3);  dispDisc(104, 46, 3); dispDisc(30, 50, 2); dispDisc(100, 12, 2);
      break;

    case Emotion::IDLE:
    default:
      eyesOpen(oh); mouthArc(MX, 46, 9, 4, true); break;
  }
  // NO dispFlush(): lo hace emotion_manager tras dibujar la transicion.
}
