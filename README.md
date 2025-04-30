# CPU Scheduling Simulator

This C++ project simulates several classical CPU scheduling algorithms. It supports both **trace mode** (timeline output) and **stats mode** (performance metrics), making it useful for visual learning and performance analysis.

##  Supported Scheduling Algorithms

- FCFS — First-Come, First-Served
- SPN — Shortest Process Next
- SRT — Shortest Remaining Time
- HRRN — Highest Response Ratio Next
- RR — Round Robin
- FB-1 — Feedback with fixed quantum 1
- FB-2i — Feedback with exponential quantum
- Aging — Aging-based priority scheduling

##  Files

- `lab6.cpp` — The main C++ source file implementing all algorithms.
- `Makefile` — (Optional) For easy compilation using `make`.
- `README.md` — Project documentation.

##  How to Compile

### Using Makefile:
```bash
make
