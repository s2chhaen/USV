% Autor: Thach
% Verwendungszweck: Gesamte Ausführung der Quellcode von anderen Daten
% Erstellt am 23.08.2023
% Version: 1.01
% Revision: 1.00

%clear;
clc;
format shortG

LidarDataExtract; %Ausführung der LidarDataExtract.m Skript
lenRA = size(dataOutputRA);
rCol = 1;%Radius
aCol = 2;%Winkel
sample = zeros(lenRA(1,1),lenRA(1,2)); %Stichprobe zum Testen


%Durchschnittberechnung
for i=1:lenRA(1)   
    sample(i,rCol) = (sum(dataOutputRA(i,rCol,:),'all')/lenRA(1,3));
    sample(i,aCol) = (sum(dataOutputRA(i,aCol,:),'all')/lenRA(1,3));
end

rSample = sample(:,rCol);
rSample = transpose(rSample);
aSample = sample(:,aCol);
tVal = transpose(aSample);

rMax = max(rSample);
rMin = min(rSample);

%Filter param
order = filterOrder(rMin,rMax);
fs = filterSampleFreq(sample);

clear lenRA rCol aCol i aSample;
%IIR - Filter
ffir = filterWindowFlatTop_p4;
result0 = rSample;

l = length(ffir.Numerator);
%Dummy-Sample erzeugen um die Phasenverschiebung zu vermeiden
rSample = [rSample,ones(1,fix(l/2))*rSample(end)];
%FIR Filter
result1 = filter(ffir.Numerator,1,rSample);
%Wegwerfen vom Dummy-Sample
result1 = result1(ceil(l/2):end-1);

figure(1)
ax(1) = subplot(2,1,1);
plot(tVal,result0,'-o',tVal,result1,'-o');
legend('Original Data','Filtered Data');
grid on

xlabel('Winkel/Grad');
ylabel('Radius/cm');

ax(2) = subplot(2,1,2);
plot(tVal,20.*log10(result1./result0),'-o');
legend('magnitude/dB');
grid on

xlabel('Winkel/Grad');
ylabel('Radius/cm');

linkaxes(ax,'x')





