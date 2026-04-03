// HCA Hill Climbing Algorithm
// Author: Matz Johansson B

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
#include <cmath>
#include <format>
#include <filesystem>

using namespace std;

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} rgba;


typedef struct {
    rgba col;
    float radius;
    int rotation;
    int x;
    int y;
    float metric; // for calculating fitness compared to original image
} polyElement;

/*
TODO:
Add Ellipsoids
Add a list of stacked polygons, test to remove each disc, only remove if it does not contribute to the final image.

*/


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

float distance(int x, int y, int ox, int oy)
{
    return sqrt(pow(x - ox, 2.0f) + pow(y - oy, 2.0f));
}

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

int main()
{
    char* ver = "0.0.5";

    int ndiscs = 100000;
    int k = 8000; // trials
    char* name = "mario";

    int template_width, template_height, template_channels;

    std::uniform_real_distribution<float> rdist(0.5f, 20.0f);
   
    std::uniform_int_distribution<int> cdist(0, 255);
    std::uniform_int_distribution<int> adist(80, 80);

    float rMin = rdist.a();
    float rMax = rdist.b();
    int   aMin = adist.a();
    int   aMax = adist.b();

    char inputFilename[256];
    char outputName[256];

    snprintf(inputFilename, sizeof(inputFilename), "C:\\temp\\%s.png", name);
    snprintf(outputName, sizeof(outputName), "%s d %d k %d r %.1f %.1f a %d %d.png",
        name, ndiscs, k, rMin, rMax, aMin, aMax);

    std::filesystem::path outputFilename = std::filesystem::path(inputFilename).parent_path() / outputName;

#pragma omp parallel
    {
#pragma omp single
        {

            printf("Hill Climber: %s\n", ver);
            printf("Threads running: %d\n", omp_get_num_threads());
            printf("Number of discs: %d\n", ndiscs);
            printf("Trials per disc: %d\n", k);
            printf("Radius range: %.1f - %.1f\n", rMin, rMax);
            printf("Alpha range: %d - %d\n", aMin, aMax);
            printf("Output file: %s\n", outputFilename);
        }
    }



    int png_channels = 4;
    int bpp = 1;

    //ignore template_channels
    stbi_uc* img_template = stbi_load(inputFilename, &template_width, &template_height, &template_channels, 4);
    unsigned char* img_tmp = (unsigned char*)calloc(template_width * template_height * png_channels * bpp, sizeof(unsigned char));
    unsigned char* img_out = (unsigned char*)calloc(template_width * template_height * png_channels * bpp, sizeof(unsigned char));

    std::uniform_int_distribution<int> xdist(0, template_width - 1);
    std::uniform_int_distribution<int> ydist(0, template_height - 1);
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
       
        return 1;
    }

    auto start = std::chrono::high_resolution_clock::now();
    //todo: add as list of pe to output as svg
    srand(time(NULL));
    float difference_metric = 1e30;

    memset(img_tmp, 255, template_width * template_height * 4);
    int x, y;
    int red, green, blue;
    float radius;
    float best_metric_global = 1e30;

    polyElement pe_tmp = polyElement();
    std::mt19937 rng((unsigned int)time(NULL) + 1337u * (unsigned int)omp_get_thread_num());

    //diff_disc_opt is assuming monotonic increasing error, this is only the case if radii is fixed

    for (int i = ndiscs; i > 0; i--) // add discs
    {
        polyElement pe_best = {};

#pragma omp parallel
        {
            polyElement pe_best_local;
            pe_best_local.metric = 1e30f;
            pe_best.metric = 1e30;


            int alpha;
            int attempts = 0;
#pragma omp for nowait

            for (int j = 0; j < k; j++)
            {
                polyElement pe_tmp = {};

                pe_tmp.x = xdist(rng);
                pe_tmp.y = ydist(rng);

                pe_tmp.col.r = (unsigned char)cdist(rng);
                pe_tmp.col.g = (unsigned char)cdist(rng);
                pe_tmp.col.b = (unsigned char)cdist(rng);
                //pe_tmp.col.a = (unsigned char)adist(rng);
                pe_tmp.col.a = (unsigned char) 80; // 80 is good

                pe_tmp.radius = rdist(rng);
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

        if (i % (ndiscs/10) == 0)
        {
            stbi_write_png(outputFilename.string().c_str(), template_width, template_height,
                png_channels, img_tmp, template_width * png_channels);
            printf("%d discs were written\n", ndiscs-i);
        }
    }

    auto finish = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "Elapsed time: " << elapsed.count() << " s\n";

    stbi_write_png(outputFilename.string().c_str(), template_width, template_height, png_channels, img_tmp, template_width * png_channels);

    printf("enter a number to quit\n");
    int input;
    cin >> input;
    return 0;
}

