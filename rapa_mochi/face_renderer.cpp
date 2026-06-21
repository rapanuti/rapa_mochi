// ===========================================================================
//  face_renderer.cpp  -  Caras procedurales estilo Dasai Mochi (Gen 3), 128x64.
//  Look de referencia: ojos = rectangulos verticales redondeados blancos; boca
//  pequena; detalles (lagrima, sudor, "z", "?", "!", X) segun la emocion.
// ===========================================================================
#include "face_renderer.h"
#include "display_manager.h"

// Geometria base de la cara (ojos juntos y arriba, boca abajo-centro).
static const int ELX = 44;   // centro ojo izquierdo
static const int ERX = 84;   // centro ojo derecho
static const int EY  = 27;   // centro vertical de los ojos
static const int EW  = 22;   // ancho de ojo
static const int EH  = 26;   // alto de ojo (rect vertical)
static const int ER  = 8;    // radio de esquinas (muy redondeado)
static const int MX  = 64;   // centro boca
static const int MY  = 48;

// Ojo: rectangulo vertical redondeado (con alto y desplazamiento variables).
// IMPORTANTE: el radio se acota a h/2 y EW/2; si no, drawRBox (u8g2_uint_t sin
// signo) hace underflow cuando h < 2*ER y dibuja una banda gigante.
static void eyeR(int cx, int h, int dy) {
  int r = ER;
  if (r > h / 2)  r = h / 2;
  if (r > EW / 2) r = EW / 2;
  if (r < 0)      r = 0;
  dispRBox(cx - EW / 2, EY - h / 2 + dy, EW, h, r);
}

// Ojo "^" (feliz/uwu): dos trazos gruesos.
static void caretEye(int cx) {
  for (int o = 0; o < 2; o++) {
    dispLine(cx - 10, EY + 5 + o, cx,      EY - 5 + o);
    dispLine(cx,      EY - 5 + o, cx + 10, EY + 5 + o);
  }
}

// Ojo medio cerrado (sleepy): dibuja el ojo y borra la parte de arriba.
static void halfEye(int cx, int coverPct) {
  eyeR(cx, EH, 0);
  dispSetColor(0);
  dispBox(cx - EW / 2 - 1, EY - EH / 2 - 1, EW + 2, (EH * coverPct) / 100 + 1);
  dispSetColor(1);
}

// Ojo en "X" (error/estornudo).
static void xEye(int cx) {
  dispLine(cx - 10, EY - 10, cx + 10, EY + 10);
  dispLine(cx - 10, EY - 9,  cx + 10, EY + 11);
  dispLine(cx + 10, EY - 10, cx - 10, EY + 10);
  dispLine(cx + 10, EY - 9,  cx - 10, EY + 11);
}

// Boca en arco parabolico. up=true sonrisa, up=false ceno.
static void mouthArc(int cx, int cy, int halfW, int amp, bool up) {
  for (int x = -halfW; x <= halfW; x++) {
    float t = (float)x / (float)halfW;
    int   d = (int)(amp * (1.0f - t * t));
    int   y = up ? (cy + d) : (cy - d);
    dispPixel(cx + x, y);
    dispPixel(cx + x, y + 1);
  }
}

// Ceja inclinada (enojo).
static void brow(int xi, int yi, int xo, int yo) {
  dispLine(xi, yi, xo, yo);
  dispLine(xi, yi + 1, xo, yo + 1);
}

// Lagrima blanca (gota + reguero).
static void tear(int cx, int topY) {
  dispTriangle(cx, topY, cx - 3, topY + 6, cx + 3, topY + 6);
  dispDisc(cx, topY + 7, 3);
}

void faceRender(Emotion e, uint32_t now) {
  dispClear();
  dispSetColor(1);

  bool blink = (now % 3500UL) < 140UL;       // parpadeo comun
  int  oh    = blink ? 4 : EH;

  switch (e) {

    case Emotion::HAPPY:                       // Smile/Laugh (ojos ^ ^)
      caretEye(ELX); caretEye(ERX);
      mouthArc(MX, 44, 22, 9, true);
      break;

    case Emotion::SAD:                         // Crying
      eyeR(ELX, blink ? 4 : 16, 4);            // ojos caidos
      eyeR(ERX, blink ? 4 : 16, 4);
      tear(ERX, EY + 13);                       // lagrima bajo el ojo (no encima)
      mouthArc(MX, 51, 16, 6, false);
      break;

    case Emotion::SURPRISED:                   // ojos grandes + boca "o"
      dispRBox(ELX - 13, EY - 15, 26, 30, 11);
      dispRBox(ERX - 13, EY - 15, 26, 30, 11);
      dispSetColor(0);                          // pupilas (huecos)
      dispDisc(ELX, EY, 4); dispDisc(ERX, EY, 4);
      dispSetColor(1);
      dispCircle(MX, 50, 5);
      break;

    case Emotion::ANGRY:                       // Angry
      eyeR(ELX, blink ? 4 : 14, 5); eyeR(ERX, blink ? 4 : 14, 5);
      brow(ELX - 12, EY - 13, ELX + 11, EY - 4);
      brow(ERX + 12, EY - 13, ERX - 11, EY - 4);
      mouthArc(MX, 50, 16, 5, false);
      break;

    case Emotion::SLEEPY:                       // Sleepy
      halfEye(ELX, 62); halfEye(ERX, 62);
      dispLine(MX - 6, MY, MX + 6, MY);
      dispTextSmall(98, 16, "z");
      dispTextSmall(106, 9, "z");
      break;

    case Emotion::HOT:                          // jadeo + sudor
      eyeR(ELX, oh, 0); eyeR(ERX, oh, 0);
      dispEllipseFilled(MX, MY + 1, 11, 7);
      dispSetColor(0); dispBox(MX - 11, MY - 6, 22, 5); dispSetColor(1);
      dispDisc(ERX + 15, EY - 8, 3);            // gota de sudor (separada del ojo)
      break;

    case Emotion::COLD:                         // tiritar
      eyeR(ELX, blink ? 4 : 15, 0); eyeR(ERX, blink ? 4 : 15, 0);
      for (int i = -3; i <= 2; i++)            // boca temblorosa centrada
        dispLine(MX + i * 6, MY + (i % 2 ? 3 : -3), MX + (i + 1) * 6, MY + ((i + 1) % 2 ? 3 : -3));
      dispTextSmall(2, 14, "*"); dispTextSmall(118, 14, "*");
      break;

    case Emotion::THINKING:                     // Look Down + "?"
      eyeR(ELX, oh, -4); eyeR(ERX, oh, -4);
      dispLine(MX - 11, MY, MX + 11, MY);
      dispTextSmall(100, 18, "?");
      break;

    case Emotion::NOTIFICATION:                 // ojos + "!"
      eyeR(ELX, oh, 0); eyeR(ERX, oh, 0);
      dispBox(MX - 2, MY - 9, 4, 9);
      dispDisc(MX, MY + 5, 2);
      break;

    case Emotion::ERROR:                        // ojos en "X"
      xEye(ELX); xEye(ERX);
      dispLine(MX - 12, MY, MX + 12, MY);
      break;

    case Emotion::WIFI_OK:                      // sonrisa + check
      eyeR(ELX, oh, 0); eyeR(ERX, oh, 0);
      mouthArc(MX, 45, 14, 6, true);
      dispLine(MX - 22, MY + 6, MX - 16, MY + 11);
      dispLine(MX - 16, MY + 11, MX - 6, MY + 1);
      dispTextSmall(82, MY + 10, "WiFi");
      break;

    case Emotion::WIFI_FAIL:                    // triste + "X"
      eyeR(ELX, blink ? 4 : 16, 4); eyeR(ERX, blink ? 4 : 16, 4);
      mouthArc(MX, 51, 14, 6, false);
      dispLine(MX - 24, MY + 2, MX - 14, MY + 12);
      dispLine(MX - 14, MY + 2, MX - 24, MY + 12);
      dispTextSmall(82, MY + 10, "WiFi");
      break;

    case Emotion::IDLE:                          // Default (ojos + sonrisa minima)
    default:
      eyeR(ELX, oh, 0); eyeR(ERX, oh, 0);
      mouthArc(MX, 46, 9, 4, true);
      break;
  }

  dispFlush();
}
