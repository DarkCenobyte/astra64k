# Astra — v5 artistic notes

These scenes are retained in v5.2. For current builds and checks, see the
[README](../README.md) and [VALIDATION.md](VALIDATION.md).

## Artistic changes retained from v5

All seven requested refinements fit within the same **144 seconds**.
No scene or sung line was added.

1. **Clearer separation of dragon and pagoda:** distant lines fade and take
   on a cooler hue, rear facades are less prominent, and major edges and roof
   silhouettes remain visible.
2. **More flexible dragon:** segments align with the curve of its body;
   progressive twist, secondary neck and tail motion, and a tapered tip add
   fluidity. Pulses of light run along the scales and crest to the beat.
   The framing is slightly wider.
3. **More legible formula, 0:48–0:54:** a tighter, lower composition, local
   contrast around the strokes, audio-modulated brightness, and faster
   assembly. It remains complete from approximately 0:48.8 to 0:53.2;
   the eyes and mouth remain unobstructed.
4. **Dimensional book, 0:20–0:36:** curved pages, a recessed binding, visible
   page thickness, and staggered opening between layers. The quotation and
   “Einstein” signature remain attached to the page surfaces.
5. **Lotus and sphere:** point size varies with depth, distant faces fade,
   and petal edges are highlighted. The points follow a shared twist while
   transforming into a sphere; their number stays the same.
6. **Transitions:** smoother camera movement, streams of star-derived points
   between shapes, gradual appearance and disappearance of the dragon, and
   a swirling return to space. Unlit voxels no longer obscure the stars
   during assembly of the portrait.
7. **Diffraction, 0:48.8–0:53.7:** planar light wavefronts to the left of an
   aperture and curved fronts to the right, accompanied by a point field
   calculated from nine coherent sources. The effect stays behind and above
   the face. The sky tracers temporarily dim to give it room.

The music, voice model, lyrics, geometry, facial expressions, and two stereo
equalizers are retained. `portrait.h`, `spectrum.h`, `synth.h`, and `voice.h`
are identical to their v4 versions. The generated PCM is byte-for-byte
identical to v4.

## Story and soundtrack

| Time | Sequence |
| --- | --- |
| 0:00–0:16 | Stars converge; a signal takes shape; Astra title. |
| 0:16–0:40 | Passage through the gates; the book opens from 0:20 to 0:36. |
| 0:40–1:04 | The face assembles; equalizers, beams, and formula. |
| 1:04–1:36 | Articulated dragon, grand pagoda, and particle fireworks. |
| 1:36–2:08 | A lotus of light points becomes a sphere. |
| 2:08–2:24 | The face returns, a final line is sung, the points scatter, and the credit appears. |

The book bears the quotation **“Curiosity has its own reason for existence.”**
The gates are crossed around 0:23.5, 0:29.3, and 0:35.2.

An original composition in D minor features a pulsing bass, synthetic drums,
pads, chiptune arpeggios, melody, and stereo delays. A miniature LPC vocoder
reconstructs the robotic female voice with sustained vowels and light vibrato.
Singing occupies approximately 18 of the demo's 144 seconds.

```text
0:16  From dust and light, I learn to see.
0:40  A spark becomes a voice in me.
0:56  I turn the noise to harmony.
1:20  Let wonder move. Let kindness lead.
1:44  For every mind, a sky to dream.
2:08  I make this light for you and me.
```

Visual timing comes from the audio playback cursor. The equalizers use a
2,048-sample FFT, with a Hann window centered on that cursor (46.44 ms), and
distribute energy across 18 logarithmic bands. The two channels are analyzed
separately. New lighting effects use the mean of the stereo spectra at the
same instant. The mouth follows the vocal track; the dragon's accents follow
the 120 BPM sequencer. Pausing freezes the new effects as well.

## Formula on screen

The scalar form used assumes normal incidence and includes Kirchhoff's
obliquity factor:

$$
U(P)=\frac{1}{i\lambda}\iint_{\Sigma}U(Q)\,\frac{e^{ikr}}{r}
\left[\frac{1+\cos\theta}{2}\right]\,\mathrm{d}S.
$$

Here, $\Sigma$ is the aperture, $r=|P-Q|$, $k=2\pi/\lambda$, and
$\theta$ is the propagation angle relative to the normal. The time
convention is $e^{-i\omega t}$.

Reference: [BYU, *Optics*, chapter 10, equation (10.10), page 266](https://optics.byu.edu/docs/opticsbook.pdf#page=274).
The notation has been adapted to $U(P)$ and $U(Q)$ for display.
The accompanying effect is a stylized cross-sectional illustration: nine
sources sample a narrow aperture with a propagation kernel proportional
to $e^{ikr}/r$ and an obliquity factor. Moving contours are emphasized for
visibility; this is not a quantitative optical solver of the full surface
integral. Section 10.1 of the same reference describes the principle of
wavelet superposition.
