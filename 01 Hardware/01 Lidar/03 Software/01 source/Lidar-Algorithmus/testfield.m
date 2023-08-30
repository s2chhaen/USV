% Autor: Thach
% Verwendungszweck: Testbereich für Programmierung/Debuggen
% Erstellt am 14.08.2023
% Version: 1.00
% Revision: 1.05

clc;
clear;
% c = stackClass();
% c = c.push(10);
% c = c.push(9);
% c = c.push(8);
% c = c.push(7);
% c = c.push(6);
% a = [5,4,3,2,1];
% c = c.push(a);
% d = c.stackLen();
% [c,a] = c.pop();
% d = c.stackLen();
% [c,a] = c.pop();
% d = c.stackLen();
% [c,a] = c.pop();
% d = c.stackLen();
% [c,a] = c.pop();
% d = c.stackLen();
% [c,a] = c.pop();
% d = c.stackLen();
% [c,a] = c.pop();
% d = c.stackLen();
% [c,a] = c.pop();
% d = c.stackLen();

a = [10,10;9,9;8,8;7,7;6,6;5,5;4,4;3,3;2,2;1,1;0,0];
deleteIdx = [5,3,1];
a(deleteIdx,:) = [];

% pointvectorX = cellfun(@(x)x(1,1),c.nodeList);
%nur beachtet die 2.Ausgabe von sort nicht die erste, in diesem Fall nur index
% [~,sortIndex] = sort(pointvectorX);
% c.nodeList = c.nodeList(sortIndex);%reorder nodeList cell in abhängig vom Indizes

% c = quadtreeNodeClass(10,15,10,1);
% 
% c.addAPoint([1,2]);
% c.addAPoint([1,2]);
% c.addAPoint([1,2]);
% c.addAPoint([1,2]);
% c.addAPoint([1,2]);
% c.addAPoint([1,2]);
% c.addAPoint([1,2]);
% c.addAPoint([1,20]);
% c.addAPoint([1,10]);
% c.addAPoint([-11,20]);
% c.addAPoint([2,20]);
% c.addAPoint([-12,20]);
% c.addAPoint([3,20]);
% c.addAPoint([4,20]);
% c.addAPoint([-2,20]);
% c.sortPointsList('x');
% c.addAPoint([1,2]);

% a = c.removePointCoord(1,2);
%a = c.removePointCoord(1,20);
%c.pointList = [];

% tic;
% c = quadtreeClass();
% c = c.setRoot([2,2],[0,0]);
% % d = c.checkPoint([1,1]);
% % d = c.checkTopAndBotPoints([2,2],[0,0]);
% root = c.node{1,1};
% c = c.addChildrenForNode(c.node(1,1));
% toc

% root = c.node{1,1};
% [row,col]=c.searchNode(root);

% a = [10 20 30 40 50 60 70 80 90 100 101 102 103 104];%Array
% b = numel(a);
% d = 105;%Hinzugefügtes Elementes
% %floor(): round down, ceil(): round up
% %Hinzufüge d in a ohne Wiedersortieren
% beginIdx = 1;
% endIdx = numel(a);
% pivot = 0;
% if(numel(a)==0)
%     a = [d];%Array, da leicht zu gucken
% else
%     while(beginIdx~=endIdx)
%         pivot = ceil((beginIdx+endIdx)/2);
%         temp = a(pivot);
%         if temp < d
%             beginIdx = pivot;
%         else
%             endIdx = pivot-1;
%         end
%     end
% 
%     if d>a(beginIdx)
%         a = [a(1:beginIdx) d a((beginIdx+1):end)];%Array, da leicht zu gucken
%     else
%         a = [a(1:(beginIdx-1)) d a(beginIdx:end)];%Array, da leicht zu gucken
%     end
% end

% c=dataOutputXY(:,:,1);
% x=c(:,1);
% y=c(:,2);
% plot(x,y,'-o')
% xlabel('x in cm');
% ylabel('y in cm');
% clear x y c;
%c=dataOutputRA(:,:,1);
% angle = arrayfun(@(a) deg2rad(a),c(:,2));%wie cellfunc aber für Array
% radius = c(:,1);
% polarplot(angle,radius,'-o');
% rlim([30 38]);
%rlim('auto');
%clear angle, radius;
% plot(x,y,'-o')
% xlabel('winkel in grad');
% ylabel('radius in cm');