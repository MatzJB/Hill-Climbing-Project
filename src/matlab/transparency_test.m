%MatzJB 24/12 2014
%Genetic programming first test of transparent polygons

clc
height = 200;
width  = 200;
src = zeros(height, width, 4); %RGBA, premultiplied alpha
out = zeros(height, width, 4);
out(:,:,4) = 1;
src(:,:,4) = 1;
dst = out;
iterations = 10;
n_discs=10;
ngon = 3;
fig1 = imagesc(out(:, :, 1:3));
tmp  = 0*src;

%all ngons in a matrix of history
hist = zeros(iterations, ngon+4);%coordinates of ngon and RGB and alpha

for i = 1:iterations
    
    x = rand(1, ngon)*width;
    y = rand(1, ngon)*height;
    col  = rand(3, 1);
    mask = double( poly2mask(x, y, width, height));
    
    src(:, :, 1) = col(1).*mask;
    src(:, :, 2) = col(2).*mask;
    src(:, :, 3) = col(3).*mask;
       src(:,:,4) = rand.*mask;
    %src(:,:,4) = mask;
    
    
    %apply transparency
    out(:, :, 4) = src(:, :, 4) + dst(:, :, 4).*(1 - src(:, :, 4));
    dst = out;
    
    for j = 1:3
        out(:, :, j) = ( src(:, :, j).*src(:, :, 4) + dst(:, :, j).*dst(:, :, 4).*(1-src(:, :, 4)) )./out(:, :, 4);
    end
    
    out(out(:, :, 4) > 1) = 1;
    out(out(:, :, 4) <= 0) = 0;
        
    %pause
    set(fig1, 'cdata', out(:, :, 1:3))
    
end

%close all
%imagesc(img./max(max(max(img))))