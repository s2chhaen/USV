% Autor: Thach
% Verwendungszweck: Extrakt der Daten aus der rohen Datei vom Lidar
% Erstellt am 08.08.2023
% Version: 1.00
% Revision: 1.06

clc;

%Laden "ImportOptions" in Var opts
opts = detectImportOptions("Testdaten_Labor_mitBuch30cm_15-45Grd.csv");
rawData=readcell("Testdaten_Labor_mitBuch30cm_15-45Grd.csv",opts);
rawDataTemp=rawData;
lenRawData=size(rawData);%Array = {Zeilenanzahl,Spaltenanzahl}

%Löschen der Einheit in der Messwerte
%Löschen letzte 2 Charakter von allen Elementen von Spalte 2,4,5 in rawData
%Löschen letztes Charakter von allen Elementen von Spalte 1 in rawData
for i=1:lenRawData(1,1)
    rawDataTemp{i,2}=rawDataTemp{i,2}(1:end-2);
    rawDataTemp{i,3}=rawDataTemp{i,3}(1:end-1);
    rawDataTemp{i,4}=rawDataTemp{i,4}(1:end-2);
    rawDataTemp{i,5}=rawDataTemp{i,5}(1:end-2);
end

numOfSubArray=uint8(0);

for i=1:lenRawData(1,1)
    temp = str2double(rawDataTemp{i,2});
    if isnan(temp)
        numOfSubArray=numOfSubArray+1;
    end  
end

avoidedLines = uint16(numOfSubArray);%Zeile mit Daten
numOfSubArray = numOfSubArray/2+1;

subArrayLen = (uint16(lenRawData(1,1))-avoidedLines)/uint16(numOfSubArray);

%Extrakt der Radius- und Winkelwerte
dataOutputRA = zeros(subArrayLen,2,numOfSubArray);
%Extrakt der Koordinatewerte
% dataOutputXY = zeros(subArrayLen,2,numOfSubArray);
%Betrachtet nur Radius und Winkel-Spalten
radiusCol = 2;
angleCol = 3;
% xCol = 4;
% yCol = 5;
% format shortG
for i=1:numOfSubArray
    beginIndex = double(363*uint16(i) - 362);
    endIndex = double(363*uint16(i) - 2);
    %Extrahiert der Daten vom Spalte Radius in temp Variable
    temp = rawDataTemp(beginIndex:endIndex,radiusCol);
    %Verwenden der Funktion str2double für alle Elemente vom Cell-Temp
    dataOutputRA(1:subArrayLen,1,i)=cellfun(@(a) str2double(a),temp);
    %Analog für Winkel-Spalte
    temp = rawDataTemp(beginIndex:endIndex,angleCol);
    dataOutputRA(1:subArrayLen,2,i)=cellfun(@(a) str2double(a),temp);
    %Analog für X Spalte
    % temp = rawDataTemp(beginIndex:endIndex,xCol);
    % dataOutputXY(1:subArrayLen,1,i)=cellfun(@(a) str2double(a),temp);
    %Analog für Y Spalte
    % temp = rawDataTemp(beginIndex:endIndex,yCol);
    % dataOutputXY(1:subArrayLen,2,i)=cellfun(@(a) str2double(a),temp);
end

%Löschen der nicht-notwendigen Variablen
clear yCol xCol temp subArrayLen rawDataTemp rawData radiusCol opts ...
      numOfSubArray lenRawData i endIndex beginIndex avoidedLines angleCol

