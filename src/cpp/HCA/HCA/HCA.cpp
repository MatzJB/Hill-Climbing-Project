// HCA.cpp : Defines the entry point for the console application.
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
	float old_alpha = *(img + coordinate + 3) / 255.0;
	float alpha = a / 255.0f;

	if (channels == 4)
	{
		//*(img + coordinate + 3) = *(img + coordinate + 3)*(1-alpha)*255 + (unsigned char) alpha*255; // alpha
		*(img + coordinate + 3) = *(img + coordinate + 3); // alpha
	}
	
	*(img + coordinate) = max(*(img + coordinate)*old_alpha + (unsigned char)r*(1-old_alpha), 255.0f); // r
	*(img + coordinate+1) = max(*(img + coordinate+1)*old_alpha*a + (unsigned char)g*(1-old_alpha), 255.0f); // g
	*(img + coordinate+2) = max(*(img + coordinate+2)*old_alpha*a + (unsigned char)b*(1-old_alpha), 255.0f); // b

	//*(img + coordinate) = *(img + coordinate)*alpha;
	//*(img + coordinate+1) = *(img + coordinate+1)*alpha;
	//*(img + coordinate+2) = *(img + coordinate+2)*alpha;


	//*(img + coordinate+2) = (unsigned char)b*(a / 255.0); // red
	//*(img + coordinate + 1) = (unsigned char)g; // green
	//*(img + coordinate + 2) = (unsigned char)b; // blue  
														//add comp
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
		std::cin >> i;
		exit(1);
	}

	printf("Loaded image with a width of %dpx, a height of %dpx and %d channels\n", template_width, template_height, template_channels);


	//for (int i = 0; i < 100; i++) // add discs
	{

		//for (int j = 0; j < 100; j++) //
		{
			// add disc


		/*	int d = diff(img, img2, template_channels,
				template_width, template_height);
*/
				int x = rand() % template_width;
			int y = rand() % template_height;
			int r = rand() % 100;
			int red = rand() % 255;
			int green = rand() % 255;
			int blue = rand() % 255;
			r = 75;
			red = 255;
			green = 0;
			blue = 0;

			add_disc(img, template_channels,
				template_width, template_height,
				100, 100,
				r, 0, 0,
				red, green, blue, 250);


			add_disc(img, template_channels,
				template_width, template_height,
				200, 100,
				r, 0, 0,
				red, green, blue, 250);

		}

		// add best disc to image

	}
	//add_pixel(img, template_channels, template_width, template_height, template_width-1, template_height-1, 255, 0, 0, 255);

	stbi_write_png("C:\\Users\\Matz\\Desktop\\output.png", template_width, template_height, template_channels, img, template_width * template_channels);



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



