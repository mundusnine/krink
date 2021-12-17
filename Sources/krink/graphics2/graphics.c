#include "graphics.h"
#include "coloredpainter.h"
#include "imagepainter.h"
#include "sdfpainter.h"
#include "textpainter.h"
#include <assert.h>
#include <stdbool.h>

#include <kinc/graphics4/graphics.h>
#include <krink/math/matrix.h>
#include <krink/math/vector.h>

static bool begin = false;
static uint32_t g2_color = 0;
static float g2_opacity = 1.0f;
static kinc_matrix3x3_t g2_transformation;
static krink_ttf_font_t *g2_font = NULL;
static kinc_matrix4x4_t g2_projection_matrix;
static int g2_font_size;
static float g2_width[KRINK_G2_MAX_WIN], g2_height[KRINK_G2_MAX_WIN];
static bool g2_painters_initialized = false;
static int g2_active_window = -1;

void krink_g2_init(int window, int window_width, int window_height) {
	if (!g2_painters_initialized) {
		krink_g2_isp_init();
		krink_g2_csp_init();
		krink_g2_tsp_init();
		krink_g2_sdf_init();
		g2_transformation = kinc_matrix3x3_identity();
	}
	krink_g2_set_window_size(window, window_width, window_height);
}

void krink_g2_set_window_size(int window, int window_width, int window_height) {
	assert(window >= 0 && window < KRINK_G2_MAX_WIN);
	g2_width[window] = (float)window_width;
	g2_height[window] = (float)window_height;

	if (!kinc_g4_render_targets_inverted_y()) {
		g2_projection_matrix = krink_matrix4x4_to_kinc(krink_matrix4x4_orthogonal_projection(
		    0.0f, g2_width[window], 0.0f, g2_height[window], 0.1f, 1000.0f));
	}
	else {
		g2_projection_matrix = krink_matrix4x4_to_kinc(krink_matrix4x4_orthogonal_projection(
		    0.0f, g2_width[window], g2_height[window], 0.0f, 0.1f, 1000.0f));
	}
	krink_g2_isp_set_projection_matrix(g2_projection_matrix);
	krink_g2_tsp_set_projection_matrix(g2_projection_matrix);
	krink_g2_csp_set_projection_matrix(g2_projection_matrix);
	krink_g2_sdf_set_projection_matrix(g2_projection_matrix);
}

float krink_g2_get_width(int window) {
	assert(window >= 0 && window < KRINK_G2_MAX_WIN);
	return g2_width[window];
}

float krink_g2_get_height(int window) {
	assert(window >= 0 && window < KRINK_G2_MAX_WIN);
	return g2_height[window];
}

void krink_g2_destroy(void) {
	// Maybe not needed?
}

void krink_g2_begin(int window) {
	assert(!begin && g2_active_window == -1);
	kinc_g4_begin(window);
	g2_active_window = window;
	begin = true;
}

void krink_g2_end(void) {
	assert(begin && g2_active_window != -1);
	kinc_g4_end(g2_active_window);
	krink_g2_csp_end();
	krink_g2_tsp_end();
	krink_g2_isp_end();
	krink_g2_sdf_end();
	begin = false;
	g2_active_window = -1;
}

void krink_g2_clear(uint32_t color) {
	assert(begin);
	kinc_g4_clear(KINC_G4_CLEAR_COLOR, color, 0.0f, 0);
}

void krink_g2_set_color(uint32_t color) {
	g2_color = color;
}

void krink_g2_set_opacity(float opacity) {
	g2_opacity = opacity;
}

void krink_g2_draw_rect(float x, float y, float width, float height, float strength) {
	assert(begin);
	krink_g2_isp_end();
	krink_g2_tsp_end();
	krink_g2_sdf_end();

	float hs = strength / 2.0f;
	krink_g2_csp_fill_rect(x - hs, y - hs, width + strength, strength, g2_color, g2_opacity,
	                       g2_transformation);
	krink_g2_csp_fill_rect(x - hs, y + hs, strength, height - hs, g2_color, g2_opacity,
	                       g2_transformation);
	krink_g2_csp_fill_rect(x - hs, y + height - hs, width + strength, strength, g2_color,
	                       g2_opacity, g2_transformation);
	krink_g2_csp_fill_rect(x + width - hs, y + hs, strength, height - hs, g2_color, g2_opacity,
	                       g2_transformation);
}

void krink_g2_fill_rect(float x, float y, float width, float height) {
	assert(begin);
	krink_g2_isp_end();
	krink_g2_tsp_end();
	krink_g2_sdf_end();

	krink_g2_csp_fill_rect(x, y, width, height, g2_color, g2_opacity, g2_transformation);
}

void krink_g2_fill_triangle(float x1, float y1, float x2, float y2, float x3, float y3) {
	assert(begin);
	krink_g2_isp_end();
	krink_g2_tsp_end();
	krink_g2_sdf_end();

	krink_g2_csp_fill_triangle(x1, y1, x2, y2, x3, y3, g2_color, g2_opacity, g2_transformation);
}

void krink_g2_draw_string(const char *text, float x, float y) {
	assert(begin);
	krink_g2_isp_end();
	krink_g2_csp_end();
	krink_g2_sdf_end();

	krink_g2_tsp_draw_string(text, g2_opacity, g2_color, x, y, g2_transformation);
}

void krink_g2_draw_characters(int *text, int start, int length, float x, float y) {
	assert(begin);
	krink_g2_isp_end();
	krink_g2_csp_end();
	krink_g2_sdf_end();

	krink_g2_tsp_draw_characters(text, start, length, g2_opacity, g2_color, x, y,
	                             g2_transformation);
}

void krink_g2_draw_line(float x1, float y1, float x2, float y2, float strength) {
	assert(begin);
	krink_g2_isp_end();
	krink_g2_tsp_end();
	krink_g2_sdf_end();

	krink_vec2_t vec;

	if (y2 == y1) {
		vec = (krink_vec2_t){0.0f, -1.0f};
	}
	else {
		vec = (krink_vec2_t){1.0f, -(x2 - x1) / (y2 - y1)};
	}

	krink_vec2_set_length(&vec, strength);

	krink_vec2_t p1 = (krink_vec2_t){x1 + 0.5f * vec.x, y1 + 0.5f * vec.y};
	krink_vec2_t p2 = (krink_vec2_t){x2 + 0.5f * vec.x, y2 + 0.5f * vec.y};
	krink_vec2_t p3 = krink_vec2_subv(p1, vec);
	krink_vec2_t p4 = krink_vec2_subv(p2, vec);
	krink_g2_csp_fill_triangle(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, g2_color, g2_opacity,
	                           g2_transformation);
	krink_g2_csp_fill_triangle(p3.x, p3.y, p2.x, p2.y, p4.x, p4.y, g2_color, g2_opacity,
	                           g2_transformation);
}

void krink_g2_draw_scaled_sub_image(krink_image_t *img, float sx, float sy, float sw, float sh,
                                    float dx, float dy, float dw, float dh) {
	assert(begin);
	krink_g2_csp_end();
	krink_g2_tsp_end();
	krink_g2_sdf_end();

	krink_g2_isp_draw_scaled_sub_image(img, sx, sy, sw, sh, dx, dy, dw, dh, g2_opacity, g2_color,
	                                   g2_transformation);
}

void krink_g2_set_transform(kinc_matrix3x3_t m) {
	g2_transformation = m;
}

kinc_matrix3x3_t krink_g2_get_transform(void) {
	return g2_transformation;
}

krink_ttf_font_t *krink_g2_get_font(void) {
	return g2_font;
}

int krink_g2_get_font_size(void) {
	return g2_font_size;
}

void krink_g2_set_font(krink_ttf_font_t *font, int size) {
	g2_font = font;
	g2_font_size = size;
	krink_g2_tsp_set_font(font);
	krink_g2_tsp_set_font_size(size);
}

void krink_g2_draw_sdf_rect(float x, float y, float width, float height,
                            krink_sdf_corner_radius_t corner, float border, uint32_t border_color,
                            float smooth) {
	assert(begin);
	krink_g2_isp_end();
	krink_g2_tsp_end();
	krink_g2_csp_end();

	krink_g2_sdf_draw_rect(x, y, width, height, corner, border, smooth, g2_color, border_color,
	                       g2_opacity, g2_transformation);
}

void krink_g2_draw_sdf_rect_symm(float x, float y, float width, float height, float corner,
                                 float border, uint32_t border_color, float smooth) {
	assert(begin);
	krink_g2_isp_end();
	krink_g2_tsp_end();
	krink_g2_csp_end();

	krink_g2_sdf_draw_rect_symm(x, y, width, height, corner, border, smooth, g2_color, border_color,
	                            g2_opacity, g2_transformation);
}

void krink_g2_draw_sdf_circle(float x, float y, float r, float border, uint32_t border_color,
                              float smooth) {
	assert(begin);
	krink_g2_isp_end();
	krink_g2_tsp_end();
	krink_g2_csp_end();

	krink_g2_sdf_draw_circle(x, y, r, border, smooth, g2_color, border_color, g2_opacity,
	                         g2_transformation);
}

void krink_g2_draw_sdf_line(float x1, float y1, float x2, float y2, float strength, float smooth) {
	assert(begin);
	krink_g2_isp_end();
	krink_g2_tsp_end();
	krink_g2_csp_end();

	krink_g2_sdf_draw_line(x1, y1, x2, y2, strength, smooth, g2_color, g2_opacity,
	                       g2_transformation);
}

void krink_g2_scissor(float x, float y, float w, float h) {
	kinc_g4_scissor((int)(x + 0.5f), (int)(y + 0.5f), (int)(w + 0.5f), (int)(h + 0.5f));
}

void krink_g2_disable_scissor(void) {
	kinc_g4_disable_scissor();
}
