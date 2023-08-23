% Autor: Thach
% Verwendungszweck: Modellierung des Quadtrees
% Erstellt am 17.08.2023
% Version: 1.00
% Revision: 1.08

classdef quadtreeClass < handle
    properties
        node={}
        child={}
        parent={}
        level={}
    end
    properties (Access = private)
        maxPointsPerNode = 4
    end
    properties (GetAccess = public, SetAccess=private)
        depth{mustBeNumeric}%Anzahl von der Schichte vom Tree
        id = 3
    end

    methods (Access=public)
        function boolVal=checkPoint(obj,point)
            boolVal = false;
            if nargin == 2
                checkEmpty = ~isempty(point);
                checkElementNo = numel(point)==2;
                checkElementType = isnumeric(point);
                if checkEmpty && checkElementType && checkElementNo
                    boolVal = true;    
                end
            end
        end

        function boolVal=checkTopAndBotPoints(obj,top,bot)
            boolVal = false;
            if nargin == 3
                checkTop = obj.checkPoint(top);
                checkBot = obj.checkPoint(bot);
                checkPointType = checkTop && checkBot;
                if checkPointType
                    checkX = top(1,1) > bot(1,1);
                    checkY = top(1,2) > bot(1,2);
                    if checkX && checkY
                        boolVal = true;
                    end
                end
            end
        end

        function [row,col]=searchNode(obj,node)
            if nargin~=2
                error('Eingabe ungültig');
            elseif isempty(obj) || isempty(node)
                error('Eingabe ungültig');
            elseif (obj.id~=3) || (node.id~=2)
                error('Eingabe ungültig');
            else

            end
        end
        % Beim Aufruf wird 4 Kinder für das Node hinzugefügt
        function cellVal=addChildForNode(obj,node)
            if nargin~=2
                error('Eingabe ungültig');
            elseif isempty(obj) || isempty(node)
                error('Eingabe ungültig');
            elseif (obj.id~=3) || (node.id~=2)
                error('Eingabe ungültig');
            else
                checkNode = (node.xValMin < node.xValMax) && ( ...
                    node.yValMin < node.yValMax);
                if checkNode
                    avgX = (node.xValMin + node.xValMax)/2;
                    avgY = (node.yValMin + node.yValMax)/2;
                    % Nord-West-Bereich OK
                    xValMax = avgX;
                    yValMax = node.yValMax;
                    xValMin = node.xValMin;
                    yValMin = avgY;
                    nordWestNode = quadtreeNodeClass(xValMin,xValMax, ...
                                                     yValMin,yValMax);
                    % Nord-Ost-Bereich OK
                    xValMax = node.xValMax;
                    yValMax = node.yValMax;
                    xValMin = avgX;
                    yValMin = avgY;
                    nordEastNode = quadtreeNodeClass(xValMin,xValMax, ...
                                                     yValMin,yValMax);
                    %Süd-Ost-Bereich OK
                    xValMax = node.xValMax;
                    yValMax = avgY;
                    xValMin = avgX;
                    yValMin = node.yValMin;
                    southEastNode = quadtreeNodeClass(xValMin,xValMax, ...
                                                      yValMin,yValMax);
                    %Süd-West OK
                    xValMax = avgX;
                    yValMax = avgY;
                    xValMin = node.xValMin;
                    yValMin = node.yValMin;
                    southWestNode = quadtreeNodeClass(xValMin,xValMax, ...
                                                      yValMin,yValMax);
                    cellVal = {nordWestNode, nordEastNode, southEastNode, ...
                               southWestNode};
                else
                    error('Eingabe ungültig');
                end
            end
        end

    end

    methods
        %Constructor
        function obj=quadtreeClass()
        end

        function [] = setRoot(obj,top,bot)
            if nargin~=3
                error('nicht genug Parameter für Funktion');
                %TODO checken top und bot Parameter noch
            elseif isempty(obj) || (obj.id~=3)
                error('Eingabe ungültig');
            else
                checkPoints = obj.checkTopAndBotPoints(top,bot);
                if checkPoints
                    root = quadtreeNodeClass(bot(1,1),top(1,1),bot(1,2),top(1,2),1);
                    obj.parent = {[0,0]};
                    obj.level = {[1,1]};
                    obj.depth = 1;
                    obj.node = root;
                else
                    error('Eingabe ungültig');
                end 
            end
        end

        function [] = addPoints(obj, points)
            if nargin~=2
                error('nicht genug Parameter für Funktion');
            elseif isempty(obj) || (obj.id~=3)
                error('Eingabe ungültig');
            elseif isempty(points) || isnumeric(points)
                error('Eingabe ungültig');
            else
                childrenNumber = numel(obj)
            end
        end
    end
end