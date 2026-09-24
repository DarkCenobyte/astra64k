/* ASTRA — by GPT-6 Astra. Prompts and artistic direction: DarkCenobyte. */
#include "common.h"
#include "win_startup.h"
#include "synth.h"
#include "spectrum.h"
#include "render.h"
#include "art.h"
static int writeWave(const char *path){
    FILE *f=fopen(path,"wb");if(!f)return 0;
    unsigned size=COUNT*4,chunk=size+36,fmt=16,rate=RATE,bytes=RATE*4;unsigned short format=1,ch=2,align=4,bits=16;
    fwrite("RIFF",1,4,f);fwrite(&chunk,4,1,f);fwrite("WAVEfmt ",1,8,f);fwrite(&fmt,4,1,f);fwrite(&format,2,1,f);fwrite(&ch,2,1,f);
    fwrite(&rate,4,1,f);fwrite(&bytes,4,1,f);fwrite(&align,2,1,f);fwrite(&bits,2,1,f);fwrite("data",1,4,f);fwrite(&size,4,1,f);
    int ok=fwrite(pcm,4,COUNT,f)==COUNT&&!ferror(f);if(fclose(f))ok=0;return ok;
}
static void writeImage(const char *path){
    unsigned char *pixels=malloc(W*H*3);glPixelStorei(GL_PACK_ALIGNMENT,1);glReadPixels(0,0,W,H,GL_RGB,GL_UNSIGNED_BYTE,pixels);
    FILE *f=fopen(path,"wb");if(f){fprintf(f,"P6\n%d %d\n255\n",W,H);for(int y=H-1;y>=0;y--)fwrite(pixels+y*W*3,3,W,f);fclose(f);}free(pixels);
}
#ifdef _WIN32
static int running=1,paused=0;
static volatile LONG ready=0;
static HWAVEOUT audio;
static WAVEHDR header;
static double clockSeconds(void){LARGE_INTEGER q,f;QueryPerformanceCounter(&q);QueryPerformanceFrequency(&f);return q.QuadPart/(double)f.QuadPart;}
static LRESULT CALLBACK windowProc(HWND h,UINT m,WPARAM w,LPARAM l){
    if(m==WM_CLOSE||m==WM_DESTROY){running=0;return 0;}
    if(m==WM_KEYDOWN&&w==VK_ESCAPE){running=0;return 0;}
    if(m==WM_KEYDOWN&&w==VK_SPACE&&audio&&!(l&(1u<<30))){paused=!paused;if(paused)waveOutPause(audio);else waveOutRestart(audio);return 0;}
    if(m==WM_SETCURSOR){SetCursor(0);return TRUE;}
    if(m==WM_ERASEBKGND)return 1;
    return DefWindowProcA(h,m,w,l);
}
static DWORD WINAPI generateThread(void *unused){compose();InterlockedExchange(&ready,1);return 0;}
void mainCRTStartup(void){
    startLog();
    const char *cmd=GetCommandLineA();int test=strstr(cmd,"--selftest")!=0,windowed=test||strstr(cmd,"--windowed")!=0;
    /* CI can exercise the real Windows entry point and all synthesis maths
       without requiring a GPU or a sound device on the hosted runner. */
    if(strstr(cmd,"--selftest-audio")){
        initArt();compose();
        int ok=pcm&&writeWave("Astra-audio-selftest.wav");
        trace(ok?"Audio selftest passed":"Audio selftest failed");ExitProcess(ok?0:20);
    }
    SetProcessDPIAware();HINSTANCE instance=GetModuleHandleA(0);
    if(!windowed){DW=GetSystemMetrics(SM_CXSCREEN);DH=GetSystemMetrics(SM_CYSCREEN);W=DW;H=DH;if(W>1920){H=DH*1920/DW;W=1920;}}
    if(strstr(cmd,"--low")){W=960;H=DH*960/DW;}
    WNDCLASSA wc={0};wc.style=CS_OWNDC;wc.lpfnWndProc=windowProc;wc.hInstance=instance;wc.lpszClassName="Astra64";
    if(!RegisterClassA(&wc))failStartup("Could not register the window class.",10);
    DWORD style=windowed?(WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX):WS_POPUP;RECT rect={0,0,DW,DH};AdjustWindowRect(&rect,style,0);
    HWND win=CreateWindowA("Astra64",ASTRA_WINDOW_TITLE,style|WS_VISIBLE,windowed?60:0,windowed?60:0,rect.right-rect.left,rect.bottom-rect.top,0,0,instance,0);
    if(!win)failStartup("Could not create the demo window.",11);
    trace("Window created");
    HDC dc=GetDC(win);PIXELFORMATDESCRIPTOR pf={0};pf.nSize=sizeof(pf);pf.nVersion=1;pf.dwFlags=PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER;pf.iPixelType=PFD_TYPE_RGBA;pf.cColorBits=32;pf.cDepthBits=24;
    if(!dc)failStartup("Could not obtain the window device context.",12);
    int pix=ChoosePixelFormat(dc,&pf);
    if(!pix||!SetPixelFormat(dc,pix,&pf))failStartup("Could not set the OpenGL pixel format.",13);
    HGLRC rc=wglCreateContext(dc);
    if(!rc||!wglMakeCurrent(dc,rc))failStartup("Could not activate an OpenGL context.",14);
    trace("OpenGL context active");
    trace((const char*)glGetString(GL_VENDOR));trace((const char*)glGetString(GL_RENDERER));trace((const char*)glGetString(GL_VERSION));
    if(!initRender())failStartup("Astra needs OpenGL 3.0 compatibility support. See the .exe.log file for details.",2);
    trace("Shaders and render targets ready");
    typedef BOOL (WINAPI *SwapInterval)(int);SwapInterval interval=(SwapInterval)wglGetProcAddress("wglSwapIntervalEXT");if(interval)interval(1);
    initArt();trace("Geometry ready; generating soundtrack");
    HANDLE thread=CreateThread(0,0,generateThread,0,0,0);
    if(!thread)failStartup("Could not start the soundtrack generator.",15);
    MSG msg;double begin=clockSeconds();
    while(!ready&&running){while(PeekMessageA(&msg,0,0,0,PM_REMOVE)){TranslateMessage(&msg);DispatchMessageA(&msg);}frame(2.5f);SwapBuffers(dc);Sleep(10);}
    if(!running)ExitProcess(0);WaitForSingleObject(thread,INFINITE);CloseHandle(thread);
    if(!pcm){MessageBoxA(win,"Not enough memory for the soundtrack.","Astra",MB_ICONERROR);ExitProcess(3);}
    trace("Soundtrack ready");
    if(test){frame(70.25f);glFinish();writeImage("Astra-v" ASTRA_VERSION "-Windows-selftest.ppm");writeWave("Astra-v" ASTRA_VERSION "-Windows-selftest.wav");trace("Selftest completed");ExitProcess(0);}
    WAVEFORMATEX wf={WAVE_FORMAT_PCM,2,RATE,RATE*4,4,16,0};
    if(waveOutOpen(&audio,WAVE_MAPPER,&wf,0,0,CALLBACK_NULL)!=MMSYSERR_NOERROR)failStartup("No audio output is available.",4);
    header.lpData=(LPSTR)pcm;header.dwBufferLength=COUNT*4;
    if(waveOutPrepareHeader(audio,&header,sizeof(header))!=MMSYSERR_NOERROR||waveOutWrite(audio,&header,sizeof(header))!=MMSYSERR_NOERROR){waveOutClose(audio);failStartup("Could not start the soundtrack.",5);}begin=clockSeconds();
    trace("Audio playback started; entering demo");
    while(running){
        while(PeekMessageA(&msg,0,0,0,PM_REMOVE)){TranslateMessage(&msg);DispatchMessageA(&msg);}
        if(!running)break;
        MMTIME pos={0};pos.wType=TIME_SAMPLES;waveOutGetPosition(audio,&pos,sizeof(pos));
        float t=pos.wType==TIME_SAMPLES?pos.u.sample/(float)RATE:pos.wType==TIME_BYTES?pos.u.cb/(float)(RATE*4):pos.u.ms*.001f;
        if(t>=DUR-.01f||(header.dwFlags&WHDR_DONE))break;
        frame(t);SwapBuffers(dc);if(paused)Sleep(30);else Sleep(1);
    }
    waveOutReset(audio);waveOutUnprepareHeader(audio,&header,sizeof(header));waveOutClose(audio);
    wglMakeCurrent(0,0);wglDeleteContext(rc);ReleaseDC(win,dc);DestroyWindow(win);
    trace("Demo closed normally");if(startupLog!=INVALID_HANDLE_VALUE)CloseHandle(startupLog);ExitProcess(0);
}
#elif defined(ASTRA_SDL)
#include "sdl_host.h"
#else
int main(int argc,char **argv){
    if(argc>2&&!strcmp(argv[1],"--audio")){compose();writeWave(argv[2]);return 0;}
    int video=argc>1&&!strcmp(argv[1],"--video");
    if(video){W=argc>2?atoi(argv[2]):1280;H=argc>3?atoi(argv[3]):720;}
    else if(argc>5){W=atoi(argv[4]);H=atoi(argv[5]);}
    DW=W;DH=H;unsigned char *buf=calloc(W*H,4);
    OSMesaContext ctx=OSMesaCreateContextExt(OSMESA_RGBA,24,0,0,0);if(!ctx||!OSMesaMakeCurrent(ctx,buf,GL_UNSIGNED_BYTE,W,H)){fprintf(stderr,"OSMesa failed\n");return 2;}
    fprintf(stderr,"GL: %s / %s\n",glGetString(GL_VERSION),glGetString(GL_RENDERER));
    if(!initRender())return 3;initArt();compose();
    if(!pcm)return 4;
    if(video){
        int fps=argc>4?atoi(argv[4]):30;float start=argc>5?atof(argv[5]):0,end=argc>6?atof(argv[6]):DUR;
        unsigned char *pixels=malloc(W*H*3);glPixelStorei(GL_PACK_ALIGNMENT,1);
        for(int i=(int)(start*fps);i<(int)(end*fps);i++){
            frame(i/(float)fps);glReadPixels(0,0,W,H,GL_RGB,GL_UNSIGNED_BYTE,pixels);
            for(int y=H-1;y>=0;y--)if(fwrite(pixels+y*W*3,3,W,stdout)!=(size_t)W){fprintf(stderr,"Video output closed\n");free(pixels);OSMesaDestroyContext(ctx);free(buf);return 5;}
            if(i%fps==0)fprintf(stderr,"render %.0f/%d\n",i/(float)fps,DUR);
        }
        free(pixels);
    }else{frame(argc>2?atof(argv[2]):70);glFinish();writeImage(argc>3?argv[3]:"frame.ppm");}
    OSMesaDestroyContext(ctx);free(buf);return 0;
}
#endif
