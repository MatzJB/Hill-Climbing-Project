// HCA Hill Climbing Algorithm
// Author: Matz Johansson B
//

#include "stdafx.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"


#include<random>
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#include <iostream>

#include "math.h"
#include <algorithm>

using namespace std;

typedef struct{
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
} rgba;

typedef struct {
	rgba col;
	int radius;
	int rotation;
	int x;
	int y;
	int metric; // for calculating fitness compared to original image
} polyElement;

rgba get_pixel(unsigned char *img, int channels, int img_width, int img_height, int x, int y)
{
	int coordinate = (x + img_width*y)*channels;

	rgba col;

	if (channels == 4)
	{
		col.a = *(img + coordinate + 3);
	}

	col.r = *(img + coordinate);
	col.g = *(img + coordinate + 1);
	col.b = *(img + coordinate + 2);

	return col;
}

void add_pixel(unsigned char *img, int channels, int img_width, int img_height, int x, int y, int r, int g, int b, int a)
{
	// image is layed out in memory row-wise [rgba, rgba], [row 1, row 2, row 3,..., row height]
	// upper left corner is (0,0)
	int coordinate = (x + img_width*y)*channels; // works fine
	float alpha_image = *(img + coordinate + 3) / 255.0f;
	float alpha_added = a / 255.0f;

	if (channels == 4)
	{
		*(img + coordinate + 3) = (alpha_image + alpha_added*(1 - alpha_image))*255.0f; // alpha
		//*(img + coordinate + 3) = (unsigned char) min(alpha_added*255.0f + 255.0f*(1-alpha_added), 255.0f); // alpha
	}

	*(img + coordinate) = min(alpha_image*( *(img + coordinate)) + (unsigned char) r*(1-alpha_image), 255.0f); // red
	*(img + coordinate + 1) = min(alpha_image*(*(img + coordinate+1)) + (unsigned char)g*(1-alpha_image), 255.0f);
	*(img + coordinate + 2) = min(alpha_image*(*(img + coordinate+2)) + (unsigned char)b*(1-alpha_image), 255.0f);

	for (int i = 0; i < 3; i++)
	{
		*(img + coordinate + i) = min(*(img + coordinate + i)*1.0f, 255.0f);
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
			if (i < img_width && i >= 0 && j >= 0 && j < img_height)
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
			diff += (int)abs(col_a.r - col_b.r + col_a.g - col_b.g + col_a.b - col_b.b);
		}
	}
	return diff;
}

int main()
{
	int i = 0;
	char* ver = "0.0.2";
	printf("Hill climbing algorithm version %s", ver);

	int template_width, template_height, template_channels;
	int png_channels = 4;
	int bpp = 4*8;
	unsigned char *img_template = (unsigned char *) stbi_load("C:\\Users\\Matz\\Desktop\\pd216-32.jpg", &template_width, &template_height, &template_channels, 0);
	unsigned char *img_tmp = (unsigned char*) calloc(template_width*template_width*png_channels*bpp, sizeof(unsigned char*));
	unsigned char *img_out = (unsigned char*) calloc(template_width*template_width*png_channels*bpp, sizeof(unsigned char*));

	if (img_template == NULL)
	{
		printf("Error in loading the image\n");
		std::cin >> i;
		exit(1);
	}

	printf("Loaded image with a width of %dpx, a height of %dpx and %d channels\n", template_width, template_height, template_channels);

	polyElement pe;
	//todo: add as list of pe to output as svg
	
	
	for (int i = 0; i < 100; i++) // add discs
	{
		//for (int j = 0; j < 100; j++) // test adding a disc
		//{
			// add disc to img2, calculate diff with img
			int x = rand() % template_width;
			int y = rand() % template_height;
			int r = 20 + rand() % 100;
			int red = rand() % 255;
			int green = rand() % 255;
			int blue = rand() % 255;

			add_disc(img_out, png_channels,
				template_width, template_height,
				x, y,
				r, 0, 0,
				red, green, blue, 200);
		/*	int d = diff(img, img2, template_channels,
				template_width, template_height);*/

			//if (d < pe.metric) // better diff
			//{

			//}
		
	/*	}*/
	/*	add_disc(img, template_channels,
			template_width, template_height,
			x, y,
			r, 0, 0,
			red, green, blue, 200);*/

		// add best disc to image

	}
	//add_pixel(img, template_channels, template_width, template_height, template_width-1, template_height-1, 255, 0, 0, 255);

	stbi_write_png("C:\\Users\\Matz\\Desktop\\output2.png", template_width, template_height, png_channels, img_out, template_width * png_channels);



	printf("enter a number to quit\n");
	std::cin >> i;


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



