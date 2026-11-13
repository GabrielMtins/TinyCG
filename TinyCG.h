/** LICENSE
 * Copyright (c) 2026 Gabriel Martins
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from the
 * use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not claim
 * that you wrote the original software. If you use this software in a product,
 * an acknowledgment in the product documentation would be appreciated but is
 * not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

/**
 * 1. Introduction
 *
 * TinyCG is an utility library for Computer Graphics. It is written in
 * C89 (ANSI C) without dependencies and distributed under the zlib license.
 *
 * TinyCG is a single-header library. To use it in your project, you must do
 * this in at least one source file:
 *
 * #define TINYCG_IMPLEMENTATION
 * #include "TinyCG.h"
 */

/**
 * 2. Usage
 *
 * A typical usage of TinyCG:
 * 
 * typedef struct {
 *     float x, y, z;
 *     float r, g, b;
 *     float u, v;
 * } Vertex;
 *
 * TCG_Data data = TCG_BaseData(TCG_TRIANGLE, sizeof(Vertex) / sizeof(float), 0);
 * TCG_Mat4 projection;
 *
 * TCG_Perspective(projection, TCG_Radians(45.0f), 1.77f, 0.1f, 10.0f);
 *
 * copy_vertex_buffer_to(data.raw);
 * data.w = create_float_buffer();
 * data.len = // setup len
 * data.capacity = // setup capacity
 *
 * TCG_ResetW(&data);
 *
 * TCG_Transform(&data, projection);
 * TCG_SmartClipping(&data);
 * TCG_ApplyProjection(&data);
 * TCG_ScaleToScreenSize(&data, 1280, 720);
 * TCG_CullFaces(&data, TCG_BACKFACE);
 * TCG_SortByDepth(&data);
 * 
 * render_to_screen(&data); 
 *
 * The (x, y) coordinates of each vertex are in screen coordinates and you can
 * render to your screen using your own render functions.
 */

/**
 * TODO
 * - Add a quicksort function.
 * - Add a rasterization function.
 */

#ifndef TINYCG_H
#define TINYCG_H

#ifdef __cplusplus
extern "C" {
#endif

#define TCG_NULL (0)
#define TCG_BACKFACE 0
#define TCG_FRONTFACE 1

#define TCG_TRIANGLE 3
#define TCG_LINE 2

typedef unsigned int TCG_Uint;
typedef float TCG_Mat4[16];
typedef float TCG_Vec3[3];

typedef struct TCG_Data {
	/**
	 * raw and w are dynamic vectors that are managed by the user.
	 */
	float *raw;
	float *w;

	void *userdata;

	/**
    * If capacity > len, a function can call increaseCapacity() to be able
    * to handle more data. The first argument is the struct that called the
    * function and the second argument is the expected capacity increase.
    */
	int (*increaseCapacity)(struct TCG_Data *, TCG_Uint);

	void (*putPixel)(void *, const void *);

	/**
	 * For line polygon_type = 2, for triangle polygon_type = 3 etc.
	 */
	TCG_Uint polygon_type;

	/**
	 * Since the library doesn't know what type of data it is handling,
	 * you need to set the stride (the number of floats that your type has)
	 * and the offset of the position vec3 relative to the start of the type.
	 *
	 * For example, if you have:
	 *
	 * typedef struct {
	 *     float x, y, z;
	 *     float u, v;
	 * } Vertex;
	 *
	 * Then:
	 * position_offset = 0 // the position vec3 starts at 0
	 * stride = 5 // the type has five floats
	 * */
	TCG_Uint position_offset;

	/**
	 * The number of floats that your data type has.
	 */
	TCG_Uint stride;

	TCG_Uint len;
	TCG_Uint capacity;
} TCG_Data;

/* ==== PUBLIC API DECLARATION BEGIN ==== */

TCG_Data TCG_BaseData(TCG_Uint polygon_type, TCG_Uint stride, TCG_Uint position_offset);

/**
 * Transforms the data by using the given matrix. The matrix must be in the
 * column-major format.
 */
void TCG_Transform(const TCG_Data *data, const TCG_Mat4 m);

/**
 * Transforms the data by using the given matrix in a given interval. The matrix
 * must be in the column-major format.
 */
void TCG_TransformPart(const TCG_Data *data, const TCG_Mat4 m, TCG_Uint start_point, TCG_Uint len);

/**
 * Resets homogenous coordinates w to 1.0f.
 */
void TCG_ResetW(const TCG_Data *data);

/**
 * Clips a polygon if it contains at least a point outside the view frustum.
 */
int TCG_DumbClipping(TCG_Data *data);

/**
 * Clips lines or triangles properly by intersecting and splitting them with the
 * view frustum planes. For other types of polygons, the
 * function just calls TCG_DumbClipping instead.
 */
void TCG_SmartClipping(TCG_Data *data);

/**
 * Cull front or back faces. Only works with triangles.
 * flags can be TCG_FRONTFACE or TCG_BACKFACE
 * This function can be called only after the projection was applied
 * to the vertices and it was scaled to the screen space.
 */
void TCG_CullFaces(TCG_Data *data, int flags);

/**
 * Divides every point by its w.
 */
void TCG_ApplyProjection(const TCG_Data *data);

/**
 * Transforms the normalized space to screen space.
 * The convention is that (0, 0) is on the top left of the screen, so
 * it inverts the Y-axis.
 */
void TCG_ScaleToScreenSize(const TCG_Data *data, TCG_Uint w, TCG_Uint h);

float TCG_Radians(float x);

/**
 * Computes a perspective matrix with the given values.
 */
void TCG_Perspective(TCG_Mat4 m, float fov_y, float ratio, float near, float far);

int TCG_ComparePolygonsDepth(const TCG_Data *data, TCG_Uint p1, TCG_Uint p2);

/**
 * Sorts polygon by depth (farthest to closest).
 * Intended for be used when rendering with painter's algorithm.
 * It works with any type of polygon.
 */
void TCG_SortByDepth(TCG_Data *data);

#ifdef __cplusplus
}
#endif

#endif /* TINYCG_H */

#ifdef TINYCG_IMPLEMENTATION 

#define vec3_copy(res, a) do{res[0]=a[0];res[1]=a[1];res[2]=a[2];} while(0)
#define vec4_copy(res, a) do{res[0]=a[0];res[1]=a[1];res[2]=a[2];res[3]=a[3];} while(0)
#define vec4_dot(a, b) (a[0]*b[0]+a[1]*b[1]+a[2]*b[2]+a[3]*b[3])
#define vec4_sub(res, a, b) do{res[0]=a[0]-b[0];res[1]=a[1]-b[1];res[2]=a[2]-b[2];res[3]=a[3]-b[3];} while(0)

#define TCG_PushPolygonToOutput(dst, src, i) TCG_PushPointsToOutput(dst, src, i, (dst)->polygon_type)

#define TCG_Swap(type, a, b) \
	do { \
		type __TMP_SWAP_VAR = a; \
		a = b; \
		b = __TMP_SWAP_VAR; \
	} while(0)

#define TCG_CLIPRES_NOCUT -2
#define TCG_CLIPRES_OUTSIDE -1
#define TCG_CLIPRES_CUT1 0
#define TCG_CLIPRES_CUT2 1

#define PI      3.1415926535f
#define HALF_PI 1.5707963267f
#define TAU     6.2831853071f

#define COS_TAYLOR_C1 0.5f
#define COS_TAYLOR_C2 0.0416666666f
#define COS_TAYLOR_C3 0.0013888889f
#define COS_TAYLOR_C4 0.0000248015f

#define COSF TCG_Cos
#define SINF TCG_Sin
#define TANF TCG_Tan

#define ABSF(a) ((a) > 0.0f ? (a) : (-(a)))
#define MAXF(a, b) ((a) > (b) ? (a) : (b))
#define MINF(a, b) ((a) < (b) ? (a) : (b))

typedef float TCG_Vec4[4];

/* vec3 reference */
typedef float * vec3r;
typedef float * vec4r;

typedef TCG_Uint uint;
typedef TCG_Vec3 vec3;
typedef TCG_Vec4 vec4;

typedef struct TCG_ClipLineRes {
	int return_state;
	float interpolation;
} TCG_ClipLineRes;

typedef struct TCG_ClipList {
	uint start;
	uint len;
} TCG_ClipList;

typedef struct TCG_ClipContext {
	TCG_Data *data;

	TCG_ClipList output_list;
	TCG_ClipList input_list;

	vec4 normal;
	uint working_index;
} TCG_ClipContext;

static const vec4 frustum_planes[6] = {
	{1.0f, 0.0f, 0.0f, 1.0f},
	{0.0f, 1.0f, 0.0f, 1.0f},
	{0.0f, 0.0f, 1.0f, 1.0f},
	{-1.0f, 0.0f, 0.0f, 1.0f},
	{0.0f, -1.0f, 0.0f, 1.0f},
	{0.0f, 0.0f, -1.0f, 1.0f},
};

/* ==== PRIVATE FUNCTIONS DECLARATION BEGIN ==== */

/* Math functions */
static float TCG_Cos(float x);
#define TCG_Sin(x) TCG_Cos((x) - HALF_PI)
#define TCG_Tan(x) (TCG_Sin(x) / TCG_Cos(x))

/* Copying/pushing functions */
static void TCG_CopyFloatN(float *dst, const float *src, uint n);
static void TCG_CopyPointsN(const TCG_Data *output, uint out_i, const TCG_Data *input, uint in_i, uint n);
static void TCG_PushPointsToOutput(TCG_Data *output, const TCG_Data *input, uint index, uint n);
static void TCG_RemovePolygon(TCG_Data *data, uint index);

/* Data manipulation */
static vec3r TCG_GetPoint(const TCG_Data *data, uint i);
static void TCG_GetVec4(const TCG_Data *data, uint i, vec4 p);
static void TCG_LerpPoints(const TCG_Data *data, uint res, uint from, uint to, float u);

/* Visibility */
static int TCG_IsPointVisible(const TCG_Data *data, uint point_index);
static int TCG_IsPolygonFullyVisible(const TCG_Data *data, uint poly_start);

/* Transforming points */
static void TCG_ApplyTransformOnPoint(const TCG_Data *data, uint point_index, const TCG_Mat4 m);
static TCG_ClipLineRes TCG_ClipLine(const vec4 p1, const vec4 p2, const vec4 normal_plane);

/* Clipping triangles */
static TCG_ClipContext TCG_CreateClipContext(TCG_Data *data, uint poly_start);
static void TCG_PushToOutputClipContext(TCG_ClipContext *ctx, uint index);
static void TCG_ClipOnClipContext(TCG_ClipContext *ctx);
static void TCG_MakeTriClipContext(TCG_ClipContext *ctx);

/* General clipping */
static uint TCG_ClipPoly2(const TCG_Data *data, uint poly_start);
static uint TCG_ClipPoly3(TCG_Data *data, uint poly_start);

static void TCG_SmartClipping2(TCG_Data *data);
static void TCG_SmartClipping3(TCG_Data *data);

/* Culling */
static int TCG_IsTriangleFrontFacing(const TCG_Data *data, uint poly_start);

/* Sorting */
static void TCG_InsertionSort(const TCG_Data *data, uint start, uint end);

static float TCG_Cos(float x) {
	float n, x2, sign = 1.0f;

	if(x < 0.0f) x = -x;

	n = (int) (x / TAU);

	x = x - n * TAU;

	if(x > PI) x = TAU - x;

	if(x > HALF_PI) {
		x = PI - x;
		sign = -1.0f;
	}

	x2 = x * x;

	return sign * (
		1 - x2 * (
		COS_TAYLOR_C1 - x2 * (
		COS_TAYLOR_C2 - x2 * (
		COS_TAYLOR_C3 - x2 * (
		COS_TAYLOR_C4)))));
}

static void TCG_CopyFloatN(float *dst, const float *src, uint n) {
	while(n-- != 0) {
		*(dst++) = *(src++);
	}
}

static void TCG_CopyPointsN(const TCG_Data *output, uint out_i, const TCG_Data *input, uint in_i, uint n) {
	TCG_CopyFloatN(
			output->raw + out_i * output->stride,
			input->raw + in_i * input->stride,
			n * input->stride
			);

	TCG_CopyFloatN(
			output->w + out_i,
			input->w + in_i,
			n
			);
}

static void TCG_PushPointsToOutput(TCG_Data *output, const TCG_Data *input, uint index, uint n) {
	if(output->capacity - output->len < n) {
		output->increaseCapacity(output, n);
	}

	TCG_CopyPointsN(
			output,
			output->len,
			input,
			index,
			n
			);

	output->len += n;
}

static void TCG_RemovePolygon(TCG_Data *data, uint index) {
	uint n = data->polygon_type;

	if(index + n == data->len) {
		data->len -= n;
		return;
	}

	TCG_CopyPointsN(
			data,
			index,
			data,
			data->len - n,
			n
			);

	data->len -= n;
}

static vec3r TCG_GetPoint(const TCG_Data *data, uint i) {
	return (vec3r) (
			data->raw + i * data->stride + data->position_offset
			);
}

static void TCG_GetVec4(const TCG_Data *data, uint i, vec4 p) {
	vec3r v3 = TCG_GetPoint(data, i);

	vec3_copy(p, v3);
	p[3] = data->w[i];
}

static void TCG_LerpPoints(const TCG_Data *data, uint res, uint from, uint to, float u) {
	uint i;

	data->w[res] = data->w[from] * (1.0f - u) + data->w[to] * u;

	res *= data->stride;
	from *= data->stride;
	to *= data->stride;

	for(i = 0; i < data->stride; i++) {
		data->raw[res + i] = 
			data->raw[from + i] * (1.0f - u) + 
			data->raw[to + i] * u;
	}
}

static int TCG_IsPointVisible(const TCG_Data *data, uint point_index) {
	vec3r point;
	float w;

	point = TCG_GetPoint(data, point_index);
	w = data->w[point_index];

	if(point[0] > w || point[0] < -w) return 0;
	if(point[1] > w || point[1] < -w) return 0;
	if(point[2] > w || point[2] < -w) return 0;

	return 1;
}

static int TCG_IsPolygonFullyVisible(const TCG_Data *data, uint poly_start) {
	uint i, n;

	n = data->polygon_type;

	for(i = 0; i < n; i++) {
		if(!TCG_IsPointVisible(data, poly_start + i)) {
			return 0;
		}
	}

	return 1;
}

static void TCG_ApplyTransformOnPoint(const TCG_Data *data, uint point_index, const TCG_Mat4 m) {
	vec3r point = TCG_GetPoint(data, point_index);
	float w = data->w[point_index];
	float res[4];
	int i;

	for(i = 0; i < 4; i++) {
		res[i] = 
			m[i+0]  * point[0] +
			m[i+4]  * point[1] +
			m[i+8]  * point[2] +
			m[i+12] * w;
	}

	vec3_copy(point, res);
	data->w[point_index] = res[3];
}

static TCG_ClipLineRes TCG_ClipLine(const vec4 p1, const vec4 p2, const vec4 normal_plane) {
	float d1, d2;
	vec4 diff;
	TCG_ClipLineRes pair;

	pair.interpolation = 0.0f;

	d1 = vec4_dot(p1, normal_plane);
	d2 = vec4_dot(p2, normal_plane);

	vec4_sub(diff, p2, p1);

	if(d1 >= 0.0f && d2 >= 0.0f) {
		pair.return_state = TCG_CLIPRES_NOCUT;
		return pair;
	}

	if(d1 < 0.0f && d2 < 0.0f) {
		pair.return_state = TCG_CLIPRES_OUTSIDE;
		return pair;
	}

	pair.interpolation = -vec4_dot(normal_plane, p1) / vec4_dot(normal_plane, diff);

	if(d1 < 0.0f) {
		pair.return_state = TCG_CLIPRES_CUT1;
	}

	if(d2 < 0.0f) {
		pair.return_state = TCG_CLIPRES_CUT2;
	}

	return pair;
}

static TCG_ClipContext TCG_CreateClipContext(TCG_Data *data, uint poly_start) {
	TCG_ClipContext ctx;
	uint stack_size = 40;
	uint sp;

	if(data->capacity - data->len < stack_size) {
		data->increaseCapacity(data, stack_size);
	}

	sp = data->capacity;

	ctx.data = data;

	ctx.working_index = --sp;

	sp -= 9;
	ctx.input_list.len = 0;
	ctx.input_list.start = sp;

	sp -= 9;
	ctx.output_list.len = 3;
	ctx.output_list.start = sp;

	TCG_CopyPointsN(
			data,
			ctx.output_list.start,
			data,
			poly_start,
			3
			);

	return ctx;
}

static void TCG_PushToOutputClipContext(TCG_ClipContext *ctx, uint index) {
	TCG_CopyPointsN(
			ctx->data,
			ctx->output_list.start + ctx->output_list.len++,
			ctx->data,
			index,
			1
			);
}

static void TCG_ClipOnClipContext(TCG_ClipContext *ctx) {
	uint i;
	uint prev_index, curr_index;
	vec4 prev_point, curr_point;
	TCG_ClipLineRes res;

	for(i = 0; i < ctx->input_list.len; i++) {
		curr_index = ctx->input_list.start + i;
		prev_index = curr_index - 1;
		if(i == 0) prev_index += ctx->input_list.len;

		TCG_GetVec4(ctx->data, prev_index, prev_point);
		TCG_GetVec4(ctx->data, curr_index, curr_point);

		res = TCG_ClipLine(prev_point, curr_point, ctx->normal);

		TCG_LerpPoints(ctx->data, ctx->working_index, prev_index, curr_index, res.interpolation);

		switch(res.return_state) {
			case TCG_CLIPRES_OUTSIDE:
				break;

			case TCG_CLIPRES_CUT1:
				TCG_PushToOutputClipContext(ctx, ctx->working_index);
				/* fallthrough */
			case TCG_CLIPRES_NOCUT:
				TCG_PushToOutputClipContext(ctx, curr_index);
				break;

			case TCG_CLIPRES_CUT2:
				TCG_PushToOutputClipContext(ctx, ctx->working_index);
				break;
		}
	}
}

static void TCG_MakeTriClipContext(TCG_ClipContext *ctx) {
	uint i = 0;
	uint s;

	s = ctx->output_list.start;

	if(ctx->output_list.len == 0) {
		return;
	}

	for(i = 1; i < ctx->output_list.len - 1; i++) {
		TCG_PushPointsToOutput(
				ctx->data,
				ctx->data,
				s,
				1
				);

		TCG_PushPointsToOutput(
				ctx->data,
				ctx->data,
				s + i,
				2
				);
	}
}

static uint TCG_ClipPoly2(const TCG_Data *data, uint poly_start) {
	vec4 p1, p2;
	uint i;
	TCG_ClipLineRes res;

	for(i = 0; i < 6; i++) {
		TCG_GetVec4(data, poly_start, p1);
		TCG_GetVec4(data, poly_start + 1, p2);

		res = TCG_ClipLine(p1, p2, frustum_planes[i]);

		if(res.return_state == TCG_CLIPRES_OUTSIDE) {
			return 0;
		} else if(res.return_state == TCG_CLIPRES_NOCUT) {
			continue;
		}

		TCG_LerpPoints(
				data,
				poly_start + res.return_state,
				poly_start,
				poly_start + 1,
				res.interpolation
				);
	}

	return 2;
}

static uint TCG_ClipPoly3(TCG_Data *data, uint poly_start) {
	TCG_ClipContext ctx;
	uint i;

	ctx = TCG_CreateClipContext(data, poly_start);

	for(i = 0; i < 6; i++) {
		vec4_copy(ctx.normal, frustum_planes[i]);
		TCG_Swap(TCG_ClipList, ctx.input_list, ctx.output_list);
		ctx.output_list.len = 0;

		TCG_ClipOnClipContext(&ctx);

		if(ctx.output_list.len == 0) return 0;
	}

	TCG_MakeTriClipContext(&ctx);

	return (ctx.output_list.len - 2) * 3;
}

static void TCG_SmartClipping2(TCG_Data *data) {
	uint i = 0, n = data->polygon_type;

	while(i < data->len) {
		uint added;

		added = TCG_ClipPoly2(data, i);

		if(added > 0) {
			i += n;
			continue;
		}

		TCG_RemovePolygon(data, i);
	}
}

static void TCG_SmartClipping3(TCG_Data *data) {
	uint i = 0, n = data->polygon_type, clipped = 0;

	while(i < data->len - clipped) {
		if(TCG_IsPolygonFullyVisible(data, i)) {
			i += n;
			continue;
		}

		clipped += TCG_ClipPoly3(data, i);

		TCG_RemovePolygon(data, i);

		if(clipped > 0) {
			clipped -= n;
			i += n;
		}
	}
}

static int TCG_IsTriangleFrontFacing(const TCG_Data *data, uint poly_start) {
	vec3r p0, p1, p2;
	float area;

	p0 = TCG_GetPoint(data, poly_start+0);
	p1 = TCG_GetPoint(data, poly_start+1);
	p2 = TCG_GetPoint(data, poly_start+2);

	area = (p1[0] - p0[0]) * (p2[1] - p0[1]) - (p2[0] - p0[0]) * (p1[1] - p0[1]);

	return area < 0.0f;
}

static void TCG_InsertionSort(const TCG_Data *data, uint start, uint end) {
	uint working_index = 0;
	uint i, n;

	n = data->polygon_type;
	working_index = data->capacity - n;

	for(i = start + n; i <= end; i += n) {
		int j;

		TCG_CopyPointsN(data, working_index, data, i, n);
		j = (int) i - (int) n;

		while((j >= 0) && TCG_ComparePolygonsDepth(data, working_index, j) < 0) {
			TCG_CopyPointsN(data, j + n, data, j, n);
			j -= n;
		}

		TCG_CopyPointsN(data, j + n, data, working_index, n);
	}
}

TCG_Data TCG_BaseData(TCG_Uint polygon_type, TCG_Uint stride, TCG_Uint position_offset) {
	TCG_Data data;

	data.raw = TCG_NULL;
	data.w = TCG_NULL;
	data.userdata = TCG_NULL;
	data.increaseCapacity = TCG_NULL;
	data.putPixel = TCG_NULL;

	data.len = 0;
	data.capacity = 0;
	data.stride = stride;
	data.position_offset = position_offset;
	data.polygon_type = polygon_type;

	return data;
}

void TCG_TransformPart(const TCG_Data *data, const TCG_Mat4 m, TCG_Uint start_point, TCG_Uint len) {
	uint i;

	for(i = 0; i < len; i++) {
		TCG_ApplyTransformOnPoint(data, start_point + i, m);
	}
}

void TCG_ResetW(const TCG_Data *data) {
	uint i;

	for(i = 0; i < data->len; i++) {
		data->w[i] = 1.0f;
	}
}

void TCG_Transform(const TCG_Data *data, const TCG_Mat4 m) {
	TCG_TransformPart(data, m, 0, data->len);
}

int TCG_DumbClipping(TCG_Data *data) {
	uint i = 0, n = data->polygon_type;
	
	while(i < data->len) {
		if(TCG_IsPolygonFullyVisible(data, i)) {
			i += n;
			continue;
		}

		TCG_RemovePolygon(data, i);
	}

	return 1;
}

void TCG_SmartClipping(TCG_Data *data) {
	switch(data->polygon_type) {
		default:
			TCG_DumbClipping(data);
			break;

		case 2:
			TCG_SmartClipping2(data);
			break;

		case 3:
			TCG_SmartClipping3(data);
			break;
	}
}

void TCG_CullFaces(TCG_Data *data, int flags) {
	uint i = 0;
	(void) flags;

	if(data->polygon_type != 3) return;

	while(i < data->len) {
		if(TCG_IsTriangleFrontFacing(data, i) != flags) {
			i += 3;
			continue;
		}

		TCG_RemovePolygon(data, i);
	}
}

void TCG_ApplyProjection(const TCG_Data *data) {
	uint i;

	for(i = 0; i < data->len; i++) {
		vec3r p = TCG_GetPoint(data, i);
		float w = data->w[i];

		if(w == 0.0f) w = 1e-6f;

		p[0] /= w;
		p[1] /= w;
		p[2] /= w;
	}
}

void TCG_ScaleToScreenSize(const TCG_Data *data, TCG_Uint w, TCG_Uint h) {
	uint i;

	for(i = 0; i < data->len; i++) {
		vec3r p = TCG_GetPoint(data, i);

		p[0] *= w;
		p[0] += w;
		p[0] /= 2;

		p[1] *= -1.0f;
		p[1] *= h;
		p[1] += h;
		p[1] /= 2;
	}
}

float TCG_Radians(float x) {
	return (x / 360.0f) * TAU;
}

void TCG_Perspective(TCG_Mat4 m, float fov_y, float ratio, float near, float far) {
	int i;
	float f;

	for(i = 0; i < 16; i++) m[i] = 0.0f;

	f = 1.0f / TANF(fov_y / 2);

	m[0] = f / ratio;
	m[5] = f;
	m[10] = (far + near) / (near - far);
	m[11] = -1.0f;
	m[14] = (2.0f * far * near) / (near - far);
}

int TCG_ComparePolygonsDepth(const TCG_Data *data, TCG_Uint p1, TCG_Uint p2) {
	uint i, n = data->polygon_type;
	float average_p1 = 0.0f, average_p2 = 0.0f;

	for(i = 0; i < n; i++) {
		average_p1 += -TCG_GetPoint(data, p1 + i)[2];
		average_p2 += -TCG_GetPoint(data, p2 + i)[2];
	}

	average_p1 /= n;
	average_p2 /= n;

	if(average_p1 > average_p2) {
		return 1;
	}

	return -1;
}

void TCG_SortByDepth(TCG_Data *data) {
	uint n = data->polygon_type;

	if(data->len < n) return;

	if(data->len == data->capacity) {
		data->increaseCapacity(data, n);
	}
	
	TCG_InsertionSort(data, 0, data->len - n);
}

#endif /* TINYCG_IMPLEMENTATION */
