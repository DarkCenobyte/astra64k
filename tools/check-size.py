#!/usr/bin/env python3
"""Fail the release build instead of silently exceeding the Windows 64k budget."""
import sys
from pathlib import Path

path=Path(sys.argv[1]);limit=int(sys.argv[2]);size=path.stat().st_size
print(f'{path.name}: {size:,} bytes / {limit:,}-byte limit')
if size>limit or size==0:
    raise SystemExit('Windows 64k size gate failed')
