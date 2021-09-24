%MatzJB 25/12 2014
function A = disc_poly(x, y, h, w, r, n)

theta = linspace(0, 2*pi, n); 
A = poly2mask(y + r*cos(theta), x + r*sin(theta), h, w);

%one should use fewer segments if the disc is smaller, we will not see any
%difference anyway



