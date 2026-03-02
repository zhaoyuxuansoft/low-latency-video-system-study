# Resume-to-Visible Delay (WebRTC Baseline)

## Role in Research

These baseline measurements justify the motivation for:

- Controlled GOP experiments
- Encoder-level tuning
- Explicit pipeline design

Future experiments will compare WebRTC behaviour with the FFmpeg + JSMpeg architecture.

## Objective

Evaluate resume latency in a WebRTC-based streaming architecture when video transitions from invisible → visible state.

The target requirement (from system design goal):

Resume delay < 100 ms.

This experiment quantifies the actual distribution of resume delay under the current WebRTC implementation.

---

## Background

In predictive coding systems (e.g., H.264), decoding may depend on IDR (keyframe) availability.

When video becomes visible again:

- The stream connection remains active.
- The player may need to wait for the next decodable frame.
- Resume latency may depend on keyframe interval and buffering policy.

This experiment investigates whether resume delay is:

- Keyframe-dominated (wide distribution, upper bound behavior), or
- Buffer-dominated (narrow, stable distribution).

---

## Experimental Environment

- OS: Windows
- Browser: Edge
- Network: Campus network
- Player: WebRTC-based webpage provided by supervisor
- Stream connection remains active during visibility change

No encoder or GOP parameters were modified.

This experiment characterizes the existing WebRTC baseline behavior.

---

## Measurement Definition

Resume-to-Visible Delay (RTV):

RTV = t_frame - t_visible

Where:

- t_visible: moment video re-enters viewport (IntersectionObserver)
- t_frame: first visible frame rendered (canvas update detection)

Measurement performed via browser DevTools script.

---

## Procedure

1. Inject measurement script into browser console.
2. Scroll video out of viewport for ~5 seconds.
3. Scroll video back into viewport.
4. Record RTV.
5. Repeat multiple times to collect distribution.

---

## Results

Sorted resume delay values (ms):

23.5, 35.8, 40.3, 45.2, 57.8, 63.6, 68.9, 97.1,  
110.1, 111.1, 120.8, 138.2, 159.7, 163.4, 168.5,  
195.7, 212.9, 223.2, 224.9, 234.7, 242.7, 268.0,  
302.3, 313.9, 440.4, 440.8

Observed characteristics:

- Large variability (≈ 20 ms → 440 ms)
- Significant tail behavior
- Frequent violations of 100 ms target

---

## Interpretation

The resume delay distribution is highly dispersed and exhibits heavy-tail characteristics.

This supports the **Keyframe-Dominance Hypothesis**:

- Resume delay depends on timing relative to next keyframe.
- If a keyframe is immediately available → low delay.
- If keyframe is missed → long waiting time.

The distribution shape does not support a buffer-dominated fixed-delay model.

---

## Implications

Under the current WebRTC architecture:

- Resume latency is not deterministic.
- Sub-100 ms recovery cannot be consistently guaranteed.
- Keyframe interval and decoding policy likely dominate resume behavior.

This motivates further investigation using a controlled FFmpeg + JSMpeg pipeline where GOP and buffering can be explicitly configured.