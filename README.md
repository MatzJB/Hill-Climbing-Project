# Hill-Climbing-Project
A simple C++ project implementing a Hill Climbing Algorithm using transparent discs.


## How it works
It creates a output image by placing transparent discs randomly
for each disc it goes through k trials
for each trial, a difference in RGB values between the input and the output + candidat disc is calculated
it picks the disc with least diff


Note: in order to use diff for measurement of progress you need to randomize position, RGB and opacity, but keep radii the same, because it would give too much errors comparing a large disc with a smaller disc.


## Optimizations
OpenMP is used to parallelize the inner loop.

## Suggestions
Bail early if the diff is greater than previous diff values. The reason this works is because the diff is an monotonic increasing function.




## Renders
<img width="500" alt="dots_hca" src="https://github.com/user-attachments/assets/eb10c95a-49e3-47ef-aba5-713f6c8b6074" />
<img width="500" alt="tmp_output" src="https://github.com/user-attachments/assets/f53d1edf-16d6-4fb9-ab80-76f7477aa64b" />
<img width="500" alt="clouds_hca" src="https://github.com/user-attachments/assets/ccb5fc3b-52c2-40aa-8264-bedfbf3751a4" />


