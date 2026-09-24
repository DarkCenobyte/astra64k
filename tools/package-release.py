#!/usr/bin/env python3
"""Package one built target with notices and portable executable permissions."""
import argparse
import hashlib
from pathlib import Path
import platform
import shutil
import struct
import subprocess
import tarfile
import tempfile
import zipfile

ROOT=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser()
p.add_argument('--platform',choices=['windows-x64','linux-x64','linux-arm64','macos-arm64'],required=True)
p.add_argument('--binary',type=Path,required=True)
p.add_argument('--output',type=Path,default=ROOT/'dist')
args=p.parse_args()
version=(ROOT/'VERSION').read_text().strip()
name=f'Astra-v{version}-{args.platform}'
args.output.mkdir(parents=True,exist_ok=True)
binary=args.binary.resolve()
if not binary.exists():p.error(f'Build output not found: {binary}')
# A release must never silently package an x64 binary as ARM64 (or vice versa).
exe=binary/'Contents/MacOS/Astra' if binary.is_dir() else binary
data=exe.read_bytes()
if args.platform.startswith('windows'):
    pe=struct.unpack_from('<I',data,0x3c)[0]
    assert data[pe:pe+4]==b'PE\0\0' and struct.unpack_from('<H',data,pe+4)[0]==0x8664
    assert len(data)<=64000, 'Windows 64k budget exceeded'
elif args.platform.startswith('linux'):
    assert data[:5]==b'\x7fELF\x02' and data[5]==1, 'Expected a 64-bit little-endian ELF'
    assert struct.unpack_from('<H',data,18)[0]==(183 if args.platform.endswith('arm64') else 62)
else:
    assert data[:4]==b'\xcf\xfa\xed\xfe', 'Expected a 64-bit Mach-O'
    assert struct.unpack_from('<I',data,4)[0]==0x100000c, 'Expected Apple Silicon ARM64'
    deps=subprocess.check_output(['otool','-L',str(exe)],text=True)
    assert '/opt/homebrew/' not in deps and '/usr/local/' not in deps, 'Non-portable dylib dependency'

with tempfile.TemporaryDirectory(prefix='astra-package-') as temp:
    stage=Path(temp)/name;stage.mkdir()
    if binary.is_dir():
        shutil.copytree(binary,stage/'Astra.app',symlinks=True)
    else:
        target=stage/(name+'.exe' if args.platform.startswith('windows') else name)
        shutil.copy2(binary,target);target.chmod(0o755)
    for document in ['LICENSE','README.md','THIRD_PARTY.md','CHANGELOG.md']:
        shutil.copy2(ROOT/document,stage/document)
    shutil.copytree(ROOT/'licenses',stage/'licenses')
    if args.platform.startswith('windows'):
        diagnostic=stage/(name+'-diagnostic.cmd')
        diagnostic.write_bytes(('@echo off\r\ncd /d "%~dp0"\r\nstart "" /wait "%~dp0'+name+'.exe" --windowed --low\r\necho Diagnostic log: '+name+'.exe.log\r\npause\r\n').encode('ascii'))
        archive=args.output/(name+'.zip')
        with zipfile.ZipFile(archive,'w',zipfile.ZIP_DEFLATED,compresslevel=9) as z:
            for entry in sorted(stage.rglob('*')):
                if entry.is_file():z.write(entry,entry.relative_to(stage))
        # The direct .exe asset is also offered; notices remain in its companion ZIP.
        shutil.copy2(binary,args.output/(name+'.exe'))
    else:
        archive=args.output/(name+'.tar.gz')
        with tarfile.open(archive,'w:gz') as tar:tar.add(stage,arcname=name)
for file in sorted(args.output.iterdir()):
    if file.is_file() and file.name.startswith(name) and not file.name.endswith('.sha256'):
        digest=hashlib.sha256(file.read_bytes()).hexdigest()
        file.with_name(file.name+'.sha256').write_text(f'{digest}  {file.name}\n')
        print(file.name,file.stat().st_size,'bytes')
