# TinyCG

TinyCG is an utility library for Computer Graphics. It is written in C89 (ANSI
C) without dependencies and distributed under the zlib license.

TinyCG is a single-header library. To use it in your project, you must do this
in at least one source file:

```c
#define TINYCG_IMPLEMENTATION
#include "TinyCG.h"
```

By default, TinyCG does not have any dependencies. This means that you need to
provide your own custom allocators (see `TCG_Data.increaseCapacity` in
`TinyCG.h`). If you want to use a default allocator that is based on `malloc`,
you can define `TINYCG_USE_STDLIB` before `TINYCG_IMPLEMENTATION`. To add new
data, you can just use `TCG_AppendData`.

# Features

- Clipping for lines and triangles with the view frustum.
- Face culling.
- Basic matrix transformations.
- Sorting triangles by depth.

# Usage

A typical usage of TinyCG:

```c
#define TINYCG_USE_STDLIB
#define TINYCG_IMPLEMENTATION
#include "TinyCG.h"

typedef struct {
    Vec3 pos;
    Vec3 color;
    Vec2 uv;
} Vertex;

extern const Vertex *vertex_data;
extern size_t vertex_data_size;

int main(void) {
    TCG_Data data = TCG_BaseData(
                        TCG_TRIANGLE, 
                        sizeof(Vertex) / sizeof(float), 
                        offsetof(Vertex, pos) / sizeof(float)
                        );
    
    TCG_AppendData(&data, vertex_data, vertex_data_size);
    
    TCG_Mat4 projection;
    
    TCG_Perspective(projection, TCG_Radians(45.0f), 1.77f, 0.1f, 10.0f);
    
    TCG_Transform(&data, projection);
    TCG_SmartClipping(&data);
    TCG_ApplyProjection(&data);
    TCG_ScaleToScreenSize(&data, 1280, 720);
    TCG_CullFaces(&data, TCG_BACKFACE);
    TCG_SortByDepth(&data);
    
    render_to_screen(&data); 

    TCG_FreeData(&data);

    return 0;
}
```
