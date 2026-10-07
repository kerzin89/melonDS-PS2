# Contributing

Contributions are welcome.

Keep changes focused and document which layer they affect: reconstructed game logic, HGPS2 compatibility, PS2 backend, build/tooling, or research.

For reverse-engineered functions, document the behavior being reproduced and prefer clean portable C over architecture-specific translation when possible. Do not claim generated pseudocode is original source.

Test PS2-facing changes with the PS2SDK/R5900 toolchain. Clearly label code that is untested or stubbed.

Do not commit ROMs, BIOS/firmware, proprietary SDK files, or extracted commercial assets. See `docs/ROM_POLICY.md`.

Useful contribution areas include PS2 pad support, GS rendering, SPU2 audio, save/filesystem support, RTC/timing, overlay/module management, source inventory, and build automation.
