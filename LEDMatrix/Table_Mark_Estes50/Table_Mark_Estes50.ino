
// Enable LEDMATRIX support in neomatrix_config
#define LEDMATRIX
#include "neomatrix_config.h"
#define zeds ledmatrix

#include "Table_Mark_Estes_config.h"
#ifdef SHOW_PATTERN_NUM
#include <Fonts/TomThumb.h>
#endif

// Aurora includes
#include "matrix.h"
#include "Effects.h"
#include "Drawable.h"
#include "Boid.h"
#include "Attractor.h"
#include "Geometry.h"
#include "PatternAttract.h"
PatternAttract attract;
#include "PatternBounce.h"
PatternBounce bounce;
#include "PatternCube.h"
PatternCube cube;
#include "PatternFlock.h"

PatternFlock flock;
#include "PatternFlowField.h"
PatternFlowField flowfield;
#include "PatternIncrementalDrift.h"
PatternIncrementalDrift incrementaldrift;
#include "PatternIncrementalDrift2.h"
PatternIncrementalDrift2 incrementaldrift2;
#include "PatternPendulumWave.h"
PatternPendulumWave pendulumwave;
#include "PatternRadar.h"
PatternRadar radar;
#include "PatternSpiral.h"
PatternSpiral spiral;
#include "PatternSpiro.h"
PatternSpiro spiro;
#include "PatternSwirl.h"
PatternSwirl swirl;
#include "PatternWave.h"
PatternWave wave;

AuroraDrawable* items[] = {
  &attract,//0
  &bounce,//1
  &cube,//2
  &flock,//3
  &flowfield,//4
  &incrementaldrift,//5
  &incrementaldrift2,//6
  &pendulumwave,//7
  &cube,//8  was radar
  &spiral,//9
  &spiro,//10
  &swirl,//11
  &wave,//12
};
AuroraDrawable *patternz;


int8_t item = random(13);

#define MIDLX               (MATRIX_WIDTH/2)
#define MIDLY               (MATRIX_HEIGHT/2)
#define mpatterns           (190)// max number of patterns
#define BallCount           30
#define BIGGER              mmax(MATRIX_WIDTH, MATRIX_HEIGHT)
#define MAXBIG              BIGGER * 2


#ifdef TME_AUDIO
EasyTransfer ETin;
#endif
struct RECEIVE_DATA_STRUCTURE {
  byte laudio[17];
  byte raudio[17];
  float scale;
  boolean bighit;
  byte peakL;
  byte peakR;
};
RECEIVE_DATA_STRUCTURE music;

int16_t slowest = 2, fastest = 30, cool,  bpm[32], targetfps = 10, adjunct = 3, sizzer;
int16_t sparky = 80, ccc, xxx, yyy, dot = 3, radius2, rr, gg, bb;
int16_t  maxiaud, pointy=4,  hue, steper=2,  xblender, hhowmany=3, blender = 120, radius3,  quietcount, ccoolloorr,  h = 0;
int16_t dot4 = 5, dot2 = 6,  dot3 = 1, phew, lender = 128, xsizer, ysizer, xx,  yy, flipme = 1, shifty = 4,  poffset, wind = 2, fancy , sinewidth=3;
uint8_t mstep, LLaudio[MAXBIG], RRaudio[MAXBIG], inner, bfade = 3;

uint16_t raad, howmany, xhowmany, how, velo = 30 , pointyfix = 4;
float scalerW = MATRIX_WIDTH / 64.0;
float scalerH = MATRIX_HEIGHT/ 64.0;
// FIXME(Mark Estes): change int and uint into the correct types (8, 16, or 32)
int8_t bigmax, directn = 1, quash = 5, quiet = 0 ;
uint16_t  counter, ringdelay, bringdelay, firedelay, hitcounter, waiter = 7;

// FIXME(Mark Estes): arrays take space, it would be nice to reuse the same arrays for some demos.
// These arrays can all have values indexed on width or height, which means they can go higher than 256
// if width or height are bigger than 256.
uint8_t fpeed[MAXBIG] ,  iscalerW = int(scalerW + .1), iscalerH = int(scalerH + .1);
uint8_t  fcolor[MAXBIG], fcountr[MAXBIG];
uint8_t kind[BallCount], xpoffset[MAXBIG], fvelo[MAXBIG];
int16_t xvort[MAXBIG], yvort[MAXBIG];
uint16_t fcount[MAXBIG];

boolean square = false, gmusic =  false, flop[12],   nextsong = false;
boolean rimmer[MAXBIG], xbouncer[MAXBIG], ybouncer[MAXBIG];
boolean   mixit = false, slowme = true;

uint32_t carlton,  lasttest, lastmilly, dwell = TIMING * 200;


float  mscale = 1.4, fps = 30;
float radius, xslope[MAXBIG], yslope[MAXBIG], xfire[MAXBIG], yfire[MAXBIG], cangle, sangle;
float locusx, locusy, driftx, drifty, xcen, ycen, yangle, xangle, eeksangle, whyangle;
float Gravity = -9.81, RATIO = (MATRIX_HEIGHT + 0.001) / (MATRIX_WIDTH + 0.001);
float Heighty[BallCount];
float ImpactVelocityStart = sqrt( -2 * Gravity );
float ImpactVelocity[BallCount];
float TimeSinceLastBounce[BallCount];
int   Positiony[BallCount], Positionx[BallCount];
long  ClockTimeSinceLastBounce[BallCount];
float Dampening[BallCount];
CRGBPalette16 thepal;
char readchar;

#ifdef SHOW_PATTERN_NUM
uint8_t print_width = 3;
#endif

#include "Table_Mark_Estes_Impl.h"

void setup()
{

  if (MATRIX_WIDTH == MATRIX_HEIGHT) square = true;
#ifdef TME_AUDIO
  
  Serial1.begin(57600);

  ETin.begin(details(music), &Serial1);
  randomSeed(analogRead(1) - analogRead(2) + analogRead(5));
  pinMode(LATCH, OUTPUT);
  digitalWrite(LATCH, LOW);    // sets the audio module hardware to off
#else
  Serial.begin(115200);
#endif
  delay(1500);
  Serial.println("Reset");
    Serial.println("scalerW, iscalerW");
  Serial.println(scalerW);
  Serial.println(iscalerW);
      Serial.println("scalerH, iscalerH");
    Serial.println(scalerH);
  Serial.println(iscalerH);
  // don't re-init the serial port.
  matrix_setup(false);

  driftx = random(3, MATRIX_WIDTH - 4);//set an initial location for the animation center
  drifty = random(3, MATRIX_HEIGHT - 4);// set an initial location for the animation center
  mstep = byte(256 / (MATRIX_WIDTH - 1)); //mstep is the step size to distribute 256 over an array the width of the matrix

  steper = random8(2, 8);// steper is used to modify h to generate a bigger color step on each move  early stuff not so common any more
  for (byte j = 0; j < 12; j++) {
    flop[j] = false;
    if (random8() < 128) flop[j] = true;
  }

  flop[1] = true;
  flop[6] = false;
  flop[1] = false;
  flop[2] = true;
  flop[3] = false;
  flop[5] = true;


  lastmilly = millis();
  lasttest = millis();
#ifdef TME_AUDIO

  digitalWrite(LATCH, HIGH);
#endif
  hue = random8();//get a starting point for the color progressions

  cangle = (sin8(random(25, 220)) - 128.0) / 128.0;//angle of movement for the center of animation gives a float value between -1 and 1
  sangle = (sin8(random(25, 220)) - 128.0) / 128.0;//angle of movement for the center of animation in the y direction gives a float value between -1 and 1

  whatami();//this prints out the current status of stuff

  effects.leds = matrixleds;
  effects.Setup();
  matrix->show();
#ifdef TME_AUDIO
  digitalWrite(LATCH, LOW);
  delay(10);
  Serial.print("Scale: ");
  audioprocess();
  Serial.println(music.scale);
#endif

#ifdef SHOW_PATTERN_NUM
  // default font is 5x7, this one is 3x5
  matrix->setFont(&TomThumb);
#endif
  // Disable showing FPS on serial port in Framebuffer_GFX, but compute it
#ifdef DISPLAY_FPS
  matrix->setfpsfreq(0);
#endif
}

void loop()
{
  hue += 1;//increment the color basis
  h = hue;  //set h to the color basis
  checkshifty();
  // test for frame rate,  every 8 frames
  checktempo();

  checkfader();

  checkfancy();

  checkwind();

  if (slowme) slowmo();
  EVERY_N_SECONDS(11) {

    cangle = random(25, 85) / 100.0;
    sangle = random(25, 85) / 100.0;
    if (random8() > 128) cangle = -cangle;
    if (random8() > 128) sangle = -sangle;
    /*
        fcool[random(64)] = random (7, 25);
        fcool[random(64)] = random (6, 28);
        fcool[random(64)] = random (8, 25);
        fcool[random(64)] = random (6, 30);
    */
    blender += random(4, 12);
    xblender -= random(4, 12);
  }

  runpattern();//go generate a updted screen

  counter++;//increment the counter which is used for many things
  // spititout();//map the 2d screen to the 8 outpus and push them out on my other matrix, not used here
  matrix->show();
  delay(waiter);//frame rate control
  if (Serial.available()) readchar = Serial.read(); else readchar = 0;
  if (readchar > 31 || millis() > lastmilly + dwell || nextsong) //when to change patterns
  {
    //Serial.print(", Actual FPS: ");
    //Serial.println (fps, 2);
    Serial.print(" count:");
    carlton++;
    Serial.println(carlton);
    newpattern();
  }
}
