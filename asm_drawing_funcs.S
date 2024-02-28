/*
 * Assembly-language implementations of the drawing functions
 * valgrind -s --track-origins=yes ./asm_test_drawing_funcs
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
  cmpl %esi, (%r13)
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
		popq %r12
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
  imull (%rdi), %edx
  addl %edx, %esi
  movl %esi, %eax
  popq %r13
  ret


	/*not implemented becasue not necessary for MS2*/
  .globl clamp
clamp:
  ret

  .globl get_r
get_r:
  movl %edi, %eax
  shr $24, %eax
  and $0xFF, %eax
  ret

  .globl get_g
get_g:
  movl %edi, %eax
  shr $16, %eax
  and $0xFF, %eax
  ret

  .globl get_b
get_b:
  movl %edi, %eax
  shr $8, %eax
  and $0xFF, %eax
  ret

  .globl get_a
get_a:
  movl %edi, %eax
  and $0xFF, %eax
  ret

/*not implemented becasue not necessary for MS2*/
  .globl set_Nth_bit
set_Nth_bit:
  ret

/*not implemented becasue not necessary for MS2*/
  .globl get_Nth_bit
get_Nth_bit:
  ret

/*
  using a different implementation than MS1 C version becasue
  we realized there was a much better method to set rgba values
*/
  .globl set_r
set_r:
  pushq %r13
  pushq %r14

  movq $0, %r13
  movq $0, %r14
  /*sets the bits for red to 0*/
  movb %sil, %r13b
  movq $255, %r14
  shl $24, %r14
  not %r14d
  and %edi, %r14d

  /*sets the bits for red to the desired value*/
  shl $24, %r13
  or %r13, %r14

  movq %r14, %rax
  popq %r14
  popq %r13
	ret

  .globl set_g
set_g:
  pushq %r13
  pushq %r14

  movq $0, %r13
  movq $0, %r14
  /*sets the bits for green to 0*/
  movb %sil, %r13b
  movq $255, %r14
  shl $16, %r14
  not %r14d
  and %edi, %r14d
  
  /*sets the bits for green to the desired value*/
  shl $16, %r13
  or %r13, %r14

  movq %r14, %rax
  popq %r14
  popq %r13
	ret

  .globl set_b
set_b:
  pushq %r13
  pushq %r14
  movq $0, %r13
  movq $0, %r14
  /*sets the bits for blue to 0*/
  movb %sil, %r13b
  movq $255, %r14
  shl $8, %r14
  not %r14d
  and %edi, %r14d
  
  /*sets the bits for blue to the desired value*/
  shl $8, %r13
  or %r13, %r14

  movq %r14, %rax
  popq %r14
  popq %r13
	ret

  .globl set_a
set_a:
  pushq %r13
  pushq %r14
  movq $0, %r13
  movq $0, %r14
  /*sets the bits for opacity to 0*/
  movb %sil, %r13b
  movq $255, %r14
  not %r14d
  and %edi, %r14d

  /*sets the bits for opacity to the desired value*/
  or %r13, %r14

  movq %r14, %rax
  popq %r14
  popq %r13
	ret

  .globl blend_color
blend_color:
  /*
  input = fg, bg, opacity
  fg -> %dil
  bg -> %sil
  opacity -> %dl
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
  pushq %r15
  pushq %r15

  

  movq $0, %rbx
  movb %dil, %bl
  movzbq %dl, %r14
  
  /*opacity * fg*/
  imulq %r14, %rbx

  /*subtraction part of (255-opacity) * bg*/
  movq $0, %rbp
  movb %r14b, %bpl
  movq $255, %r15
  sub %bpl, %r15b
  movb %r15b, %bpl

  /*multiplication part of (255-opacity) * bg*/
  movzbq %sil, %r12
  imulq %rbp, %r12

  /*opacity scaled fg + opacity scaled bg*/
  addq %rbx, %r12
  movq $255, %rbp

  /*divide (scaled fg + scaled bg) by 255*/
  movq $0, %rdx
  movq %r12, %rax
  idivq %rbp

  popq %r15
  popq %r15
  popq %r14
  popq %r13
  popq %r12
  popq %rbp
  popq %rbx

	ret

  .globl make_color
make_color:
	/* %dil (%rdi) = r
  *  %sil (%rsi) = g
  *  %dl (%rdx) = b
  */ 
  pushq %r12
  pushq %r13
  pushq %r14
  pushq %r15

  movq $0, %r12
  movq $0, %r13
  movq $0, %r14
  movq $0, %r15

  /*r12 - new color
    r13 - r
    r14 - g
    r15 - b*/
  movb %dil, %r13b
  movb %sil, %r14b
  movb %dl, %r15b

  /*sets the r component of the new color*/
  movq $0, %rdi
  movq $0, %rsi
  movl %r12d, %edi
  movb %r13b, %sil
  call set_r
  movl %eax, %r12d

  /*sets the g component of the new color*/
  movq $0, %rdi
  movq $0, %rsi
  movb %r14b, %sil
  movl %r12d, %edi
  call set_g
  movl %eax, %r12d
  
  /*sets the b component of the new color*/
  movq $0, %rdi
  movq $0, %rsi
  movb %r15b, %sil
  movl %r12d, %edi
  call set_b
  movl %eax, %r12d

  /*sets the a component of the new color*/
  movq $0, %rdi
  movq $0, %rsi
  movb $255, %sil
  movl %r12d, %edi
  call set_a

  popq %r15
  popq %r14
  popq %r13
  popq %r12

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

  /*
  zeroing registers so that when setting registers using 
  <64 bits, the value accessed at higher precisions
  will be the same value
  */
  movq $0, %rbx
  movq $0, %rbp
  movq $0, %r12
  movq $0, %r13
  movq $0, %r14
  movq $0, %r15

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
  movb %r13b, %dil
  movb %r14b, %sil
  movb %r12b, %dl
  call blend_color
  movb %al, %r13b


  /*get g value of fg*/
  movl %ebx, %edi
  call get_g
  movb %al, %r14b

  /*gets g value of bg*/
  movl %ebp, %edi
  call get_g
  movb %al, %r15b

  /*blends the g values*/
  movb %r14b, %dil
  movb %r15b, %sil
  movb %r12b, %dl
  call blend_color
  movb %al, %r14b


  /*get b value of fg*/
  movl %ebx, %edi
  call get_b
  movb %al, %r15b

  /*gets b value of bg*/
  movl %ebp, %edi
  call get_b
  movb %al, %bl

  /*blends the b values*/
  movb %r15b, %dil
  movb %bl, %sil
  movb %r12b, %dl
  call blend_color
  movb %al, %r15b

  /*move color components into arg registers and combine them*/
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
  /*input - # to square
  in %rdi - 64 bit
  */
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
	pushq %r14

	/*x1-x2*/
  movq %rdi, %r12
  subq %rdx, %r12

	/*y1-y2*/
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

	popq %r14
  popq %r13
  popq %r12
	ret

  .globl is_in_circle
is_in_circle:
	pushq %r13
	pushq %r14
	movq %r9, %r13 /*saves radius values in r13*/

	/*lines up parameters for square_dist function*/
	movq %rsi, %rdi
	movq %rdx, %rsi
	movq %rcx, %rdx
	movq %r8, %rcx

	call square_dist
	movq $0, %r14
	movl %eax, %r14d

	movq %r13, %rdi
	call square
	cmp %eax, %r14d
	jg .LnotInCircle
	movq $1, %rax
	jmp .LcircleCheckEnd
	
	.LnotInCircle:
	movq $0, %rax

	.LcircleCheckEnd:
	popq %r14
	popq %r13

	ret


  .globl is_in_range
is_in_range:
	/* edi = val
		edi = min
		edx = max
	*/
	movq $0, %rax  /* default to returning false */
	cmp %edi, %esi /*compare val to min*/
	jg .LendBoundsCheck
	cmp %edi, %edx  /* compare val to max */
	jl .LendBoundsCheck
	movq $1, %rax 

	.LendBoundsCheck:
	ret

  .globl is_in_rect
is_in_rect:
	pushq %rbx
	pushq %rbp
	pushq %r12 
	pushq %r13
	pushq %r14

	movq %rdi, %rbx /* store img* */
	movq %rsi, %rbp /* store rect pointer */
	movq %rdx, %r12 /* store x */
	movq %rcx, %r13 /* store y */

	movl RECT_X_OFFSET(%rbp), %r10d /* get rect->x */
	movl RECT_WIDTH_OFFSET(%rbp), %edx /* get rect->width */
	subl $1, %edx /* subtract one form rect->width */
	addl %r10d, %edx /* (rect->x + rect->width - 1) into third argument */

	movl %r12d, %edi /* move x into first argument */
	movl RECT_X_OFFSET(%rbp), %esi/* move rect->x into second armugent */
	call is_in_range /* checks if x is inside the rectangle */
	cmp $0, %rax
	je .LnotInRect

	movl RECT_Y_OFFSET(%rbp), %r10d /* get rect->y */
	movl RECT_HEIGHT_OFFSET(%rbp), %edx /* get rect->height */
	subl $1, %edx /* subtract one form rect->height */
	addl %r10d, %edx /* (rect->y + rect->height - 1) into third argument */

	movl %r13d, %edi /* move y into first argument */
	movl RECT_Y_OFFSET(%rbp), %esi/* move rect->y into second armugent */
	call is_in_range /* checks if y is inside the rectangle */
	cmp $0, %rax
	je .LnotInRect
	movq $1, %rax

	.LnotInRect:
	popq %r14
	popq %r13
	popQ %r12
	popq %rbp
	popq %rbx

	ret

  .globl rect_in_img
rect_in_img:
	pushq %rbx
	pushq %rbp
	pushq %r12 
	pushq %r13
	pushq %r14
 /*  */
	movq %rdi, %rbx /* store img* */
	movq %rsi, %rbp /* store rect* */

	movl IMAGE_WIDTH_OFFSET(%rbp), %edx
	subl RECT_WIDTH_OFFSET(%rbp), %edx

	movl RECT_X_OFFSET(%rbp), %edi
	movq $0, %rsi
	cmp $0, %rax
	je .LrectNotInImage


	movl IMAGE_HEIGHT_OFFSET(%rbp), %edx
	subl RECT_HEIGHT_OFFSET(%rbp), %edx

	movl RECT_Y_OFFSET(%rbp), %edi
	movq $0, %rsi
	cmp $0, %rax
	je .LrectNotInImage
	movq $1, %rax


	.LrectNotInImage:
	popq %r14
	popq %r13
	popQ %r12
	popq %rbp
	popq %rbx
	ret

/*not implemented becasue not necessary for MS2*/
  .globl draw_pixel_no_blending
draw_pixel_no_blending:
/* r12 stores img*
  *  r13 stores color value
  *  r14 stores index
  *  rbx stores x
  *  rbp stores y*/
  pushq %r12
  pushq %r13
  pushq %r14
  pushq %rbx
  pushq %rbp

  movq $0, %r14

  /*store arguments in preserved registers*/
	movq %rdi, %r12
	movq %rsi, %rbx
  movq %rdx, %rbp
	movq %rcx, %r13


  /*check if x y coord is in bounds of img*/
  movl %eax, %r14d
  call in_bounds
  cmp $0, %eax
  je .LDrawReturn

  /*gets index from xy*/
  movq %r12, %rdi
  movq %rbx, %rsi
  movq %rdx, %rbp
  call compute_index
  movq %rax, %r14

  /*sets pixel at correct index to new color*/
  movq %r12, %rdi /* set img* to first argument*/
	movl %r14d, %esi /* set index to first argument*/
	movl %eax, %edx /* set color to first argument*/
  call set_pixel


  .LDrawReturn:
  popq %rbp
  popq %rbx
  popq %r14
  popq %r13
  popq %r12
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
  /* r12 stores img*
  *  r13 stores color value
  *  r14 stores index
  *  rbx stores x
  *  rbp stores y*/
  pushq %r12
  pushq %r13
  pushq %r14
  pushq %rbx
  pushq %rbp

  movq $0, %r13
  movq $0, %r14

  /*store arguments in preserved registers*/
  movq %rsi, %rbx
  movq %rdx, %rbp
  movq %rdi, %r12
  movl %ecx, %r13d

  /*check if x y coord is in bounds of img*/
  movl %eax, %r14d
  call in_bounds
  cmp $0, %eax
  je .LDrawNBReturn

  /*gets index from xy*/
  movq %r12, %rdi
  movq %rbx, %rsi
  movq %rdx, %rbp
  call compute_index
  movq %rax, %r14
  /*rbx and rbp are now freed up 
    as we dont need x or y anymore*/

  /*get new color*/
  movq IMAGE_DATA_OFFSET(%r12), %rbx
  movl (%rbx, %r14, 4), %esi

  movl %r13d, %edi

  call blend_colors

  /*sets pixel at correct index to new color*/
  movl %eax, %edx
  movq %r12, %rdi
  movl %r14d, %esi
  call set_pixel


  .LDrawNBReturn:
  popq %rbp
  popq %rbx
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
  /* rbx -> img pointer
		 rbp -> rect pointer
		 r12 -> color
		 r13 -> i counter
		 r14 -> j counter

	*/
	 /*  */


	pushq %rbx
	pushq %rbp
	pushq %r12
	pushq %r13
	pushq %r14


	movq %rdi, %rbx /* store img* */
	movq %rsi, %rbp /* store rect* */
	movq %rdx, %r12 /* store color*/

	movq $0, %r13 /*initialize i counter*/
	movq $0, %r14 /*initialize j counter*/


	jmp .LRectCheckOuterLoopCondition /* enter the loop */

	.LRectUpdateOuterLoopCondition:
	add $1, %r13  /* i ++ */
	movq $0, %r14  /* set j = 0 */
	.LRectCheckOuterLoopCondition:
	cmpl %r13d, IMAGE_WIDTH_OFFSET(%rbx)  /* check if image width <= i */
	jle .LRectEnd
	jmp .LRectCheckInnerLoopCondition
		.LRectUpdateInnerLoopCondition:
		addq $1, %r14 /* j++ */
		.LRectCheckInnerLoopCondition:
		cmpl %r14d, IMAGE_HEIGHT_OFFSET(%rbx) /* check if image height <= j */
		jle .LRectUpdateOuterLoopCondition
			movq %rbx, %rdi  /* move img* into first argument */
			movq %rbp, %rsi  /* move rect into second argument */
			movq %r13, %rdx  /* move i into third argument */
			movq %r14, %rcx  /* move j into fourth argument */
			call is_in_rect
			cmp $0, %rax /* check if is_in_rect returned false(0) */
			je .LRectUpdateInnerLoopCondition
				movq %rbx, %rdi  /* move img* into first argument */
				movq %r13, %rsi /* move i into second argument */
				movq %r14, %rdx /* move j into third argument */
				movl %r12d, %ecx /* move color into fourth argument */
				call draw_pixel
				jmp .LRectUpdateInnerLoopCondition /* go back to next iteration of inner loop */

	.LRectEnd:
	popq %r14
	popq %r13
	popq %r12
	popq %rbp
	popq %rbx
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
  /* pseudo code for how control flow of loop works */
	/*initialize outer loop stuff*/
	/*i condition*/
	/*fails, jump to return*/
	/*out loop body*/
		/*initialize inner loop variables*/
		/*inner loop condition*/
		/*if it fails jump to outer loop update*/
		/*inner loop body*/
			/*check bounds*/
			/*if out of bounds jump to inner loop update*/
		/*update j and jump to inner loop condition*/
	/*update i, set j = 0 and jump to outer loop condition*/
	
	/* rbx -> img pointer
		 rbp -> used for stack allocation of variables
		 		radius and color stored on stack
		 r12 -> x
		 r13 -> y
		 r14 -> i counter
		 r15 -> j counter

		 stack:
		 -4(%rbp) = radius
		 -8(%rbp) = color
	*/
	 /*  */


	pushq %rbx
	pushq %r12
	pushq %r13
	pushq %r14
	pushq %r15

	pushq %rbp
	movq %rsp, %rbp 
	subq $8, %rsp

	movq %rdi, %rbx /* store img* */
	movq %rsi, %r12 /* store x */
	movq %rdx, %r13 /* store y*/

	movq $0, %r14 /*initialize i counter*/
	movq $0, %r15 /*initialize j counter*/
	movl %ecx, -4(%rbp) /*store the radius onto the stack */
	movl %r8d, -8(%rbp) /*store the color onto the stack */


	jmp .LcheckOuterLoopCondition /* enter the loop */

	.LupdateOuterLoopCondition:
	add $1, %r14  /* i ++ */
	movq $0, %r15  /* set j = 0 */
	.LcheckOuterLoopCondition:
	cmpl %r14d, IMAGE_WIDTH_OFFSET(%rbx)  /* check if image width <= i */
	jle .Lend
	jmp .LcheckInnerLoopCondition
		.LupdateInnerLoopCondition:
		addq $1, %r15 /* j++ */
		.LcheckInnerLoopCondition:
		cmpl %r15d, IMAGE_HEIGHT_OFFSET(%rbx) /* check if image height <= j */
		jle .LupdateOuterLoopCondition
		movq %rbx, %rdi  /* move img* into first argument */
		movq %r12, %rsi  /* move x into second argument */
		movq %r13, %rdx  /* move y into third argument */
		movq %r14, %rcx  /* move i into fourth argument */
		movq %r15, %r8  /* move j into fifth argument */
		movl -4(%rbp), %r9d  /* move radius into sixth argument */
		call is_in_circle
		cmp $0, %rax /* check if is_in_circle returned false(0) */
		je .LupdateInnerLoopCondition
		movq %rbx, %rdi  /* move img* into first argument */
		movq %r14, %rsi /* move i into second argument */
		movq %r15, %rdx /* move j into third argument */
		movl -8(%rbp), %ecx /* move color into fourth argument */
		call draw_pixel
		jmp .LupdateInnerLoopCondition /* go back to next iteration of inner loop */

	.Lend:

	addq $8, %rsp /* deallocate local variables on stack */
	popq %rbp


	popq %r15
	popq %r14
	popq %r13
	popq %r12
	popq %rbx


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
