# TinyCG

TinyCG is a small, header-only C89 library with utilities for computer
graphics. It works on a generic vertex buffer, transforms points by a matrix,
clips lines and triangles against the view frustum, applies the perspective
divide and maps the result to screen space.

The library is designed so that it does not need to know the format of your
vertices. You describe your vertex type with a `stride` and a
`position_offset`, and the library handles the rest. This way, it works with
any vertex layout, such as a position-only vertex or a position + texture
coordinate vertex.

The library has no dependencies.

TinyCG is distributed under the **zlib license**.

# Features

The library supports the following operations:

- Transforming points by a 4x4 matrix in column-major format
- Dumb clipping, which discards polygons that are not fully inside the frustum
- Smart clipping, which properly splits lines and triangles at the frustum
  planes
- Perspective divide and screen-space mapping
- Perspective matrix generation

It compiles in C89 without warnings on **GCC** and **Clang**.

# Usage

In exactly one source file, define `TINYCG_IMPLEMENTATION` before including the
header:

```c
#define TINYCG_IMPLEMENTATION
#include "TinyCG.h"
```

The library never allocates memory itself. The `raw` and `w` buffers are owned
by the user. When the library needs more room, it calls the `increaseCapacity`
callback stored in the `TCG_Data`.

# Data layout

Because the library doesn't know what type of data it is handling, you need to
set the `stride` (the number of floats that your type has) and the
`position_offset` (the offset of the position `vec3` in number of floats
relative to the start of the type).

For example, if you have:

```c
typedef struct {
    float x, y, z;
    float u, v;
} Vertex;
```

Then:

- `position_offset = 0` — the position `vec3` starts at 0
- `stride = 5` — the type has five floats

The `w` buffer holds one float per point and is used during clipping and the
perspective divide. The `w` of the input points should be set to `1.0f` before
the transform.

The `polygon_type` is the number of points per polygon: `2` for lines, `3` for
triangles, etc.

# Clipping

`TCG_DumbClipping` keeps only the polygons that are fully inside the view
frustum. It is fast, but it may discard polygons that are partially visible.
For this function, it is recommended for the output capacity to be greater than
or equal to the input length, otherwise `increaseCapacity()` will be called.

`TCG_SmartClipping` properly clips lines and triangles by intersecting and
splitting them with the view frustum planes. For other types of polygons
(`polygon_type > 3`), the function just calls `TCG_DumbClipping` instead.

The output and input must point to different objects.

# Matrices

`TCG_Transform` expects the matrix to be in column-major format, which is the
format produced by `TCG_Perspective`.
