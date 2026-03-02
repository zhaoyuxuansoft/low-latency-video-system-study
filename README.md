# Low Latency Video System Study

## Experiments Included
- Resolution vs TTFF (FFmpeg + WebSocket + JSMpeg, localhost)
- GOP vs TTFF (+ bandwidth trade-off)

## Project Overview

This project benchmarks end-to-end startup latency (TTFF: Time To First Frame) in a low-latency video streaming pipeline based on:

- FFmpeg encoding
- MPEG-TS over WebSocket
- JSMpeg browser playback

The goal is to understand which components dominate first-frame delay and how system parameters affect latency stability.

---

## System Architecture

Video File  
↓  
FFmpeg Encoder  
↓  
WebSocket Transport  
↓  
JSMpeg Decoder  
↓  
Canvas Rendering  

TTFF is defined as:

TTFF = t_visible_frame - t_page_load

Where:

- t_page_load = performance.now() at refresh
- t_visible_frame = first non-black pixel detected on canvas

---

## Experiments

### 1. Resolution vs TTFF
Investigates whether decoding complexity affects startup latency.  
→ See: experiments/resolution_vs_ttff/report.md

### 2. GOP vs TTFF (+ Bandwidth Trade-off)
Evaluates the impact of keyframe interval on startup latency and bandwidth cost.  
→ See: experiments/gop_vs_ttff/report.md

Conclusion:

Resolution and decoding complexity are not dominant factors in startup latency under this architecture.  
Pipeline initialization and keyframe arrival are more critical.

---

More experiments (GOP tradeoff, ring buffer optimization) will be added.
