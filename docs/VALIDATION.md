# Verification of the v5.2 GitHub-ready sources

These checks were performed on September 24, 2026, in the Linux preparation
environment. The workflows have not run in a remote GitHub repository.

| Check | Local result |
| --- | --- |
| CMake configuration and Windows x64 build / MinGW GCC 13 | Passed |
| PE32+ AMD64 format | Verified |
| Direct Windows calls/jumps | 1,149 checked; no non-executable destinations |
| Windows file-size limit | **56,320 bytes**, below 64,000 |
| UPX 4.2.2 compression and integrity check | Passed |
| CMake configuration and Linux x64 / SDL2 build | Passed |
| Linux SDL player execution | Passed offscreen with Mesa and dummy audio output |
| SDL rendering | 2.5, 28, 51.4, 72, 113, and 136 s; no OpenGL errors |
| SDL audio cursor, pause, and resume | Verified |
| Complete soundtrack | Stereo, 44,100 Hz, 16-bit, 144 s; active sections, 0.790 peak, and final fade verified |
| Linux audio fidelity against v5 | Byte-for-byte identical WAV |
| Engine / FFT / spectrum / 41-frame tests | Passed via CTest; assertions enabled in Release build |
| OSMesa image comparison with v5.1 | Identical at 28, 51.4, 72, and 113 s |
| Windows and Linux x64 archives | Contents, checksums, and executable permissions verified |
| Incorrect architecture during packaging | x64 binary rejected for an ARM64 archive |
| Version/tag | `v5.2` accepted; mismatched tag rejected |
| GitHub Actions | YAML checked with actionlint; pinned commits and inputs for checkout, upload/download, and attest actions verified |

Reference Linux WAV SHA-256:

```text
10a2f49f8a803d63136cd2dcfcd7d41d9014403fc5c3601e8c03d5fc3b8f6da2
```

## Checks awaiting the first GitHub run

- **Artifact attestations:** generate signed provenance on branch/tag pushes
  and manual runs (except pull requests), then verify every release file
  against its tag, commit, and signing
  workflow. This requires a public repository or GitHub Enterprise Cloud.
- **Windows:** launch the compressed PE and generate/verify its WAV on
  `windows-2022`. This job does not claim to test Windows GPU rendering.
- **Linux ARM64:** compile and test the engine and SDL player on
  `ubuntu-24.04-arm`.
- **macOS ARM64:** build the app bundle, apply an ad hoc signature, check
  dependencies, and synthesize audio on `macos-15`. Graphics rendering still
  needs checking in an actual macOS desktop session.
- **Release:** download the artifacts, verify the five downloadable builds
  (EXE, Windows ZIP, and three TAR.GZ archives), their SHA-256 sidecars,
  and their signed provenance; then publish through `gh` after the jobs succeed.

Local compilation and rendering do not substitute for those native runs.
The Linux preparation runner cannot start Wine; no Windows machine, ARM64
processor, or macOS SDK is available here.

## Scope of verification

The **64,000-byte** limit is enforced for the Windows EXE, not for the
archives with notices or the SDL ports. README screenshots come from the
shared engine running on Linux. Dummy audio and software rendering do not
measure actual audio/GPU device performance or latency.

The generation tools, builds, and tests require no internet connection to
play the work. Build dependencies are fetched while configuring runners;
macOS SDL2 is pinned by version and SHA-256.
