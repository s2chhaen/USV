% Autor: Thach
% Verwendungszweck: Modellierung des Quadtrees
% Erstellt am 17.08.2023
% Version: 1.00
% Revision: 1.07

classdef quadtreeClass < handle
    properties
        node=[]
        pointData=[]%Wo die Koordinatedaten gespeicheirt werden, cell array
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
            obj.depth=1;
            obj.node = [nodeClass([],[],0)];%Root erzeugt
            obj.parent = [{0,0}];
        end

        function []=setPointData(obj,inputVal)
            if nargin~=1
                error('Eingabe nicht gültig');
            elseif ~isnumeric(inputVal)
                error('Eingabe ist kein nodeClass');
            elseif (obj.id~=3) || isempty(obj)
                error('obj ist kein quadtreeClass oder ist leer');
            else
                obj.pointData=inputVal;
            end
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