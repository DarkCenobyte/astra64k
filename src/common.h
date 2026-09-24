#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "astra_version.h"
#if defined(ASTRA_SDL)
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_opengl.h>
#elif defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#include <GL/gl.h>
#include <GL/glext.h>
#else
#include <GL/osmesa.h>
#include <GL/gl.h>
#include <GL/glext.h>
#endif
#define PI 3.14159265358979323846f
#define TAU (2*PI)
#define DUR 144
#define RATE 44100
#define COUNT (DUR*RATE)
typedef struct {float x,y,z;} V;
static V v(float x,float y,float z){V a={x,y,z};return a;}
static V add(V a,V b){return v(a.x+b.x,a.y+b.y,a.z+b.z);}
static V sub(V a,V b){return v(a.x-b.x,a.y-b.y,a.z-b.z);}
static V mul(V a,float k){return v(a.x*k,a.y*k,a.z*k);}
static float dot(V a,V b){return a.x*b.x+a.y*b.y+a.z*b.z;}
static V cross(V a,V b){return v(a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x);}
static V norm(V a){return mul(a,1.0f/(sqrt(dot(a,a))+.00001f));}
static float clamp(float x,float a,float b){return x<a?a:(x>b?b:x);}
static float fract(float a){return a-floor(a);}
static float mix(float a,float b,float t){return a+(b-a)*t;}
static V lerp(V a,V b,float t){return add(mul(a,1-t),mul(b,t));}
static float smooth(float a,float b,float x){x=clamp((x-a)/(b-a),0,1);return x*x*(3-2*x);}
static unsigned rng=0xA576A;
static unsigned randi(void){rng^=rng<<13;rng^=rng>>17;rng^=rng<<5;return rng;}
static float rnd(void){return (randi()&0xFFFFFF)/16777216.0f;}
static float hashf(int i){unsigned u=(unsigned)i*747796405u+2891336453u;u=((u>>((u>>28)+4))^u)*277803737u;return ((u>>22)^u)/4294967296.0f;}
static float ss(float a){return sin(a);}
static float cc(float a){return cos(a);}
static int W=1280,H=720,DW=1280,DH=720;
static float T,beat,kick,hat,energy;
static V cyan={.13f,.78f,1},gold={1,.52f,.13f},pink={.93f,.14f,.48f},white={.82f,.91f,1};
static void color(V c,float a){glColor4f(c.x,c.y,c.z,a);}
static void vertex(V p){glVertex3f(p.x,p.y,p.z);}
