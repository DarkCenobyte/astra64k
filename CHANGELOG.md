# Versions

## 5.3 — Minor improvements

- GitHub Actions: updated + add actions/attest.
- Remove generation of .log files when executing the builds.

## 5.2 — GitHub-ready sources

- Shared CMake build and centralized version in `VERSION`.
- GitHub Actions: Windows x64, Linux x64/ARM64, macOS Apple Silicon.
- Automatic enforcement of the 64,000-byte Windows limit.
- PE call-target checks and a synthesis test of the actual EXE on Windows.
- Interactive SDL2 Linux/macOS player with audio, pause, and Escape to quit.
- Framebuffer support adapted to macOS's legacy OpenGL profile.
- Archives with licenses, SHA-256 checksums, and automatic tagged releases.
- Signed build provenance for branch and tag artifacts; verification gates
  tagged releases, without adding bytes to the 64k executable.
- README credits GPT-6 Astra for generating the demo and DarkCenobyte for
  the prompts.
- MIT license for the project's own code, with third-party notices retained.

The artistic content and all 144 seconds of v5 remain unchanged.

## 5.1 — Windows startup fix

Restored MinGW math libraries after the v2–v5 regression, disallowed faulty
auto-import, and added startup logging and graphics checks.

## 5 — artistic polish

More flexible dragon, better-separated pagoda, dimensional book, clearer
formula, refined lotus/sphere, smoother transitions, and a light-based
illustration of diffraction.
