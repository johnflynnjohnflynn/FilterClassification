close all;
clear all;

%x = [0.751962, 0.0631137] % pretty much the same
%x = [0.112189, -0.898336] % high pass
x = [1 -1 1]
[H W] = freqz(x);
figure
plot(W, 20*log10(abs(H)));

figure
zplane(x)
