// C implementations of drawing functions (and helper functions)

#include <assert.h>
// #include <cstdint>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ucontext.h>
#include "drawing_funcs.h"

////////////////////////////////////////////////////////////////////////
// Helper functions
////////////////////////////////////////////////////////////////////////

// TODO: implement helper functions

int32_t in_bounds(struct Image *img, int32_t x, int32_t y) {
	
	
  return ((x >= 0 && x < img->width) && (y >= 0 && y < img->height)); // do they want a boolean or int32_t...?
}

uint32_t compute_index(struct Image *img, int32_t x, int32_t y) {
	return img->width*y + x;
}

int32_t clamp(int32_t val, int32_t min, int32_t max) {
  if(val < min) {
    return min;
  }
  else if(val > max) {
    return max;
  }
  return val;
}

uint8_t get_r(uint32_t color) {
  return (color >> 24) & 0xFF; // right shift to cut off all bits up to 24; this applies to the other gets respectively
}

uint8_t get_g(uint32_t color) {
  return (color >> 16) & 0xFF;
}

uint8_t get_b(uint32_t color) {
  return (color >> 8) & 0xFF;
}

uint8_t get_a(uint32_t color){ 
  return color & 0xFF;
}

uint32_t set_Nth_bit(uint32_t x, uint32_t n, uint8_t bit) {
  if(bit){
    return x | (1UL<<n);
  }
  return x & (~((1UL)<<n));
}

uint8_t get_Nth_bit(uint32_t x, uint32_t n) {
  return (x & (1UL<<n) ? 1:0);
}

uint32_t set_r(uint32_t color, uint8_t r_component){
	for(int i = 24; i <= 31; i++){
		color = set_Nth_bit(color, i, get_Nth_bit(r_component, i-24));
	}
	return color;
}
uint32_t set_g(uint32_t color, uint8_t g_component){
	for(int i = 16; i <= 23; i++){
		color = set_Nth_bit(color, i, get_Nth_bit(g_component, i-16));
	}
	return color;
}
uint32_t set_b(uint32_t color, uint8_t b_component){
	for(int i = 8; i <= 15; i++){
		color = set_Nth_bit(color, i, get_Nth_bit(b_component, i-8));
	}
	return color;
}
uint32_t set_a(uint32_t color, uint8_t a_component){
	for(int i = 0; i <= 7; i++){
		color = set_Nth_bit(color, i, get_Nth_bit(a_component, i));
	}
	return color;
}

uint8_t blend_color(uint8_t fg, uint8_t bg, uint8_t opacity){
	uint32_t fg_color = opacity*fg;
	uint32_t bg_color = ((255 - opacity) * bg);
	return (( fg_color + bg_color )/255);
}

uint32_t make_color(uint8_t r, uint8_t g, uint8_t b){
	uint32_t newColor = 0;
	newColor = set_r(newColor, r);
	newColor = set_g(newColor, g);
	newColor = set_b(newColor, b);
	newColor = set_a(newColor, 255);
	return newColor;
}


uint32_t blend_colors(uint32_t fg, uint32_t bg){
	uint32_t opacity = get_a(fg);
	uint8_t bg_r = get_r(bg);
	uint8_t bg_g = get_g(bg);
	uint8_t bg_b = get_b(bg);

	uint8_t fg_r = get_r(fg);
	uint8_t fg_g = get_g(fg);
	uint8_t fg_b = get_b(fg);

	uint8_t blended_r = blend_color(fg_r, bg_r, opacity);
	uint8_t blended_g = blend_color(fg_g, bg_g, opacity);
	uint8_t blended_b = blend_color(fg_b, bg_b, opacity);

	return make_color(blended_r, blended_g, blended_b);
	// return blend_components(fg, bg, opacity);
}

//sets the pixel at the specified index to color. NO BLENDING
//precondition: index is bounds for the image
void set_pixel(struct Image *img, uint32_t index, uint32_t color){
	img->data[index] = color;
}

int64_t square(int64_t x){
	return x * x;
}
int64_t square_dist(int64_t x1, int64_t y1, int64_t x2, int64_t y2){
	return square(x2-x1) + square(y2-y1);
}

//checks if a pixel is contained in a circle(inclusive) and if so
//returns 1 = contained
//returns 0 = outside the circle
int is_in_circle(struct Image* image, int32_t center_x, int32_t center_y, int32_t x, int32_t y, int32_t radius){
	if(square_dist(center_x, center_y, x, y) <= square(radius)){
		return 1;
	}
	return 0;
}

//inclusive bounds check on unsigned bounds and value
//returns 1 if in bounds
//returns 0 if out of bounds
int is_in_range(uint32_t val, uint32_t min, uint32_t max){
	if(val >= min && val <= max){
		return 1;
	}
	return 0;
}

//checks if a pixel is contained in a rectangle(inclusive) and if so
//returns 1 = contained
//returns 0 = outside the rectangle
int is_in_rect(struct Image* image, const struct Rect* rect, int32_t x, int32_t y){
	if(is_in_range(x, rect->x, rect->x+rect->width-1) && is_in_range(y, rect->y, rect->height+rect->y-1)){
		return 1;
	}
	return 0;
}

int rect_in_img(struct Image* image, const struct Rect* rect){
	if(rect->x >= 0 && rect->y >= 0 && rect ->x <= (image->width - rect->width) && rect->y <= (image->height-rect->height)){
		return 1;
	}
	return 0;
}



void draw_pixel_no_blending(struct Image *img, int32_t x, int32_t y, uint32_t color){
	if(!in_bounds(img, x, y)){
		return;
	}
	uint32_t index = compute_index(img, x, y);
	set_pixel(img, index, color);
}

////////////////////////////////////////////////////////////////////////
// API functions
////////////////////////////////////////////////////////////////////////

//
// Draw a pixel.
//
// Parameters:
//   img   - pointer to struct Image
//   x     - x coordinate (pixel column)
//   y     - y coordinate (pixel row)
//   color - uint32_t color value
//
void draw_pixel(struct Image *img, int32_t x, int32_t y, uint32_t color) {
  // TODO: implement
	if(!in_bounds(img, x, y)){
		return;
	}
	uint32_t index = compute_index(img, x, y);
	uint32_t newColor = blend_colors(color, img->data[index]);
	set_pixel(img, index, newColor);

}

//
// Draw a rectangle.
// The rectangle has rect->x,rect->y as its upper left corner,
// is rect->width pixels wide, and rect->height pixels high.
//
// Parameters:
//   img     - pointer to struct Image
//   rect    - pointer to struct Rect
//   color   - uint32_t color value
//
void draw_rect(struct Image *img,
               const struct Rect *rect,
               uint32_t color) {

	for(int i = 0; i < img->width; i++){
		for(int j = 0; j < img->height; j++){

			if(is_in_rect(img, rect, i, j)){
				draw_pixel(img, i, j, color);
			}

		}
	}


}

//
// Draw a circle.
// The circle has x,y as its center and has r as its radius.
//
// Parameters:
//   img     - pointer to struct Image
//   x       - x coordinate of circle's center
//   y       - y coordinate of circle's center
//   r       - radius of circle
//   color   - uint32_t color value
//
void draw_circle(struct Image *img,
                 int32_t x, int32_t y, int32_t r,
                 uint32_t color) {

	for(int i = 0; i < img->width; i++){
		for(int j = 0; j < img->height; j++){
			if(is_in_circle(img, x, y, i, j, r)){
				uint32_t index = compute_index(img, i, j);
				uint32_t newColor = blend_colors(color, img->data[index]);
				set_pixel(img, index, newColor);
			}
		}
	}

}

//
// Draw a tile by copying all pixels in the region
// enclosed by the tile parameter in the tilemap image
// to the specified x/y coordinates of the destination image.
// No blending of the tile pixel colors with the background
// colors should be done.
//
// Parameters:
//   img     - pointer to Image (dest image)
//   x       - x coordinate of location where tile should be copied
//   y       - y coordinate of location where tile should be copied
//   tilemap - pointer to Image (the tilemap)
//   tile    - pointer to Rect (the tile)
//
void draw_tile(struct Image *img,
               int32_t x, int32_t y,
               struct Image *tilemap,
               const struct Rect *tile) {
 // TODO: implement
 	if(!rect_in_img(tilemap, tile)){
		return;
	}
	for(uint32_t i = 0; i < tile->width; i++){
		for(uint32_t j = 0; j < tile->height; j++){
			if(in_bounds(img, x+i, j+y) && in_bounds(tilemap, i+tile->x, j+tile->y)){
				uint32_t tilemap_index = compute_index(tilemap, i+tile->x, j+tile->y);
				draw_pixel_no_blending(img, x+i, j+y, tilemap->data[tilemap_index]);
        // set_pixel(img, img_index, tilemap->data[tilemap_index]);
				// img->data[img_index] = tilemap->data[tilemap_index];
			}
		}
	}

}

//
// Draw a sprite by copying all pixels in the region
// enclosed by the sprite parameter in the spritemap image
// to the specified x/y coordinates of the destination image.
// The alpha values of the sprite pixels should be used to
// blend the sprite pixel colors with the background
// pixel colors.
//
// Parameters:
//   img       - pointer to Image (dest image)
//   x         - x coordinate of location where sprite should be copied
//   y         - y coordinate of location where sprite should be copied
//   spritemap - pointer to Image (the spritemap)
//   sprite    - pointer to Rect (the sprite)
//
void draw_sprite(struct Image *img,
                 int32_t x, int32_t y,
                 struct Image *spritemap,
                 const struct Rect *sprite) {
  // TODO: implement

	if(!rect_in_img(spritemap, sprite)){
		return;
	}

	for(uint32_t i = 0; i < sprite->width; i++){
		for(uint32_t j = 0; j < sprite->height; j++){
			if(in_bounds(img, x+i, j+y) && in_bounds(spritemap, i+sprite->x, j+sprite->y)){
        // uint32_t img_index = compute_index(img, x+i+sprite->x, j+y+sprite->y);
        uint32_t sprite_index = compute_index(spritemap, i+sprite->x, j+sprite->y);
        // uint32_t newColor = blend_colors(spritemap->data[sprite_index], img->data[img_index]);
        // set_pixel(img, img_index, newColor);

				draw_pixel(img, x+i, j+y, spritemap->data[sprite_index]);
				// img->data[compute_index(img, x+i+tile->x, j+y+tile->y)] = tilemap->data[compute_index(tilemap, i+tile->x, j+tile->y)];
			}
		}
	}
}
