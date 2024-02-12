#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image.h"
#include "drawing_funcs.h"
#include "tctest.h"
// TODO: add prototypes for your helper functions

// an expected color identified by a (non-zero) character code
typedef struct {
  char c;
  uint32_t color;
} ExpectedColor;

// struct representing a "picture" of an expected image
typedef struct {
  ExpectedColor colors[20];
  const char *pic;
} Picture;

typedef struct {
  struct Image small;
  struct Image large;
  struct Image tilemap;
  struct Image spritemap;
} TestObjs;

// dimensions and pixel index computation for "small" test image (objs->small)
#define SMALL_W        8
#define SMALL_H        6
#define SMALL_IDX(x,y) ((y)*SMALL_W + (x))

// dimensions of the "large" test image
#define LARGE_W        24
#define LARGE_H        20

// create test fixture data
TestObjs *setup(void) {
  TestObjs *objs = (TestObjs *) malloc(sizeof(TestObjs));
  init_image(&objs->small, SMALL_W, SMALL_H);
  init_image(&objs->large, LARGE_W, LARGE_H);
  objs->tilemap.data = NULL;
  objs->spritemap.data = NULL;
  return objs;
}

// clean up test fixture data
void cleanup(TestObjs *objs) {
  free(objs->small.data);
  free(objs->large.data);
  free(objs->tilemap.data);
  free(objs->spritemap.data);

  free(objs);
}

uint32_t lookup_color(char c, const ExpectedColor *colors) {
  for (unsigned i = 0; ; i++) {
    assert(colors[i].c != 0);
    if (colors[i].c == c) {
      return colors[i].color;
    }
  }
}

void check_picture(struct Image *img, Picture *p) {
  unsigned num_pixels = img->width * img->height;
  assert(strlen(p->pic) == num_pixels);

  for (unsigned i = 0; i < num_pixels; i++) {
    char c = p->pic[i];
    uint32_t expected_color = lookup_color(c, p->colors);
    uint32_t actual_color = img->data[i];
    ASSERT(actual_color == expected_color);
  }
}

// prototypes of test functions
void test_draw_pixel(TestObjs *objs);
void test_draw_rect(TestObjs *objs);
void test_draw_circle(TestObjs *objs);
void test_draw_circle_clip(TestObjs *objs);
void test_draw_tile(TestObjs *objs);
void test_draw_sprite(TestObjs *objs);

// prototypes of test functions for helper function
void test_set_Nth_bit(TestObjs *objs);
void test_get_Nth_bit(TestObjs *objs);
void test_in_bounds(TestObjs *objs);
void test_compute_index(TestObjs *objs);
void test_clamp(TestObjs *objs);
void test_get_r(TestObjs *objs);
void test_get_g(TestObjs *objs);
void test_get_b(TestObjs *objs);
void test_get_a(TestObjs *objs);
void test_set_r(TestObjs *objs);
void test_set_g(TestObjs *objs);
void test_set_b(TestObjs *objs);
void test_set_a(TestObjs *objs);
void test_blend_color(TestObjs *objs);
void test_make_color(TestObjs *objs);
void test_blend_components(TestObjs *objs);
void test_blend_colors(TestObjs *objs);
void test_set_pixel(TestObjs *objs);
void test_square(TestObjs *objs);
void test_square_dist(TestObjs *objs);
void test_is_in_circle(TestObjs *objs);
void test_is_in_range(TestObjs *objs);
void test_is_in_rect(TestObjs *objs);

int main(int argc, char **argv) {
  if (argc > 1) {
    // user specified a specific test function to run
    tctest_testname_to_execute = argv[1];
  }

  TEST_INIT();

  // TODO: add TEST() directives for your helper functions
  // TEST(test_draw_pixel);
  // TEST(test_draw_rect);
  // TEST(test_draw_circle);
  // TEST(test_draw_circle_clip);
  // TEST(test_draw_tile);
  // TEST(test_draw_sprite);

	TEST(test_set_Nth_bit);
	TEST(test_get_Nth_bit);
	TEST(test_in_bounds);
	TEST(test_compute_index);
	TEST(test_clamp);
	TEST(test_get_r);
	TEST(test_get_g);
	TEST(test_get_b);
	TEST(test_get_a);
	TEST(test_set_r);
	TEST(test_set_g);
	TEST(test_set_b);
	TEST(test_set_a);
	TEST(test_blend_color);
	TEST(test_make_color);
	TEST(test_blend_components);
	TEST(test_blend_colors);
	TEST(test_set_pixel);
	TEST(test_square);
	TEST(test_square_dist);
	TEST(test_is_in_circle);
	TEST(test_is_in_range);
	TEST(test_is_in_rect);

  TEST_FINI();
}

void test_draw_pixel(TestObjs *objs) {
  // initially objs->small pixels are opaque black
  ASSERT(objs->small.data[SMALL_IDX(3, 2)] == 0x000000FFU);
  ASSERT(objs->small.data[SMALL_IDX(5, 4)] == 0x000000FFU);

  // test drawing completely opaque pixels
  draw_pixel(&objs->small, 3, 2, 0xFF0000FF); // opaque red
  ASSERT(objs->small.data[SMALL_IDX(3, 2)] == 0xFF0000FF);
  draw_pixel(&objs->small, 5, 4, 0x800080FF); // opaque magenta (half-intensity)
  ASSERT(objs->small.data[SMALL_IDX(5, 4)] == 0x800080FF);

  // test color blending
  draw_pixel(&objs->small, 3, 2, 0x00FF0080); // half-opaque full-intensity green
  ASSERT(objs->small.data[SMALL_IDX(3, 2)] == 0x7F8000FF);
  draw_pixel(&objs->small, 4, 2, 0x0000FF40); // 1/4-opaque full-intensity blue
  ASSERT(objs->small.data[SMALL_IDX(4, 2)] == 0x000040FF);
}

void test_draw_rect(TestObjs *objs) {
  struct Rect red_rect = { .x = 2, .y = 2, .width=3, .height=3 };
  struct Rect blue_rect = { .x = 3, .y = 3, .width=3, .height=3 };
  draw_rect(&objs->small, &red_rect, 0xFF0000FF); // red is full-intensity, full opacity
  draw_rect(&objs->small, &blue_rect, 0x0000FF80); // blue is full-intensity, half opacity

  const uint32_t red   = 0xFF0000FF; // expected full red color
  const uint32_t blue  = 0x000080FF; // expected full blue color
  const uint32_t blend = 0x7F0080FF; // expected red/blue blend color
  const uint32_t black = 0x000000FF; // expected black (background) color

  Picture expected = {
    { {'r', red}, {'b', blue}, {'n', blend}, {' ', black} },
    "        "
    "        "
    "  rrr   "
    "  rnnb  "
    "  rnnb  "
    "   bbb  "
  };

  check_picture(&objs->small, &expected);
}

void test_draw_circle(TestObjs *objs) {
  Picture expected = {
    { {' ', 0x000000FF}, {'x', 0x00FF00FF} },
    "   x    "
    "  xxx   "
    " xxxxx  "
    "  xxx   "
    "   x    "
    "        "
  };

  draw_circle(&objs->small, 3, 2, 2, 0x00FF00FF);

  check_picture(&objs->small, &expected);
}

void test_draw_circle_clip(TestObjs *objs) {
  Picture expected = {
    { {' ', 0x000000FF}, {'x', 0x00FF00FF} },
    " xxxxxxx"
    " xxxxxxx"
    "xxxxxxxx"
    " xxxxxxx"
    " xxxxxxx"
    "  xxxxx "
  };

  draw_circle(&objs->small, 4, 2, 4, 0x00FF00FF);

  check_picture(&objs->small, &expected);
}

void test_draw_tile(TestObjs *objs) {
  ASSERT(read_image("img/PrtMimi.png", &objs->tilemap) == IMG_SUCCESS);

  struct Rect r = { .x = 4, .y = 2, .width = 16, .height = 18 };
  draw_rect(&objs->large, &r, 0x1020D0FF);

  struct Rect grass = { .x = 0, .y = 16, .width = 16, .height = 16 };
  draw_tile(&objs->large, 3, 2, &objs->tilemap, &grass);

  Picture pic = {
    {
      { ' ', 0x000000ff },
      { 'a', 0x52ad52ff },
      { 'b', 0x1020d0ff },
      { 'c', 0x257b4aff },
      { 'd', 0x0c523aff },
    },
    "                        "
    "                        "
    "             a     b    "
    "            a      b    "
    "            a     ab    "
    "           ac      b    "
    "           ac a    b    "
    "      a a  ad  a   b    "
    "     a  a aad  aa ab    "
    "     a  a acd aaacab    "
    "    aa  cdacdaddaadb    "
    "     aa cdaddaaddadb    "
    "     da ccaddcaddadb    "
    "    adcaacdaddddcadb    "
    "   aaccacadcaddccaab    "
    "   aacdacddcaadcaaab    "
    "   aaaddddaddaccaacb    "
    "   aaacddcaadacaaadb    "
    "    bbbbbbbbbbbbbbbb    "
    "    bbbbbbbbbbbbbbbb    "
  };

  check_picture(&objs->large, &pic);
}

void test_draw_sprite(TestObjs *objs) {
  ASSERT(read_image("img/NpcGuest.png", &objs->spritemap) == IMG_SUCCESS);

  struct Rect r = { .x = 1, .y = 1, .width = 14, .height = 14 };
  draw_rect(&objs->large, &r, 0x800080FF);

  struct Rect sue = { .x = 128, .y = 136, .width = 16, .height = 15 };
  draw_sprite(&objs->large, 4, 2, &objs->spritemap, &sue);

  Picture pic = {
    {
      { ' ', 0x000000ff },
      { 'a', 0x800080ff },
      { 'b', 0x9cadc1ff },
      { 'c', 0xefeae2ff },
      { 'd', 0x100000ff },
      { 'e', 0x264c80ff },
      { 'f', 0x314e90ff },
    },
    "                        "
    " aaaaaaaaaaaaaa         "
    " aaaaaaaaaaaaaa         "
    " aaaaaaaaaaaaaa         "
    " aaaaaaabccccccbc       "
    " aaaaacccccccccccc      "
    " aaaacbddcccddcbccc     "
    " aaacbbbeccccedbccc     "
    " aaacbbceccccebbbccc    "
    " aaabbbccccccccbbccc    "
    " aaaabbbcccccccb ccb    "
    " aaaabaaaaabbaa  cb     "
    " aaaaaaaaafffea         "
    " aaaaaaaaaffeea         "
    " aaaaaaacffffcc         "
    "        cffffccb        "
    "         bbbbbbb        "
    "                        "
    "                        "
    "                        "
  };

  check_picture(&objs->large, &pic);
}


void test_set_Nth_bit(TestObjs *objs){
	uint32_t num = 0;
	num = set_Nth_bit(num, 4, 1);
	ASSERT(num == 16);
	num = set_Nth_bit(num, 4, 0);
	ASSERT(num == 0);
}
void test_get_Nth_bit(TestObjs *objs){
  uint32_t color = 0x314e90ff;
	ASSERT(get_Nth_bit(color, 0) == 1);
	ASSERT(get_Nth_bit(color, 1) == 1);
	ASSERT(get_Nth_bit(color, 2) == 1);
	ASSERT(get_Nth_bit(color, 3) == 1);
	ASSERT(get_Nth_bit(color, 4) == 1);
	ASSERT(get_Nth_bit(color, 5) == 1);
	ASSERT(get_Nth_bit(color, 6) == 1);
	ASSERT(get_Nth_bit(color, 7) == 1);

	ASSERT(get_Nth_bit(color, 8) == 0);
	ASSERT(get_Nth_bit(color, 9) == 0);
	ASSERT(get_Nth_bit(color, 10) == 0);
	ASSERT(get_Nth_bit(color, 11) == 0);
	ASSERT(get_Nth_bit(color, 12) == 1);
	ASSERT(get_Nth_bit(color, 13) == 0);
	ASSERT(get_Nth_bit(color, 14) == 0);
	ASSERT(get_Nth_bit(color, 15) == 1);

	ASSERT(get_Nth_bit(color, 16) == 0);
	ASSERT(get_Nth_bit(color, 17) == 1);
	ASSERT(get_Nth_bit(color, 18) == 1);
	ASSERT(get_Nth_bit(color, 19) == 1);
	ASSERT(get_Nth_bit(color, 20) == 0);
	ASSERT(get_Nth_bit(color, 21) == 0);
	ASSERT(get_Nth_bit(color, 22) == 1);
	ASSERT(get_Nth_bit(color, 23) == 0);

	ASSERT(get_Nth_bit(color, 24) == 1);
	ASSERT(get_Nth_bit(color, 25) == 0);
	ASSERT(get_Nth_bit(color, 26) == 0);
	ASSERT(get_Nth_bit(color, 27) == 0);
	ASSERT(get_Nth_bit(color, 28) == 1);
	ASSERT(get_Nth_bit(color, 29) == 1);
	ASSERT(get_Nth_bit(color, 30) == 0);
	ASSERT(get_Nth_bit(color, 31) == 0);
	

}
void test_in_bounds(TestObjs *objs){
	
}
void test_compute_index(TestObjs *objs){
	
}
void test_clamp(TestObjs *objs){
	
}
void test_get_r(TestObjs *objs){
	uint32_t color1 = 0x000000ff;
	uint32_t color2 = 0x800080ff;
	uint32_t color3 = 0x9cadc1ff;
	uint32_t color4 = 0xefeae2ff;
	uint32_t color5 = 0x100000ff;
	uint32_t color6 = 0x264c80ff;
	uint32_t color7 = 0x314e90ff;
	ASSERT(get_r(color1) == 0x00);
	ASSERT(get_r(color2) == 0x80);
	ASSERT(get_r(color3) == 0x9c);
	ASSERT(get_r(color4) == 0xef);
	ASSERT(get_r(color5) == 0x10);
	ASSERT(get_r(color6) == 0x26);
	ASSERT(get_r(color7) == 0x31);
	
}
void test_get_g(TestObjs *objs){
	uint32_t color1 = 0x000000ff;
	uint32_t color2 = 0x800080ff;
	uint32_t color3 = 0x9cadc1ff;
	uint32_t color4 = 0xefeae2ff;
	uint32_t color5 = 0x100000ff;
	uint32_t color6 = 0x264c80ff;
	uint32_t color7 = 0x314e90ff;
	ASSERT(get_g(color1) == 0x00);
	ASSERT(get_g(color2) == 0x00);
	ASSERT(get_g(color3) == 0xad);
	ASSERT(get_g(color4) == 0xea);
	ASSERT(get_g(color5) == 0x00);
	ASSERT(get_g(color6) == 0x4c);
	ASSERT(get_g(color7) == 0x4e);
}
void test_get_b(TestObjs *objs){
	uint32_t color1 = 0x000000ff;
	uint32_t color2 = 0x800080ff;
	uint32_t color3 = 0x9cadc1ff;
	uint32_t color4 = 0xefeae2ff;
	uint32_t color5 = 0x100000ff;
	uint32_t color6 = 0x264c80ff;
	uint32_t color7 = 0x314e90ff;
	ASSERT(get_b(color1) == 0x00);
	ASSERT(get_b(color2) == 0x80);
	ASSERT(get_b(color3) == 0xc1);
	ASSERT(get_b(color4) == 0xe2);
	ASSERT(get_b(color5) == 0x00);
	ASSERT(get_b(color6) == 0x80);
	ASSERT(get_b(color7) == 0x90);
}
void test_get_a(TestObjs *objs){
	uint32_t color1 = 0x000000ff;
	uint32_t color2 = 0x800080ff;
	uint32_t color3 = 0x9cadc1ff;
	uint32_t color4 = 0xefeae2ff;
	uint32_t color5 = 0x100000ff;
	uint32_t color6 = 0x264c80ff;
	uint32_t color7 = 0x314e90ff;
	ASSERT(get_a(color1) == 0xff);
	ASSERT(get_a(color2) == 0xff);
	ASSERT(get_a(color3) == 0xff);
	ASSERT(get_a(color4) == 0xff);
	ASSERT(get_a(color5) == 0xff);
	ASSERT(get_a(color6) == 0xff);
	ASSERT(get_a(color7) == 0xff);
}
void test_set_r(TestObjs *objs){
	
}
void test_set_g(TestObjs *objs){
	
}
void test_set_b(TestObjs *objs){
	
}
void test_set_a(TestObjs *objs){
	
}
void test_blend_color(TestObjs *objs){
	
}
void test_make_color(TestObjs *objs){
	
}
void test_blend_components(TestObjs *objs){
	
}
void test_blend_colors(TestObjs *objs){
	
}
void test_set_pixel(TestObjs *objs){
	
}
void test_square(TestObjs *objs){
	
}
void test_square_dist(TestObjs *objs){
	
}
void test_is_in_circle(TestObjs *objs){
	ASSERT(is_in_circle(&objs->small, 3, 2, 0, 0, 2) == 0);
	ASSERT(is_in_circle(&objs->small, 3, 2, 1, 0, 2) == 0);
	ASSERT(is_in_circle(&objs->small, 3, 2, 2, 0, 2) == 0);
	ASSERT(is_in_circle(&objs->small, 3, 2, 3, 0, 2) == 1);
	ASSERT(is_in_circle(&objs->small, 3, 2, 4, 0, 2) == 0);
	ASSERT(is_in_circle(&objs->small, 3, 2, 5, 0, 2) == 0);
	ASSERT(is_in_circle(&objs->small, 3, 2, 6, 0, 2) == 0);
	
	ASSERT(is_in_circle(&objs->small, 3, 2, 0, 1, 2) == 0);
	ASSERT(is_in_circle(&objs->small, 3, 2, 1, 1, 2) == 0);
	ASSERT(is_in_circle(&objs->small, 3, 2, 2, 1, 2) == 1);
	ASSERT(is_in_circle(&objs->small, 3, 2, 3, 1, 2) == 1);
	ASSERT(is_in_circle(&objs->small, 3, 2, 4, 1, 2) == 1);
	ASSERT(is_in_circle(&objs->small, 3, 2, 5, 1, 2) == 0);
	ASSERT(is_in_circle(&objs->small, 3, 2, 6, 1, 2) == 0);

	ASSERT(is_in_circle(&objs->small, 3, 2, 0, 2, 2) == 0);
	ASSERT(is_in_circle(&objs->small, 3, 2, 1, 2, 2) == 1);
	ASSERT(is_in_circle(&objs->small, 3, 2, 2, 2, 2) == 1);
	ASSERT(is_in_circle(&objs->small, 3, 2, 3, 2, 2) == 1);
	ASSERT(is_in_circle(&objs->small, 3, 2, 4, 2, 2) == 1);
	ASSERT(is_in_circle(&objs->small, 3, 2, 5, 2, 2) == 1);
	ASSERT(is_in_circle(&objs->small, 3, 2, 6, 2, 2) == 0);

	ASSERT(is_in_circle(&objs->small, 3, 2, 0, 3, 2) == 0);
	ASSERT(is_in_circle(&objs->small, 3, 2, 1, 3, 2) == 0);
	ASSERT(is_in_circle(&objs->small, 3, 2, 2, 3, 2) == 1);
	ASSERT(is_in_circle(&objs->small, 3, 2, 3, 3, 2) == 1);
	ASSERT(is_in_circle(&objs->small, 3, 2, 4, 3, 2) == 1);
	ASSERT(is_in_circle(&objs->small, 3, 2, 5, 3, 2) == 0);
	ASSERT(is_in_circle(&objs->small, 3, 2, 6, 3, 2) == 0);

	ASSERT(is_in_circle(&objs->small, 3, 2, 0, 4, 2) == 0);
	ASSERT(is_in_circle(&objs->small, 3, 2, 1, 4, 2) == 0);
	ASSERT(is_in_circle(&objs->small, 3, 2, 2, 4, 2) == 0);
	ASSERT(is_in_circle(&objs->small, 3, 2, 3, 4, 2) == 1);
	ASSERT(is_in_circle(&objs->small, 3, 2, 4, 4, 2) == 0);
	ASSERT(is_in_circle(&objs->small, 3, 2, 5, 4, 2) == 0);
	ASSERT(is_in_circle(&objs->small, 3, 2, 6, 4, 2) == 0);

	ASSERT(is_in_circle(&objs->small, 3, 2, 0, 5, 2) == 0);
	ASSERT(is_in_circle(&objs->small, 3, 2, 1, 5, 2) == 0);
	ASSERT(is_in_circle(&objs->small, 3, 2, 2, 5, 2) == 0);
	ASSERT(is_in_circle(&objs->small, 3, 2, 3, 5, 2) == 0);
	ASSERT(is_in_circle(&objs->small, 3, 2, 4, 5, 2) == 0);
	ASSERT(is_in_circle(&objs->small, 3, 2, 5, 5, 2) == 0);
	ASSERT(is_in_circle(&objs->small, 3, 2, 6, 5, 2) == 0);

}
void test_is_in_range(TestObjs *objs){
	
}
void test_is_in_rect(TestObjs *objs){
	struct Rect r = { .x = 2, .y = 2, .width = 3, .height = 3};

	ASSERT(is_in_rect(&(objs->small), &r, 1, 1) == 0);
	ASSERT(is_in_rect(&(objs->small), &r, 2, 1) == 0);
	ASSERT(is_in_rect(&(objs->small), &r, 3, 1) == 0);
	ASSERT(is_in_rect(&(objs->small), &r, 4, 1) == 0);
	ASSERT(is_in_rect(&(objs->small), &r, 5, 1) == 0);

	ASSERT(is_in_rect(&(objs->small), &r, 1, 2) == 0);
	ASSERT(is_in_rect(&(objs->small), &r, 2, 2) == 1);
	ASSERT(is_in_rect(&(objs->small), &r, 3, 2) == 1);
	ASSERT(is_in_rect(&(objs->small), &r, 4, 2) == 1);
	ASSERT(is_in_rect(&(objs->small), &r, 5, 2) == 0);

	ASSERT(is_in_rect(&(objs->small), &r, 1, 3) == 0);
	ASSERT(is_in_rect(&(objs->small), &r, 2, 3) == 1);
	ASSERT(is_in_rect(&(objs->small), &r, 3, 3) == 1);
	ASSERT(is_in_rect(&(objs->small), &r, 4, 3) == 1);
	ASSERT(is_in_rect(&(objs->small), &r, 5, 3) == 0);

	ASSERT(is_in_rect(&(objs->small), &r, 1, 4) == 0);
	ASSERT(is_in_rect(&(objs->small), &r, 2, 4) == 1);
	ASSERT(is_in_rect(&(objs->small), &r, 3, 4) == 1);
	ASSERT(is_in_rect(&(objs->small), &r, 4, 4) == 1);
	ASSERT(is_in_rect(&(objs->small), &r, 5, 4) == 0);

	ASSERT(is_in_rect(&(objs->small), &r, 1, 5) == 0);
	ASSERT(is_in_rect(&(objs->small), &r, 2, 5) == 0);
	ASSERT(is_in_rect(&(objs->small), &r, 3, 5) == 0);
	ASSERT(is_in_rect(&(objs->small), &r, 4, 5) == 0);
	ASSERT(is_in_rect(&(objs->small), &r, 5, 5) == 0);
	
	
}
