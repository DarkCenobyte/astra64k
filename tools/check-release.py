#!/usr/bin/env python3
"""Require every target and verify checksums before publishing a GitHub release."""
import hashlib
from pathlib import Path
import sys

root=Path(sys.argv[1]);version=(Path(__file__).resolve().parents[1]/'VERSION').read_text().strip()
suffixes=['windows-x64.exe','windows-x64.zip','linux-x64.tar.gz','linux-arm64.tar.gz','macos-arm64.tar.gz']
for suffix in suffixes:
    file=root/f'Astra-v{version}-{suffix}'
    expected=file.with_name(file.name+'.sha256').read_text().split()[0]
    assert hashlib.sha256(file.read_bytes()).hexdigest()==expected, f'Checksum mismatch: {file.name}'
    print('PASS:',file.name)
