# Three-Stage Memory Chain Design

## Goal

Turn the current memory-driven event gating into a true three-stage narrative chain that rewards broader exploration.

## Chosen Chain

- Stage 1: discovering `meadow-swing` unlocks `swing_memory_echo` in `meadow`
- Stage 2: discovering both `meadow-swing` and `ruins-gate` unlocks `platform_convergence` in `station`
- Stage 3: discovering `meadow-swing`, `ruins-gate`, and `lakeside-reeds` unlocks a final station event

## Final Event

Use a new station event `terminal_refrain` with the highest weight among station events, gated by all three memory tags.

## Why this design

- builds directly on the existing single- and multi-memory gating system
- encourages cross-region exploration before the strongest station narrative appears
- stays deterministic and easy to test without adding new subsystems

## Out of Scope

- branching choices between memory chains
- persistent campaign state
- dynamic music composition driven by chain stage
