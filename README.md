# Hill-Climbing-Project (UNDER CONSTRUCTION)

[![Build](https://github.com/MatzJB/Hill-Climbing-Project/actions/workflows/build.yml/badge.svg)](https://github.com/MatzJB/Hill-Climbing-Project/actions/workflows/build.yml)
[![Docs](https://github.com/MatzJB/Hill-Climbing-Project/actions/workflows/docs.yml/badge.svg)](https://github.com/MatzJB/Hill-Climbing-Project/actions/workflows/docs.yml)


This project is based on a Matlab experiment I did (look under src/matlab).
This project goes further into C++ and OpenMP (and later, CUDA).

## What is it?
An implementation of a image approximation via stochastic disc placement. Basically it reconstructs a target image
by iteratively placing the best candidate shape (disc for now) from *k* random trials, evaluated
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
Just run build.bat and cmake will create the project solution file *HCA.sln* under /build, compile in VS and the release executable will be placed under build/Release.


> **Note:** The first build downloads dependencies via FetchContent and may
> take several minutes. Subsequent builds are fast.

Input images are configured in `main.cpp` under the `names` vector and `path`
variable. Default path is `C:\temp\images\`. Output images are written
alongside the inputs with the parameter string appended to the filename.

A `_results.csv` is also written per input with RMSE and timing data for
parameter sweep analysis.


## Lessons learned
The plot functionality is only to see how the approximation improves. Unfortunately, to update the plot one thread has to be sacrificed, so I am thinking of removing it and just use the parameters struct properties to get the improvement info I need.

My Matlab version is smarter than the C++ version. I wanted to test how far a naive hill climbing algorithm can be optimized with only threading and some constraints (opacity). We can make lots of assumptions about the distribution of colors, but I want to add more shapes and unexpected negative shapes to see what will happen (what about a sinc disc?). Additionally, making the core idea simple is good because of the later transition to CUDA.


## Documentation
To generate the report, navigate to doc/ run *lat.bat* and the report will be generated. Prerequisites: *MiKTeX*

