%MatzJB 2/1 2015
function A = quad_poly(x, y, h, w, r, delta) %r is the 'diagonal length'

theta = linspace(delta, 2*pi+delta, 5); 
A = poly2mask(y + r*cos(theta), x + r*sin(theta), h, w);




