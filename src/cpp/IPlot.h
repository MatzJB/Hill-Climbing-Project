#pragma once
#include <string>

struct IPlot {
  virtual void init(const std::string &title) = 0;
  virtual void push(float rmse, int iteration) = 0;
  virtual void savePNG(const std::string &filename) = 0;
  virtual void shutdown() = 0;
  virtual ~IPlot() = default;
};