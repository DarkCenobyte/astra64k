/* Targeted numerical and memory checks for the new spectrum and scene paths. */
#define main astra_renderer_main
#include "../src/astra.c"
#undef main
#include <assert.h>
static int largest(float *a){int m=0;for(int b=1;b<BANDS;b++)if(a[b]>a[m])m=b;return m;}
static void tone(float f,int channel){
    for(int i=RATE-FFT_N;i<RATE+FFT_N;i++)pcm[2*i+channel]=(short)(.30f*32767*ss(TAU*f*(i-RATE)/(float)RATE));
}
static int bandFor(float f){
    int bin=(int)(f*FFT_N/RATE+.1f);
    for(int b=0;b<BANDS;b++)if(bin>=(int)(2*pow(1.41f,b))&&bin<(int)(2*pow(1.41f,b+1)))return b;
    return -1;
}
int main(void){
    float levels[2][BANDS];
    pcm=calloc(COUNT*2,sizeof(short));
    spectrum(1,levels);for(int c=0;c<2;c++)for(int b=0;b<BANDS;b++)assert(levels[c][b]==0);
    const float tones[]={86.1328125f,1378.125f,11025};
    for(int i=0;i<3;i++){
        memset(pcm,0,COUNT*4);tone(tones[i],0);spectrum(1,levels);
        assert(largest(levels[0])==bandFor(tones[i]));assert(levels[0][largest(levels[0])]>.65f);
        for(int b=0;b<BANDS;b++)assert(levels[1][b]<.001f);
        printf("Tone %.4f Hz: band %d; silent opposite channel: PASS\n",tones[i],largest(levels[0]));
    }
    memset(pcm,0,COUNT*4);tone(tones[0],0);tone(tones[2],1);spectrum(1,levels);
    assert(largest(levels[0])==bandFor(tones[0]));assert(largest(levels[1])==bandFor(tones[2]));
    puts("Stereo separation: PASS");
    memset(pcm,0,COUNT*4);pcm[RATE*2]=32767;
    spectrum(.94f,levels);for(int b=0;b<BANDS;b++)assert(levels[0][b]==0);
    spectrum(1,levels);float peak=levels[0][largest(levels[0])];assert(peak>.2f);
    spectrum(1.06f,levels);for(int b=0;b<BANDS;b++)assert(levels[0][b]==0);
    spectrum(-1,levels);spectrum(DUR+1,levels);
    puts("Cursor-centered impulse, silence and buffer boundaries: PASS");
    free(pcm);pcm=0;rng=0xA576A;compose();assert(pcm&&vocal);
    assert(writeWave("Astra-engine-selftest.wav"));
    for(int i=0;i<COUNT;i++)assert(isfinite(vocal[i]));
    spectrum(57.3f,levels);for(int c=0;c<2;c++)for(int b=0;b<BANDS;b++)assert(isfinite(levels[c][b])&&levels[c][b]>=0&&levels[c][b]<=1);
    puts("Full soundtrack and spectrum from final PCM: PASS");
    W=960;H=540;DW=W;DH=H;
    unsigned char *buffer=calloc(W*H,4);OSMesaContext ctx=OSMesaCreateContextExt(OSMESA_RGBA,24,0,0,0);
    assert(ctx&&OSMesaMakeCurrent(ctx,buffer,GL_UNSIGNED_BYTE,W,H));assert(initRender());initArt();
    const float times[]={15.99f,16.0f,20.0f,23.0f,28.0f,33.1f,37.5f,39.0f,39.99f,40.0f,40.5f,41.5f,44.0f,47.99f,48.0f,48.8f,49.2f,50.5f,51.4f,53.3f,53.69f,53.99f,54.0f,62.5f,63.99f,64.0f,65.0f,72.0f,84.0f,94.0f,95.99f,96.0f,98.0f,104.0f,113.0f,116.0f,119.0f,126.0f,130.3f,136.0f,143.99f};
    for(unsigned i=0;i<sizeof(times)/sizeof(times[0]);i++){frame(times[i]);glFinish();assert(glGetError()==GL_NO_ERROR);printf("Frame %.2f, GL state: PASS\n",times[i]);}
    OSMesaDestroyContext(ctx);free(buffer);free(vocal);free(pcm);return 0;
}
