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

%Anmerkung space = 32, new line = 10, CarriageReturn = 13
%Erzeugt der "input.txt" Data in C-Daten-Ordner
fileID = fopen('../FilterTesting/FilterTesting/input.txt','w+','l','UTF-8');

%Umwandlung zur Fixpoint-Form mit 15 Bits für Nachkommateil
%Fixed Point Format 17.15
fractionLen = 15;
fixedPoint = zeros(1,numel(rSample));
for i=1:numel(rSample)
    fixedPoint(i) = floor(rSample(i)*2^fractionLen);
end
%Schreiben der Daten in "input.txt"
fprintf(fileID,'%d \n',fixedPoint);
%Schließen der "input.txt" Datei
status = fclose(fileID);







