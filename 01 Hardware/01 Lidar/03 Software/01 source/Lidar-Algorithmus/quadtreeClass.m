% Autor: Thach
% Verwendungszweck: Modellierung des Quadtrees
% Erstellt am 17.08.2023
% Version: 1.00
% Revision: 1.07

classdef quadtreeClass < handle
    properties
        node=[]
    end
    properties (Access = private)
        maxPointsPerNode = 4
    end
    properties (GetAccess = public, SetAccess=private)
        depth{mustBeNumeric}%Anzahl von der Schichte vom Tree
        parent = []%Stelle der Eltern von einem Node, 0,0 bedeutet root
        id = 3
    end

    methods (Access=private)
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
            obj.depth = 0;
            obj.node = {[]};
            obj.parent = [];
        end

        function []=setRoot(obj,top,bot)
            if nargin~=3
                error('nicht genug Parameter für Funktion');
            elseif isempty(obj) || (obj.id~=3)
                error('Eingabe ungültig');
            else
                checkPoints = obj.checkTopAndBotPoints(top,bot);
                if checkPoints
                    root = quadtreeNodeClass(bot(1,1),top(1,1), ...
                        bot(1,2),top(1,2));
                    obj.depth = 1;
                    obj.node = {root};
                    obj.parent = {[0,0]};
                else
                    error('Eingabe ungültig');
                end 
            end
        end


    end
end