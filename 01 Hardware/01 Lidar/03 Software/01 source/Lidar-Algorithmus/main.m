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
%aSample = arrayfun(@(a) deg2rad(a),sample(:,aCol));
aSample = sample(:,aCol);
tVal = transpose(aSample);

rMax = max(rSample);
rMin = min(rSample);

%Filter param
order = filterOrder(rMin,rMax);
fs = filterSampleFreq(sample);

% plot(aSample,rSample,'-o');
% xlabel('Winkel/Grad');
% ylabel('Radius/cm');

% polarplot(aSample,rSample,'-o');
% thetalim([0,180]);
% rlim([30 40]);

clear lenRA rCol aCol i aSample;
%IIR - Filter
fiir = filterChebyshevI_p12;
[b,a] = sos2tf(fiir.sosMatrix);
result0 = rSample;
result1 = filter(b,a,rSample);

%FIR Filter
%ffir
%result1 = filter(Num,1,rSample);

plot(tVal,result0,'-o',tVal,result1,'-o');
legend('Original Data','Filtered Data');

xlabel('Winkel/Grad');
ylabel('Radius/cm');
xlim([15,60]);





