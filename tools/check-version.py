#!/usr/bin/env python3
"""Keep the tag, file names and embedded version in agreement."""
import os
from pathlib import Path
import re

version=(Path(__file__).resolve().parents[1]/'VERSION').read_text().strip()
assert re.fullmatch(r'[0-9]+\.[0-9]+(?:\.[0-9]+)?',version), 'Invalid VERSION'
if os.environ.get('GITHUB_REF_TYPE')=='tag':
    assert os.environ['GITHUB_REF_NAME']=='v'+version, 'Tag must match VERSION'
print('Astra version:',version)
