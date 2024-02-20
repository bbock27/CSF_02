/*
 * Assembly-language implementations of the drawing functions
 */

/* Offsets of struct Image fields */
#define IMAGE_WIDTH_OFFSET   0
#define IMAGE_HEIGHT_OFFSET  4
#define IMAGE_DATA_OFFSET    8

/* Offsets of struct Rect fields */
#define RECT_X_OFFSET        0
#define RECT_Y_OFFSET        4
#define RECT_WIDTH_OFFSET    8
#define RECT_HEIGHT_OFFSET   12

	.section .text

/***********************************************************************
   Helper functions
   You should declare these as .globl so that
   they can be called from the unit test code.
 ***********************************************************************/

/* TODO: implement your helper functions here */

	.globl in_bounds
in_bounds:
	/* %rdi = img*
	*  %rsi = x
	*  %rdx = y
	* defaults to returning 0
	*/ 
	pushq %r13
	pushq %r12
	movl $0, %eax

	movq %rdi, %r13
	/*if out of bounds, jump to .LReturn
	need to imolement jumps but not sure which one to use
	width - x*/
	cmp %esi, (%r13d)
	jle .LReturn
	cmp $0, %esi
	jl .LReturn
	movl IMAGE_HEIGHT_OFFSET(%rdi), %r13d
	movl %r13d, %r12d
	cmp %edx, %r13d
	jle .LReturn
	cmp $0, %edx
	jl .LReturn

	//if in bounds
	movq $1, %rax

.LReturn:
	popq %r13
	popq %r13
	ret

	.globl compute_index
compute_index:
	/* %rdi = img*
	*  %rsi = x
	*  %rdx = y
	* defaults to returning 0
	*/ 

	pushq %r13

	imull (%edi), %edx
	addl %edx, %esi
	movl %esi, %eax
	popq %r13
	ret
	.globl clamp
clamp:
	ret

	.globl get_r
get_r:
ret

	.globl get_g
get_g:
ret

	.globl get_b
get_b:
ret

	.globl get_a
get_a:
ret

	.globl set_Nth_bit
set_Nth_bit:
ret

	.globl get_Nth_bit
get_Nth_bit:
ret

	.globl set_r
set_r:
ret

	.globl set_g
set_g:
ret

	.globl set_b
set_b:
ret

	.globl set_a
set_a:
ret

	.globl blend_color
blend_color:
ret

	.globl make_color
make_color:
ret

	.globl blend_colors
blend_colors:
ret

	.globl set_pixel
set_pixel:
ret

	.globl square
square:
	//input - # to square
	//in %rdi - 64 bit
	movq %rdi, %rax
	mulq %rax
	ret

	.globl square_dist
square_dist:
	/* %rdi = x1
	*  %rsi = y1
	*  %rdx = x2
	*  %rcx = y2
	*/ 

	//stack alignment
	pushq %r12 
	pushq %r13

	//need to handle underflow somehow
	//right now only works if x1>x2 and y1>y2
	movq %rdi, %r12
	subq %rdx, %r12
	movq %rsi, %r13
	subq %rcx, %r13
	movq %r12, %rdi
	call square
	movq %rax, %r12
	movq %r13, %rdi
	call square
	movq %rax, %r13
	addq %r13, %r12
	movq %r12, %rax
	popq %r13
	popq %r12
	ret

	.globl is_in_circle
is_in_circle:
ret

	.globl is_in_range
is_in_range:
ret

	.globl is_in_rect
is_in_rect:
ret

	.globl rect_in_img
rect_in_img:
ret

	.globl draw_pixel_no_blending
draw_pixel_no_blending:
ret

/***********************************************************************
   Public API functions
 ***********************************************************************/

/*
 * Draw a pixel.
 *
 * Parameters:
 *   %rdi     - pointer to struct Image
 *   %esi     - x coordinate (pixel column)
 *   %edx     - y coordinate (pixel row)
 *   %ecx     - uint32_t color value
 */
	.globl draw_pixel
draw_pixel:
	/* TODO: implement */
	/* r12 stores img*
	*  r13 stores color value
	*  r14 stores index*/
	pushq %r12
	pushq %r13
	pushq %r14

	movq %rdi, %r12
	movl %ecx, %r13d

	movl %eax, %r14d
	call in_bounds
	cmp %eax, $0
	je .LDrawReturn

	


	


	

	.LDrawReturn:
	//realign stack
	popq %r14
	popq %r13
	popq %r12
	ret
/*
 * Draw a rectangle.
 * The rectangle has rect->x,rect->y as its upper left corner,
 * is rect->width pixels wide, and rect->height pixels high.
 *
 * Parameters:
 *   %rdi     - pointer to struct Image
 *   %rsi     - pointer to struct Rect
 *   %edx     - uint32_t color value
 */
	.globl draw_rect
draw_rect:
	/* TODO: implement */
	ret

/*
 * Draw a circle.
 * The circle has x,y as its center and has r as its radius.
 *
 * Parameters:
 *   %rdi     - pointer to struct Image
 *   %esi     - x coordinate of circle's center
 *   %edx     - y coordinate of circle's center
 *   %ecx     - radius of circle
 *   %r8d     - uint32_t color value
 */
	.globl draw_circle
draw_circle:
	/* TODO: implement */
	ret

/*
 * Draw a tile by copying all pixels in the region
 * enclosed by the tile parameter in the tilemap image
 * to the specified x/y coordinates of the destination image.
 * No blending of the tile pixel colors with the background
 * colors should be done.
 *
 * Parameters:
 *   %rdi - pointer to Image (dest image)
 *   %esi - x coordinate of location where tile should be copied
 *   %edx - y coordinate of location where tile should be copied
 *   %rcx - pointer to Image (the tilemap)
 *   %r8  - pointer to Rect (the tile)
 */
	.globl draw_tile
draw_tile:
	/* TODO: implement */
	ret

/*
 * Draw a sprite by copying all pixels in the region
 * enclosed by the sprite parameter in the spritemap image
 * to the specified x/y coordinates of the destination image.
 * The alpha values of the sprite pixels should be used to
 * blend the sprite pixel colors with the background
 * pixel colors.
 *
 * Parameters:
 *   %rdi - pointer to Image (dest image)
 *   %esi - x coordinate of location where sprite should be copied
 *   %edx - y coordinate of location where sprite should be copied
 *   %rcx - pointer to Image (the spritemap)
 *   %r8  - pointer to Rect (the sprite)
 */
	.globl draw_sprite
draw_sprite:
	/* TODO: implement */
	ret

/*
vim:ft=gas:
*/
