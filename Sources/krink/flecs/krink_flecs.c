#include "krink_flecs.h"
#include "internal/os_api.h"
#include "modules/animation_comp.h"
#include "modules/animation_sys.h"
#include "modules/render_comp.h"
#include "modules/render_sys.h"
#include <kinc/log.h>
#include <stdbool.h>

ecs_world_t *kr_world;

void kr_flecs_init(void) {
	kr_set_flecs_os_api();
	kr_world = ecs_init();
	ecs_singleton_set(kr_world, EcsRest, {0});
	ECS_IMPORT(kr_world, ComponentsRender);
	ECS_IMPORT(kr_world, SystemsRender);
	ECS_IMPORT(kr_world, ComponentsAnimation);
	ECS_IMPORT(kr_world, SystemsAnimation);
	ecs_singleton_set(kr_world, KrSingletonClearColor, {.color = 0});
	kinc_log(KINC_LOG_LEVEL_INFO, "flecs initialized");
}

void kr_flecs_destroy(void) {
	ecs_fini(kr_world);
	kr_world = NULL;
}

void kr_flecs_tick(void) {
	ecs_progress(kr_world, 0);
}

static inline int32_t ptr2sort(void *img) {
	return (int32_t)((uint64_t)img % 214783648);
}

ecs_entity_t kr_flecs_create_sprite(kr_init_sprite_t args) {
	ecs_entity_t e = ecs_new_w_pair(kr_world, EcsIsA, KrPrefabSprite);
	ecs_add(kr_world, e, KrVisible);

	ecs_set(kr_world, e, KrDrawable,
	        {.depth = 0, .sort_extra = ptr2sort((void *)args.image), .pipeline = KR_COMP_PP_IMAGE});
	ecs_set(kr_world, e, KrPos2, {.x = args.dest.x, .y = args.dest.y});
	ecs_set(kr_world, e, KrColor, {.color = args.color});
	ecs_set(kr_world, e, KrImage,
	        {.image = args.image,
	         .sx = args.src.x,
	         .sy = args.src.y,
	         .sw = args.src.w,
	         .sh = args.src.h,
	         .dw = args.dest.w,
	         .dh = args.dest.h});
	return e;
}

ecs_entity_t kr_flecs_create_text(kr_init_text_t args) {
	ecs_entity_t e = ecs_new_w_pair(kr_world, EcsIsA, KrPrefabText);
	ecs_add(kr_world, e, KrVisible);

	ecs_set(kr_world, e, KrDrawable,
	        {.depth = 0, .sort_extra = ptr2sort((void *)args.font), .pipeline = KR_COMP_PP_TEXT});
	ecs_set(kr_world, e, KrPos2, {.x = args.pos.x, .y = args.pos.y});
	ecs_set(kr_world, e, KrColor, {.color = args.color});
	ecs_set(kr_world, e, KrText, {.font = args.font, .text = args.text, .size = args.size});
	return e;
}

ecs_entity_t kr_flecs_create_stroked_rect(kr_init_stroked_rect_t args) {
	ecs_entity_t e = ecs_new_w_pair(kr_world, EcsIsA, KrPrefabStrokedRect);
	ecs_add(kr_world, e, KrVisible);

	ecs_set(kr_world, e, KrDrawable,
	        {.depth = 0, .sort_extra = 0, .pipeline = KR_COMP_PP_COLOR_QUAD_STROKE});
	ecs_set(kr_world, e, KrPos2, {.x = args.rect.x, .y = args.rect.y});
	ecs_set(kr_world, e, KrColor, {.color = args.color});
	ecs_set(kr_world, e, KrRect, {.w = args.rect.w, .h = args.rect.h});
	ecs_set(kr_world, e, KrStroke, {.strength = args.stroke});
	return e;
}

ecs_entity_t kr_flecs_create_filled_rect(kr_init_filled_rect_t args) {
	ecs_entity_t e = ecs_new_w_pair(kr_world, EcsIsA, KrPrefabFilledRect);
	ecs_add(kr_world, e, KrVisible);

	ecs_set(kr_world, e, KrDrawable,
	        {.depth = 0, .sort_extra = 0, .pipeline = KR_COMP_PP_COLOR_QUAD_FILL});
	ecs_set(kr_world, e, KrPos2, {.x = args.rect.x, .y = args.rect.y});
	ecs_set(kr_world, e, KrColor, {.color = args.color});
	ecs_set(kr_world, e, KrRect, {.w = args.rect.w, .h = args.rect.h});
	return e;
}

ecs_entity_t kr_flecs_create_stroked_triangle(kr_init_stroked_triangle_t args) {
	ecs_entity_t e = ecs_new_w_pair(kr_world, EcsIsA, KrPrefabStrokedTriangle);
	ecs_add(kr_world, e, KrVisible);

	ecs_set(kr_world, e, KrDrawable,
	        {.depth = 0, .sort_extra = 0, .pipeline = KR_COMP_PP_COLOR_TRIS_STROKE});
	ecs_set(kr_world, e, KrColor, {.color = args.color});
	ecs_set(
	    kr_world, e, KrTriangle,
	    {.x1 = args.x1, .y1 = args.y1, .x2 = args.x2, .y2 = args.y2, .x3 = args.x3, .y3 = args.y3});
	ecs_set(kr_world, e, KrStroke, {.strength = args.stroke});
	return e;
}

ecs_entity_t kr_flecs_create_filled_triangle(kr_init_filled_triangle_t args) {
	ecs_entity_t e = ecs_new_w_pair(kr_world, EcsIsA, KrPrefabFilledTriangle);
	ecs_add(kr_world, e, KrVisible);

	ecs_set(kr_world, e, KrDrawable,
	        {.depth = 0, .sort_extra = 0, .pipeline = KR_COMP_PP_COLOR_TRIS_FILL});
	ecs_set(kr_world, e, KrColor, {.color = args.color});
	ecs_set(
	    kr_world, e, KrTriangle,
	    {.x1 = args.x1, .y1 = args.y1, .x2 = args.x2, .y2 = args.y2, .x3 = args.x3, .y3 = args.y3});
	return e;
}

ecs_entity_t kr_flecs_create_line(kr_init_line_t args) {
	ecs_entity_t e = ecs_new_w_pair(kr_world, EcsIsA, KrPrefabLine);
	ecs_add(kr_world, e, KrVisible);

	ecs_set(kr_world, e, KrDrawable, {.depth = 0, .sort_extra = 0, .pipeline = KR_COMP_PP_LINE});
	ecs_set(kr_world, e, KrColor, {.color = args.color});
	ecs_set(kr_world, e, KrPos2, {.x = args.x1, .y = args.y1});
	ecs_set(kr_world, e, KrLine, {.x_to = args.x2, .y_to = args.y2});
	ecs_set(kr_world, e, KrStroke, {.strength = args.stroke});
	return e;
}

ecs_entity_t kr_flecs_create_sdf_rect(kr_init_sdf_rect_t args) {
	ecs_entity_t e = ecs_new_w_pair(kr_world, EcsIsA, KrPrefabSdfRect);
	ecs_add(kr_world, e, KrVisible);

	ecs_set(kr_world, e, KrDrawable,
	        {.depth = 0, .sort_extra = 0, .pipeline = KR_COMP_PP_SDF_RECT});
	ecs_set(kr_world, e, KrPos2, {.x = args.rect.x, .y = args.rect.y});
	ecs_set(kr_world, e, KrColor, {.color = args.color});
	ecs_set(kr_world, e, KrRect, {.w = args.rect.w, .h = args.rect.h});
	ecs_set(kr_world, e, KrCorner, {.radius = args.corner});
	ecs_set(kr_world, e, KrSmooth, {.px = args.smooth});
	return e;
}

ecs_entity_t kr_flecs_create_sdf_asymm_rect(kr_init_sdf_asymm_rect_t args) {
	ecs_entity_t e = ecs_new_w_pair(kr_world, EcsIsA, KrPrefabSdfRectAsymm);
	ecs_add(kr_world, e, KrVisible);

	ecs_set(kr_world, e, KrDrawable,
	        {.depth = 0, .sort_extra = 0, .pipeline = KR_COMP_PP_SDF_RECT_ASYMM});
	ecs_set(kr_world, e, KrPos2, {.x = args.rect.x, .y = args.rect.y});
	ecs_set(kr_world, e, KrColor, {.color = args.color});
	ecs_set(kr_world, e, KrRect, {.w = args.rect.w, .h = args.rect.h});
	ecs_set(kr_world, e, KrCornerAsymm,
	        {.tl = args.corner.top_left,
	         .bl = args.corner.bottom_left,
	         .tr = args.corner.top_right,
	         .br = args.corner.bottom_right});
	ecs_set(kr_world, e, KrSmooth, {.px = args.smooth});
	return e;
}

ecs_entity_t kr_flecs_create_sdf_rect_wborder(kr_init_sdf_rect_wborder_t args) {
	ecs_entity_t e = kr_flecs_create_sdf_rect(args.sdf_rect);
	ecs_add(kr_world, e, KrBorder);

	ecs_set(kr_world, e, KrDrawable,
	        {.depth = 0, .sort_extra = 0, .pipeline = KR_COMP_PP_SDF_RECT_WBORDER});
	ecs_set(kr_world, e, KrBorder, {.strength = args.border, .color = args.border_color});
	return e;
}

ecs_entity_t kr_flecs_create_sdf_asymm_rect_wborder(kr_init_sdf_asymm_rect_wborder_t args) {
	ecs_entity_t e = kr_flecs_create_sdf_asymm_rect(args.sdf_asymm_rect);
	ecs_add(kr_world, e, KrBorder);

	ecs_set(kr_world, e, KrDrawable,
	        {.depth = 0, .sort_extra = 0, .pipeline = KR_COMP_PP_SDF_RECT_ASYMM_WBORDER});
	ecs_set(kr_world, e, KrBorder, {.strength = args.border, .color = args.border_color});
	return e;
}

ecs_entity_t kr_flecs_create_sdf_circle(kr_init_sdf_circle_t args) {
	ecs_entity_t e = ecs_new_w_pair(kr_world, EcsIsA, KrPrefabSdfCircle);
	ecs_add(kr_world, e, KrVisible);

	ecs_set(kr_world, e, KrDrawable,
	        {.depth = 0, .sort_extra = 0, .pipeline = KR_COMP_PP_SDF_CIRCLE});
	ecs_set(kr_world, e, KrPos2, {.x = args.pos.x, .y = args.pos.y});
	ecs_set(kr_world, e, KrColor, {.color = args.color});
	ecs_set(kr_world, e, KrCircle, {.radius = args.radius});
	ecs_set(kr_world, e, KrSmooth, {.px = args.smooth});
	return e;
}

ecs_entity_t kr_flecs_create_sdf_circle_wborder(kr_init_sdf_circle_wborder_t args) {
	ecs_entity_t e = kr_flecs_create_sdf_circle(args.sdf_circle);
	ecs_add(kr_world, e, KrBorder);

	ecs_set(kr_world, e, KrDrawable,
	        {.depth = 0, .sort_extra = 0, .pipeline = KR_COMP_PP_SDF_CIRCLE_WBORDER});
	ecs_set(kr_world, e, KrBorder, {.strength = args.border, .color = args.border_color});
	return e;
}

void kr_flecs_set_depth(ecs_entity_t e, int32_t depth) {
	bool is_added;
	KrDrawable *d = ecs_get_mut(kr_world, e, KrDrawable, &is_added);
	assert(!is_added);
	d->depth = depth;
	ecs_modified(kr_world, e, KrDrawable);
}

/* Animation */

static ecs_entity_t entity_buffer[16];
static int entity_buffer_top = 0;

static void fill_entity_buffer(void) {
	if (entity_buffer_top == 16) return;
	while (entity_buffer_top < 16) {
		entity_buffer[entity_buffer_top++] = ecs_new_id(kr_world);
	}

	// TODO: replace bubble with other sort algo if necessary
	for (int i = 0; i < 16; ++i)
		for (int j = 0; j < 15 - i; ++j)
			if (entity_buffer[j] < entity_buffer[j + 1]) {
				ecs_entity_t tmp = entity_buffer[j];
				entity_buffer[j] = entity_buffer[j + 1];
				entity_buffer[j + 1] = tmp;
			}
}

static void add_animation(ecs_entity_t e, ecs_entity_t anim_e, const kr_init_animation_t *anim) {
	int i = 0;
	ecs_set(kr_world, anim_e, KrAnimation, {anim->start, anim->duration, anim->ease});

	for (int i = 0; i < 8; ++i) {
		switch (anim->modifiers[i].modifier) {
		case KR_MODIFIER_ANGLE: {
			ecs_set_pair(kr_world, anim_e, KrAngle, KrAnimateToAngle, {anim->modifiers[i].value});
		} break;
		case KR_MODIFIER_ROTATION_CENTER: {
			ecs_set_pair(kr_world, anim_e, KrRotationCenter, KrAnimateRotationCenter,
			             {anim->modifiers[i].value, anim->modifiers[i].opt});
		} break;
		case KR_MODIFIER_X: {
			ecs_set_pair(kr_world, anim_e, KrModifier, KrAnimateToX, {anim->modifiers[i].value});
		} break;
		case KR_MODIFIER_Y: {
			ecs_set_pair(kr_world, anim_e, KrModifier, KrAnimateToY, {anim->modifiers[i].value});
		} break;
		case KR_MODIFIER_SCALE_X: {
			ecs_set_pair(kr_world, anim_e, KrScaleX, KrAnimateToScaleX, {anim->modifiers[i].value});
		} break;
		case KR_MODIFIER_SCALE_Y: {
			ecs_set_pair(kr_world, anim_e, KrScaleY, KrAnimateToScaleY, {anim->modifiers[i].value});
		} break;
		case KR_MODIFIER_OPACITY: {
			ecs_set_pair(kr_world, anim_e, KrOpacity, KrAnimateToOpacity,
			             {anim->modifiers[i].value});
		} break;

		default:
			break;
		}
	}
	ecs_add_pair(kr_world, e, KrAnimate, anim_e);
}

void kr_flecs_create_animation(ecs_entity_t e, const kr_init_animation_t *anim) {
	fill_entity_buffer();
	ecs_entity_t anim_e = entity_buffer[--entity_buffer_top];
	add_animation(e, anim_e, anim);
}

void kr_flecs_create_sequence(ecs_entity_t e, const kr_init_sequence_t *sequence) {
	assert(sequence->count > 0 && sequence->count <= 16);
	fill_entity_buffer();
	for (int i = 0; i < sequence->count; ++i) {
		ecs_entity_t anim_e = entity_buffer[--entity_buffer_top];
		add_animation(e, anim_e, &sequence->animations[i]);
	}
}
