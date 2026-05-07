# Hill-Climbing-Project
This project shows some experiments with a simple Hill Climbing Algorithm (transparent discs) implemented in C++ (OpenMP).

Image approximation via stochastic disc placement. Reconstructs a target image
by iteratively placing the best candidate shape from *k* random trials, evaluated
by RMSE improvement.

## Prerequisites

| Tool | Version |
|------|---------|
| Visual Studio 2022 | MSVC v143 (C++20) |
| CUDA Toolkit | 12.x |
| CMake | 3.20+ |
| Git | any recent |

CMake fetches all other dependencies automatically (GLM, SDL2, ImGui, ImPlot,
GoogleTest). No manual installs beyond the table above.

## Build (Windows for now)
Just run build.bat and cmake will create the project solution file *HCA.sln* under /build.


> **Note:** The first build downloads dependencies via FetchContent and may
> take several minutes. Subsequent builds are fast.

## Run

```bat
build\Release\HCA.exe
```

Input images are configured in `main.cpp` under the `names` vector and `path`
variable. Default path is `C:\temp\images\`. Output images are written
alongside the inputs with the parameter string appended to the filename.

A `_results.csv` is also written per input with RMSE and timing data for
parameter sweep analysis.


