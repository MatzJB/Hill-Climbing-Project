%MatzJB 27i12~14
%Render painting using hist and dimensions <height>-by-<width>

%http://en.wikipedia.org/wiki/Alpha_compositing

%This version returns two matrices, the render before (out1) and after
%(out2) the current disc. This method is more efficient mostly because we
%test each disc for several iterations until we move on to another disc.

function [out1, out2] = render_scene_2(hist, index, height, width, disc_type)


dst  = zeros(height, width, 4);
dst(:, :, 4) = 1;
src  = 0*dst;
out1 = 0*dst;
out2 = 0*dst;

%render scene excluding disc <index>
for i = 1:index%size(hist, 1)%Note: we only need to draw until index
    
%    if all( hist(i,:) == 0*hist(i,:) ) %we don't need to render non-existant discs
 %       %fprintf(1,'Ruby\n')
  %      break
  %  end

    
    if i<index
        out1 = add_disc(i, hist, out1, disc_type);
    elseif i>index
        out2 = add_disc(i, hist, out2, disc_type);
    end
end

    
