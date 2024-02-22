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
    movl %edi, %eax
    shr %eax, $24
    and %eax, $0xFF
    ret

	.globl get_g
get_g:
    movl %edi, %eax
    shr %eax, $16
    and %eax, $0xFF
    ret

	.globl get_b
get_b:
    movl %edi, %eax
    shr %eax, $8
    and %eax, $0xFF
    ret

	.globl get_a
get_a:
    movl %edi, %eax
    and %eax, $0xFF
    ret

	.globl set_Nth_bit
set_Nth_bit:
ret

	.globl get_Nth_bit
get_Nth_bit:
ret

//return color | ((uint32_t)r_component << 24);
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
	/*
	input = fg, bg, opacity
	zero registers before use and treat colors as 64 bit
	then downcast to 8 bit on the return
	opacity * fg -> %rbx 
	255 - opacity -> %rbp
	%rbp * bg -> %r12
	%rbx + %r12 -> %r13
	%r13 / 255 -> %rax
	*/

	pushq %rbx
	pushq %rbp
	pushq %r12
	pushq %r13
	pushq %r14

	movb %dil, %bl
	movb %dl, %bl
	
	imulq %rbx, %rbx


ret

	.globl make_color
make_color:
/* %dil (%rdi) = r
	*  %sil (%rsi) = g
	*  %dl (%rdx) = b
	*/ 

	movl $0, %r13d
	movb %dil, %r13b
	movb %sil, %r14b
	movb %dil, %r15b

	movb %r13b, %sil
	movl %r13d, %edi
	call set_r
	movl %eax, %r13d

	movb %r14b, %sil
	movl %r13d, %edi
	call set_r
	movl %eax, %r13d

	movb %r15b, %sil
	movl %r13d, %edi
	call set_r
	movl %eax, %r13d

	movb $255, %sil
	movl %r13d, %edi
	call set_r

ret

	.globl blend_colors
blend_colors:
	pushq %rbx
	pushq %rbp
	pushq %r12
	pushq %r13
	pushq %r14
	pushq %r15
	pushq %r15

	/*saves fg and bg colors in register*/
	movl %edi, %ebx
	movl %esi, %ebp

	/*moves fg into first argument and gets the opacity*/
	movl %ebx, %edi
	call get_a
	movb %al, %r12b

	/*get r value of fg*/
	movl %ebx, %edi
	call get_r
	movb %al, %r13b

	/*gets r value of bg*/
	movl %ebp, %edi
	call get_r
	movb %al, %r14b

	/*blends the r values*/
	movb %r13b, %bl
	movb %r14b, %sil
	movb %r12b, %dl
	call blend_color
	movb %al, %r13b


	/*get g value of fg*/
	movl %ebx, %edi
	call get_r
	movb %al, %r14b

	/*gets g value of bg*/
	movl %ebp, %edi
	call get_r
	movb %al, %r15b

	/*blends the g values*/
	movb %r14b, %bl
	movb %r15b, %sil
	movb %r12b, %dl
	call blend_color
	movb %al, %r14b


	/*get b value of fg*/
	movl %ebx, %edi
	call get_r
	movb %al, %r15b

	/*gets b value of bg*/
	movl %ebp, %edi
	call get_r
	movb %al, %bl

	/*blends the b values*/
	movb %r15b, %bl
	movb %bl, %sil
	movb %r12b, %dl
	call blend_color
	movb %al, %r15b


	movb %r13b, %dil
	movb %r14b, %sil
	movb %r15b, %dl
	call make_color

	popq %r15
	popq %r15
	popq %r14
	popq %r13
	popq %r12
	popq %rbp
	popq %rbx

ret

	.globl set_pixel
set_pixel:
	pushq %r14
	movq IMAGE_DATA_OFFSET(%rdi), %rdi
	movl %edx, (%rdi, %rsi, 4)
	popq %r14
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
ret:

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
	cmp $0, %eax
	je .LDrawReturn

	movq %r12, %rdi



	
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
