/* A centered 46 ms Hann window of the final stereo PCM, at the playback cursor.
   One complex FFT carries both channels. No beat proxy, animation clock or
   previous-frame state: seeking, pausing and offline rendering give the same bars. */
#define FFT_N 2048
#define BANDS 18
static void spectrum(float time,float level[2][BANDS]){
    static float re[FFT_N],im[FFT_N];
    int center=(int)(time*RATE);
    for(int i=0;i<FFT_N;i++){
        int p=center+i-FFT_N/2;
        float w=(.5f-.5f*cc(TAU*i/(FFT_N-1)))/32768;
        re[i]=pcm&&p>=0&&p<COUNT?pcm[p*2]*w:0;
        im[i]=pcm&&p>=0&&p<COUNT?pcm[p*2+1]*w:0;
    }
    for(int i=1,j=0;i<FFT_N;i++){
        int bit=FFT_N>>1;for(;j&bit;bit>>=1)j^=bit;j^=bit;
        if(i<j){float a=re[i];re[i]=re[j];re[j]=a;a=im[i];im[i]=im[j];im[j]=a;}
    }
    for(int n=2;n<=FFT_N;n*=2){
        float c=cc(TAU/n),s=-ss(TAU/n);
        for(int i=0;i<FFT_N;i+=n){
            float wr=1,wi=0;
            for(int j=0;j<n/2;j++){
                int a=i+j,b=a+n/2;
                float r=wr*re[b]-wi*im[b],q=wr*im[b]+wi*re[b];
                re[b]=re[a]-r;im[b]=im[a]-q;re[a]+=r;im[a]+=q;
                r=wr*c-wi*s;wi=wr*s+wi*c;wr=r;
            }
        }
    }
    for(int b=0;b<BANDS;b++){
        int low=(int)(2*pow(1.41f,b)),high=(int)(2*pow(1.41f,b+1));
        if(high>FFT_N/2)high=FFT_N/2;
        float power[2]={0,0};
        for(int k=low;k<high;k++){
            int j=FFT_N-k;
            float ar=(re[k]+re[j])*.5f,ai=(im[k]-im[j])*.5f;
            float br=(im[k]+im[j])*.5f,bi=(re[j]-re[k])*.5f;
            power[0]+=ar*ar+ai*ai;power[1]+=br*br+bi*bi;
        }
        for(int ch=0;ch<2;ch++)level[ch][b]=clamp(log(1+1600*sqrt(power[ch])/FFT_N)*.20f-.045f,0,1);
    }
}
