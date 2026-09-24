#!/usr/bin/env python3
"""Check a native runner's full soundtrack without third-party Python packages."""
from array import array
import math
from pathlib import Path
import sys
import wave

path=Path(sys.argv[1])
with wave.open(str(path),'rb') as wav:
    assert wav.getnchannels()==2, 'Expected stereo'
    assert wav.getsampwidth()==2 and wav.getframerate()==44100, 'Expected 16-bit / 44100 Hz'
    assert wav.getnframes()==144*44100, 'Expected the complete 144-second soundtrack'
    samples=array('h',wav.readframes(wav.getnframes()))
if sys.byteorder!='little':samples.byteswap()
peak=max(abs(v) for v in samples)/32768
assert .5<peak<.95, f'Invalid peak: {peak}'
for second in (2,18,42,58,72,82,106,130):
    a=2*44100*second;b=a+2*44100
    rms=math.sqrt(sum(v*v for v in samples[a:b])/(b-a))/32768
    assert .005<rms<.6, f'Invalid RMS at {second}s: {rms}'
left=samples[::2];right=samples[1::2]
assert any(a!=b for a,b in zip(left,right)), 'Stereo channels must differ'
assert max(abs(v) for v in samples[-882:])<150, 'The ending must fade to silence'
print(f'PASS: 144 s / stereo / 44100 Hz / valid active sections / peak {peak:.3f} / final fade')
