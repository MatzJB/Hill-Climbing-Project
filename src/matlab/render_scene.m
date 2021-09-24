%Render painting using hist and dimensions <height>-by-<width>

%http://en.wikipedia.org/wiki/Alpha_compositing

function out=render_scene(hist, height, width)

dst = zeros(height, width, 4);
dst(:,:,4)=1;
src = 0*dst;

for i = 1:size(hist, 1)
    
    tmp = hist(i, :);
    x = tmp(1);
    y = tmp(2);
    r = tmp(3);
    rgba = tmp(4:7);
    
    mask = disc_poly(x, y, height, width, r, 35);
    
    for j = 1:3
        src(:, :, j) = rgba(j).*mask;
    end
    
    src(:, :, 4) = rgba(4).*mask;
    
    %Apply transparency
    out(:, :, 4) = src(:, :, 4) + dst(:, :, 4).*(1 - src(:, :, 4));
    dst = out;
    
    %how can we speed this up?
    for j = 1:3
        out(:, :, j) = ( src(:, :, j).*src(:, :, 4) + dst(:, :, j).*dst(:, :, 4).*...
            (1-src(:, :, 4)) );%./out(:, :, 4);
    end
    
    out(out(:, :, 4) > 1) = 1;
    out(out(:, :, 4) <= 0) = 0;
    src = 0*out;
end
