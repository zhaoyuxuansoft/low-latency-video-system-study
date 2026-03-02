# Low Latency Video System Study

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

## Experiment 1: Resolution vs TTFF

### Objective

To measure whether video resolution impacts startup latency.

### Variables

Resolutions tested:

- 426x240
- 640x360
- 854x480
- 1280x720

All other parameters fixed:
- Same PC
- Same browser (Edge)
- Same WebSocket localhost
- Same bitrate and FPS

### Result Summary

TTFF remained stable (~55–58 ms) across all resolutions.

### Visualization

![Resolution vs TTFF](resolution_vs_ttff.png)

Conclusion:

Resolution and decoding complexity are not dominant factors in startup latency under this architecture.  
Pipeline initialization and keyframe arrival are more critical.

---

More experiments (GOP tradeoff, ring buffer optimization) will be added.
