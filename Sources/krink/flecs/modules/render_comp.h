#pragma once
#include <krink/flecs/flecs.h>
#include <krink/graphics2/ttf.h>
#include <krink/image.h>
#include <krink/math/vector.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Component types */
typedef struct kr_comp_drawable {
	int depth;
	bool shown;
} kr_comp_drawable_t;

typedef uint32_t kr_comp_color_t;

typedef struct kr_comp_image {
	kr_image_t *image;
	float sx, sy, sw, sh, dx, dy, dw, dh;
} kr_comp_image_t;

typedef struct kr_comp_text {
	const char *text;
	kr_ttf_font_t *font;
	float size;
} kr_comp_text_t;

typedef float kr_comp_stroke_t;

typedef bool kr_comp_fill_t;

typedef struct kr_comp_triangle {
    float x1, y1, x2, y2, x3, y3;
} kr_comp_triangle_t;

typedef struct kr_comp_rect {
    float x, y, w, h;
} kr_comp_triangle_t, kr_comp_scissor_t;

typedef float kr_comp_corner_t;

typedef struct kr_comp_corner_asymm {
    float tl, bl, tr, br;
} kr_comp_corner_asymm_t;

typedef struct kr_comp_circle {
    float x, y, radius;
} kr_comp_triangle_t;

typedef struct kr_comp_border {
    float strength;
    uint32_t color;
} kr_comp_border_t;

typedef struct kr_comp_line {
    float x1, y1, x2, y2;
} kr_comp_line_t;

/* This type is used to store handles to everything that the module contains.
 * When the module is loaded, this type will be registered as component, and
 * added to the singleton entity so applications can access module handles from
 * anywhere. */
typedef struct RenderCompModule {
	ECS_DECLARE_COMPONENT(kr_comp_drawable_t);
	ECS_DECLARE_ENTITY(Move);
} SimpleModule;

/* This is the function that implements the module loader. It is automatically
 * invoked by the ECS_IMPORT macro. */
void SimpleModuleImport(ecs_world_t *world);

/* This macro is used to declare variables that contain the handles inside the
 * module. It is invoked by the ECS_IMPORT macro to declare variables in the
 * scope where the macro is invoked so the contents of the module can be used
 * after the ECS_IMPORT macro. */
#define SimpleModuleImportHandles(handles)                                                         \
	ECS_IMPORT_COMPONENT(handles, Position);                                                       \
	ECS_IMPORT_COMPONENT(handles, Velocity);                                                       \
	ECS_IMPORT_ENTITY(handles, Move);

#ifdef __cplusplus
}
#endif