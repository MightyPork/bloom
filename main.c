#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Xutil.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

typedef struct RGB_struct {
  uint8_t b;
  uint8_t g;
  uint8_t r;
} RGB;

int main()
{
  Display *display = XOpenDisplay(NULL);
  Window root = DefaultRootWindow(display);

  XWindowAttributes gwa;

  XGetWindowAttributes(display, root, &gwa);
  uint32_t width = gwa.width;
  uint32_t height = gwa.height;

  // The first image, to measure the screen and allocate the buffer
  XImage *image = XGetImage(display, root, 0, 0, width, height, AllPlanes, ZPixmap);   
  uint8_t array[width * height * 3];
  const uint32_t red_mask = image->red_mask;
  const uint32_t green_mask = image->green_mask;
  const uint32_t blue_mask = image->blue_mask;
  
  printf("mask: %p %p %p\nbyte_order: %d\nunit: %d\nbit_order: %d\npad: %d\ndepth: %d\nBPL: %d\nbPPx: %d\n", 
         red_mask, green_mask, blue_mask, image->byte_order, image->bitmap_unit, 
         image->bitmap_bit_order, image->bitmap_pad, image->depth, image->bytes_per_line, image->bits_per_pixel);

  union Color {
    uint32_t hex;
    RGB rgb;    
    struct {
      uint8_t b;
      uint8_t g;
      uint8_t r;
    };
  };
  
  // sampling steps
  const uint32_t xnum = 32;
  const uint32_t ynum = 24;
  const uint32_t xstep = width/xnum;
  const uint32_t ystep = height/ynum;
  
  // ideally we should downscale it using nearest-neighbour
  
  union Color *C;
  while (XGetSubImage(display, root, 0, 0, width, height, AllPlanes, ZPixmap, image, 0, 0)) {  
#define P_GOTO(x,y) C = ((union Color*)image->data + y*width + x);
    
    printf("\033[2J\033[H");
    for (int i = 0; i < ynum; i++) {
      printf("\n");
      for (int j = 0; j < xnum; j++) {
        P_GOTO(j*xstep, i*ystep);
        printf("\x1b[48;2;%d;%d;%dm  \x1b[m", C->r, C->g, C->b);
      }
    }
    
    usleep(100000);
  }

  return 0;
}

#if 0
typedef struct _XImage {
	int width, height;		/* size of image */
	int xoffset;			/* number of pixels offset in X direction */
	int format;			/* XYBitmap, XYPixmap, ZPixmap */
	char *data;			/* pointer to image data */
	int byte_order;			/* data byte order, LSBFirst, MSBFirst */
	int bitmap_unit;		/* quant. of scanline 8, 16, 32 */
	int bitmap_bit_order;		/* LSBFirst, MSBFirst */
	int bitmap_pad;			/* 8, 16, 32 either XY or ZPixmap */
	int depth;			/* depth of image */
	int bytes_per_line;		/* accelerator to next scanline */
	int bits_per_pixel;		/* bits per pixel (ZPixmap) */
	unsigned long red_mask;		/* bits in z arrangement */
	unsigned long green_mask;
	unsigned long blue_mask;
	XPointer obdata;		/* hook for the object routines to hang on */
	struct funcs {			/* image manipulation routines */
		struct _XImage *(*create_image)();
		int (*destroy_image)();
		unsigned long (*get_pixel)();
		int (*put_pixel)();
		struct _XImage *(*sub_image)();
		int (*add_pixel)();
	} f;
} XImage;
#endif

