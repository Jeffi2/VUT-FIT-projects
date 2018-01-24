% ISS - projekt
% Petr Jůda | xjudap00
% Příkazy je potřeba zadávat postupně od úkolu 1.

%1) 
[s, Fs] = audioread(‘xjudap00.wav’);
%—> Fs = 16kH, vzorky = 16000, t = 1s

%2)
N = length(s);
G = 10*log10(1/N*abs(fft(s)).^2);
f = (0:N/2-1)/N*Fs;
G = G(1:N/2);
plot(f,G);
grid;
xlabel(‘f[HZ]’);
ylabel(‘PSD[dB]‘);
% obrázek

%3)
MAXIMUM = find (G == max(G));
%úprava indexu
MAXIMUM = MAXIMUM - 1;
% Výsledek na = 669Hz;

%4)
b = [0.2324 -0.4112 0.2324]; 
a = [1 0.2289 0.4662];
zplane(b,a); 
% obrázek
p = roots(a);
abs(p) < 1  
% výsledek true, filtr je stabilní

%5)
N = 8000;
Fs = 16000;
b = [0.2324 -0.4112 0.2324]; 
a = [1 0.2289 0.4662];
H = freqz(b,a, N);
f = (0:7999) / N * Fs /2;
xlabel('f[Hz]');
ylabel('|H(f)|');
grid;  
% obrázek, horní propust

%6)
sf = filter(b, a, s);
N = length(sf);
G = 10*log10(1/N*abs(fft(sf)).^2);
f = (0:N/2-1)/N*Fs;
G = G(1:N/2);
plot(f,G);
grid;
xlabel(‘f[HZ]’);
ylabel(‘PSD[dB]‘);
% obrázek

7)
MAXIMUM = find (G == max(G));
%úprava indexu
MAXIMUM = MAXIMUM - 1;
% Výsledek na = 4965Hz;

9)
[s, Fs] = audioread(‘xjudap00.wav’);
N = 16000;
R = xcorr(s, 'biased');
k = -N+1:N-1;
%omezeni pouze na 50 vzorku
k = k(16000-50:16000+50);
R = R(16000-50:16000+50);
plot(k, R);
% obrázek

10)
% hodnota R[10] je ve vektoru uložena na indexu 61
R[10] = -0.0190