// ===========================================================================
//  face_renderer.cpp  -  Caras procedurales del Mochi (128x64).
// ===========================================================================
#include "face_renderer.h"
#include "display_manager.h"

// Geometria base de la cara
static const int ELX = 42;   // centro ojo izquierdo X
static const int ERX = 86;   // centro ojo derecho X
static const int EY  = 26;   // centro ojos Y
static const int EW  = 22;   // ancho ojo
static const int EH  = 24;   // alto ojo
static const int ER  = 6;    // radio esquinas ojo
static const int MX  = 64;   // centro boca X
static const int MY  = 50;   // centro boca Y

// Ojo redondeado relleno (con parpadeo opcional y desplazamiento vertical).
static void eyeBox(int cx, int h, int dy) {
  dispRBox(cx - EW / 2, EY - h / 2 + dy, EW, h, ER);
}

// Boca en arco parabolico. up=true -> sonrisa (U), up=false -> ceno (n).
static void mouthArc(int cx, int cy, int halfW, int amp, bool up) {
  for (int x = -halfW; x <= halfW; x++) {
    float t  = (float)x / (float)halfW;
    int   d  = (int)(amp * (1.0f - t * t));
    int   y  = up ? (cy + d) : (cy - d);
    dispPixel(cx + x, y);
    dispPixel(cx + x, y + 1);   // un poco mas gruesa
  }
}

// Ceja inclinada (para enojo): de (xi,yi) a (xo,yo), gruesa.
static void brow(int xi, int yi, int xo, int yo) {
  dispLine(xi, yi, xo, yo);
  dispLine(xi, yi + 1, xo, yo + 1);
}

void faceRender(Emotion e, uint32_t now) {
  dispClear();
  dispSetColor(1);

  // Parpadeo comun: ~140 ms cada 3.5 s (no aplica a ojos ya "cerrados").
  bool blink = (now % 3500UL) < 140UL;
  int  oh    = blink ? 4 : EH;     // alto de ojo efectivo

  switch (e) {

    case Emotion::HAPPY:
      eyeBox(ELX, oh, 0);
      eyeBox(ERX, oh, 0);
      mouthArc(MX, MY - 4, 22, 9, true);     // gran sonrisa
      break;

    case Emotion::SAD:
      eyeBox(ELX, oh, 3);
      eyeBox(ERX, oh, 3);
      mouthArc(MX, MY + 4, 18, 7, false);    // ceno (boca triste)
      dispDisc(ERX + 8, EY + 14, 2);         // lagrima
      dispLine(ERX + 8, EY + 14, ERX + 8, EY + 22);
      break;

    case Emotion::SURPRISED:
      dispCircle(ELX, EY, 12); dispDisc(ELX, EY, 4);
      dispCircle(ERX, EY, 12); dispDisc(ERX, EY, 4);
      dispCircle(MX, MY, 7);                  // boca "O"
      break;

    case Emotion::ANGRY:
      eyeBox(ELX, blink ? 4 : 14, 4);
      eyeBox(ERX, blink ? 4 : 14, 4);
      brow(ELX - 11, EY - 12, ELX + 11, EY - 4);   // cejas hacia adentro
      brow(ERX + 11, EY - 12, ERX - 11, EY - 4);
      mouthArc(MX, MY + 3, 16, 5, false);
      break;

    case Emotion::SLEEPY:
      dispLine(ELX - 11, EY, ELX + 11, EY);  // ojos cerrados (lineas)
      dispLine(ERX - 11, EY, ERX + 11, EY);
      mouthArc(MX, MY, 8, 3, true);
      dispTextSmall(96, 16, "z");
      dispTextSmall(104, 10, "z");
      break;

    case Emotion::HOT:
      eyeBox(ELX, oh, 0);
      eyeBox(ERX, oh, 0);
      dispEllipseFilled(MX, MY, 10, 6);      // boca abierta jadeando
      dispSetColor(0); dispEllipseFilled(MX, MY - 2, 8, 3); dispSetColor(1);
      dispDisc(ERX + 12, EY - 6, 3);         // gota de sudor
      break;

    case Emotion::COLD:
      eyeBox(ELX, blink ? 4 : 16, 0);
      eyeBox(ERX, blink ? 4 : 16, 0);
      for (int i = -3; i <= 3; i++)          // boca temblorosa (zigzag)
        dispLine(MX + i * 6, MY + (i % 2 ? 3 : -3), MX + (i + 1) * 6, MY + ((i + 1) % 2 ? 3 : -3));
      dispTextSmall(2, 14, "*"); dispTextSmall(118, 14, "*");
      break;

    case Emotion::THINKING:
      eyeBox(ELX, oh, -4);                    // mirando arriba
      eyeBox(ERX, oh, -4);
      dispLine(MX - 12, MY, MX + 12, MY);     // boca recta
      dispTextSmall(100, 20, "?");
      break;

    case Emotion::NOTIFICATION:
      eyeBox(ELX, oh, 0);
      eyeBox(ERX, oh, 0);
      dispBox(MX - 2, MY - 8, 4, 10);         // signo "!"
      dispDisc(MX, MY + 5, 2);
      break;

    case Emotion::ERROR:
      dispLine(ELX - 10, EY - 10, ELX + 10, EY + 10);   // ojos en "X"
      dispLine(ELX + 10, EY - 10, ELX - 10, EY + 10);
      dispLine(ERX - 10, EY - 10, ERX + 10, EY + 10);
      dispLine(ERX + 10, EY - 10, ERX - 10, EY + 10);
      dispLine(MX - 12, MY, MX + 12, MY);
      break;

    case Emotion::WIFI_OK:
      eyeBox(ELX, oh, 0); eyeBox(ERX, oh, 0);
      mouthArc(MX, MY - 2, 16, 7, true);
      dispLine(MX - 22, MY + 8, MX - 16, MY + 12);      // check
      dispLine(MX - 16, MY + 12, MX - 6, MY + 2);
      dispTextSmall(80, MY + 12, "WiFi");
      break;

    case Emotion::WIFI_FAIL:
      eyeBox(ELX, oh, 3); eyeBox(ERX, oh, 3);
      mouthArc(MX, MY + 3, 16, 6, false);
      dispLine(MX - 24, MY + 4, MX - 14, MY + 14);      // "X"
      dispLine(MX - 14, MY + 4, MX - 24, MY + 14);
      dispTextSmall(80, MY + 12, "WiFi");
      break;

    case Emotion::IDLE:
    default:
      dispDisc(ELX, EY, 9);
      dispDisc(ERX, EY, 9);
      mouthArc(MX, MY, 12, 5, true);
      break;
  }

  dispFlush();
}
