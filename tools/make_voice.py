#!/usr/bin/env python3
"""Build Astra's compact singing-vocoder score. No audio sample in the EXE.
Requires espeak-ng, numpy, scipy and scikit-learn (build time only).
Generated speech spectra are vector-quantised; the runtime supplies its own pitch.
"""
from pathlib import Path
import subprocess, json
import numpy as np
from scipy.io import wavfile
from scipy.signal import resample_poly, lfilter
from sklearn.cluster import KMeans

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT/'src'/'voice.h'
TMP = ROOT/'render'/'voice-build-feminine'
TMP.mkdir(parents=True, exist_ok=True)
SR, HOP, ORDER = 16000, 320, 16
# Word lengths are in eighth notes. The melody is in D minor.
PHRASES = [
 (16, 'From dust and light I learn to see', [1,1,1,2,1,1,1,4], [57,57,60,62,60,57,55,57]),
 (40, 'A spark becomes a voice in me', [1,2,2,1,2,1,3], [57,60,60,57,65,64,60]),
 (56, 'I turn the noise to harmony', [1,2,1,2,1,5], [55,57,60,62,60,57]),
 (80, 'Let wonder move let kindness lead', [1,3,2,1,3,2], [57,60,62,60,57,55]),
 (104, 'For every mind a sky to dream', [1,2,2,1,2,1,3], [57,60,65,64,60,57,60]),
 (128, 'I make this light for you and me', [1,1,1,2,1,2,1,3], [57,60,62,65,64,60,57,62]),
]

def lpc(frame):
    r = np.correlate(frame, frame, 'full')[len(frame)-1:len(frame)+ORDER]
    r[0] *= 1.0008
    a = np.zeros(ORDER+1); a[0] = 1
    e = max(r[0], 1e-9); ks=[]
    for m in range(1, ORDER+1):
        k = -float(r[m] + np.dot(a[1:m], r[m-1:0:-1])) / e
        k = np.clip(k, -.985, .985)
        a[1:m] += k*a[m-1:0:-1].copy()
        a[m] = k; ks.append(k)
        e *= 1-k*k
    return np.array(ks), np.sqrt(e/len(frame))

frames=[]; gains=[]; flags=[]; words=[]; phrases=[]
for pi,(start,text,steps,notes) in enumerate(PHRASES):
    t=start
    first=len(words)
    for wi,(word,step,note) in enumerate(zip(text.split(),steps,notes)):
        fn=TMP/(word.lower()+'.wav')
        if not fn.exists():
            subprocess.run(['espeak-ng','-v','en-us+f3','-s','145','-p','55','-a','160','-w',str(fn),word],check=True)
        sr,x=wavfile.read(fn); x=x.astype(float)/32768
        x=resample_poly(x,320,441) if sr==22050 else resample_poly(x,SR,sr)
        nz=np.flatnonzero(np.abs(x)>.006)
        x=x[max(0,nz[0]-160):min(len(x),nz[-1]+280)]
        x=x/max(.01,np.max(np.abs(x)))*.85
        emphasis=lfilter([1,-.94],[1],x)
        # Allocate more time to pitched vowel nuclei, preserving consonant attacks.
        # This changes articulation, not word timings or the original lyrics.
        centres=np.arange(0,len(x),160)
        weights=[]
        for pos in centres:
            ids=np.arange(pos-200,pos+200)
            y=np.interp(ids,np.arange(len(x)),x,left=0,right=0)
            corr=np.correlate(y,y,'full')[399:]
            periodic=np.clip((np.max(corr[30:160])/(corr[0]+1e-10)-.25)/.4,0,1)
            weights.append(.80+periodic*.95)
        time_map=np.cumsum(weights);time_map=(time_map-.5)/(time_map[-1])
        count=max(5,round(step*.25*50))
        offset=len(frames)
        for fi in range(count):
            pos=np.interp((fi+.5)/count,time_map,centres)
            ids=np.arange(int(pos)-200,int(pos)+200)
            y=np.interp(ids,np.arange(len(x)),x,left=0,right=0)
            z=np.interp(ids,np.arange(len(x)),emphasis,left=0,right=0)*np.hamming(400)
            k,g=lpc(z)
            corr=np.correlate(y,y,'full')[399:]
            ratio=np.max(corr[30:160])/(corr[0]+1e-10)
            # eSpeak's vowels are periodic; fricatives and stops use noise.
            voiced=ratio>.32 and np.mean(y*y)>.00008
            frames.append(np.arctanh(k)); gains.append(g); flags.append(voiced)
        words.append(dict(text=word,start=t,length=step*.25,note=note,frame=offset,count=count))
        t+=step*.25
    phrases.append(dict(text=text,start=start,end=t,first=first,count=len(words)-first))

data=np.array(frames)
km=KMeans(n_clusters=112,random_state=6,n_init=4).fit(data)
cb=np.round(km.cluster_centers_*4096).astype(np.int16)
idx=km.labels_
gq=np.clip(np.round((np.log2(np.maximum(gains,1e-6))+16)*12),0,255).astype(int)
with OUT.open('w') as f:
    f.write('/* Astra v2: female LPC spectra, vowel-weighted singing; original lyrics. */\n')
    f.write('#define VOICE_ORDER 16\n')
    f.write('static const short voiceBook[112][16]={\n')
    for row in cb: f.write('{'+','.join(map(str,row))+'},\n')
    f.write('};\nstatic const unsigned char voiceFrames[][2]={\n')
    for a,b,v in zip(idx,gq,flags):f.write('{%d,%d},'%(int(a)+(128 if v else 0),int(b)))
    f.write('\n};\ntypedef struct {const char *text;float start,len;int note,frame,count;} Word;\n')
    f.write('static const Word words[]={\n')
    for w in words:f.write('{"%(text)s",%(start).3ff,%(length).3ff,%(note)d,%(frame)d,%(count)d},\n'%w)
    f.write('};\ntypedef struct {float start,end;int first,count;} Phrase;\nstatic const Phrase phrases[]={\n')
    for p in phrases:f.write('{%(start).3ff,%(end).3ff,%(first)d,%(count)d},\n'%p)
    f.write('};\n#define WORDS (sizeof(words)/sizeof(words[0]))\n')
(ROOT/'render'/'lyrics-timing.json').write_text(json.dumps(dict(phrases=phrases,words=words),indent=2))
print(f'{len(frames)} frames, {len(words)} words; voice model {cb.nbytes+2*len(frames)} bytes')
