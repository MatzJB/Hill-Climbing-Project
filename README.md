# Hill-Climbing-Project
This project shows some experiments with a simple Hill Climbing Algorithm (transparent discs) implemented in C++ (OpenMP) and CUDA.

<img width="500" height="750" alt="David d 20000 k 10000 r 1 15 a 255" src="https://github.com/user-attachments/assets/0b79efc5-07ed-4e3c-b7b4-78ab33872754" />


## How it works
It creates an output image (PNG) using $d$ transparent discs to approximate the input image.
```
for each disc from 1 to m
 for each disc there are k trials
  a disc if created with a new RGB value, position, opacity and radius, diff between input image and new image is calculated
  it picks the disc which leads to the least diff in the trial run
```



## Renders
Each render was created using m transparent discs and k trials for each disc.
I have experimented with alpha/opacity to make the search space smaller and I believe that making it very narow toward opaque may be better compared to relying on overlapping transarent discs to produce shaded areas. Given a high enough k.

For each trial the randomized values are:

  $x \in [0, templateWidth)$, 
  
  $y \in [0, templateHeight)$,
  
  $color RGB \in [20, 250)$, 
  
  $alpha \in [80, 80)$, 
  
  $radius \in [1, 15)$

The following secions will show different trial number ($k$). The higher the $k$, the more accurate the discs will be. However, if the radii of the disc is fixed, there is a limit to how accurate the resulting image can be, even if the discs are transparent.


### Original images
Introducing Leonardo Da Vincis Mona Lisa and Michelangelo's David.


| ![mona](https://github.com/user-attachments/assets/f5f07264-f76e-424c-8e91-fc5fcf5a1431) |
|:--:|
| *Mona Lisa* https://en.wikipedia.org/wiki/Mona_Lisa|


| ![david](https://github.com/user-attachments/assets/85df9ae2-f81e-4b3b-ae0a-786e1d87a5c4) |
|:--:|
| *David* https://en.wikipedia.org/wiki/David_(Michelangelo) |

## Building the images

...


## Compression idea
Even though this project is all about creating art, if you increase the numbere of discs and trials and decrease the size of the radii, the resulting image will start to look more like the original image. For instance a 500x750 pixel image contains a total of 375 000 pixels, if we only used radius=1 we would need that many discs, however using only 100 000 discs we can approximate the original image surprisingly well.



### $k$=10,000, 20,000 discs
Testing with alpha 30, 80 and 120

<img width="500" height="750" alt="David d 20000 k 10000 r 1 15 a 30" src="https://github.com/user-attachments/assets/6cd094c7-443e-4ac3-a33a-03c072bbdb04" />
<img width="500" height="750" alt="David d 20000 k 10000 r 1 15 a 80" src="https://github.com/user-attachments/assets/52c856b6-d0f9-4903-845b-dc01c9cb538a" />
<img width="500" height="750" alt="David d 20000 k 10000 r 1 15 a 120" src="https://github.com/user-attachments/assets/966f1f8d-5bd1-48b8-be7b-6a0ccc417d87" />

This test show that alpha should be fixed to around 80 for most accurate output.


| d=100,000 | d=40,000 |
| :---: | :---: |
| <img width="500" height="750" alt="David d 40000 k 10000 r 1 15 a 80" src="https://github.com/user-attachments/assets/6ba30f4e-e476-485b-a459-8ac5c96d0305" /> | <img width="500" height="750" alt="David d 100000 k 10000 r 0 5 15 a 80 c 0 255" src="https://github.com/user-attachments/assets/ce3ed561-8f9f-4950-8d30-a60d2d2776c7" /> |

Using d=100 000 will capture the small details because as you increase the number of discs the smaller features are the only ones making a difference.


## Other shapes
Discs are fine, but many discs are required to cover a square feature or an elongated feature where for a square that is trivial or for ellipsoids (e.g. Gaussian Splatting). An old distance function I have played with in the past is the Manhattan distance. It creates a diamond shape distance mapping, so we essentially create a 45 degree rotated square as our disc.

### Manhattan distance


### Squares
Pefect for (orthogonal) pixel art.

### Ellipsoids
The best compromise between discs and squares.

## Optimizations (CPU)
OpenMP is used to parallelize the inner loop of the algorithm on the CPU. Additionally, the distance function is squared, so we avoid costly sqrt.


## Todo
* Store each disc element and prune if they do not contribute in the final image.
* Try to lower $k$ by gating the trials (totally random at first, pick best and then continue for a few iterations, only modifing position, I did this in Matlab and it seems to reduce the search space).
* Visualize error using a plot.



## Taking it to the GPU (CUDA)

This will happen soon. 
May want to investigate a hybrid version that uses CPU and GPU at the same time.
GPU: RTX 4070



