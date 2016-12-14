close all;
clear all;

%x = [1 1]                  % basic training data
%x = [1 -1]
%x = [1 0]

x = [1 0.1]                % threshold for 'not a filter'

%x = [-1 -1]                % negative filters should work

%x = [1.0 0.49]
%x = [0.751962, 0.0631137] % pretty much the same
%x = [0.112189, -0.898336] % high pass

[H W] = freqz(x);
figure
plot(W, 20*log10(abs(H)));

%{
figure
zplane(x)
%}
