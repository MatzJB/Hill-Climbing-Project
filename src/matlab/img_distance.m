%Simple "distance" between two images of type MxNx3

%MatzJB 25/12 2014
function k = img_distance(A,B)


k = imabsdiff(A,B);
k = sum(sum(sum(k)));

return

n = 3; %distance between samples
R1 = A(1:n:end,1:n:end,1);
R2 = B(1:n:end,1:n:end,1);

%k = sum( sum(abs(R1-R2)));
%return

G1 = A(1:n:end,1:n:end,2);
G2 = B(1:n:end,1:n:end,2);

B1 = A(1:n:end,1:n:end,3);
B2 = B(1:n:end,1:n:end,3);

k = sum( sum(abs(R1-R2)) + sum(abs(G1-G2)) + sum(abs(B1-B2)) );
