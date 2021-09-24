%MatzJB 25/12 2014
%Hill climbing painting actress Sarah Snook using transparent discs

clc
close all

speedup=1;
n_discs = 100; %number of discs used in painting
k_olds = []; %old fitness values
im_target = double(imread('Maddie.jpg'))/255;
im_target = double(imread('Sarah.jpg'))/255;
%{
summa=0;
for i=1:3
summa = summa + im_target(:,:,i);
end
summa = summa/3;
for i=1:3
im_target(:,:,i) = summa;
end
%}
%im_target = double(imread('mickey.jpg'))/255;
%[width,height, depth] = size(im_target);
%im_target = im_target(1:300,1:300,:);
[height,width, depth] = size(im_target);

src = zeros(height, width, 4); %RGBA
out = zeros(height, width, 4);
out(:,:,4) = 1;
src(:,:,4) = 1;

dst = out;

figure
fig1 = imagesc(out(:, :, 1:3));
truesize
title('Hill Climbing Test')


figure
fig2 = imagesc(im_target);
truesize
title('Sarah Snook')

fig3 = figure
plot_progress=plot(1:2,[0,0],'r','linewidth',3)
title('Accuracy')

iterations = 10000;

hist = zeros(n_discs, 7); %x, y, r, rgba

%Create random discs
for i=1:n_discs
    
    hist(i, 1) = 10 + ceil(width*rand); %x
    hist(i, 2) = 10 + ceil(height*rand); %y
    hist(i, 3) = 10 + ceil(60*rand); %+ceil(rand*height/2); %radius
    hist(i, 4) = rand; %R
    hist(i, 5) = rand; %G
    hist(i, 6) = rand; %B
    hist(i, 7) = 0.5;  %A
end


%read from history and render
k_old = 1000000;

for i = 1:iterations
    tic
    %pick current disc we wish to modify
    cur_disc(1) = ceil(n_discs*rand);
    cur_disc(2:8) = hist(cur_disc(1), :);
    
    %Random modification of disc:
    %randbin = circshift([1,0,0,0,0,0,0], [0,ceil(7*rand)]);
    randbin = circshift([1,0,0,0,0,0,0], [0,round(2*rand)]); %corrected
    
    rr = 0.3+0.7*rand(1, 7);
    
    rr(3) = 10+ceil(60*rand);
    %rr(1:2) = ceil(10*rand);%10+ceil(width*rand(2,1));%x,y
    rr(1:2) = ceil(10*rand-5);%movement
    
    %rr(7)=0.5 + 0.5*rand;
    %hist(cur_disc(1), :) = hist(cur_disc(1),:).*randbin + rr.*randbin;
    %%random increment from previous data
    hist(cur_disc(1), 3:6) = rr(3:6).*randbin(3:6); %
    %hist(cur_disc(1),1:2) = hist(cur_disc(1), 1:2).*randbin(1:2)+rr(1:2).*randbin(1:2);%relative increment
    hist(cur_disc(1), 1:2) = [height*rand, width*rand];%rr(1:2).*randbin(1:2);%random position
    %hist(cur_disc(1), 4:6) = rand(3,1);
    
    hist(:, 3) =  max(hist(:,3), 5);
    hist(:, 3) =  min(hist(:,3), 100);
    
    %we must have colors
    hist(:, 4) =  max(hist(:,4), 0.5);
    hist(:, 5) =  max(hist(:,5), 0.5);
    hist(:, 6) =  max(hist(:,6), 0.5);
    
    if mod(i,2)==0 %update
        
        set(fig1, 'cdata', out(:, :, 1:3))
        set(plot_progress, 'xdata',1:length(k_olds),'ydata', k_olds)
        
        drawnow
    end
    
    out = render_scene(hist, height, width); %build disc image
    %hist
    k_new = img_distance(out(:, :, 1:3), im_target); %calculate distance to target image
    
    %simulate a 'slow search' by taking the mean of previous k:s
    %if k_new > mean(k_olds) %this "simulated annealing" doesn't really work
    if k_new > k_old %better
        %fprintf(1, 'Error')
        %out = out_old;
        hist(cur_disc(1), 1:7) = cur_disc(2:8); %restore old value
        
        k_new = k_old; %recall old value
        
    else %nice change
        %if change was in radius, change its radius more and see if it is
        %better
        
        %add later
    end
    
    
    
    
    k_old = k_new;
    k_olds(end+1) = k_old;
    
    out = out./max(max(max(out)));
    toc
end

