# Licenses and provenance

The [LICENSE](LICENSE) file applies MIT to Astra-specific contributions. It
does not replace third-party component licenses. Release archives include
this document and the `licenses/` directory; neither DarkCenobyte nor
GPT-6 Astra imposes a license on others' contributions.

| Component | Use | License / notice |
| --- | --- | --- |
| SDL 2.32.10 | macOS player, statically linked from unmodified source | zlib; [SDL2.txt](licenses/SDL2.txt); HIDAPI/BSD, YUV, and libm notices retained |
| System SDL2 | Linux players, dynamically linked | zlib; the distribution supplies the library and its notices |
| MinGW-w64 | Windows binary libraries and adapters | Public domain and permissive licenses, depending on the files; [MINGW-W64.txt](licenses/MINGW-W64.txt) |
| GCC / libgcc | Compilation and possible runtime support | GPL with the GCC Runtime Library Exception for the relevant runtime portions; [GCC.txt](licenses/GCC.txt) |
| UPX | Windows PE compression and unmodified decompression stub | GPL with an exception for compressed executables; [UPX.txt](licenses/UPX.txt) |
| DejaVu / Bitstream Vera / Arev and STIX | Formula outlines converted to points in `src/formula_points.h` | Bitstream/Arev and SIL OFL 1.1 notices retained in [FONTS.txt](licenses/FONTS.txt) |
| eSpeak NG | Optional tool for generating intermediate WAVs, built-in `en-us+f3` voice | GPL-3.0-or-later for the program; no eSpeak engine or dictionary is redistributed |
| NumPy, SciPy, scikit-learn, Matplotlib | Optional generation scripts; none of these packages are distributed here | Their respective licenses; the player does not depend on them at runtime |
| OSMesa / Mesa | Optional offscreen rendering and Linux tests | System libraries, not bundled with releases |

## Why MIT applies to the project's own code

SDL2 uses the zlib license and allows static or dynamic linking with software
under different licenses. The UPX stub is exactly the one produced by the
tool: it is neither modified nor used for anything beyond initial
decompression. UPX's exception permits distribution of the compressed
executable without applying the GPL to Astra's code. The terms for the
MinGW/GCC runtimes still apply to their own portions.

The `voice.h` model contains the results of LPC analysis of original lyrics
rendered by eSpeak NG's `en-us+f3` formant voice. It contains no copy of the
eSpeak engine, its dictionaries, or an MBROLA voice bank. eSpeak's author has
explicitly stated that the program's GPL does not apply to WAV files it
produces. Astra's script analyzes that output and builds its own spectrum
table and score.

The typographic points represent the outlines of a typeset formula, rather
than a font file for general typesetting. The original notices are retained
out of caution and to credit the fonts. They are not replaced by a claim
that the fonts themselves are MIT-licensed.

## Official sources

- [SDL2: licensing](https://wiki.libsdl.org/SDL2/FAQLicensing)
- [SDL2 2.32.10: source release](https://github.com/libsdl-org/SDL/releases/tag/release-2.32.10)
- [UPX: exception for compressed executables](https://upx.github.io/upx-license.html)
- [GCC Runtime Library Exception](https://www.gnu.org/licenses/gcc-exception-3.1.html)
- [MinGW-w64: notices](https://github.com/mingw-w64/mingw-w64/blob/master/COPYING)
- [eSpeak: Jonathan Duddington's clarification on audio output](https://sourceforge.net/p/espeak/discussion/538920/thread/c6944a60/)
- [eSpeak NG](https://github.com/espeak-ng/espeak-ng)
- [DejaVu: license](https://dejavu-fonts.github.io/License.html)
- [SIL OFL](https://openfontlicense.org/)

Rez and Hollywood Burns are artistic references; no game assets or existing
recordings are incorporated. The short quotation in the book is attributed
to Einstein on-screen and is not presented as original writing by GPT-6
Astra or DarkCenobyte.
