% Autor: Thach
% Verwendungszweck: Gesamte Ausführung der Quellcode von anderen Daten
% Erstellt am 23.08.2023
% Version: 1.00
% Revision: 1.02

clear;
clc;
format short g

%cd 'C:\BA ME\USV\01 Hardware\01 Lidar\03 Software\01 source\Lidar-Algorithmus';


LidarDataExtract; %Ausführung der LidarDataExtract.m Skript
lenXY = size(dataOutputXY);
xCol = 1;
yCol = 2;
sample = zeros(lenXY(1),lenXY(2)); %Stichprobe zum Testen

%Durchschnittberechnung
for i=1:lenXY(1)   
    sample(i,xCol) = (sum(dataOutputXY(i,xCol,:),'all')/lenXY(1,3));
    sample(i,yCol) = (sum(dataOutputXY(i,yCol,:),'all')/lenXY(1,3));
end

xMaxVal = max(sample(:,xCol));
yMaxVal = max(sample(:,yCol));
xMinVal = min(sample(:,xCol));
yMinVal = min(sample(:,yCol));
top = [xMaxVal,yMaxVal];
bot = [xMinVal,yMinVal];
tree = quadtreeClass();
tree = tree.setRoot(top,bot);
clear xMaxVal yMaxVal xMinVal yMinVal dataOutputXY dataOutputRA i temp;
clear xCol yCol lenXY;
tree = tree.addPoints(sample);



