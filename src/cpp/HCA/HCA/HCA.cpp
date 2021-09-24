// HCA.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"

#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#include <iostream>

#include "math.h"

using namespace std;

struct rgba {
	(unsigned char)r;
	(unsigned char)g;
	(unsigned char)b;
	(unsigned char)a;
};

rgba get_pixel(unsigned char *img, int channels, int img_width, int img_height, int x, int y)
{

	int coordinate = (x + img_width*y)*channels;

	rgba col;

	col.r = *(img + coordinate);
	col.g = *(img + coordinate + 1);
	col.b = *(img + coordinate + 2);


	if (channels == 4)
	{
		col.a = *(img + coordinate + 3);
	}

	return col;
}

void add_pixel(unsigned char *img, int channels, int img_width, int img_height, int x, int y, int r, int g, int b, int a)
{
	// assume image is layed out in memory row-wise [rgba, rgba], [row 1, row 2, row 3,..., row height]
	// upper left corner is (0,0)

	int coordinate = (x + img_width*y)*channels; // works

	*(img + coordinate) = (unsigned char)r; // red
	*(img + coordinate + 1) = (unsigned char)g; // green
	*(img + coordinate + 2) = (unsigned char)b; // blue  

	if (channels == 4)
	{
		*(img + coordinate + 2) = (unsigned char)a; // alpha
	}

}

// squared distance from xy to oxy
float distance(int x, int y, int ox, int oy)
{
	return sqrt(pow(x - ox, 2.0f) + pow(y - oy, 2.0f));
}

void add_disc(unsigned char *img, int channels,
	int img_width, int img_height,
	int x, int y,
	int radius, int n, int rotation,
	int r, int g, int b, int a)
{
	for (int i = x - radius; i < x + radius; i++)
	{
		for (int j = y - radius; j < y + radius; j++)
		{
			if (i < img_width && i > 0 && j > 0 && j < img_height)
			{
				if (distance(x, y, i, j) < radius)
				{

					add_pixel(img, channels, img_width, img_height, i, j, r, g, b, a);
				}
			}
		}
	}
}


int diff(unsigned char *img, unsigned char*img2, int channels,
	int img_width, int img_height)
{
	int diff = 100000;
	for (int x = 0; x < img_width; x++)
	{
		for (int y = 0; y < img_height; y++)
		{
			rgba col_a = get_pixel(img, channels, img_width, img_height, x, y);
			rgba col_b = get_pixel(img2, channels, img_width, img_height, x, y);
			diff += (int)(col_a.r - col_b.r + col_a.g - col_b.g + col_a.b - col_b.b);
		}
	}
	return diff;
}



int main()
{
	int i = 0;
	char* ver = "0.0.1";
	printf("Hill climbing algorithm version %s", ver);
	//cin >> i;

	int template_width, template_height, template_channels;
	//unsigned char *img = (unsigned char *) stbi_load("input.png", &template_width, &template_height, &template_channels, 0);

	unsigned char *img = (unsigned char *)stbi_load("C:\\Users\\Matz\\Desktop\\blank_canvas.png", &template_width, &template_height, &template_channels, 0);



	if (img == NULL)
	{
		printf("Error in loading the image\n");
		cin >> i;
		exit(1);
	}

	printf("Loaded image with a width of %dpx, a height of %dpx and %d channels\n", template_width, template_height, template_channels);

	/*
		int shape_width, shape_height, shape_channels;
		unsigned char *shp = stbi_load("D:\github\Hill-Climbing-Project\src\cpp\HCA\HCA\triangle.png", &shape_width, &shape_height, &shape_channels, 0);

		if (shp == NULL)
		{
			printf("Error in loading the image\n");
			cin >> i;
			exit(1);
		}

		printf("Loaded image with a width of %dpx, a height of %dpx and %d channels\n", shape_width, shape_height, shape_channels);

		int channels = 4;
		int width = 1000;
		int height = 1000;
		unsigned char *canvas = (unsigned char*) malloc(width*height*channels*sizeof(unsigned char));

	*/


	add_disc(img, template_channels,
		template_width, template_height,
		160, 160,
		70, 0, 0,
		255, 0, 0, 255);
	//add_pixel(img, template_channels, template_width, template_height, template_width-1, template_height-1, 255, 0, 0, 255);


	stbi_write_png("C:\\Users\\Matz\\Desktop\\output.png", template_width, template_height, template_channels, img, template_width * template_channels);


	//printf("enter a number to quit\n");
	//cin >> i;


	return 0;
}

//// add a gray image and modify its color, rotation and transparency
//void add_image(unsigned char *img, unsigned char *shp, int channels, int img_width, int img_height, int x, int y, int radius, int n, int rotation, int r, int g, int b)
//{
//
//
//
//
//}



