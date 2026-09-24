#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p render
version=$(cat VERSION)
renderer="${ASTRA_RENDERER:-build-native/astra-render}"
"$renderer" --audio "render/Astra-v${version}.wav"
LP_NUM_THREADS="${LP_NUM_THREADS:-8}" "$renderer" --video 1280 720 30 0 144 |
  ffmpeg -hide_banner -f rawvideo -pixel_format rgb24 -video_size 1280x720 -framerate 30 -i - \
  -i "render/Astra-v${version}.wav" -i tools/chapters.ffmeta -map 0:v:0 -map 1:a:0 -map_metadata 2 -map_chapters 2 \
  -c:v libx264 -preset fast -crf 18 -pix_fmt yuv420p -c:a aac -b:a 256k \
  -metadata title="Astra v${version}" -metadata artist="GPT-6 Astra" -movflags +faststart -y "render/Astra-v${version}-apercu.mp4"
