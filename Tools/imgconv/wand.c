#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <wand/MagickWand.h>

uint16_t rgb888torgb565(uint8_t red, uint8_t green, uint8_t blue)
{
    uint16_t b = (blue >> 3) & 0x1f;
    uint16_t g = ((green >> 2) & 0x3f) << 5;
    uint16_t r = ((red >> 3) & 0x1f) << 11;

    uint16_t res = (uint16_t) (r | g | b);
    uint16_t res_swap = (((res & 0xFF) << 8) | (res >> 8));
    return res_swap;
}

int main(int argc,char **argv)
{
#define QuantumScale  ((MagickRealType) 1.0/(MagickRealType) QuantumRange)
#define SigmoidalContrast(x) \
  (QuantumRange*(1.0/(1+exp(10.0*(0.5-QuantumScale*x)))-0.0066928509)*1.0092503)
#define ThrowWandException(wand) \
{ \
  char \
    *description; \
 \
  ExceptionType \
    severity; \
 \
  description=MagickGetException(wand,&severity); \
  (void) fprintf(stderr,"%s %s %lu %s\n",GetMagickModule(),description); \
  description=(char *) MagickRelinquishMemory(description); \
  exit(-1); \
}

  long
    y;

  MagickBooleanType
    status;

  MagickPixelPacket
    pixel;

  MagickWand
    *image_wand;

  PixelIterator
    *iterator;

  PixelWand
    **pixels;

  register long
    x;

  unsigned long
    width;

  int first = 0;

  if (argc != 2)
    {
      (void) fprintf(stdout,"Usage: %s image\n",argv[0]);
      exit(0);
    }
  /*
    Read an image.
  */
  MagickWandGenesis();
  image_wand=NewMagickWand();
  status=MagickReadImage(image_wand,argv[1]);
  if (status == MagickFalse)
    ThrowWandException(image_wand);

  PixelWand *background;
  background=NewPixelWand();
  PixelSetColor(background,"#000000");

  status=MagickRotateImage(image_wand,background,-90.0);
  if (status == MagickFalse)
    ThrowWandException(image_wand);

  /*
    Sigmoidal non-linearity contrast control.
  */
  iterator=NewPixelIterator(image_wand);
  if ((iterator == (PixelIterator *) NULL))
    ThrowWandException(image_wand);

  char buffer[80];
  snprintf(buffer, 80, "%s.c",argv[1]);

  FILE *fp = fopen(buffer,"w");


  fprintf(fp,"#include \"stdint.h\"\n");
  fprintf(fp,"uint16_t usImg[] = {\n");

  for (y=0; y < (long) MagickGetImageHeight(image_wand); y++)
  {
    pixels=PixelGetNextIteratorRow(iterator,&width);
    if ((pixels == (PixelWand **) NULL))
      break;

    for (x=0; x < (long) width; x++)
    {
      PixelGetMagickColor(pixels[x],&pixel);
      if (first == 0)
      {
	first = 1;
      }
      else
      {
	fprintf(fp,",");
        if (first%10==0)
          fprintf(fp,"\n");
      }
      fprintf(fp,"0x%04X", rgb888torgb565((uint8_t)pixel.red,(uint8_t)pixel.green,(uint8_t)pixel.blue));
      first++;
    }
  }
  if (y < (long) MagickGetImageHeight(image_wand))
    ThrowWandException(image_wand);

  fprintf(fp,"};\n");

  iterator=DestroyPixelIterator(iterator);
  image_wand=DestroyMagickWand(image_wand);
  MagickWandTerminus();
  fclose(fp);
  return(0);
}
