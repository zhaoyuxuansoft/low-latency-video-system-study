# Theoretical Latency Lower Bound under Full I-Frame Mode

## Objective

This experiment establishes the theoretical startup latency lower bound of the FFmpeg + JSMpeg pipeline by eliminating GOP-induced waiting.

By setting:

GOP = 1 (every frame is an IDR frame)

we remove the randomness introduced by keyframe interval timing and measure:

- The minimum achievable TTFF
- The stability of startup latency
- The bandwidth cost required to achieve this lower bound

This experiment serves as a reference anchor for later optimization design.

---

## Experimental Environment

- OS: Windows
- Browser: Edge
- Playback: JSMpeg + Canvas (disableGL=true)
- Transport: WebSocket (localhost)
- Video Source: test.mp4 (looped push)
- Resolution: 1280×720
- FPS: 30
- Encoder Control: stream-server.js

No network variability (localhost environment).

---

## Variable Design

Independent Variable:

- GOP = 1 (full I-frame encoding)
  - `-g 1`
  - `-keyint_min 1`
  - `-sc_threshold 0`

Comparison Baseline:

- GOP = 30 (from Experiment 2)

Dependent Variables:

- Startup TTFF (ms)
- Standard deviation of TTFF
- Average bandwidth (Mbps)
- P95 bandwidth
- Peak bandwidth
- Data consumption (MB/min)

---

## Measurement Procedure

1. Modify stream-server.js to enforce GOP=1.
2. Start streaming server.
3. Refresh browser page 10 consecutive times.
4. Record TTFF values.
5. Record bandwidth statistics over 60 seconds.
6. Ensure no other high-load processes running.

---

## Results

### TTFF Comparison

| Metric | GOP=30 | GOP=1 | Difference |
|--------|--------|--------|------------|
| Mean TTFF (ms) | 65.69 | 60.61 | -5.08 ms (-7.7%) |
| Std (ms) | 14.36 | 11.18 | -22% fluctuation |

### Bandwidth Comparison

| Metric | GOP=30 | GOP=1 | Increase |
|--------|--------|--------|----------|
| Avg Mbps | 1.140 | 4.491 | +294% |
| P95 Mbps | 1.904 | 5.553 | +192% |
| Peak Mbps | 2.719 | 5.798 | +113% |
| MB/min | 8.55 | 33.69 | +294% |

---

## Analysis

### 1. Latency Improvement Is Marginal

Full I-frame encoding reduces average TTFF by approximately 5 ms.

This indicates that the system is already close to its startup latency floor under GOP=30.

The removal of keyframe waiting reduces variability but does not significantly change the mean startup delay.

---

### 2. Bandwidth Cost Is Disproportionately High

Achieving this ~5 ms improvement requires:

≈ 4× average bandwidth increase  
≈ 4× data consumption per minute  

Such overhead is unacceptable in:

- Embedded systems (e.g., Raspberry Pi)
- Multi-user concurrent scenarios
- Bandwidth-constrained environments

---

### 3. Stability Improves but Not Fully

Although variance decreases:

Std: 14.36 ms → 11.18 ms

Non-zero jitter remains.

This suggests that:

Browser-side scheduling (WASM / Canvas rendering) contributes to residual instability beyond keyframe timing.

---

## Conclusion

1. Full I-frame encoding establishes a practical lower bound for TTFF in this architecture (~60 ms).
2. Startup latency is already near architectural limits under GOP=30.
3. Global GOP=1 strategy is not viable due to excessive bandwidth cost.
4. Future optimization should focus on selective or on-demand keyframe injection rather than global GOP reduction.

This experiment provides the theoretical anchor point for subsequent adaptive design.