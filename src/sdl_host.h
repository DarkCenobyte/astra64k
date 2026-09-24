/* Native desktop host for Linux x64/ARM64 and macOS Apple Silicon.
   Scene geometry, GLSL, score, vocoder and lyrics are shared with Windows. */
static SDL_atomic_t musicReady;
static int makeMusic(void *unused){
    (void)unused;compose();SDL_AtomicSet(&musicReady,1);return pcm?0:1;
}
static int hostError(const char *where){
    fprintf(stderr,"Astra: %s: %s\n",where,SDL_GetError());return 1;
}
static int queuedAudioTest(SDL_AudioDeviceID device){
    Uint32 initial=SDL_GetQueuedAudioSize(device),start=SDL_GetTicks();
    SDL_PauseAudioDevice(device,0);
    while(SDL_GetQueuedAudioSize(device)==initial&&SDL_GetTicks()-start<2000)SDL_Delay(5);
    if(SDL_GetQueuedAudioSize(device)==initial){fprintf(stderr,"Audio cursor did not advance\n");return 0;}
    SDL_PauseAudioDevice(device,1);
    Uint32 pausedBytes=SDL_GetQueuedAudioSize(device);SDL_Delay(40);
    if(SDL_GetQueuedAudioSize(device)!=pausedBytes){fprintf(stderr,"Audio cursor moved during pause\n");return 0;}
    SDL_PauseAudioDevice(device,0);start=SDL_GetTicks();
    while(SDL_GetQueuedAudioSize(device)==pausedBytes&&SDL_GetTicks()-start<2000)SDL_Delay(5);
    if(SDL_GetQueuedAudioSize(device)==pausedBytes)return 0;
    SDL_PauseAudioDevice(device,1);puts("Audio cursor, pause and resume: PASS");return 1;
}
int main(int argc,char **argv){
    int windowed=0,low=0,test=0,audioTest=0,finishAfter=0;
    const char *audioPath=0;
    for(int i=1;i<argc;i++){
        if(!strcmp(argv[i],"--windowed"))windowed=1;
        else if(!strcmp(argv[i],"--low"))low=1;
        else if(!strcmp(argv[i],"--selftest"))test=windowed=1;
        else if(!strcmp(argv[i],"--selftest-audio"))audioTest=1;
        else if(!strcmp(argv[i],"--audio")&&i+1<argc)audioPath=argv[++i];
        else if(!strcmp(argv[i],"--help")){
            puts("Astra v" ASTRA_VERSION " - by GPT-6 Astra\nPrompts: DarkCenobyte\n"
                 "--windowed  --low  --audio file.wav  --selftest  --selftest-audio\n"
                 "Escape: quit. Space: pause/resume. Duration: 144 seconds.");return 0;
        }else{fprintf(stderr,"Unknown or incomplete option: %s\n",argv[i]);return 2;}
    }
    if(audioPath||audioTest){
        initArt();compose();
        int ok=pcm&&writeWave(audioPath?audioPath:"Astra-audio-selftest.wav");
        free(pcm);free(vocal);return ok?0:3;
    }
    SDL_SetMainReady();
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_TIMER))return hostError("SDL initialization");
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,24);
    Uint32 flags=SDL_WINDOW_OPENGL|SDL_WINDOW_ALLOW_HIGHDPI;
    flags|=test?SDL_WINDOW_HIDDEN:windowed?SDL_WINDOW_SHOWN:SDL_WINDOW_FULLSCREEN_DESKTOP;
    SDL_Window *window=SDL_CreateWindow(ASTRA_WINDOW_TITLE,SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,1280,720,flags);
    if(!window){hostError("window creation");SDL_Quit();return 4;}
    SDL_GLContext context=SDL_GL_CreateContext(window);
    if(!context){hostError("OpenGL 2.1 compatibility context");SDL_DestroyWindow(window);SDL_Quit();return 5;}
    SDL_GL_SetSwapInterval(test?0:1);SDL_ShowCursor(SDL_DISABLE);
    SDL_GL_GetDrawableSize(window,&DW,&DH);
    if(DW<=0||DH<=0){DW=1280;DH=720;}
    W=test?640:low?960:DW>1920?1920:DW;H=DH*W/DW;
    fprintf(stderr,"OpenGL: %s / %s\n",glGetString(GL_VERSION),glGetString(GL_RENDERER));
    int result=0,running=1,paused=0;
    SDL_AudioDeviceID device=0;
    if(!initRender()){fprintf(stderr,"OpenGL shaders or framebuffer initialization failed\n");result=6;goto cleanup;}
    initArt();
    /* Draw loading art before compose writes the shared PCM and voice buffers. */
    frame(2.5f);SDL_GL_SwapWindow(window);
    SDL_Thread *thread=SDL_CreateThread(makeMusic,"Astra soundtrack",0);
    if(!thread){result=hostError("soundtrack thread");goto cleanup;}
    while(!SDL_AtomicGet(&musicReady)){
        SDL_Event e;
        while(SDL_PollEvent(&e))if(e.type==SDL_QUIT||(e.type==SDL_KEYDOWN&&e.key.keysym.sym==SDLK_ESCAPE))running=0;
        if(!running){SDL_DetachThread(thread);SDL_Quit();exit(0);}
        SDL_Delay(10);
    }
    SDL_WaitThread(thread,0);
    if(!pcm){fprintf(stderr,"Not enough memory for soundtrack\n");result=7;goto cleanup;}
    SDL_AudioSpec want={0},have={0};want.freq=RATE;want.format=AUDIO_S16SYS;want.channels=2;want.samples=512;
    device=SDL_OpenAudioDevice(0,0,&want,&have,0);
    if(!device){result=hostError("audio output");goto cleanup;}
    if(have.freq!=RATE||have.format!=AUDIO_S16SYS||have.channels!=2){fprintf(stderr,"Unexpected audio format\n");result=8;goto cleanup;}
    if(SDL_QueueAudio(device,pcm,COUNT*4)){result=hostError("audio queue");goto cleanup;}
    if(test){
        const float times[]={2.5f,28.0f,51.4f,72.0f,113.0f,136.0f};
        for(unsigned i=0;i<sizeof(times)/sizeof(times[0]);i++){
            frame(times[i]);glFinish();GLenum error=glGetError();
            if(error){fprintf(stderr,"Frame %.2f: GL error 0x%x\n",times[i],error);result=9;goto cleanup;}
            printf("Frame %.2f: PASS\n",times[i]);
        }
        if(!queuedAudioTest(device)){result=10;goto cleanup;}
        if(!writeWave("Astra-audio-selftest.wav")){result=11;goto cleanup;}
        puts("Native SDL host selftest: PASS");goto cleanup;
    }
    SDL_PauseAudioDevice(device,0);
    while(running){
        SDL_Event e;
        while(SDL_PollEvent(&e)){
            if(e.type==SDL_QUIT)running=0;
            if(e.type==SDL_KEYDOWN&&!e.key.repeat){
                if(e.key.keysym.sym==SDLK_ESCAPE)running=0;
                if(e.key.keysym.sym==SDLK_SPACE){paused=!paused;SDL_PauseAudioDevice(device,paused);}
            }
        }
        if(!running)break;
        Uint32 remaining=SDL_GetQueuedAudioSize(device);
        if(!remaining){
            /* Allow the final device buffer to drain before closing the stream. */
            if(!finishAfter)finishAfter=(int)SDL_GetTicks()+100;
            if((int)SDL_GetTicks()>=finishAfter)break;
        }
        float t=(COUNT*4-remaining)/(4.0f*RATE);
        frame(t);SDL_GL_SwapWindow(window);SDL_Delay(paused?20:1);
    }
cleanup:
    if(device)SDL_CloseAudioDevice(device);
    free(pcm);free(vocal);SDL_GL_DeleteContext(context);SDL_DestroyWindow(window);SDL_Quit();return result;
}
