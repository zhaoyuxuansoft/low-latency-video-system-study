# Low Latency Video System Study

## Project Overview

This project investigates startup and resume latency in low-latency video streaming systems.

The research focuses on:

- Time To First Frame (TTFF)
- Resume-to-visible delay
- Latency stability (variance / heavy-tail behaviour)
- Bandwidth cost trade-offs
- Architectural performance limits

Two streaming architectures are evaluated:

1. **WebRTC-based baseline system**
2. **FFmpeg + MPEG-TS + WebSocket + JSMpeg (proposed pipeline)**

---

## Proposed Streaming Architecture

Video File  
↓  
FFmpeg Encoder  
↓  
MPEG-TS over WebSocket  
↓  
JSMpeg (WASM Decoder)  
↓  
Canvas Rendering  

Startup latency (TTFF) is defined as:

TTFF = t_visible_frame − t_page_load  

Where:

- `t_page_load` = performance.now() at page refresh  
- `t_visible_frame` = first non-black pixel detected on Canvas  

---

## Baseline: WebRTC Architecture

Location: `experiments/baseline_webrtc/`

### Resume-to-Visible Delay Experiment

Measures latency when video transitions from invisible → visible while the stream connection remains active.

Key findings:

- Resume delay exhibits heavy-tail distribution  
- Large variability (≈ 20 ms → 440 ms observed)  
- Sub-100 ms recovery cannot be consistently guaranteed  
- Behaviour consistent with keyframe-dominated decoding mechanism  

This baseline characterisation motivates controlled encoder-level experiments.

---

## FFmpeg + JSMpeg Pipeline Experiments

Location: `experiments/ffmpeg_jsmpeg/`

### 1. Resolution vs TTFF

Objective:  
Evaluate whether decoding complexity (resolution scaling) affects startup latency.

Result:

- TTFF remains stable (~55–58 ms) across 240p–720p  
- Resolution is not the dominant startup latency factor  

---

### 2. GOP vs TTFF and Bandwidth Trade-off

Objective:

Evaluate how keyframe interval (GOP size) impacts:

- Startup latency  
- Bandwidth consumption  
- Latency stability  

Findings:

- Larger GOP reduces bandwidth cost  
- TTFF is only weakly affected  
- Very large GOP increases variability  
- GOP=30 provides a practical compromise  

This experiment characterises the latency–bandwidth trade-off.

---

### 3. Full I-Frame Mode (Theoretical Lower Bound)

Configuration:

GOP = 1 (every frame encoded as IDR)

Purpose:

- Remove keyframe waiting randomness  
- Establish theoretical startup latency lower bound  

Findings:

- Mean TTFF improves by only ~5 ms  
- Bandwidth increases ≈ 4×  
- Startup latency already near architectural floor  
- Global full I-frame mode is not practical  

---

## Repository Structure

Experiments are organised under:

- `experiments/baseline_webrtc/`  
- `experiments/ffmpeg_jsmpeg/`  

Each experiment follows a reproducible structure:

- `raw_data/`
- `analysis/`
- `results/`
- `report.md`

---

## Current Status

Baseline characterisation complete.  
Latency lower bound experiment prepared (raw data will be re-recorded to ensure reproducibility).  
Preparing transition to Raspberry Pi edge encoder validation phase.