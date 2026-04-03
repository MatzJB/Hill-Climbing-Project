# Hill-Climbing-Project
A simple C++ project implementing a Hill Climbing Algorithm using transparent discs.


## How it works
It creates an output image (PNG) using m transparent discs to approximate the input image.
```
for each disc from 1 to m
 for each disc there are k trials
  a disc if created with a new RGB value, position, opacity and radius, diff between input image and new image is calculated
  it picks the disc which leads to the least diff in the trial run
```

## Optimizations
OpenMP is used to parallelize the inner loop.

## Todo
* Bail early if the diff is greater than previous diff values. The reason this works is because the diff is an monotonic increasing function.
* Use ellipsoids.
* Store each disc element and prune if they do not contribute in the final image.
* Try to lower $k$ by gating the trials (totally random at first, pick best and then continue for a few iterations, only modifing position, I did this in Matlab and it seems to reduce the search space).
* Visualize error using a plot.
* Test if two discs overlapping will produce a better trial


## Renders
Each render was created using m transparent discs and k trials for each disc.
I have experimented with alpha/opacity to make the search space smaller and I believe that making it very narow toward opaque may be better compared to relying on overlapping transarent discs to produce shaded areas. Given a high enough k.

For each trial the randomized values are:
 $x \in [0, templateWidth)$, $y \in [0, templateHeight)$
 
 $color RGB \in [20, 250)$
 
 $alpha \in [30, 250)$
 
 $radius \in [1, 90)$

The following secions will show different trial number ($k$). The higher the $k$, the more accurate the discs will be. However, if the radii of the disc is fixed, there is a limit to how accurate the resulting image can be, even if the discs are transparent.


### Original images


### $k$=400


### $k$=4000

