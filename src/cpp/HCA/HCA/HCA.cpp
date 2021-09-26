// HCA Hill Climbing Algorithm
// Author: Matz Johansson B
//

#include "stdafx.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"
#include <random>
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#include <iostream>

#include "math.h"
#include <algorithm>
#include <string>   
#include <time.h> 

using namespace std;

typedef struct {
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
	float metric; // for calculating fitness compared to original image
} polyElement;


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
	// image is layed out in memory row-wise [rgba, rgba], [row 1, row 2, row 3,..., row height]
	// upper left corner is (0,0)
	int coordinate = (x + img_width*y)*channels; // works fine
	float alpha_image = *(img + coordinate + 3) / 255.0f;
	float alpha_added = a / 255.0f;

	if (channels == 4)
	{
		*(img + coordinate + 3) = (alpha_image + alpha_added*(1 - alpha_image))*255.0f; // alpha
	}

	*(img + coordinate) = min(alpha_image*(*(img + coordinate)) + (unsigned char)r*(1 - alpha_image), 255.0f); // red
	*(img + coordinate + 1) = min(alpha_image*(*(img + coordinate + 1)) + (unsigned char)g*(1 - alpha_image), 255.0f); // green
	*(img + coordinate + 2) = min(alpha_image*(*(img + coordinate + 2)) + (unsigned char)b*(1 - alpha_image), 255.0f); // blue

	for (int i = 0; i < 3; i++)
	{
		*(img + coordinate + i) = min(*(img + coordinate + i)*1.0f, 255.0f);
	}
}

// distance from xy to oxy
float distance(int x, int y, int ox, int oy)
{
	return sqrt(pow(x - ox, 2.0f) + pow(y - oy, 2.0f));
}

void add_disc(unsigned char *img, int channels,
	int img_width, int img_height, polyElement pe)
{
	int x = pe.x;
	int y = pe.y;
	int radius = pe.radius;
	int r = pe.col.r;
	int g = pe.col.g;
	int b = pe.col.b;
	int a = pe.col.a;

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


float diff(unsigned char *img, unsigned char*img2, int channels,
	int img_width, int img_height)
{
	float diff = 0;
	for (int x = 0; x < img_width; x++)
	{
		for (int y = 0; y < img_height; y++)
		{
			rgba col_a = get_pixel(img, channels, img_width, img_height, x, y);
			rgba col_b = get_pixel(img2, channels, img_width, img_height, x, y);
			float diff_now = fabs(col_a.r - col_b.r) / 255.0f + fabs(col_a.g - col_b.g) / 255.0f + fabs(col_a.b - col_b.b) / 255.0f + fabs(col_a.a - col_b.a) / 255.0f;
			diff += diff_now;
			//printf("(%d %d) %f\n", x, y, diff_now);
		}
	}
	return diff;
}



void diff_images(unsigned char *img, unsigned char*img2, unsigned char*img3, int channels,
	int img_width, int img_height)
{

	for (int x = 0; x < img_width; x++)
	{
		for (int y = 0; y < img_height; y++)
		{
			rgba col_a = get_pixel(img, channels, img_width, img_height, x, y);
			rgba col_b = get_pixel(img2, channels, img_width, img_height, x, y);
			rgba col_diff;
			int r = abs(col_a.r - col_b.r);
			int g = abs(col_a.g - col_b.g);
			int b = abs(col_a.b - col_b.b);
			int a = abs(col_a.a - col_b.a);

			add_pixel(img3, channels, img_width, img_height,
				x, y, r,
				g, b, a);
		}
	}
}


int main()
{
	int i = 0;
	char* ver = "0.0.2";
	printf("Hill climbing algorithm version %s\n", ver);

	//char* inputFilename = "C:\\Users\\Matz\\Desktop\\pd216-32.jpg";
	char* inputFilename = "C:\\Users\\Matz\\Desktop\\Mona.png";
	int template_width, template_height, template_channels;
	int png_channels = 4;
	int bpp = 8; // bits per pixel
	unsigned char *img_template = (unsigned char *)stbi_load(inputFilename, &template_width, &template_height, &template_channels, 0);
	unsigned char *img_tmp = (unsigned char*)calloc(template_width*template_height*png_channels*bpp, sizeof(unsigned char*));
	unsigned char *img_out = (unsigned char*)calloc(template_width*template_height*png_channels*bpp, sizeof(unsigned char*));
	unsigned char *img_diff = (unsigned char*)calloc(template_width*template_height*png_channels*bpp, sizeof(unsigned char*));

	unsigned char *img_empty = (unsigned char*)calloc(template_width*template_height*png_channels*bpp, sizeof(unsigned char*));


	if (img_template == NULL)
	{
		printf("Error in loading the image\n");
		std::cin >> i;
		exit(1);
	}

	/*
	setbuf(stdout, NULL);
	setbuf(stdin, NULL);
	*/
	// float d = diff(img_tmp, img_out, png_channels, template_width, template_height);

	/*
	polyElement pe;

	pe.x = 50;
	pe.y = 50;
	pe.radius = 200;
	pe.col.r = 255;
	pe.col.g = 50;
	pe.col.b = 5;

	add_disc(img_tmp, png_channels,
		template_width, template_height,
		pe);

	memcpy(img_out, img_tmp, template_width*template_width*png_channels*bpp* sizeof(unsigned char*));
	stbi_write_png("C:\\Users\\Matz\\Desktop\\output2.png", template_width, template_height, png_channels, img_out, template_width * png_channels);

	return 0;*/
	//printf("Loaded image with a width of %dpx, a height of %dpx and %d channels\n", template_width, template_height, template_channels);

	//todo: add as list of pe to output as svg
	srand(time(NULL));

	for (int i = 0; i < 500; i++) // add discs
	{
		polyElement pe_tmp;
		polyElement pe_best;
		pe_best.metric = 10000000000000;
		//img_tmp = (unsigned char*)calloc(template_width*template_width*png_channels*bpp, sizeof(unsigned char*));

		for (int j = 0; j < 100; j++) // test adding a disc
		{
			// add disc to img2, calculate diff with img
			// int x = j;
			// int y = i;
			int x = rand() % template_width;
			int y = rand() % template_height;

			int rad = 10 + rand() % 100;
			/*int red = rand() % 255;
			int green = rand() % 255;
			int blue = rand() % 255;*/
			int red, green, blue;
			red = green = blue = rand() % 255;
			int alpha = min(10 + rand() % 255, 255);
			pe_tmp.x = x;
			pe_tmp.y = y;
			pe_tmp.radius = rad;

			pe_tmp.col.r = red;
			pe_tmp.col.g = green;
			pe_tmp.col.b = blue;
			pe_tmp.col.a = alpha;

			// add disc to tmp to calculate if the added disc is better
			add_disc(img_tmp, png_channels,
				template_width, template_height,
				pe_tmp);

			float difference_metric = diff(img_template, img_tmp, png_channels,
				template_width, template_height);

			//img_diff = (unsigned char*)calloc(template_width*template_width*png_channels*bpp, sizeof(unsigned char*));
			//memset(img_diff, 0, template_height*template_width*png_channels*bpp* sizeof(unsigned char*));
			
			/*memcpy(img_diff, img_empty, template_width*template_height*png_channels*bpp * sizeof(unsigned char*));
			diff_images(img_template, img_tmp, img_diff, png_channels, template_width, template_height);
			stbi_write_png("C:\\Users\\Matz\\Desktop\\img_diff.png", template_width, template_height, png_channels, img_diff, template_width * png_channels);
*/

			/*
			*/

			//printf("diff: %f\n", difference_metric);
			memcpy(img_tmp, img_out, template_width*template_height*png_channels*bpp * sizeof(unsigned char*));

			if (difference_metric < pe_best.metric) // better diff
			{
				//printf("diff: %f\n", difference_metric);
				//printf("x: %d  y:%d %f\n", x, y, difference_metric);
				pe_best.x = x;
				pe_best.y = y;
				pe_best.radius = rad;
				pe_best.col.r = red;
				pe_best.col.g = green;
				pe_best.col.b = blue;
				pe_best.col.a = alpha;
				pe_best.metric = difference_metric;
			}
		}

		// add the best disc to img_out (pe)
		add_disc(img_out, png_channels,
			template_width, template_height,
			pe_best);

		printf("diff: %f\n", pe_best.metric);
		printf("added disc: %d\n", i);

		memcpy(img_tmp, img_out, template_height*template_width*png_channels*bpp * sizeof(unsigned char*));
		stbi_write_png("C:\\Users\\Matz\\Desktop\\output3.png", template_width, template_height, png_channels, img_out, template_width * png_channels);
		//stbi_write_png("C:\\Users\\Matz\\Desktop\\output_template_temp.png", template_width, template_height, png_channels, img_template, template_width * png_channels);

	}

	stbi_write_png("C:\\Users\\Matz\\Desktop\\output_new.png", template_width, template_height, png_channels, img_out, template_width * png_channels);

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



