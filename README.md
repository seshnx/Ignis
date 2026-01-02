# SeshNx Ignis - Soft Clipper

A soft clipping saturation plugin in the style of Fruity Soft Clipper.

## Features

- **Cubic Soft Clipping** - Warm, musical saturation (y = 1.5x - 0.5x³)
- **Multiple Clip Modes** - Cubic, Tanh, and Hard clipping
- **Drive Control** - Add warmth and character
- **Threshold & Ceiling** - Precise control over clipping point
- **Wet/Dry Mix** - Parallel processing for subtle to extreme effects
- **I/O Meters** - Visual feedback for input/output levels
- **Clipping Indicator** - LED warning when signal exceeds threshold

## Parameters

| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| Input Gain | -24 to +24 dB | 0 dB | Input level adjustment |
| Drive | 0-100% | 50% | Amount of saturation to apply |
| Threshold | -60 to 0 dB | 0 dB | Point where clipping begins |
| Ceiling | -20 to 0 dB | -0.1 dB | Output limiting ceiling |
| Mix | 0-100% | 100% | Wet/dry mix |
| Output Gain | -24 to +12 dB | 0 dB | Output level adjustment |
| Clip Mode | Cubic/Tanh/Hard | Cubic | Waveshaping algorithm |
| Oversampling | 1x/2x/4x | 1x | Anti-aliasing quality |
| Link Channels | On/Off | On | Stereo linking |

## Factory Presets

### Default
1. **Init** - Neutral starting point with unity gain and 50% drive

### Saturation
2. **Subtle Warmth** - Gentle saturation for adding warmth (25% drive, cubic mode)
   - Use for: Adding subtle harmonic content to sterile recordings
   - Characteristics: Clean, transparent, smooth

3. **Tube Boost** - Warm tube-like saturation (60% drive, tanh mode, 2x oversample)
   - Use for: Adding vintage warmth to individual tracks
   - Characteristics: Rich harmonics, smooth saturation, slight compression

4. **Tape Saturation** - Smooth tape-like compression (50% drive, cubic mode, 2x oversample)
   - Use for: Gluing tracks together, adding cohesion
   - Characteristics: Soft clipping, musical compression, slight boost

### Mix Bus
5. **Glue** - Parallel compression-style blending (40% drive, 40% mix, cubic mode)
   - Use for: Mix bus processing to glue elements together
   - Characteristics: Parallel blending preserves transients while adding body

### Distortion
6. **Aggressive** - Hard clipping for distortion (90% drive, hard clip mode)
   - Use for: Creating aggressive distortion effects
   - Characteristics: Gritty, edgy, high-gain distortion

### Instrument-Specific
7. **Bass Enhancer** - Adds harmonics to bass (70% drive, cubic mode, unity latency)
   - Use for: Adding presence and harmonics to bass instruments
   - Characteristics: Musical harmonics, fast transients, 1x oversample for low latency

8. **Drum Punch** - Adds punch to drums (65% drive, cubic mode, unity latency)
   - Use for: Enhancing drum transients and adding attack
   - Characteristics: Fast response, 1x oversample for minimal latency

### Mastering
9. **Master Limiter** - Ceiling limiting for mastering (20% drive, cubic mode, 4x oversample)
   - Use for: Transparent peak limiting on the master bus
   - Characteristics: Clean limiting, highest quality oversampling, -0.3 dB ceiling

### Creative
10. **Lo-Fi** - Creative lo-fi effect (100% drive, hard clip mode, 1x oversample)
    - Use for: Adding grit and character for creative effects
    - Characteristics: Heavy distortion, aliasing for texture, aggressive squashing

## Building

Requires:
- CMake 3.22+
- JUCE 8.0.0
- C++17 compatible compiler

```bash
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

## License

Copyright © 2025 SeshNx. All rights reserved.
