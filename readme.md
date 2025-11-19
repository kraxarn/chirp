# chirp

A chirp is a small sound from a small bird, but also (soon) a small game engine for small games. *chirp!*

> [!WARNING]
> The main branch currently contains the old raylib/C++ version of the engine.
> A rewrite of the engine in SDL3/C is currently in progress but still far from usable.

## Goals/plans (incomplete, non-final and in no particular order)

* [-] Supporting all platforms supported by SDL3 (that makes some sense)
  * [x] Android
  * [ ] Emscripten (Web)
  * [ ] FreeBSD / NetBSD / OpenBSD
  * [ ] Haiku OS
  * [ ] iOS / tvOS / visionOS
  * [x] Linux / SteamOS
  * [x] macOS
  * [ ] Nintendo 3DS 
  * [ ] Nokia N-Gage
  * [ ] PlayStation 2
  * [ ] PlayStation Portable
  * [ ] PlayStation Vita
  * [ ] RISC OS
  * [-] Windows
  * [ ] Xbox
* [-] 2D support ([SDL Renderer](https://wiki.libsdl.org/SDL3/CategoryRender)) with 2D physics ([box2d](https://github.com/erincatto/box2d))
* [ ] Basic 3D support ([SDL Renderer](https://wiki.libsdl.org/SDL3/CategoryRender)) with 3D physics ([jolt](https://github.com/amerkoleci/joltc))
* [-] Custom UI library ([shiny](https://github.com/kraxarn/shiny))
* [ ] Python scripting ([pocketpy](https://github.com/pocketpy/pocketpy))