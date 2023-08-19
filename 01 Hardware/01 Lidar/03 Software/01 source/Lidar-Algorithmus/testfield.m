% Autor: Thach
% Verwendungszweck: Testbereich für Programmierung/Debuggen
% Erstellt am 14.08.2023
% Version: 1.00
% Revision: 1.05

clc;
% pointvectorX = cellfun(@(x)x(1,1),c.nodeList);
%nur beachtet die 2.Ausgabe von sort nicht die erste, in diesem Fall nur index
% [~,sortIndex] = sort(pointvectorX);
% c.nodeList = c.nodeList(sortIndex);%reorder nodeList cell in abhängig vom Indizes

c = quadtreeNodeClass(10,15,10,1);

c.addAPoint([1,2]);
c.addAPoint([1,2]);
c.addAPoint([1,2]);
c.addAPoint([1,2]);
c.addAPoint([1,2]);
c.addAPoint([1,2]);
c.addAPoint([1,2]);
c.addAPoint([1,20]);
c.addAPoint([1,10]);
c.addAPoint([-11,20]);
c.addAPoint([2,20]);
c.addAPoint([-12,20]);
c.addAPoint([3,20]);
c.addAPoint([4,20]);
c.addAPoint([-2,20]);
c.sortPointsList('x');
c.addAPoint([1,2]);

a = c.removePointCoord(1,2);
%a = c.removePointCoord(1,20);
%c.pointList = [];


%c=dataOutputXY(:,:,1);
%x=c(:,1);
%y=c(:,2);
% plot(x,y,'-o')
% xlabel('x in cm');
% ylabel('y in cm');
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