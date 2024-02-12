#ifndef DRAWING_FUNCS_H
#define DRAWING_FUNCS_H

#include <stdint.h>
#include "image.h"

struct Rect {
  int32_t x, y, width, height;
};

int32_t in_bounds(struct Image *img, int32_t x, int32_t y);

uint32_t compute_index(struct Image *img, int32_t x, int32_t y);

int32_t clamp(int32_t val, int32_t min, int32_t max);

uint8_t get_r(uint32_t color);

uint8_t get_g(uint32_t color);

uint8_t get_b(uint32_t color);

uint8_t get_a(uint32_t color);

uint32_t set_Nth_bit(uint32_t x, uint32_t n, uint8_t bit);

uint8_t get_Nth_bit(uint32_t x, uint32_t n);

uint32_t set_r(uint32_t color, uint8_t r_component);

uint32_t set_g(uint32_t color, uint8_t g_component);

uint32_t set_b(uint32_t color, uint8_t b_component);

uint32_t set_a(uint32_t color, uint8_t a_component);

uint8_t blend_color(uint8_t fg, uint8_t bg, uint8_t opacity);

uint32_t make_color(uint8_t r, uint8_t g, uint8_t b);

uint32_t blend_components(uint32_t fg, uint32_t bg, uint32_t alpha);

uint32_t blend_colors(uint32_t fg, uint32_t bg);

//sets the pixel at the specified index to color. NO BLENDING
//precondition: index is bounds for the image
void set_pixel(struct Image *img, uint32_t index, uint32_t color);

int64_t square(int64_t x);
int64_t square_dist(int64_t x1, int64_t y1, int64_t x2, int64_t y2);
//checks if a pixel is contained in a circle(inclusive) and if so
//returns 1 = contained
//returns 0 = outside the circle
int is_in_circle(struct Image* image, int32_t center_x, int32_t center_y, int32_t x, int32_t y, int32_t radius);

//inclusive bounds check
//returns 1 if in bounds
//returns 0 if out of bounds
int is_in_range(uint32_t val, uint32_t min, uint32_t max);

//checks if a pixel is contained in a rectangle(inclusive) and if so
//returns 1 = contained
//returns 0 = outside the rectangle
int is_in_rect(struct Image* image, const struct Rect* rect, int32_t x, int32_t y);

int rect_in_img(struct Image* image, const struct Rect* rect);


void draw_pixel_no_blending(struct Image *img, int32_t x, int32_t y, uint32_t color);

void draw_pixel(struct Image *img, int32_t x, int32_t y, uint32_t color);

void draw_rect(struct Image *img,
               const struct Rect *rect,
               uint32_t color);

void draw_circle(struct Image *img,
                 int32_t x, int32_t y, int32_t r,
                 uint32_t color);

void draw_tile(struct Image *img,
               int32_t x, int32_t y,
               struct Image *tilemap,
               const struct Rect *tile);

void draw_sprite(struct Image *img,
                 int32_t x, int32_t y,
                 struct Image *spritemap,
                 const struct Rect *sprite);

#endif // DRAWING_FUNCS_H
