%modify parameter of random disc
function hist = random_change(disc, hist)

j = disc;

param = ceil(7*rand); %choose which parameter we want to change

doublestep = 30; %double the step length, must be even

if param==1 %x
    hist(j, 1) = hist(j, 1) + ceil(50*rand-25); %ceil(doublestep*rand-doublestep/2);
elseif param==2 %y
    hist(j, 2) = hist(j, 2) + ceil(50*rand-25);%ceil(doublestep*rand-doublestep/2);
elseif param==3 %r
    hist(j, 3) = hist(j, 3) + ceil(20*rand-10);%ceil(doublestep*rand-doublestep/2);
elseif param==4 %red
    hist(j, 4) = rand;
elseif param==5 %green
    hist(j, 5) = rand;
elseif param==6 %blue
    hist(j, 6) = rand;
elseif param==7 %alpha
    hist(j, 7) = 1;%rand;
end
