%Created by MatzJB 26/12-27/12 2014

%This program "Hill climb"-paints actress Sarah Snook (Predestination, 2014)
%http://www.imdb.com/title/tt2397535/combined using transparent discs.

%In this version (2) I have sped up the search process by slicing the
%render portion into two parts. The search is also smarter, because I
%change the parameters on a single disc until I am satisfied.
%Next version will possibly continue to change the discs after they have
%been placed. More importantly, the search patterns will be smarter. If a
%change was fruitful, the search algorithm will continue in the same
%direction of change. I will possibly explore the "Genetic programming"
%approach in the future.
%                                                          -MatzJB


%TODO:
%Fix first run "size bug"
%Fixed sub search bug, we now get sub search results too [2/1 2015]
%Added support for quads [2/1]
%Batch run of all images in a cell array
%Bugfix of saved hist variable [3/1]

clc
close all

speedup=1;
%n_discs = 1; %start off with 1 disc, look for best position and then move on
%min_iterations = 15; %obsolete minimum iterations per test
%iterations     = 10000;
loc_iterations = 200; %200, number of iterations for each disc, first level of detail
sub_loc_iterations = 50; %50, small changes, second level of detail
%refinement_size = 50;%for sub refinement
max_discs      = 500; %300, max discs in the scene
current_disc   = 1;


animated = 1;
frame=1;
total_frames = 30*60 %FPS and number of seconds, total amount of frames we wish to store
saved_frame = 1;
%skip_frames = loc_iterations*sub_loc_iterations/total_frames;
%skip_frames = ceil(max_discs*loc_iterations/total_frames)
skip_frames = 100;


k_old  = 1000000;
k_olds = []; %old fitness values

filenames = {'Janet Leigh 3'};
%filenames = {'Sarah', 'Mona', 'Eva'};
%filenames = {'Eva2', 'watercolor', 'Einstein', 'Mandelbrot',...
%            'yinyang', 'Marilyn', 'Skriet'};
disc_type_name = {'quad'};



fprintf(1, 'Running Hill climbing algorithm\n')
fprintf(1, 'Local iteration search: %d\n', loc_iterations);
fprintf(1, 'Sub local iteration search: %d\n', sub_loc_iterations);
fprintf(1, 'Max number of discs: %d\n', max_discs);

fprintf(1,'Painting:\n')
fprintf(1, '%s\n', filenames{:})


for ii=1:length(filenames)
    
    for disc_type = 1:length(disc_type_name)
        
        filename = filenames{ii}
        %filename='Maddie'; %
        %filename='Sarah'; %
        %filename='Noomi'; %OK
        %filename='Mona'; %OK
        %filename='mickey'; %
        %filename='Focus';
        %filename='Natalie';
        %filename='Eva';
        
        im_target = double(imread([filename,'.jpg']))/255;
        
        
        render_filename = [filename, '_', disc_type_name{disc_type}];
        
        %im_target=zeros(10,10,4);
        
        %for i=1:4
        %im_target = rand(10,10,3);
        %end
        %{
summa = 0;
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
        
        
        [height, width, depth] = size(im_target);
        hist   = zeros(max_discs, 9); %x, y, r, rgba,k,angle
        src    = zeros(height, width, 4); %RGBA
        out    = zeros(height, width, 4);
        out(:,:,4) = 1;
        src(:,:,4) = 1;
        
        dst = out;
        
        
        figure
        fig1 = imagesc(out(:, :, 1:3));
        truesize
        title('Hill Climbing')
        
        
        figure
        fig2 = imagesc(im_target);
        truesize
        title(render_filename)
        
        fig3 = figure;
        hold on
        plot_progress=plot(1:2,[0,0],'r','linewidth',3);
        title('Accuracy')
        
        fprintf(1,'Starting simulation...\n')
        
        
        out1 = 0*out;
        out2 = 0*out;
        
        tic
        for current_disc = 1:max_discs
            
            fprintf(1,'disc %d\n', current_disc)
            toc
            tic
            [out1, out2] = render_scene_2(hist, current_disc, height, width, disc_type);
            hist_loc = zeros(loc_iterations,9); %local history:x,y,r,r,g,b,a,k,angle
            hist_loc(:,8) = 1e8;%important
            %hist_loc(:, 1:2) = rand(15,2);
            %hist_loc(:, 1) = ceil(height*hist_loc(:, 1));
            %hist_loc(:, 2) = ceil(width*hist_loc(:, 2));
            
            %choose color before search
            %Colors...
            %  col = rand;
            %  hist_loc(current_disc, 4:6) = col*ones(1, 3); %B/W
            %%{
            col = rand;
            hist_loc(:, 4) =  col;
            hist_loc(:, 5) =  col;
            hist_loc(:, 6) =  col;
            %%}
            %col = rand*ones(1, 3); %B/W
            %random=rand;
            %hist_loc(:, 4:6) = rand;
            
            %search optimal solution for each disc
            for t = 1:loc_iterations
                %randbin = circshift([1,0,0,0,0,0,0], [0,ceil(7*rand)]);
                %randbin = circshift([1,0,0,0,0,0,0], [0,round(2*rand)]); %corrected
                
                rr(1:2) = [height*rand, width*rand];
                rr(3) = 2 + ceil(0.3*min(width,height)*rand); %smarter radius
                %old: rr(3) = 15+ceil(0.5*min(width,height)*rand); %smarter radius
                
                hist_loc(t, 1:3) = rr(1:3);
                %rr(3) = 5 + rand*(max_discs-current_disc); %decrease radius as we iterate
                %hist(cur_disc(1), :) = hist(cur_disc(1),:).*randbin + rr.*randbin;
                %%random increment from previous data
                %hist_loc(t, 1:3) = rr(1:3).*randbin(1:3);
                %hist(cur_disc(1),1:2) = hist(cur_disc(1), 1:2).*randbin(1:2)+rr(1:2).*randbin(1:2);%relative increment
                %  hist_loc(t, 1:2) = [height*rand, width*rand];
                
                
                hist_loc(t, 7) = 0.2 + 0.9*rand; %should binary search alpha as another step?
                %hist_loc(t, 7) = 0.3; %transparency
                %alpha
                %hist_loc(t, 7) = rand;
                %hist_loc(t, 7) = 0.3 + 0.7*rand;
                %hist(cur_disc(1), 4:6) = rand(3,1);
                
                %hist_loc(t, 3) =  max(hist_loc(t, 3), 20);
                %hist_loc(t, 3) =  min(hist_loc(t, 3), 150);
                
                
                %out_tmp = add_transparency_image(mask, out1, rgba);
                
                hist_loc(t,9) = 2*pi*rand;
                out_tmp = add_disc(t, hist_loc, out1,disc_type);
                
                out_tmp = add_transparency_image(out_tmp, out2);
                %hist_loc(t, 8) = 1e9;
                hist_loc(t, 8) = img_distance(out_tmp(:, :, 1:3), im_target); %calculate distance to target image
                
                
                %Save frame
                if mod(frame, skip_frames)==0% animated
                    
                    set(fig1, 'cdata', out_tmp(:, :, 1:3))
                    drawnow
                    
                    %set(plot_progress, 'xdata', 1:length(k_olds), 'ydata', k_olds)
                    imwrite(out_tmp(:, :, 1:3), [render_filename,'_', num2str(saved_frame),'.png'])
                    %fprintf(1,'Saving frame...\n')
                    saved_frame=saved_frame+1;
                    
                end
                
                frame = frame+1;
            end
            
            
            [k,ind1] = min(hist_loc(:,8));
            hist_sub_loc = zeros(sub_loc_iterations, 9); %local history:x,y,r,r,g,b,a,k
            hist_loc(:,8) = 1e8;%important
            
            %Take the best one, change radius and translation slightly, compare
            %with the best one and pick a new
            hist_sub_loc(1,1:9) = hist_loc(ind1, 1:9);
            refinement_size = 1.5*hist_loc(ind1, 3);%refinement size depends on radius
            %test random xyr
            %ss=[]; for i=1:30; ss(i,:)=circshift([0,0,1], [0,round(150*rand)]);end; sum(ss)
            
            for t = 2:sub_loc_iterations
                
                %randbin = circshift([0,0,1], [0,round(150*rand)]);
                %%previous
                randbin = circshift([0,1,0], [0,round(2*rand)]); %corrected
                %the step size should be percentage of image size
                tmp      = hist_loc(ind1, 1:9);
                
                tmp(1:3) =  tmp(1:3) + randbin(1:3).*(refinement_size*rand(1,3)-0.5*refinement_size*rand(1,3));
                hist_sub_loc(t,1:3) = tmp(1:3);
                hist_sub_loc(t,4:7) = hist_sub_loc(1, 4:7);%4:7
                %modify only one parameter
                %angle = 2*pi*rand;
                %out_tmp = add_disc(t, hist_loc, out1,disc_type,angle);
                hist_sub_loc(t,9) = 2*pi*rand;%angle
                out_tmp = add_disc(t, hist_sub_loc, out1,disc_type);
                
                
                out_tmp  = add_transparency_image(out_tmp, out2);
                %hist_loc(t, 8) = 1e9;
                hist_sub_loc(t, 8) = img_distance(out_tmp(:, :, 1:3), im_target); %calculate distance to target image
                
                %update draw
                if mod(frame, skip_frames)==0
                    
                    set(fig1, 'cdata', out_tmp(:, :, 1:3))
                    %imwrite(out_tmp(:, :, 1:3), [render_filename,'_', num2str(frame),'_',...
                    %   num2str(current_disc), '.png'])
                    %frame=frame+1;
                    %set(plot_progress, 'xdata', 1:length(k_olds), 'ydata', k_olds)
                    %frame=frame+1;
                    drawnow
                end
            end
            
            %now we are satisfied with the selection, save it and continue
            [k, ind2] = min(hist_sub_loc(:,8));
            hist(current_disc, 1:9) = hist_sub_loc(ind2, 1:9); %copy locally best
            k_olds(end+1) = k;
            
            if ind2==1
                fprintf(1, 'Best fit was already found\n')
            end
            
            
            if mod(current_disc, 2)==0 %update,
                
                %render all but last disc
                [out1, ~] = render_scene_2(hist, size(hist,1), height, width, disc_type);
                %out1 = out1./max(max(max(out1)));
                
                set(fig1, 'cdata', out1(:, :, 1:3))
                set(plot_progress, 'xdata', 1:length(k_olds), 'ydata', k_olds)
                
                drawnow
            end
            
        end%current_disc
        
        %{
        if ceil(rand*30)==1 %update,
            save([render_filename, '_', num2str(max_discs), '.mat'], 'hist')
        end
        %}
    end%disctype
    
    
end%filenames