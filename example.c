#include <stdio.h>
#include <stdlib.h>

#include <SDL3/SDL.h>

#define TINYCG_USE_STDLIB
#define TINYCG_IMPLEMENTATION
#include "TinyCG.h"

typedef struct Vertex {
	float x, y, z;
} Vertex;

static const Vertex cube_vertices[] = {
	{-0.5f, -0.5f,  0.5f}, 
	{ 0.5f, -0.5f,  0.5f}, 
	{ 0.5f,  0.5f,  0.5f}, 
	{-0.5f, -0.5f,  0.5f}, 
	{ 0.5f,  0.5f,  0.5f}, 
	{-0.5f,  0.5f,  0.5f}, 
	{ 0.5f, -0.5f, -0.5f}, 
	{-0.5f, -0.5f, -0.5f}, 
	{-0.5f,  0.5f, -0.5f}, 
	{ 0.5f, -0.5f, -0.5f}, 
	{-0.5f,  0.5f, -0.5f}, 
	{ 0.5f,  0.5f, -0.5f}, 
	{ 0.5f, -0.5f,  0.5f}, 
	{ 0.5f, -0.5f, -0.5f}, 
	{ 0.5f,  0.5f, -0.5f}, 
	{ 0.5f, -0.5f,  0.5f}, 
	{ 0.5f,  0.5f, -0.5f}, 
	{ 0.5f,  0.5f,  0.5f}, 
	{-0.5f, -0.5f, -0.5f}, 
	{-0.5f, -0.5f,  0.5f}, 
	{-0.5f,  0.5f,  0.5f}, 
	{-0.5f, -0.5f, -0.5f}, 
	{-0.5f,  0.5f,  0.5f}, 
	{-0.5f,  0.5f, -0.5f}, 
	{-0.5f,  0.5f, -0.5f}, 
	{-0.5f,  0.5f,  0.5f}, 
	{ 0.5f,  0.5f,  0.5f}, 
	{-0.5f,  0.5f, -0.5f}, 
	{ 0.5f,  0.5f,  0.5f}, 
	{ 0.5f,  0.5f, -0.5f}, 
	{-0.5f, -0.5f, -0.5f}, 
	{ 0.5f, -0.5f, -0.5f}, 
	{ 0.5f, -0.5f,  0.5f}, 
	{-0.5f, -0.5f, -0.5f}, 
	{ 0.5f, -0.5f,  0.5f}, 
	{-0.5f, -0.5f,  0.5f}, 
};

static SDL_Window *window;
static SDL_Renderer *renderer;
static TCG_Data data;
static float rotation = 0.0f;

void render(void) {
	TCG_Mat4 res, op;
	TCG_Vec3 pos = {0.0f, 0.0f, -2.0f};
	TCG_Vec3 scale = {0.5f, 0.5f, 0.5f};
	unsigned int i = 0;

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

	TCG_AppendData(&data, cube_vertices, sizeof(cube_vertices) / sizeof(Vertex));
	TCG_Transform(&data, res);
	/* Clipping must be done before dividing the points by w. */
	TCG_SmartClipping(&data);
	TCG_ApplyProjection(&data);
	TCG_ScaleToScreenSize(&data, 1280, 720);
	/* culling can be only applied after the points are already in screen
	 * coordinates. */
	TCG_CullFaces(&data, TCG_BACKFACE);
	/* sorting is optional; you only really need it if you are going to render
	 * the data using painter's algorithm. */
	TCG_SortByDepth(&data);

	SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);

	for(i = 0; i < data.len; i += 3) {
		Vertex *vertices = ((Vertex *) data.raw) + i;

		SDL_RenderLine(
				renderer,
				vertices[0].x,
				vertices[0].y,
				vertices[1].x,
				vertices[1].y
				);

		SDL_RenderLine(
				renderer,
				vertices[2].x,
				vertices[2].y,
				vertices[1].x,
				vertices[1].y
				);

		SDL_RenderLine(
				renderer,
				vertices[2].x,
				vertices[2].y,
				vertices[0].x,
				vertices[0].y
				);
	}
}

int main(void) {
	SDL_Event event;
	int running = 1;

	SDL_CreateWindowAndRenderer("Example", 1280, 720, 0, &window, &renderer);
	SDL_SetRenderVSync(renderer, 1);

	data = TCG_BaseData(
			TCG_TRIANGLE,
			sizeof(Vertex) / sizeof(float),
			0
			);

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
}
