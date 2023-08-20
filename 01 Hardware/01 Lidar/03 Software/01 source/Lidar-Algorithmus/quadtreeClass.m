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
        maxChild = 4
    end
    properties (GetAccess = public, SetAccess=private)
        depth{mustBeNumeric}%Anzahl von der Schichte vom Tree
        parent = []%Stelle der Eltern von einem Node, 0,0 bedeutet root
        id = 3
    end
    methods
        %Constructor
        function obj=quadtreeClass()
            obj.depth = 0;
            obj.node = [];
            obj.parent = [];
        end

        function boolVal=checkPoint(point)
            boolVal = false;
            if nargin == 1
                checkEmpty = isempty(point);
                checkElementNo = numel(point)==2;
                checkElementType = isnumeric(point);
                if checkEmpty && checkElementType && checkElementNo
                    boolVal = true;    
                end
            end
        end

        function boolVal=checkTopAndBotPoints(top,bot)
            boolVal = false;
            checkPointType = checkPoint(top) && checkPoint(bot);
            if checkPointType
                checkX = top(1,1) > bot(1,1);
                checkY = top(1,2) > bot(1,2);
                if checkX && checkY
                    boolVal = true;
                end
            end
        end

        function []=setRoot(obj,top,bot)
            if nargin~=3
                error('nicht genug Parameter für Funktion');
            elseif isempty(obj)
                error('Eingabe ungültig');
            else
                checkPoints = checkTopAndBotPoints(top,bot);
                if checkPoints
                    root = quadtreeNodeClass(bot(1,1),top(1,1), ...
                        bot(1,2),top(1,2));
                    obj.depth = 1;
                    obj.node = [root];
                    obj.parent = {[0,0]};
                else
                    error('Eingabe ungültig');
                end
                
            end
        end

	%TODO Noch in Bearbeitung
        % function []=addChild(obj,child,parentPosition)
        %     if (parentPosition(1,1)==0) && (parentPosition(1,2)==0)
        %         error 'root kann nicht gesetzt mit dieser Methode';
        %     elseif isempty(obj) ||  isempty(child)
        %         error 'Eltern oder Kind ist leer';
        %     elseif (obj.id~=1) ||  (child.id~=1)
        %         error 'Eltern oder Kind ist kein nodeClass';
        %     end
        %     obj.parent = []
        % end
    end
end