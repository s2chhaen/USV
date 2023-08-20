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
        function obj=quadtreeClass(inputVal,mainCoord)
            obj.depth = 0;
            obj.node = [];
            obj.parent = [];
        end

        function []=makeNodes(obj)
            if nargin~=1
                error('Eingabe nicht gültig');
            elseif obj.id~=3
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