# GOP vs Time-To-First-Frame (TTFF)

## Objective
Evaluate how GOP size (keyframe interval) impacts TTFF.

## Setup
- Pipeline: (fill in: WebRTC / FFmpeg+JSMpeg / etc.)
- Resolution: (e.g., 1280x720)
- FPS: (e.g., 30)
- Network: (Local / Campus)
- Repetitions: 5 runs per GOP
- Metric: Median TTFF (ms)

## Parameters
- GOP sizes tested: 15 / 30 / 60 (edit as needed)

## Files
- gop_ttff.csv → raw data
- plot_gop.py → plotting script
- gop_vs_ttff.png → result plot

## Notes / Observation
- (write your conclusion after you get results)