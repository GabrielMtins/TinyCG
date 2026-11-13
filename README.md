# TinyCG

TinyCG is an utility library for Computer Graphics. It is written in C89 (ANSI
C) without dependencies and distributed under the zlib license.

TinyCG is a single-header library. To use it in your project, you must do this
in at least one source file:

```c
#define TINYCG_IMPLEMENTATION
#include "TinyCG.h"
```

# Usage

A typical usage of TinyCG:

```c
typedef struct {
    float x, y, z;
    float r, g, b;
    float u, v;
} Vertex;

TCG_Data data = TCG_BaseData(TCG_TRIANGLE, sizeof(Vertex) / sizeof(float), 0);
TCG_Mat4 projection;

TCG_Perspective(projection, TCG_Radians(45.0f), 1.77f, 0.1f, 10.0f);

copy_vertex_buffer_to(data.raw);
data.w = create_float_buffer();
data.len = // setup len
data.capacity = // setup capacity

TCG_ResetW(&data);

TCG_Transform(&data, projection);
TCG_SmartClipping(&data);
TCG_ApplyProjection(&data);
TCG_ScaleToScreenSize(&data, 1280, 720);
TCG_CullFaces(&data, TCG_BACKFACE);
TCG_SortByDepth(&data);

render_to_screen(&data); 
```
