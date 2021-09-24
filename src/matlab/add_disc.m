%MatzJB 27/12 2014
%add a disc defined by <ind> in <hist> to m-by-n-by-4 matrix <out>

function disc = add_disc(ind, hist, src, disc_type)

[height, width, depth] = size(src);
%dst  = 0*src;
disc  = 0*src;

tmp  = hist(ind, :);

x    = tmp(1);
y    = tmp(2);
r    = tmp(3);
rgba = tmp(4:7);
angle = tmp(9); %angle %only makes sense for non-disc types

if disc_type==1
    disc(:, :, 1) = disc_poly(x, y, height, width, r, 40); %35
elseif disc_type==2
    disc(:, :, 1) = quad_poly(x, y, height, width, r, angle);
end

for i=1:4
    disc(:, :, i) = rgba(i)*disc(:, :, 1); %note alpha
end

disc = add_transparency_image(disc, src);

