/*
 * Compile with:
 * cc example2.c -o main -lSDL3
 */

#include <stdio.h>
#include <stdlib.h>

#include <SDL3/SDL.h>

#define TINYCG_USE_STDLIB
#define TINYCG_IMPLEMENTATION
#include "TinyCG.h"

typedef struct Vertex {
	float x, y, z;
	float r, g, b;
	float u, v;
} Vertex;

static const Vertex cube_vertices[] = {
	{ -0.5f, -0.5f,  0.5f, 0.4f, 0.4f, 0.4f, 0.0f, 0.0f,},
	{  0.5f, -0.5f,  0.5f, 0.4f, 0.4f, 0.4f, 1.0f, 0.0f,},
	{  0.5f,  0.5f,  0.5f, 0.4f, 0.4f, 0.4f, 1.0f, 1.0f,},
	{ -0.5f,  0.5f,  0.5f, 0.4f, 0.4f, 0.4f, 0.0f, 1.0f,},

	{  0.5f, -0.5f, -0.5f, 0.4f, 0.4f, 0.4f, 0.0f, 0.0f,},
	{ -0.5f, -0.5f, -0.5f, 0.4f, 0.4f, 0.4f, 1.0f, 0.0f,},
	{ -0.5f,  0.5f, -0.5f, 0.4f, 0.4f, 0.4f, 1.0f, 1.0f,},
	{  0.5f,  0.5f, -0.5f, 0.4f, 0.4f, 0.4f, 0.0f, 1.0f,},

	{  0.5f, -0.5f,  0.5f, 0.7f, 0.7f, 0.7f, 0.0f, 0.0f,},
	{  0.5f, -0.5f, -0.5f, 0.7f, 0.7f, 0.7f, 1.0f, 0.0f,},
	{  0.5f,  0.5f, -0.5f, 0.7f, 0.7f, 0.7f, 1.0f, 1.0f,},
	{  0.5f,  0.5f,  0.5f, 0.7f, 0.7f, 0.7f, 0.0f, 1.0f,},

	{ -0.5f, -0.5f, -0.5f, 0.7f, 0.7f, 0.7f, 0.0f, 0.0f,},
	{ -0.5f, -0.5f,  0.5f, 0.7f, 0.7f, 0.7f, 1.0f, 0.0f,},
	{ -0.5f,  0.5f,  0.5f, 0.7f, 0.7f, 0.7f, 1.0f, 1.0f,},
	{ -0.5f,  0.5f, -0.5f, 0.7f, 0.7f, 0.7f, 0.0f, 1.0f,},

	{ -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,},
	{ -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,},
	{  0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,},
	{  0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,},

	{ -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,},
	{  0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,},
	{  0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,},
	{ -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,},
};

static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Texture *my_texture;
static SDL_Surface *surface_data;
static TCG_Data data;
static float rotation = 0.0f;

void get_pixel_color(float u, float v, float *r, float *g, float *b) {
	if(u < 0.1f || v < 0.1f || u > 0.9f || v > 0.9f) {
		*r = 1.0f;
		*g = 0.0f;
		*b = 0.0f;
		return;
	}

	if(u + v < 1.05f && u + v > 0.95f) {
		*r = 1.0f;
		*g = 0.0f;
		*b = 0.0f;
		return;
	}

	if(u - v > -0.05f && u - v < 0.05f) {
		*r = 1.0f;
		*g = 0.0f;
		*b = 0.0f;
		return;
	}

	*r = 1.0f;
	*g = 1.0f;
	*b = 1.0f;
}

void put_pixel(void *userdata, const void *vertex_ptr) {
	uint32_t *pixels = surface_data->pixels;
	const Vertex *v = vertex_ptr;
	int x = v->x;
	int y = v->y;
	float r, g, b;
	(void) userdata;

	get_pixel_color(v->u, v->v, &r, &g, &b);

	r *= v->r;
	g *= v->g;
	b *= v->b;

	pixels[x + y * surface_data->pitch / 4] = SDL_MapSurfaceRGB(
			surface_data,
			r * 255,
			g * 255,
			b * 255
			);
}

void render(void) {
	TCG_Mat4 res, op;
	TCG_Vec3 pos = {0.0f, 0.0f, -1.5f};
	TCG_Vec3 scale = {0.5f, 0.5f, 0.5f};

	TCG_Mat4Identity(res);

	TCG_Mat4Scale(op, scale);
	TCG_Mat4Mul(op, res);

	TCG_Mat4RotateY(op, rotation);
	TCG_Mat4Mul(op, res);

	TCG_Mat4RotateX(op, rotation * 2.0f);
	TCG_Mat4Mul(op, res);

	TCG_Mat4Translate(op, pos);
	TCG_Mat4Mul(op, res);

	TCG_Mat4Perspective(op, TCG_Radians(45.0f), 1.77f, 0.01f, 10.0f);
	TCG_Mat4Mul(op, res);

	data.len = 0;
	data.polygon_type = TCG_QUAD;

	TCG_AppendData(&data, cube_vertices, sizeof(cube_vertices) / sizeof(Vertex));
	TCG_Transform(&data, res);
	/* Triangulate faces before calling smart clipping */
	TCG_Triangulate(&data);
	/* Clipping must be done before dividing the points by w. */
	TCG_SmartClipping(&data);
	TCG_ApplyProjection(&data);
	TCG_ScaleToScreenSize(&data, 640, 360);
	/* culling can be only applied after the points are already in screen
	 * coordinates. */
	TCG_CullFaces(&data, TCG_BACKFACE);
	/* sorting is optional; you only really need it if you are going to render
	 * the data using painter's algorithm. */
	TCG_SortByDepth(&data);

	SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);

	SDL_LockTextureToSurface(my_texture, NULL, &surface_data);
	SDL_ClearSurface(surface_data, 0x00, 0x00, 0x00, 0xff);
	TCG_RenderPerspective(&data);
	SDL_UnlockTexture(my_texture);

	SDL_RenderTexture(renderer, my_texture, NULL, NULL);
}

int main(void) {
	SDL_Event event;
	int running = 1;

	SDL_CreateWindowAndRenderer("Example", 1280, 720, 0, &window, &renderer);
	SDL_SetRenderVSync(renderer, 1);

	data = TCG_BaseData(
			4,
			sizeof(Vertex) / sizeof(float),
			0
			);

	my_texture = SDL_CreateTexture(
			renderer,
			SDL_PIXELFORMAT_RGBA32,
			SDL_TEXTUREACCESS_STREAMING,
			640,
			360
			);

	data.putPixel = put_pixel;

	while(running) {
		while(SDL_PollEvent(&event)) {
			if(event.type == SDL_EVENT_QUIT) running = 0;
		}

		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
		SDL_RenderClear(renderer);

		render();

		rotation += 0.001f;

		SDL_RenderPresent(renderer);
	}

	return 0;
}
