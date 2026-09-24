#!/usr/bin/env python3
"""Reject Astra's v2-v5 Windows regression: direct branches into import data.

Run on the unpacked executable. Requires Python 3 and GNU objdump (the
ASTRA_OBJDUMP environment variable can select a MinGW objdump).
This is static verification of PE machine code, not a Windows runtime test.
"""
import argparse
import os
from pathlib import Path
import re
import struct
import subprocess
import sys


def check(path):
    data=path.read_bytes()
    u16=lambda p: struct.unpack_from('<H',data,p)[0]
    u32=lambda p: struct.unpack_from('<I',data,p)[0]
    pe=u32(0x3c)
    if data[:2]!=b'MZ' or data[pe:pe+4]!=b'PE\0\0' or u16(pe+4)!=0x8664 or u16(pe+24)!=0x20b:
        raise ValueError('Expected a PE32+ AMD64 executable')
    base=struct.unpack_from('<Q',data,pe+48)[0]
    sh=pe+24+u16(pe+20)
    sections=[]
    for i in range(u16(pe+6)):
        p=sh+i*40
        name=data[p:p+8].split(b'\0')[0].decode('ascii')
        start=base+u32(p+12)
        sections.append((start,start+max(u32(p+8),u32(p+16)),u32(p+36),name))
    if any(s[3].startswith('UPX') for s in sections):
        raise ValueError('Run this check before UPX compression, or unpack to a temporary copy')
    disasm=subprocess.check_output([os.environ.get('ASTRA_OBJDUMP','objdump'),'-d',str(path)],text=True)
    branches=0
    invalid=[]
    for line in disasm.splitlines():
        m=re.match(r'^\s*([0-9a-f]+):.*\s(?:callq?|jmpq?)\s+(?:0x)?([0-9a-f]+)(?:\s|$)',line)
        if not m:
            continue
        at,target=(int(x,16) for x in m.groups())
        branches+=1
        dest=next((s for s in sections if s[0]<=target<s[1]),None)
        if not dest or not dest[2]&0x20000000:
            invalid.append({'instruction':hex(at),'target':hex(target),'section':dest[3] if dest else 'outside image'})
    print(f'{path.name}: PE32+ AMD64; {branches} direct branches; {len(invalid)} invalid destinations')
    for item in invalid[:8]:
        print(f"  {item['instruction']} -> {item['target']} ({item['section']})")
    if not branches:
        raise ValueError('No direct branches found; disassembly cannot be verified')
    if invalid:
        return False
    print('PASS: every direct CALL/JMP targets executable code')
    return True


if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('files',nargs='+',type=Path)
    args=parser.parse_args()
    ok=True
    for file in args.files:
        try:
            ok=check(file) and ok
        except (ValueError,OSError,struct.error,subprocess.CalledProcessError) as e:
            print(f'{file}: FAIL: {e}',file=sys.stderr)
            ok=False
    sys.exit(0 if ok else 1)
