% Autor: Thach
% Verwendungszweck: Gesamte Ausführung der Quellcode von anderen Daten
% Erstellt am 23.08.2023
% Version: 1.00
% Revision: 1.02

clear;
clc;
format short g

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
tree = tree.updateChildAttLvl(1);

temp = tree.node(1);
tree = tree.addChildrenForNode(temp);
tree = tree.updateChildAttLvl(2);

temp = tree.node(5);
tree = tree.addChildrenForNode(temp);

temp = tree.node(2);
tree = tree.addChildrenForNode(temp);

temp = tree.node(4);
tree = tree.addChildrenForNode(temp);

temp = tree.node(3);
tree = tree.addChildrenForNode(temp);

tree = tree.updateChildAttLvl(2);


clear xMaxVal yMaxVal xMinVal yMinVal dataOutputXY dataOutputRA i top bot ...
      temp;

