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

  // TEST(test_set_Nth_bit);
  // TEST(test_get_Nth_bit);
  TEST(test_in_bounds);
  // TEST(test_compute_index);
  // TEST(test_clamp);
  // TEST(test_get_r);
  // TEST(test_get_g);
  // TEST(test_get_b);
  // TEST(test_get_a);
  // TEST(test_set_r);
  // TEST(test_set_g);
  // TEST(test_set_b);
  // TEST(test_set_a);
  // TEST(test_blend_color);
  // TEST(test_make_color);
  // TEST(test_blend_colors);
  // TEST(test_set_pixel);
  TEST(test_square);
  TEST(test_square_dist);
  // TEST(test_is_in_circle);
  // TEST(test_is_in_range);
  // TEST(test_is_in_rect);

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

  //past top left corner
  // ASSERT(in_bounds(&objs->small, -1, 0) == 0);
  // ASSERT(in_bounds(&objs->small, 0, -1) == 0);

  //past top right corner
  ASSERT(in_bounds(&objs->small, 0, 8) == 0);
  // ASSERT(in_bounds(&objs->small, 7, -1) == 0);

  //past bottom left corner
  ASSERT(in_bounds(&objs->small, 0, 6) == 0);
  // ASSERT(in_bounds(&objs->small, -1, 5) == 0);

  //past bottom right corner
  ASSERT(in_bounds(&objs->small, 8, 5) == 0);
  ASSERT(in_bounds(&objs->small, 7, 6) == 0);

  //check corners(in bounds)
  ASSERT(in_bounds(&objs->small, 0, 0) == 1);
  ASSERT(in_bounds(&objs->small, 7, 0) == 1);
  ASSERT(in_bounds(&objs->small, 0, 5) == 1);
  ASSERT(in_bounds(&objs->small, 7, 5) == 1);

  //random points in the middle
  ASSERT(in_bounds(&objs->small, 3, 5) == 1);
  ASSERT(in_bounds(&objs->small, 5, 4) == 1);
  ASSERT(in_bounds(&objs->small, 2, 2) == 1);



}

void test_compute_index(TestObjs *objs){
  
  for(int i = 0; i < SMALL_W; i++){
    for(int j = 0; j < SMALL_H; j++){
      ASSERT(compute_index(&objs->small, i, j) == SMALL_IDX(i, j));
    }
  }

  int counter = 0;
  for(int i = 0; i < LARGE_H; i++){
    for(int j = 0; j < LARGE_W; j++){
      ASSERT(compute_index(&objs->large, j, i) == counter);
      counter++;
    }
  }
  
}

void test_clamp(TestObjs *objs){
	ASSERT(clamp(90, 88, 95) == 90);
	ASSERT(clamp(87, 88, 95) == 88);
	ASSERT(clamp(97, 88, 95) == 95);
	ASSERT(clamp(97, 95, 95) == 95);
	ASSERT(clamp(93, 95, 95) == 95);
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
  uint32_t color1 = 0xefeae2ff;
  uint32_t color2 = 0x100000ff;
  uint32_t color3 = 0x264c80ff;
  uint32_t color4 = 0x314e90ff;

  ASSERT(set_r(color1, 0x12) == 0x12eae2ff);
  ASSERT(set_r(color2, 0x12) == 0x120000ff);
  ASSERT(set_r(color3, 0x12) == 0x124c80ff);
  ASSERT(set_r(color4, 0x12) == 0x124e90ff);

  ASSERT(set_r(color1, 0x00) == 0x00eae2ff);
  ASSERT(set_r(color2, 0x00) == 0x000000ff);
  ASSERT(set_r(color3, 0x00) == 0x004c80ff);
  ASSERT(set_r(color4, 0x00) == 0x004e90ff);

  ASSERT(set_r(color1, 0xFF) == 0xFFeae2ff);
  ASSERT(set_r(color2, 0xFF) == 0xFF0000ff);
  ASSERT(set_r(color3, 0xFF) == 0xFF4c80ff);
  ASSERT(set_r(color4, 0xFF) == 0xFF4e90ff);

  ASSERT(set_r(color1, 0x80) == 0x80eae2ff);
  ASSERT(set_r(color2, 0x80) == 0x800000ff);
  ASSERT(set_r(color3, 0x80) == 0x804c80ff);
  ASSERT(set_r(color4, 0x80) == 0x804e90ff);

  ASSERT(set_r(color1, 0x05) == 0x05eae2ff);
  ASSERT(set_r(color2, 0x05) == 0x050000ff);
  ASSERT(set_r(color3, 0x05) == 0x054c80ff);
  ASSERT(set_r(color4, 0x05) == 0x054e90ff);

}

void test_set_g(TestObjs *objs){

  uint32_t color1 = 0xefeae2ff;
  uint32_t color2 = 0x100000ff;
  uint32_t color3 = 0x264c80ff;
  uint32_t color4 = 0x314e90ff;

  ASSERT(set_g(color1, 0x12) == 0xef12e2ff);
  ASSERT(set_g(color2, 0x12) == 0x101200ff);
  ASSERT(set_g(color3, 0x12) == 0x261280ff);
  ASSERT(set_g(color4, 0x12) == 0x311290ff);

  ASSERT(set_g(color1, 0x00) == 0xef00e2ff);
  ASSERT(set_g(color2, 0x00) == 0x100000ff);
  ASSERT(set_g(color3, 0x00) == 0x260080ff);
  ASSERT(set_g(color4, 0x00) == 0x310090ff);

  ASSERT(set_g(color1, 0xFF) == 0xefFFe2ff);
  ASSERT(set_g(color2, 0xFF) == 0x10FF00ff);
  ASSERT(set_g(color3, 0xFF) == 0x26FF80ff);
  ASSERT(set_g(color4, 0xFF) == 0x31FF90ff);

  ASSERT(set_g(color1, 0x80) == 0xef80e2ff);
  ASSERT(set_g(color2, 0x80) == 0x108000ff);
  ASSERT(set_g(color3, 0x80) == 0x268080ff);
  ASSERT(set_g(color4, 0x80) == 0x318090ff);

  ASSERT(set_g(color1, 0x05) == 0xef05e2ff);
  ASSERT(set_g(color2, 0x05) == 0x100500ff);
  ASSERT(set_g(color3, 0x05) == 0x260580ff);
  ASSERT(set_g(color4, 0x05) == 0x310590ff);
}

void test_set_b(TestObjs *objs){
  
  uint32_t color1 = 0xefeae2ff;
  uint32_t color2 = 0x100000ff;
  uint32_t color3 = 0x264c80ff;
  uint32_t color4 = 0x314e90ff;

  ASSERT(set_b(color1, 0x12) == 0xefea12ff);
  ASSERT(set_b(color2, 0x12) == 0x100012ff);
  ASSERT(set_b(color3, 0x12) == 0x264c12ff);
  ASSERT(set_b(color4, 0x12) == 0x314e12ff);

  ASSERT(set_b(color1, 0x00) == 0xefea00ff);
  ASSERT(set_b(color2, 0x00) == 0x100000ff);
  ASSERT(set_b(color3, 0x00) == 0x264c00ff);
  ASSERT(set_b(color4, 0x00) == 0x314e00ff);

  ASSERT(set_b(color1, 0xFF) == 0xefeaFFff);
  ASSERT(set_b(color2, 0xFF) == 0x1000FFff);
  ASSERT(set_b(color3, 0xFF) == 0x264cFFff);
  ASSERT(set_b(color4, 0xFF) == 0x314eFFff);

  ASSERT(set_b(color1, 0x80) == 0xefea80ff);
  ASSERT(set_b(color2, 0x80) == 0x100080ff);
  ASSERT(set_b(color3, 0x80) == 0x264c80ff);
  ASSERT(set_b(color4, 0x80) == 0x314e80ff);

  ASSERT(set_b(color1, 0x05) == 0xefea05ff);
  ASSERT(set_b(color2, 0x05) == 0x100005ff);
  ASSERT(set_b(color3, 0x05) == 0x264c05ff);
  ASSERT(set_b(color4, 0x05) == 0x314e05ff);
}

void test_set_a(TestObjs *objs){
  
  uint32_t color1 = 0xefeae2ff;
  uint32_t color2 = 0x100000ff;
  uint32_t color3 = 0x264c80ff;
  uint32_t color4 = 0x314e90ff;

  ASSERT(set_a(color1, 0x12) == 0xefeae212);
  ASSERT(set_a(color2, 0x12) == 0x10000012);
  ASSERT(set_a(color3, 0x12) == 0x264c8012);
  ASSERT(set_a(color4, 0x12) == 0x314e9012);

  ASSERT(set_a(color1, 0x00) == 0xefeae200);
  ASSERT(set_a(color2, 0x00) == 0x10000000);
  ASSERT(set_a(color3, 0x00) == 0x264c8000);
  ASSERT(set_a(color4, 0x00) == 0x314e9000);

  ASSERT(set_a(color1, 0xFF) == 0xefeae2FF);
  ASSERT(set_a(color2, 0xFF) == 0x100000FF);
  ASSERT(set_a(color3, 0xFF) == 0x264c80FF);
  ASSERT(set_a(color4, 0xFF) == 0x314e90FF);

  ASSERT(set_a(color1, 0x80) == 0xefeae280);
  ASSERT(set_a(color2, 0x80) == 0x10000080);
  ASSERT(set_a(color3, 0x80) == 0x264c8080);
  ASSERT(set_a(color4, 0x80) == 0x314e9080);

  ASSERT(set_a(color1, 0x05) == 0xefeae205);
  ASSERT(set_a(color2, 0x05) == 0x10000005);
  ASSERT(set_a(color3, 0x05) == 0x264c8005);
  ASSERT(set_a(color4, 0x05) == 0x314e9005);
}

void test_blend_color(TestObjs *objs) {
	ASSERT(blend_color(0xff, 0xee, 0) == 0xee);
	ASSERT(blend_color(0x5c, 0x11, 0x1f) == 26);
	ASSERT(blend_color(0x26, 0x47, 0x93) == 51);
	ASSERT(blend_color(0x43, 0xe1, 0x1f) == 205);
}

void test_blend_colors(TestObjs *objs) {
	ASSERT(blend_colors(0x43110b1f, 0xe1121100) == 0xcd1110ff);
}

void test_make_color(TestObjs *objs){
  ASSERT(make_color(0xff, 0x34, 0xfe) == 0xff34feff);
}

void test_set_pixel(TestObjs *objs){
  set_pixel(&objs->small, 8, 0x95748364);
  ASSERT(objs->small.data[8] == 0x95748364);

  set_pixel(&objs->small, 8, 0x95748364);
  ASSERT(objs->small.data[8] == 0x95748364);  
  
  set_pixel(&objs->small, 8, 0x95748364);
  ASSERT(objs->small.data[8] == 0x95748364);  
  
  set_pixel(&objs->small, 8, 0x95748364);
  ASSERT(objs->small.data[8] == 0x95748364);  
  
  set_pixel(&objs->small, 8, 0x95748364);
  ASSERT(objs->small.data[8] == 0x95748364);  
  
  set_pixel(&objs->small, 8, 0x95748364);
  ASSERT(objs->small.data[8] == 0x95748364);  
  
  set_pixel(&objs->small, 8, 0x95748364);
  ASSERT(objs->small.data[8] == 0x95748364);  
  
  set_pixel(&objs->small, 8, 0x95748364);
  ASSERT(objs->small.data[8] == 0x95748364);
  
}

void test_square(TestObjs *objs){
  ASSERT(square(2) == 4);
  ASSERT(square(5) == 25);
  ASSERT(square(19) == 361);
  
}

void test_square_dist(TestObjs *objs){

  ASSERT(square_dist(4, 6, 1, 2) == 25);
  ASSERT(square_dist(0, 0, 3, 4) == 25);
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

  uint32_t min = 8;
  uint32_t max = 40;
  for(int i = 0; i < 80; i++){
    ASSERT(is_in_range(i, min, max) == (i >= min && i <= max) ? 1 : 0);
  }
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