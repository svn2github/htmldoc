//
// "png2fti.cxx"
//
// Simple utility which converts a PNG or XPM icon file to a 3D SGI FTI
// icon file.
//
// Copyright 2004 by Michael Sweet.
//

#include <stdio.h>
#include <FL/Fl_File_Icon.H>
#include <FL/Fl_Shared_Image.H>


int
get_color(const short *&data)
{
  const unsigned short	*temp;
  unsigned 		color,
			red,
			green,
			blue;


  temp  = (const unsigned short *)data;
  color = (temp[0] << 16) | temp[1];

  if (color > 255)
  {
    red   = (color >> 24) & 255;
    green = (color >> 16) & 255;
    blue  = (color >> 8) & 255;

    color = fl_color_cube(red * (FL_NUM_RED - 1) / 255,
                          green * (FL_NUM_GREEN - 1) / 255,
                          blue * (FL_NUM_BLUE - 1) / 255);
  }

  data += 2;

  return ((int)color);
}


int
main(int  argc,
     char *argv[])
{
  Fl_File_Icon	*fti;
  const short	*data;
  int		num_data;
  float		x, y;
  float		xmin, xmax, ymin, ymax;


  if (argc != 2)
  {
    puts("Usage: png2fti filename.png >filename.fti");
    return (1);
  }

  fl_register_images();

  fti = new Fl_File_Icon("", 0);
  if (fti->load_image(argv[1]))
  {
    perror(argv[1]);
    return (1);
  }

  num_data = fti->size();
  data     = fti->value();
  xmin     = 1000.0f;
  xmax     = -1000.0f;
  ymin     = 1000.0f;
  ymax     = -1000.0f;

  printf("# Converted from %s by png2fti...\n\n", argv[1]);

  while (num_data > 0)
  {
    switch (*data)
    {
      case Fl_File_Icon::END :
          puts("endpolygon();\n");
	  num_data --;
	  data ++;
          break;

      case Fl_File_Icon::COLOR :
	  num_data -= 3;
	  data ++;
          printf("color(%d);\n", get_color(data));
          break;

      case Fl_File_Icon::POLYGON :
          puts("bgnpolygon();\n");
	  num_data --;
	  data ++;
          break;

      case Fl_File_Icon::VERTEX :
	  num_data -= 3;
	  data ++;
          // Project x, y in image space to 3D (x = x, y = x + y), removing
	  // the offset added by load_image()...
	  x = *data++ * 0.01f - 10.0f;
	  y = *data++ * 0.01f - 5.0f;

	  y = (x * 0.7071 + y) / 1.414 + 2.5f;
          x = x / 1.414 + 19.0f;

          if (x < xmin)
	    xmin = x;
	  if (x > xmax)
	    xmax = x;

          if (y < ymin)
	    ymin = y;
	  if (y > ymax)
	    ymax = y;

          printf("\tvertex(%.2f, %.2f);\n", x, y);
          break;
    }
  }

  printf("# bounding box: [ %.2f %.2f %.2f %.2f ]\n", xmin, ymin, xmax, ymax);

  return (0);
}


//
// End of "png2fti.cxx".
//
