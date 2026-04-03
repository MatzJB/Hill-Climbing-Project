# Hill-Climbing-Project
A simple C++ project implementing a Hill Climbing Algorithm using transparent discs.


## How it works
It creates an output image (PNG) using m transparent discs to approximate the input image.
```
for each disc it goes through $k$ trials
 for each trial, a disc if created with a new RGB value, position, opacity and radius, diff between input image and new image is calculated
  it picks the disc which leads to the least diff in the trial run
```

## Optimizations
OpenMP is used to parallelize the inner loop.

## Todo
Bail early if the diff is greater than previous diff values. The reason this works is because the diff is an monotonic increasing function.
Use ellipsoids.
Store each disc element and prune if they do not contribute in the final image.
Try to lower $k$ by gating the trials (totally random at first, pick best and then continue for a few iterations, only modifing position, I did this in Matlab and it seems to reduce the search space).
Visualize error using a plot.



## Renders
Each render was created using 50 000 transparent discs.
Each disc is placed uniformly randomly.
For these experiments I used:

 $x \in [0, templateWidth)$
 
 $y \in [0, templateHeight)$
 
 $color RGB \in [20, 250)$
 
 $alpha \in [30, 250)$
 
 $radius \in [1, 90)$

The following secions will show different trial number ($k$). The higher the $k$, the more accurate the discs will be. However, if the radii of the disc is fixed, there is a limit to how accurate the resulting image can be, even if the discs are transparent.

### $k$=400

<img width="500" alt="dots_hca" src="https://github.com/user-attachments/assets/eb10c95a-49e3-47ef-aba5-713f6c8b6074" />
<img width="500" alt="tmp_output" src="https://github.com/user-attachments/assets/f53d1edf-16d6-4fb9-ab80-76f7477aa64b" />

### $k$=4000

<img width="750" height="1125" alt="tmp_output_3_50" src="https://github.com/user-attachments/assets/d896e174-a51a-48b6-8a80-bfc5cbe8316c" />
<img width="740" height="740" alt="tmp_output" src="https://github.com/user-attachments/assets/08107bfc-fd8d-4b07-9dc0-a6916c6d08cb" />
<img width="1094" height="1500" alt="Mona_5000_3" src="https://github.com/user-attachments/assets/4d25b3f7-7e36-42df-8a13-886e164e3915" />
