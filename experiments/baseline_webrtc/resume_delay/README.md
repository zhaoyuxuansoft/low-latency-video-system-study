# WebRTC Baseline Experiments

This directory contains measurements performed on the existing WebRTC-based streaming architecture provided by the supervisor.

These experiments serve as a **baseline reference** before implementing the proposed FFmpeg + JSMpeg pipeline.

The goal is not to optimize WebRTC, but to:

- Characterize its latency behaviour
- Understand resume and startup delay distributions
- Identify architectural limitations
- Provide comparison data for the proposed system

---

## Included Experiments

### Resume-to-Visible Delay

Measures the latency when video transitions from invisible to visible state while the stream connection remains active.

This experiment reveals distribution characteristics and highlights heavy-tail behaviour.

See:
