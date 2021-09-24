%MatzJB 27i12~14
%used to add discs and images

%A=zeros(5,5,4); B=zeros(5,5,4); A(2,:)=1; B(:,3,:)=1; A(2,:,4)=0.5; B(:,3,4)=0.3; C=add_transparency_image(A,B)
function out = add_transparency_image(src, img)

[height, width, depth] = size(src);

%img = zeros(height, width, 4);
img(:,:,4) = 1;


out(:, :, 4) = src(:, :, 4) + img(:, :, 4).*(1 - src(:, :, 4));

%img = out;

for j = 1:3
    out(:, :, j) = ( src(:, :, j).*src(:, :, 4) +...
        img(:, :, j).*img(:, :, 4).*(1-src(:, :, 4)));%./out(:, :, 4));
end

out(out(:, :, 4) > 1) = 1;
out(out(:, :, 4) <= 0) = 0;
