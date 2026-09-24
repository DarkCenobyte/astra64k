#include "voice.h"
static float *left,*right,*vocal;
static short *pcm;
static float sine[8192];
static float osc(float phase){return sine[(int)(phase*8192)&8191];}
static float freq(int note){return 440*pow(2,(note-69)/12.0);}
static void put(int i,float a,float pan){if(i<0||i>=COUNT)return;left[i]+=a*(1-pan*.38f);right[i]+=a*(1+pan*.38f);}
static float ads(float t,float len,float attack,float release){return clamp(t/attack,0,1)*clamp((len-t)/release,0,1);}
static void note(float start,float len,int midi,float amp,int type,float pan){
    int a=start*RATE,n=len*RATE;float f=freq(midi)/RATE,p=0,p2=.23f,p3=.51f,lo=0;
    for(int j=0;j<n&&a+j<COUNT;j++){
        float t=j/(float)RATE,e=ads(t,len,type==1?.003f:(type==2?.23f:.009f),type==2?.7f:.06f),x;
        p+=f;p2+=f*1.003f;p3+=f*.997f;
        if(type==1){float saw=2*fract(p)-1;lo+=.075f*(saw-lo);x=.68f*lo+.56f*osc(p*.5f);e*=.62f+.38f*exp(-t*14);}
        else if(type==2){x=(osc(p)+.25f*osc(p*2)+.32f*osc(p2)+.32f*osc(p3));e*=.78f-.4f*exp(-fract((start+t)*2)*9);}
        else if(type==3){x=(osc(p)+.34f*osc(p*2)+.21f*osc(p3*3)+.12f*osc(p2*5));e*=.7f+.3f*exp(-t*9);}
        else if(type==4){x=osc(p)+.33f*osc(p*3)+.2f*osc(p*5)+.12f*osc(p*7);e*=exp(-t*11);}
        else {x=osc(p)+.42f*osc(p*2.003f)+.22f*osc(p*3.997f);e*=exp(-t*4);}
        put(a+j,x*e*amp,pan);
    }
}
static void drum(float start,int type,float gain){
    int a=start*RATE,n=(type==0?.42f:type==1?.30f:type==2?.065f:.36f)*RATE;
    float p=0,prev=0,low=0;
    for(int j=0;j<n&&a+j<COUNT;j++){
        float t=j/(float)RATE,x=0,noise=rnd()*2-1;
        if(type==0){p+=(47+115*exp(-t*42))/RATE;x=.88f*osc(p)*exp(-t*9)+.07f*noise*exp(-t*240);}
        if(type==1){low+=.27f*(noise-low);x=(noise-low)*.60f*exp(-t*17)+.32f*osc(185*t)*exp(-t*26);x+=.17f*(noise-low)*exp(-fabs(t-.023f)*95);}
        if(type>=2){x=(noise-prev)*exp(-t*(type==2?72:14))*.22f;prev=noise;}
        put(a+j,x*gain,type>=2?(.25f*ss(start*5)):0);
    }
}
static void coefficients(int frame,int next,float blend,float *a){
    int idx=voiceFrames[frame][0]&127,idx2=voiceFrames[next][0]&127;float k[VOICE_ORDER],tmp[VOICE_ORDER+1];
    for(int j=0;j<VOICE_ORDER;j++){float z=exp(mix(voiceBook[idx][j],voiceBook[idx2][j],blend)/2048.0f);k[j]=(z-1)/(z+1);}
    a[0]=1;
    for(int m=1;m<=VOICE_ORDER;m++){
        for(int j=1;j<m;j++)tmp[j]=a[j]+k[m-1]*a[m-j];
        for(int j=1;j<m;j++)a[j]=tmp[j];
        a[m]=k[m-1];
    }
}
static void sing(void){
    unsigned savedRng=rng;rng=0xA57A62u;
    float *sample=calloc(16000*4,sizeof(float));
    for(unsigned wi=0;wi<WORDS;wi++){
        Word w=words[wi];int n=w.len*16000;float mem[VOICE_ORDER]={0},a[VOICE_ORDER+1]={0},de=0,phase=0,dc=0,lastExc=0;
        float gain=0,voiced=0,peak=.001,power=0;
        float previous=wi&&fabs(words[wi-1].start+words[wi-1].len-w.start)<.02f?words[wi-1].note:w.note;
        for(int i=0;i<n;i++){
            if(i%64==0){
                float pos=i*(float)w.count/n;int frame=w.frame+(int)pos,next=frame+1<w.frame+w.count?frame+1:frame;float blend=fract(pos);
                coefficients(frame,next,blend,a);gain=pow(2,mix(voiceFrames[frame][1],voiceFrames[next][1],blend)/12.0-16);
                voiced=mix((voiceFrames[frame][0]&128)?1:0,(voiceFrames[next][0]&128)?1:0,blend);
            }
            float time=i/16000.0f;
            float pitch=mix(previous,w.note,smooth(0,.055f,time));
            float f=440*pow(2,(pitch-69)/12.0f)*(1+.009f*smooth(.10f,.28f,time)*ss(time*TAU*5.15f));
            if(w.len>.8f&&i>n*.55f)f*=pow(2,-2*smooth(n*.55f,n*.8f,i)/12.0f);
            float period=16000/f;phase+=1/period;
            float excitation=(rnd()*2-1)*1.732f*(1-voiced*.94f)-voiced/sqrt(period);
            if(phase>=1){phase-=1;excitation+=sqrt(period)*voiced;}
            float y=(excitation*.72f+lastExc*.28f)*gain;lastExc=excitation;
            for(int j=0;j<VOICE_ORDER;j++)y-=a[j+1]*mem[j];
            // Bandwidth expansion and a safety bound make coefficient changes stable.
            y=clamp(y,-3,3);
            for(int j=VOICE_ORDER-1;j>0;j--)mem[j]=mem[j-1]*.995f;
            mem[0]=y;de=y+.94f*de;dc+=.008f*(de-dc);
            sample[i]=(de-dc)*ads(i/16000.0f,w.len,.012f,.028f);
            power+=sample[i]*sample[i];
            if(fabs(sample[i])>peak)peak=fabs(sample[i]);
        }
        float gainword=.68f/(peak+.10f),normal=.11f/(sqrt(power/n)+.015f);
        if(normal<gainword)gainword=normal;
        int offset=w.start*RATE,len=w.len*RATE;
        for(int i=0;i<len;i++){
            float p=i*16000.0f/RATE;int k=p;
            float y=mix(sample[k],sample[k+1<n?k+1:k],p-k)*gainword;
            vocal[offset+i]+=y;
        }
    }
    free(sample);rng=savedRng;
}
static void compose(void){
    left=calloc(COUNT,sizeof(float));right=calloc(COUNT,sizeof(float));vocal=calloc(COUNT,sizeof(float));pcm=calloc(COUNT*2,sizeof(short));
    if(!left||!right||!vocal||!pcm)return;
    for(int i=0;i<8192;i++)sine[i]=sin(TAU*i/8192);
    static const int roots[]={38,34,41,36};
    static const int melody[]={0,7,12,10,7,3,5,7, 10,7,3,0,5,3,-2,0};
    static const int arp[]={0,7,12,15,19,15,12,7};
    for(int bar=0;bar<72;bar++){
        float start=bar*2;int root=roots[(bar/2)%4];
        int quiet=(bar>=48&&bar<52)||bar>=68;
        int strong=bar>=32&&bar<48||bar>=56&&bar<64;
        if(!(bar%2))for(int c=0;c<3;c++)note(start,4.8f,root+24+(c==1?(root==38?3:4):c==2?7:0),.027f,2,(c-1)*.8f);
        for(int b=0;b<4;b++){
            if(!quiet||b==0)drum(start+b*.5f,0,bar<4?.55f:.83f);
            if((b&1)&&!quiet)drum(start+b*.5f,1,bar<4?.35f:.65f);
        }
        if(!quiet)for(int k=0;k<16;k++){
            drum(start+k*.125f,k%4==2?3:2,(k&1)?.33f:.50f);
            if(k%4!=3)note(start+k*.125f,.18f,root+(k%8==6?12:0),bar<4?.055f:.115f,1,0);
        }
        if(bar<68)for(int k=0;k<8;k++){
            int n=root+36+arp[(k+bar%2*2)%8];
            // Keep the major chords diatonic to D minor.
            if(root!=38&&((k+bar%2*2)%8==3||(k+bar%2*2)%8==5))n++;
            note(start+k*.25f,.26f,n,quiet?.028f:strong?.048f:.037f,4,ss(k*1.4f+bar));
        }
        if((bar>=4&&bar<20)||(bar>=24&&bar<48)||(bar>=54&&bar<64)){
            for(int k=0;k<4;k++){
                int n=62+melody[(bar%4)*4+k];
                note(start+k*.5f,(k==3?.68f:.42f),n,strong?.12f:.085f,3,ss(bar*.7f)*.4f);
                if(strong&&k%2==0)note(start+k*.5f,.38f,n+12,.035f,5,-.6f);
            }
        }
        if(bar%8==7&&bar<64){
            for(int k=0;k<8;k++)drum(start+1+k*.125f,1,.22f+.035f*k);
        }
        if(bar%8==0)note(start,3.4f,root+48,.06f,5,.65f);
    }
    sing();
    // Stereo dotted-eighth delay, a diffuse tail and gentle bus saturation.
    float dl=0,dr=0,peak=1;
    for(int i=0;i<COUNT;i++){
        float t=i/(float)RATE,duck=1;
        for(int p=0;p<6;p++)if(t>phrases[p].start-.08f&&t<phrases[p].end+.15f)duck=.72f;
        left[i]=left[i]*duck+vocal[i]*.92f+(i>529?vocal[i-529]*.065f:0);
        right[i]=right[i]*duck+vocal[i]*.92f+(i>749?vocal[i-749]*.065f:0);
        if(i>16537){dl=.38f*dl+.62f*right[i-16537];left[i]+=.22f*dl;}
        if(i>22051){dr=.38f*dr+.62f*left[i-22051];right[i]+=.20f*dr;}
        if(i>33113){left[i]+=.10f*right[i-33113];right[i]+=.10f*left[i-32947];}
        float fade=smooth(0,.035f,t)*(1-smooth(138.0f,144.0f,t));
        float l=left[i]*1.12f,r=right[i]*1.12f;
        l=l/(1+.38f*fabs(l));r=r/(1+.38f*fabs(r));
        left[i]=l*fade;right[i]=r*fade;
        if(fabs(left[i])>peak)peak=fabs(left[i]);if(fabs(right[i])>peak)peak=fabs(right[i]);
    }
    for(int i=0;i<COUNT;i++){pcm[2*i]=left[i]*(.79f/peak)*32767;pcm[2*i+1]=right[i]*(.79f/peak)*32767;}
    free(left);free(right);left=right=0;
}
static float mouth(float time){
    if(!vocal)return 0;int i=time*RATE;float m=0;
    for(int j=0;j<512&&i+j<COUNT;j+=8)if(i+j>=0)m+=fabs(vocal[i+j]);
    return clamp(m*.20f,0,1);
}
