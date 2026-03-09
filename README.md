# Low Latency Video System Study

## Project Overview

This project investigates ultra-low-latency video streaming over WebSocket using a custom C++ relay server.

The system is designed to study **Time-To-First-Frame (TTFF)** behaviour during playback recovery scenarios such as:

- reconnect events  
- browser tab switching  
- stream visibility changes  

The architecture uses:

- **FFmpeg** for video encoding and streaming  
- **Custom C++ relay server** built with Boost.Asio / Boost.Beast  
- **WebSocket transport**  
- **JSMpeg browser decoder with Canvas rendering**

The main research goal is to explore techniques to reduce playback recovery latency and approach **sub-100 ms TTFF**.

---

# System Architecture

The streaming pipeline is shown below:

FFmpeg (encoder)
      │
      │ TCP ingest
      ▼
C++ Relay Server
      │
      │ WebSocket
      ▼
Browser (JSMpeg + Canvas)

The relay server performs three main tasks:

1. Accept encoded video stream from FFmpeg  
2. Maintain a replay buffer for reconnect recovery  
3. Broadcast video stream to connected WebSocket clients  

---

# Key Features

## 1. Custom C++ Relay Server

The relay server is implemented using **Boost.Asio** and **Boost.Beast** and supports:

- TCP ingest from FFmpeg  
- WebSocket streaming to browsers  
- multi-client broadcast  
- low-latency socket configuration (`TCP_NODELAY`)

The relay is designed specifically for **low-latency live streaming experiments**.

---

## 2. GOP-Aware Ring Buffer

A **GOP-aligned buffering strategy** is implemented to improve reconnect playback latency.

Traditional relay buffers store a fixed sliding window of recent chunks.  
This can cause reconnecting clients to receive stream data starting from predictive frames (P/B frames), forcing the decoder to wait for the next keyframe.

To address this issue, the relay scans incoming stream data for **MPEG sequence headers (`0x000001B3`)** indicating the start of a new GOP.

When a new GOP is detected:

1. The existing buffer is cleared  
2. Buffering restarts from the new keyframe  

This ensures reconnecting clients always receive a **decodable starting point**.

### Benefits

- Reduced reconnect TTFF  
- Improved playback recovery stability  
- Lower variance in first-frame latency  

---

## 3. Browser TTFF Measurement Tool

A browser-based measurement tool is implemented to evaluate playback recovery latency.

The tool records:

- player initialization time  
- first frame render time  
- reconnect TTFF  

Measurement results can be exported as **CSV files for analysis**.

---

# Baseline: WebRTC Architecture

Location: `experiments/baseline_webrtc/`

## Resume-to-Visible Delay Experiment

This experiment measures latency when video transitions from **invisible → visible** while the stream connection remains active.

### Key Findings

- Resume delay exhibits a **heavy-tail distribution**  
- Large variability observed (≈ 20 ms → 440 ms)  
- Sub-100 ms recovery cannot be consistently guaranteed  
- Behaviour is consistent with **keyframe-dominated decoding mechanisms**

This baseline characterisation motivates further **encoder-level and relay-level optimisation experiments**.

---

# FFmpeg + JSMpeg Pipeline Experiments

Location: `experiments/ffmpeg_jsmpeg/`

---

## 1. Resolution vs TTFF

**Objective**

Evaluate whether decoding complexity (resolution scaling) affects startup latency.

**Result**

- TTFF remains stable (~55–58 ms) across **240p–720p**  
- Resolution is **not the dominant startup latency factor**

---

## 2. GOP vs TTFF and Bandwidth Trade-off

**Objective**

Evaluate how keyframe interval (**GOP size**) impacts:

- startup latency  
- bandwidth consumption  
- latency stability  

**Findings**

- Larger GOP reduces bandwidth cost  
- TTFF is only weakly affected  
- Very large GOP increases latency variability  
- **GOP = 30 provides a practical compromise**

This experiment characterises the **latency–bandwidth trade-off**.

---

## 3. Full I-Frame Mode (Theoretical Lower Bound)

**Configuration**

GOP = 1 (every frame encoded as IDR)


**Purpose**

- Remove keyframe waiting randomness  
- Establish a theoretical startup latency lower bound  

**Findings**

- Mean TTFF improves by only ~5 ms  
- Bandwidth increases ≈ **4×**  
- Startup latency already close to architectural floor  
- Global full I-frame mode is not practical  

---

# Relay Server Design

The relay server consists of three main components:

1. **TCP ingest server**  
   Receives MPEG-TS stream from FFmpeg  

2. **Ring buffer**  
   Stores recent stream chunks for reconnect replay  

3. **WebSocket broadcast server**  
   Delivers stream chunks to browser clients  

Incoming stream data is processed in real time and immediately broadcast to connected clients.

---

# Repository Structure

Experiments are organised under:

- experiment/baseline_webrtc/
- experiments/ffmpeg_jsmpeg/

Each experiment follows a reproducible structure:

- raw_data/ 
- analysis/ 
- results/ 
- report.md 


This structure ensures **experiment reproducibility and traceability**.

---

# Current Status

- Baseline architecture validation complete  
- TTFF characterisation experiments completed  
- GOP-aware relay buffering implemented  
- Measurement tooling integrated  

**Next phase**

- Raspberry Pi edge encoder validation  
- End-to-end latency evaluation in edge streaming scenarios