// HCA Hill Climbing Algorithm
// Author: Matz Johansson B
// 27/9 2021

#include "stdafx.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"
#include <random>
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#include <iostream>

#include <cmath>
#include <algorithm>
#include <string>   
#include <time.h> 

#include <chrono>
#include <climits>
#include <omp.h>

//#include "matplotlibcpp.h"

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


rgba get_pixel(unsigned char* img, int channels, int img_width, int img_height, int x, int y)
{
    int coordinate = (x + img_width * y) * channels;
    rgba col;
    col.a = 0;
    col.r = *(img + coordinate);
    col.g = *(img + coordinate + 1);
    col.b = *(img + coordinate + 2);

    if (channels == 4)
    {
        col.a = *(img + coordinate + 3);
    }

    return col;
}

// change to put_pixel 
void add_pixel(unsigned char* img, int channels, int img_width, int img_height,
    int x, int y,
    int r, int g, int b, int a)
{
    // image is layed out in memory row-wise [rgba, rgba], [row 1, row 2, row 3,..., row height]
    // upper left corner is (0,0)
    int coordinate = (x + img_width * y) * channels; // works fine
    float a_disc = a / 255.0f;
    float a_img = *(img + coordinate + 3) / 255.0f;

    float rr = *(img + coordinate) * (1 - a_disc) + a_disc * r;
    float gg = *(img + coordinate + 1) * (1 - a_disc) + (a_disc)*g;
    float bb = *(img + coordinate + 2) * (1 - a_disc) + (a_disc)*b;

    *(img + coordinate) = int(rr);
    *(img + coordinate + 1) = int(gg);
    *(img + coordinate + 2) = int(bb);
    *(img + coordinate + 3) = 255.0f * (a_disc + a_img * (1 - a_disc));

}

// distance from xy to oxy
//6.7 s
float distance(int x, int y, int ox, int oy)
{
    return sqrt(pow(x - ox, 2.0f) + pow(y - oy, 2.0f));
}
//6.4 s
//float distance(int x, int y, int ox, int oy)
//{
//    return pow(x - ox, 2.0f) + pow(y - oy, 2.0f);
//}

//5.03 s, diamond shaped "manhattan distance" circles
//float distance(int x, int y, int ox, int oy)
//{
//    return abs(x - ox) + abs(y - oy);
//}


void add_disc(unsigned char* img, int channels,
    int img_width, int img_height, polyElement pe)
{
    int x = pe.x;
    int y = pe.y;
    int radius = pe.radius;
    int r = pe.col.r;
    int g = pe.col.g;
    int b = pe.col.b;
    int a = pe.col.a;

    for (int j = y - radius; j < y + radius; j++)
    {
        for (int i = x - radius; i < x + radius; i++)
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

//
//// maybe add argument to add disc and only focusing on that part of the image for diff
//float diff(unsigned char* img, unsigned char* img2, int channels,
//	int img_width, int img_height)
//{
//	float diff = 0;
//	for (int y = 0; y < img_height; y++)
//	{
//		for (int x = 0; x < img_width; x++)
//		{
//			rgba col_a = get_pixel(img, channels, img_width, img_height, x, y);
//			rgba col_b = get_pixel(img2, channels, img_width, img_height, x, y);
//
//			float bn = col_b.a / 255.0;
//			float rr = col_a.r * (1-bn) + ( bn) * col_b.r;
//			float gg = col_a.g * (1-bn) +  (bn) * col_b.g;
//			float bb = col_a.b * (1-bn) + ( bn) * col_b.b;
//
//			diff += fabs(col_a.r - col_b.r) + fabs(col_a.g - col_b.g) + fabs(col_a.b - col_b.b);
//
//			//printf("(%d %d) %f\n", x, y, diff);
//		}
//	}
//
//	return diff;
//}
//
//
//void diff_images(unsigned char* img, unsigned char* img2, unsigned char* img3, int channels,
//	int img_width, int img_height)
//{
//
//	for (int x = 0; x < img_width; x++)
//	{
//		for (int y = 0; y < img_height; y++)
//		{
//			rgba col_a = get_pixel(img, channels, img_width, img_height, x, y);
//			rgba col_b = get_pixel(img2, channels, img_width, img_height, x, y);
//			rgba col_diff;
//			int r = abs(col_a.r - col_b.r);
//			int g = abs(col_a.g - col_b.g);
//			int b = abs(col_a.b - col_b.b);
//			int a = abs(col_a.a - col_b.a);
//
//			add_pixel(img3, channels, img_width, img_height,
//				x, y, r,
//				g, b, a);
//		}
//	}
//}

float add_disc_diff_opt(unsigned char* img_in,
    unsigned char* img_buff,
    int channels,
    polyElement pe_tmp,
    int width,
    int height)
{
    float diff = 0.0f;

    const int cx = pe_tmp.x;
    const int cy = pe_tmp.y;
    const int radius = pe_tmp.radius;
    const int radius2 = radius * radius;

    const float an = pe_tmp.col.a / 255.0f;
    const float inv_an = 1.0f - an;

    const float disc_r = (float)pe_tmp.col.r;
    const float disc_g = (float)pe_tmp.col.g;
    const float disc_b = (float)pe_tmp.col.b;

    const int min_x = max(0, cx - radius);
    const int max_x = min(width - 1, cx + radius - 1);
    const int min_y = max(0, cy - radius);
    const int max_y = min(height - 1, cy + radius - 1);

    for (int j = min_y; j <= max_y; ++j)
    {
        const int dy = j - cy;
        const int dy2 = dy * dy;
        const int row = j * width;

        for (int i = min_x; i <= max_x; ++i)
        {
            const int dx = i - cx;

            if (dx * dx + dy2 >= radius2)
                continue;

            const int coordinate = (row + i) * channels;

            const float in_r = (float)img_in[coordinate];
            const float in_g = (float)img_in[coordinate + 1];
            const float in_b = (float)img_in[coordinate + 2];

            const float buff_r = (float)img_buff[coordinate];
            const float buff_g = (float)img_buff[coordinate + 1];
            const float buff_b = (float)img_buff[coordinate + 2];

            const float tmp_r = buff_r * inv_an + disc_r * an;
            const float tmp_g = buff_g * inv_an + disc_g * an;
            const float tmp_b = buff_b * inv_an + disc_b * an;

            const float dr_old = in_r - buff_r;
            const float dg_old = in_g - buff_g;
            const float db_old = in_b - buff_b;

            const float dr_new = in_r - tmp_r;
            const float dg_new = in_g - tmp_g;
            const float db_new = in_b - tmp_b;

            const float old_err = dr_old * dr_old +
                dg_old * dg_old +
                db_old * db_old;

            const float new_err = dr_new * dr_new +
                dg_new * dg_new +
                db_new * db_new;

            diff += (new_err - old_err);
        }
    }

    return diff;
}

float add_disc_diff_opt_mono(unsigned char* img_in,
    unsigned char* img_buff,
    int channels,
    polyElement pe_tmp,
    int width,
    int height,
    float best_err)
{
    float diff = 0.0f;

    const int cx = pe_tmp.x;
    const int cy = pe_tmp.y;
    const int radius = pe_tmp.radius;
    const int radius2 = radius * radius;

    const float an = pe_tmp.col.a / 255.0f;
    const float inv_an = 1.0f - an;

    const float disc_r = (float)pe_tmp.col.r;
    const float disc_g = (float)pe_tmp.col.g;
    const float disc_b = (float)pe_tmp.col.b;

    const int min_x = max(0, cx - radius);
    const int max_x = min(width - 1, cx + radius - 1);
    const int min_y = max(0, cy - radius);
    const int max_y = min(height - 1, cy + radius - 1);

    for (int j = min_y; j <= max_y; ++j)
    {
        const int dy = j - cy;
        const int dy2 = dy * dy;
        const int row = j * width;

        for (int i = min_x; i <= max_x; ++i)
        {
            const int dx = i - cx;
            if (dx * dx + dy2 >= radius2)
                continue;

            const int coordinate = (row + i) * channels;

            const float in_r = (float)img_in[coordinate];
            const float in_g = (float)img_in[coordinate + 1];
            const float in_b = (float)img_in[coordinate + 2];

            const float buff_r = (float)img_buff[coordinate];
            const float buff_g = (float)img_buff[coordinate + 1];
            const float buff_b = (float)img_buff[coordinate + 2];

            const float tmp_r = buff_r * inv_an + disc_r * an;
            const float tmp_g = buff_g * inv_an + disc_g * an;
            const float tmp_b = buff_b * inv_an + disc_b * an;

            const float dr_new = in_r - tmp_r;
            const float dg_new = in_g - tmp_g;
            const float db_new = in_b - tmp_b;

            const float new_err = dr_new * dr_new + dg_new * dg_new + db_new * db_new;

            diff += new_err;

            if (diff >= best_err)
                return diff;
        }
    }

    return diff;
}

// 21 seconds
float add_disc_diff(unsigned char* img_in, unsigned char* img_buff, int channels, polyElement pe_tmp, int width, int height)
{
    float diff = 0;
    for (int i = pe_tmp.x - pe_tmp.radius; i < pe_tmp.x + pe_tmp.radius; i++)
    {
        for (int j = pe_tmp.y - pe_tmp.radius; j < pe_tmp.y + pe_tmp.radius; j++)
        {
            if (i >= 0 && j >= 0 && i < width && j < height) // boundary check
            {
                rgba col_in = get_pixel(img_in, channels, width, height, i, j);
                rgba col_buff = get_pixel(img_buff, channels, width, height, i, j);
                rgba col_tmp;
                float an = pe_tmp.col.a / 255.0f;
                // fix this

                if (distance(pe_tmp.x, pe_tmp.y, i, j) < pe_tmp.radius)
                {
                    col_tmp.r = col_buff.r * (1 - an) + an * pe_tmp.col.r;
                    col_tmp.g = col_buff.g * (1 - an) + an * pe_tmp.col.g;
                    col_tmp.b = col_buff.b * (1 - an) + an * pe_tmp.col.b;
                }
                else //outside of disc but getting image template
                {
                    continue;
                }


                float old_err =
                    fabs((float)col_in.r - col_buff.r) +
                    fabs((float)col_in.g - col_buff.g) +
                    fabs((float)col_in.b - col_buff.b);

                float new_err =
                    fabs((float)col_in.r - col_tmp.r) +
                    fabs((float)col_in.g - col_tmp.g) +
                    fabs((float)col_in.b - col_tmp.b);

                diff += (new_err - old_err);


                //diff += fabs(col_in.r - col_tmp.r) + fabs(col_in.g - col_tmp.g) + fabs(col_in.b - col_tmp.b);
            }
        }
    }
    return diff;
}


int random(int min, int max) //range : [min, max]
{
    static bool first = true;
    if (first)
    {
        srand(time(NULL)); //seeding for the first time only!
        first = false;
    }
    return min + rand() % ((max + 1) - min);
}


inline int random_int(int min, int max)
{
    return min + rand() % (max - min + 1);
}

//13.5 to 4.8 sec with openMP 2.8 times faster
// 4 threads: 3.5 sec
// 8 threads 2.8 sec
// 2 threads 6.4 sec
// 1 thread 13.5 sec
// 1000 iterations, 200 attempts per iteration, image size around 512x512

int main()
{

    //omp_set_num_threads(4);

#pragma omp parallel
    {
#pragma omp single
        {
            printf("Threads running: %d\n", omp_get_num_threads());
        }
    }
    int ndiscs = 50000;
    int i = 0;
    char* ver = "0.0.4";
    printf("Hill climbing algorithm version %s\n", ver);

    char* inputFilename = "C:\\Temp\\Einar.png";
    //char* inputFilename = "C:\\temp\\hierro.png";

    int template_width, template_height, template_channels;
    int png_channels = 4;
    int bpp = 1; // bits per pixel?

    //ignore template_channels
    unsigned char* img_template = (unsigned char*) stbi_load(inputFilename, &template_width, &template_height, &template_channels, 4);
    unsigned char* img_tmp = (unsigned char*) calloc(template_width * template_height * png_channels * bpp, sizeof(unsigned char));
    //unsigned char *img_tmp = (unsigned char*) calloc(template_width*template_height*png_channels*bpp, sizeof(unsigned char*));
    unsigned char* img_out = (unsigned char*) calloc(template_width * template_height * png_channels * bpp, sizeof(unsigned char));

    template_channels = 4; // force 4 channels for output and calculations, ignore alpha in template if it exists
    printf("template channels: %d\n", template_channels);

    if (template_width * template_height * png_channels * bpp > INT_MAX)
    {
        printf("Image is too large\n");
        return 1;
    }

    if (img_template == NULL)
    {
        printf("Error in loading the image\n");
        std::cin >> i;
        return 1;
    }

    auto start = std::chrono::high_resolution_clock::now();
    //todo: add as list of pe to output as svg
    srand(time(NULL));
    float difference_metric = 1e30;

    memset(img_tmp, 255, template_width * template_height * 4);
    int x, y;
    int red, green, blue;
    int radius;
    float best_metric_global = 1e30;
    polyElement pe_tmp = polyElement();
    //diff_disc_opt is assuming monotonic increasing error, this is only the case if radii is fixed
    std::uniform_int_distribution<int> rdist(5, 200);
    

    for (int i = ndiscs; i > 0; i--) // add discs
    {
        int k = 5; // minimum radius
        int radius = k + 200*(float(i) / float(ndiscs));
        // radius decreases as we add more discs, start with big discs to get rough shape and then smaller discs to get details 

        polyElement pe_best = {};

#pragma omp parallel
        {
            polyElement pe_best_local;
            pe_best_local.metric = 1e30f;
            pe_best.metric = 1e30;

            std::mt19937 rng((unsigned int)time(NULL) + 1337u * (unsigned int)omp_get_thread_num());

            std::uniform_int_distribution<int> xdist(0, template_width - 1);
            std::uniform_int_distribution<int> ydist(0, template_height - 1);
            std::uniform_int_distribution<int> cdist(0, 254);
            std::uniform_int_distribution<int> adist(0, 254);
            //radius = 5 + xdist(rng) + ydist(rng);

            //8.2 s
            //48 s with ompenmp
            int alpha;
            int attempts = 0;
#pragma omp for nowait

            // test just cycling colors
            for (int j = 0; j < 1000; j++) // test adding a disc
            {
                /*x = random_int(0, template_width);
                y = random_int(0, template_height);

                radius = random_int(4, 50);
                alpha = random_int(50, 255);
                red = random_int(0, 255);
                green = random_int(0, 255);
                blue = random_int(0, 255);
                pe_tmp.x = x;
                pe_tmp.y = y;

                pe_tmp.radius = radius;

                pe_tmp.col.r = red;
                pe_tmp.col.g = green;
                pe_tmp.col.b = blue;
                pe_tmp.col.a = alpha;*/

                polyElement pe_tmp = {};

                pe_tmp.x = xdist(rng);
                pe_tmp.y = ydist(rng);

                pe_tmp.col.r = (unsigned char)cdist(rng);
                pe_tmp.col.g = (unsigned char)cdist(rng);
                pe_tmp.col.b = (unsigned char)cdist(rng);
                pe_tmp.col.a = (unsigned char)adist(rng);
                //pe_tmp.radius = rdist(rng);
                pe_tmp.radius = radius;



                // add disc to tmp to calculate if the added disc is better
                //test_fun(pe_tmp);
                pe_tmp.metric = add_disc_diff_opt(img_template, img_tmp, template_channels, pe_tmp, template_width, template_height);
                //pe_tmp.metric = add_disc_diff_opt_mono(img_template, img_tmp, template_channels, pe_tmp, template_width, template_height, pe_best_local.metric);


                if (pe_tmp.metric < pe_best_local.metric)
                {
                    pe_best_local = pe_tmp;
                }
            }

#pragma omp critical
            {
                if (pe_best_local.metric < pe_best.metric)
                {
                    pe_best = pe_best_local;
                }
            }
        }

        // add the best disc to img_out (pe)
        add_disc(img_tmp, png_channels,
            template_width, template_height,
            pe_best);

           if (i % 100 == 0)
           {
               stbi_write_png("C:\\temp\\output_tmp.png", template_width, template_height,
                   png_channels, img_tmp, template_width * png_channels);
           }


    }

    auto finish = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "Elapsed time: " << elapsed.count() << " s\n";

    stbi_write_png("C:\\temp\\output_tmp.png", template_width, template_height, png_channels, img_tmp, template_width * png_channels);

    printf("enter a number to quit\n");
    std::cin >> i;

    return 0;
}

