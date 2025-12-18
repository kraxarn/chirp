# chirp

A chirp is a small sound from a small bird, but also (soon) a small game engine for small games. *chirp!*

> [!WARNING]
> The engine is currently being re-written in SDL/C23 from the old raylib/C++20 version.
> Therefore, the codebase currently consists of both new C code and old C++ code.

## Goals/plans (incomplete, non-final and in no particular order)

* [ ] Truly cross-platform (see [docs/platforms](https://github.com/kraxarn/chirp/blob/main/docs/platforms.md) for details)
* [ ] 2D support ([SDL Renderer](https://wiki.libsdl.org/SDL3/CategoryRender)) with 2D physics ([box2d](https://github.com/erincatto/box2d))
* [ ] Basic 3D support ([SDL Renderer](https://wiki.libsdl.org/SDL3/CategoryRender)) with 3D physics ([jolt](https://github.com/amerkoleci/joltc))
* [ ] Custom UI library ([shiny](https://github.com/kraxarn/shiny))
* [ ] Python scripting ([pocketpy](https://github.com/pocketpy/pocketpy))
