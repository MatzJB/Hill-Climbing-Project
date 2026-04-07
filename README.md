# Hill-Climbing-Project
A simple C++ project implementing a Hill Climbing Algorithm using transparent discs.

<img width="500" height="750" alt="David d 20000 k 10000 r 1 15 a 255" src="https://github.com/user-attachments/assets/0b79efc5-07ed-4e3c-b7b4-78ab33872754" />



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
 
 $alpha \in [80, 80)$
 
 $radius \in [1, 15)$

The following secions will show different trial number ($k$). The higher the $k$, the more accurate the discs will be. However, if the radii of the disc is fixed, there is a limit to how accurate the resulting image can be, even if the discs are transparent.


### Original images

| ![mona](https://github.com/user-attachments/assets/f5f07264-f76e-424c-8e91-fc5fcf5a1431) |
|:--:|
| *Mona Lisa* |


| ![david](https://github.com/user-attachments/assets/85df9ae2-f81e-4b3b-ae0a-786e1d87a5c4) |
|:--:|
| *David* |

## Building the images

...


## Compression idea
Even though this project is all about creating art, if you increase the numbere of discs, trials and decrease the size of the radii, the resulting image will start to look more like the original image.
For instance a 500x750 pixel image contains a total of 375 000 pixels, if we only used radius=1 we would need that many discs, however using only 100 000 discs we can approximate the original image surprisingly well.



### $k$=10000, 20000 discs
Alpha 30, 80 and 120

<img width="500" height="750" alt="David d 20000 k 10000 r 1 15 a 30" src="https://github.com/user-attachments/assets/6cd094c7-443e-4ac3-a33a-03c072bbdb04" />
<img width="500" height="750" alt="David d 20000 k 10000 r 1 15 a 80" src="https://github.com/user-attachments/assets/52c856b6-d0f9-4903-845b-dc01c9cb538a" />
<img width="500" height="750" alt="David d 20000 k 10000 r 1 15 a 120" src="https://github.com/user-attachments/assets/966f1f8d-5bd1-48b8-be7b-6a0ccc417d87" />

This test show that alpha should be fixed to 80.


| d=100,000 | d=40,000 |
| :---: | :---: |
| <img width="500" height="750" alt="David d 40000 k 10000 r 1 15 a 80" src="https://github.com/user-attachments/assets/6ba30f4e-e476-485b-a459-8ac5c96d0305" /> | <img width="500" height="750" alt="David d 100000 k 10000 r 0 5 15 a 80 c 0 255" src="https://github.com/user-attachments/assets/ce3ed561-8f9f-4950-8d30-a60d2d2776c7" /> |

Using d=100 000 will capture the small details because as you increase the number of discs the smaller features are the only ones making a difference.

