# Phase 04 — Priority queue
- goal: a hand-written binary min-heap of dongle requests ordered by key then arrival ticket, supporting the fifo key (arrival ticket) and the edf key (last_compile_start + time_to_burnout)

## 4.1 Request order
- layer: core
- goal: the dongle request type and its strict order (key, then arrival ticket), plus the fifo/edf key computation with no overflow

## 4.2 Heap storage
- layer: core
- goal: a fixed-capacity request heap whose array is allocated and freed explicitly, with overflow-checked sizing and a safe repeated free

## 4.3 Heap operations
- layer: core
- goal: push, peek and pop on the min-heap (sift up/down on the request order); a rejected operation (full, empty) leaves the heap and the output untouched
