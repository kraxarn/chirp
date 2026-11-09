# chirp :bird:

Modern, lightweight, and cross-platform game engine.

# structure

The engine is split into two separate front-ends:
* ~~`engine2d/` 2D renderer of the engine.~~ *Soon!*
* `engine3d/`: 3D renderer for the engine.
* `engine/`: Shared engine logic.

# status

Currently very early in development and not very usable quite yet.

# features

* Lightweight, without any unnecessary bloat.
* Directly native for each supported platform.
* No generative-AI.

# goals (non-final)

* There should only be one way to do something.

# why c?

* It's easier to interface with lower-level libraries.
* It compiles superfast.
* I like shooting myself in the foot :3

# compiler support

Any compiler with C23 support should work, including Clang 19+ (recommended) or GCC 15+.
MSVC is currently not supported, as it doesn't support C23 yet.

# why yet another game engine?

It's fun to make and very cool to have your own game engine B)
