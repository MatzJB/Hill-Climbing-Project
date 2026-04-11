// HCA Hill Climbing Algorithm
// Author: Matz Johansson B

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <random>

#include "stb_image/stb_image_write.h"
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#include <omp.h>
#include <time.h>

#include <algorithm>
#include <cctype>
#include <chrono>
#include <climits>
#include <cmath>
#include <filesystem>
#include <format>
#include <glm/glm.hpp>
#include <iostream>
#include <map>
#include <string>
#include <thread>

#include "IPlot.h"
#include "rmse_plot.h"

using namespace std;

typedef struct {
  unsigned char r;
  unsigned char g;
  unsigned char b;
  unsigned char a;
} rgba;

enum class Shape { Rect, Disc, Ellipsoid };

typedef struct {
  Shape shape;
  rgba col;
  glm::vec2 radii; // Disc: radii.x = radius; Rect: (w, h); Ellipsoid: (a, b)
  int x;
  int y;
  float metric;
} polyElement;

struct HCAParameters {
  int d;
  int k;
  tuple<int, int> alphaRange;
  tuple<float, float> radiusRange;
  tuple<int, int> colorRange;
  Shape shape;
  float RMSError;
  string filename;
  double time;

  string toString() const {
    return "d " + to_string(d) + " k " + to_string(k) + " shape " +
           to_string((int)shape) + " alpha " + to_string(get<0>(alphaRange)) +
           "-" + to_string(get<1>(alphaRange)) + " radius " +
           std::format("{:.2f}", get<0>(radiusRange)) + "-" +
           std::format("{:.2f}", get<1>(radiusRange));
  }
};

void writeResultsByFile(const std::vector<HCAParameters> &params) {
  string filename = params[0].filename;
  std::string outFile = filename;
  size_t pos = outFile.find(".png");
  if (pos != std::string::npos)
    outFile.replace(pos, 4, "_results.csv");

  std::string tmpFile = outFile + ".tmp";

  FILE *f = fopen(tmpFile.c_str(), "w");
  if (!f) {
    printf("Warning: could not write to %s\n", tmpFile.c_str());
    return;
  }

  fprintf(f, "d,k,alphaMin,alphaMax,radiusMin,radiusMax,colorMin,colorMax,"
             "RMSError,time\n");
  for (const auto &p : params) {

    fprintf(f, "%d,%d,%d,%d,%.4f,%.4f,%d,%d,%.6f,%.2f\n", p.d, p.k,
            get<0>(p.alphaRange), get<1>(p.alphaRange), get<0>(p.radiusRange),
            get<1>(p.radiusRange), get<0>(p.colorRange), get<1>(p.colorRange),
            p.RMSError, p.time);

    /*fprintf(f, "%d,%d,%d,%d,%.4f,%.4f,%d,%d,%.6f\n", p.d, p.k,
            get<0>(p.alphaRange), get<1>(p.alphaRange), get<0>(p.radiusRange),
            get<1>(p.radiusRange), get<0>(p.colorRange), get<1>(p.colorRange),
            p.RMSError);*/
  }
  fclose(f);

  // rename temp to target - will fail if target is locked
  if (std::filesystem::exists(outFile))
    std::filesystem::remove(outFile);

  std::error_code ec;
  std::filesystem::rename(tmpFile, outFile, ec);
  if (ec) {
    printf("Warning: could not replace %s (file may be open). Temp file saved "
           "as %s\n",
           outFile.c_str(), tmpFile.c_str());
  } else {
    printf("Results written to %s\n", outFile.c_str());
  }
}

// TO DO: change to putPixel
void add_pixel(unsigned char *img, int channels, int img_width, int img_height,
               int x, int y, int r, int g, int b, int a) {
  int coordinate = (x + img_width * y) * channels;
  float an = a / 255.0f;
  float inv_an = 1.0f - an;

  // buffer is always opaque � no need to read or write alpha channel
  *(img + coordinate) = (unsigned char)(*(img + coordinate) * inv_an + r * an);
  *(img + coordinate + 1) =
      (unsigned char)(*(img + coordinate + 1) * inv_an + g * an);
  *(img + coordinate + 2) =
      (unsigned char)(*(img + coordinate + 2) * inv_an + b * an);
  // leave channel 3 alone - always 255
}

float distance(int x, int y, int ox, int oy) {
  return sqrt(pow(x - ox, 2.0f) + pow(y - oy, 2.0f));
}
// TODO: change to putShape
void add_disc(unsigned char *img, int channels, int img_width, int img_height,
              polyElement pe) {
  const int x = pe.x;
  const int y = pe.y;
  const int radius = (int)pe.radii.x;
  const int radius2 = radius * radius;

  for (int j = y - radius; j <= y + radius; j++) {
    for (int i = x - radius; i <= x + radius; i++) {
      if (i < 0 || i >= img_width || j < 0 || j >= img_height)
        continue;

      const int dx = i - x;
      const int dy = j - y;
      const int d2 = dx * dx + dy * dy;

      if (d2 > radius2)
        continue;

      const float t = (float)d2 / (float)radius2;
      // const float falloff = 1.0f - t;
      // const float falloff = expf(-4.0f * t);
      // const float falloff = 0.5f * (1.0f + cosf(t * 3.14159f));

      /*  const float sinc_arg = t * 3.14159f;
        const float falloff = (t < 1e-6f) ? 1.0f : sinf(sinc_arg) / sinc_arg;
        const float an = (pe.col.a / 255.0f) * falloff;
        const float inv_an = 1.0f - an;*/

      /*   const float lobes = 3.0f;
         const float sinc_arg = t * lobes * 3.14159f;
         const float falloff = (t < 1e-6f) ? 1.0f : sinf(sinc_arg) / sinc_arg;
         const float an = (pe.col.a / 255.0f) * falloff;
         const float inv_an = 1.0f - an;*/
      float falloff = 1.0f;
      const float an = (pe.col.a / 255.0f) * falloff;
      const float inv_an = 1.0f - an;

      const int coord = (j * img_width + i) * channels;

      // blend disc color over existing buffer � no alpha channel written
      img[coord] = (unsigned char)(img[coord] * inv_an + pe.col.r * an);
      img[coord + 1] = (unsigned char)(img[coord + 1] * inv_an + pe.col.g * an);
      img[coord + 2] = (unsigned char)(img[coord + 2] * inv_an + pe.col.b * an);
    }
  }
}

// TODO: change to putShapeDiffOPT
float add_disc_diff_opt(unsigned char *img_in, unsigned char *img_buff,
                        int channels, polyElement pe_tmp, int width,
                        int height) {
  // TODO add Shape::Disc
  float diff = 0.0f;

  const int cx = pe_tmp.x;
  const int cy = pe_tmp.y;
  const int radius = pe_tmp.radii.x;
  const int radius2 = radius * radius;

  const float max_a = pe_tmp.col.a / 255.0f; // peak alpha at center

  const float disc_r = (float)pe_tmp.col.r;
  const float disc_g = (float)pe_tmp.col.g;
  const float disc_b = (float)pe_tmp.col.b;

  const int min_x = max(0, cx - radius);
  const int max_x = min(width - 1, cx + radius - 1);
  const int min_y = max(0, cy - radius);
  const int max_y = min(height - 1, cy + radius - 1);

  for (int j = min_y; j <= max_y; ++j) {
    const int dy = j - cy;
    const int dy2 = dy * dy;
    const int row = j * width;

    for (int i = min_x; i <= max_x; ++i) {
      const int dx = i - cx;
      const int d2 = dx * dx + dy2;

      if (d2 >= radius2)
        continue;

      const float t = (float)d2 / (float)radius2;

      // experiment with lobes of sinc
      /*const float lobes = 3.0f;
      const float sinc_arg = t * lobes * 3.14159f;
      const float falloff = (t < 1e-6f) ? 1.0f : sinf(sinc_arg) / sinc_arg;*/
      float falloff = 1.0f;
      const float an = (pe_tmp.col.a / 255.0f) * falloff;
      const float inv_an = 1.0f - an;

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

      diff += (dr_new * dr_new + dg_new * dg_new + db_new * db_new) -
              (dr_old * dr_old + dg_old * dg_old + db_old * db_old);
    }
  }

  return diff;
}

float computeRMSE(unsigned char *img_in, unsigned char *img_buff, int width,
                  int height, int channels) {
  float sse = 0.0f;
  int n = width * height * channels;
  for (int i = 0; i < n; i++) {
    float d = (float)img_in[i] - (float)img_buff[i];
    sse += d * d;
  }
  return sqrt(sse / n);
}

void render(HCAParameters &p, IPlot *plot = nullptr) {
  char inputFilename[256];
  int ndiscs = p.d;
  int k = p.k;

  int alphaMin = get<0>(p.alphaRange);
  int alphaMax = get<1>(p.alphaRange);
  float radiusMin = get<0>(p.radiusRange);
  float radiusMax = get<1>(p.radiusRange);
  int colorMin = get<0>(p.colorRange);
  int colorMax = get<1>(p.colorRange);

  int template_width, template_height, template_channels;

  snprintf(inputFilename, sizeof(inputFilename), "%s", p.filename.c_str());
  std::string lower = inputFilename;
  std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

  if (plot)
    plot->init(p.filename);

  char outputName[256];

  std::string outputFilename = lower;
  size_t pos = outputFilename.find(".png");
  if (pos != std::string::npos)
    outputFilename.replace(pos, 4, " " + p.toString() + ".png");
  const char *outputFilenameCStr = outputFilename.c_str();

  int png_channels = 4;
  int bpp = 1;
  stbi_uc *img_template = stbi_load(inputFilename, &template_width,
                                    &template_height, &template_channels, 4);

  if (img_template == NULL) {
    throw exception("Error in loading the image");
  }
  unsigned char *img_tmp = (unsigned char *)calloc(
      template_width * template_height * png_channels * bpp,
      sizeof(unsigned char));
  unsigned char *img_out = (unsigned char *)calloc(
      template_width * template_height * png_channels * bpp,
      sizeof(unsigned char));

  // stbi_uc* img_tmp = (stbi_uc*) calloc(template_width * template_height *
  // png_channels * bpp, sizeof(unsigned char)); stbi_uc* img_out = (stbi_uc*)
  // calloc(template_width * template_height * png_channels * bpp,
  // sizeof(unsigned char));

  std::uniform_int_distribution<int> xdist(0, template_width - 1);
  std::uniform_int_distribution<int> ydist(0, template_height - 1);
  template_channels = 4; // force 4 channels for output and calculations,
                         // ignore alpha in template if it exists
  // printf("template channels: %d\n", template_channels);

  if (template_width * template_height * png_channels * bpp > INT_MAX) {
    throw exception("Image is too large");
  }

  auto start = std::chrono::high_resolution_clock::now();
  srand(time(NULL));
  float difference_metric = 1e30;

  memset(img_tmp, 255, template_width * template_height * 4);
  int x, y;
  int red, green, blue;
  float radius;
  float best_metric_global = 1e30;

  polyElement pe_tmp = polyElement();
  std::mt19937 rng((unsigned int)time(NULL) +
                   1337u * (unsigned int)omp_get_thread_num());

  std::thread computeThread([&]() {
    for (int i = ndiscs; i > 0; i--) // add discs
    {
      polyElement pe_best = {};

#pragma omp parallel
      {
        std::uniform_real_distribution<float> rdist(
            radiusMin, radiusMax); // recommended values: 0.5,20
        std::uniform_int_distribution<int> cdist(colorMin, colorMax);
        std::uniform_int_distribution<int> adist(alphaMin, alphaMax);

        polyElement pe_best_local;
        pe_best_local.metric = 1e30f;
        pe_best.metric = 1e30f;

        int attempts = 0;
#pragma omp for nowait

        for (int j = 0; j < k; j++) {
          polyElement pe_tmp = {};

          pe_tmp.x = xdist(rng);
          pe_tmp.y = ydist(rng);

          pe_tmp.col.r = (unsigned char)cdist(rng);
          pe_tmp.col.g = (unsigned char)cdist(rng);
          pe_tmp.col.b = (unsigned char)cdist(rng);
          pe_tmp.col.a = (unsigned char)adist(rng);
          pe_tmp.radii.x = rdist(rng);
          pe_tmp.metric =
              add_disc_diff_opt(img_template, img_tmp, template_channels,
                                pe_tmp, template_width, template_height);

          if (pe_tmp.metric < pe_best_local.metric) {
            pe_best_local = pe_tmp;
          }
        }

#pragma omp critical
        {
          if (pe_best_local.metric < pe_best.metric) {
            pe_best = pe_best_local;
          }
        }
      }

      add_disc(img_tmp, png_channels, template_width, template_height, pe_best);

      if (i % (ndiscs / 100) == 0) {
        float currentRmse = computeRMSE(img_tmp, img_template, template_width,
                                        template_height, template_channels);
        p.RMSError = currentRmse;

        if (plot) {

          if (plot)
            plot->push(currentRmse, ndiscs - i);
        }
      }
    }
  });
  computeThread.join();

  if (plot) {
    plot->savePNG(outputFilename + "_rmse.png");
    plot->shutdown();
  }

  auto finish = std::chrono::high_resolution_clock::now();

  std::chrono::duration<double> elapsed = finish - start;
  std::cout << "Elapsed time: " << elapsed.count() << " s\n";

  p.time = elapsed.count();

  stbi_write_png(outputFilenameCStr, template_width, template_height,
                 png_channels, img_tmp, template_width * png_channels);
  free(img_tmp);
  free(img_out);
  stbi_image_free(img_template);
}

int main(int argc, char *argv[]) {
  const char *ver = "0.0.15";
  string path = "C:\\temp\\images\\";

  // vector<HCAParameters> params;
  std::vector<std::string> names = {"dots"}; //, "Mona", "mario", "geo_shapes"

  // std::map<std::string, HCAParameters> testData;
  std::map<std::string, std::vector<HCAParameters>> testData;

  tuple<int, int> alphaRange = {70, 80};       // recommended: 80, 80
  tuple<float, float> radiusRange = {0.5, 20}; // recommended: 0.5, 20
  tuple<int, int> colorRange = {0, 255};
  Shape shape = Shape::Disc;
  string filename;
  HCAParameters parameter = {};

  for (const auto &name : names) {
    string inputFilename = path + name + ".png";
    if (!std::filesystem::exists(inputFilename)) {
      printf("File '%s' not found", inputFilename.c_str());
      std::cin.get();
      return -1;
    }
    for (int alphaMin : {60, 80, 100}) {

      for (int alphaMax = alphaMin; alphaMax < 100; alphaMax += 20) {
        for (int radiusMax : {20, 40, 60}) {
          for (int d : {1000, 10000, 20000, 100000}) { // #shapes
            for (int k : {100, 1000, 4000}) {   // #trials
              parameter = {.d = d,
                           .k = k,
                           .alphaRange = {alphaMin, alphaMax},
                           .radiusRange = {get<0>(radiusRange), radiusMax},
                           .colorRange = colorRange,
                           .shape = Shape::Disc,
                           .filename = inputFilename};

              testData[name].push_back(parameter);
            }
          }
        }
      }
    }
  }

  printf("Hill Climber: %s\n", ver);
  size_t total = 0;
  for (const auto &[key, params] : testData)
    total += params.size();

  printf("Total parameter sets: %zu\n", total);

  int nThreads = max(1, omp_get_max_threads() - 1);
  omp_set_num_threads(nThreads);
  printf("Threads: %d\n", nThreads);

  RmsePlot plot;

  for (auto &[key, params] : testData) {
    printf("Processing image: %s\n", key.c_str());
    for (auto &p : params) {
      printf("Running with parameters: %s\n", p.toString().c_str());
      render(p, nullptr);
    }
  }

  for (auto &[key, params] : testData) {
    writeResultsByFile(params);
  }

  std::cin.get();
  return 0;
}
