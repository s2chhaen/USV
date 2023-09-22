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

printInFile('fixed',rSample);
printInFile('float',rSample);

%IIR
figureNo = 1;
choosedIIR = 'mf';
filterIIRScript;
printOutFile('fixed',choosedIIR,result1);
printOutFile('float',choosedIIR,result1);

%FIR
choosedFIR = 'wk';
filterFIRScript;
printOutFile('fixed',choosedFIR,result1);
printOutFile('float',choosedFIR,result1);

choosedFIR = 'lsq';
filterFIRScript;
printOutFile('fixed',choosedFIR,result1);
printOutFile('float',choosedFIR,result1);

choosedFIR = 'eq';
filterFIRScript;
printOutFile('fixed',choosedFIR,result1);
printOutFile('float',choosedFIR,result1);

choosedFIR = 'wb';
filterFIRScript;
printOutFile('fixed',choosedFIR,result1);
printOutFile('float',choosedFIR,result1);

choosedFIR = 'wr';
filterFIRScript;
printOutFile('fixed',choosedFIR,result1);
printOutFile('float',choosedFIR,result1);

choosedFIR = 'wt';
filterFIRScript;
printOutFile('fixed',choosedFIR,result1);
printOutFile('float',choosedFIR,result1);






