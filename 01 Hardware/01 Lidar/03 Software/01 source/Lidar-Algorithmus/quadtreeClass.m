% Autor: Thach
% Verwendungszweck: Modellierung des Quadtrees
% Erstellt am 17.08.2023
% Version: 1.00
% Revision: 1.09

classdef quadtreeClass < handle
    properties
        node=[]
        child=[]
        parent=[]
        level=[]
    end
    properties (Access = private)
        maxPointsPerNode = 4
    end
    properties (GetAccess = public, SetAccess=private)
        depth{mustBeNumeric}%Anzahl von der Schichte vom Tree
        id = 3
    end

    %TODO als Private nach dem Debuggen festzulegen
    methods (Access=public)
        function boolVal=checkTopAndBotPoints(obj,top,bot)
            boolVal = false;
            if nargin == 3
                checkTop = checkPoint(top);
                checkBot = checkPoint(bot);
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

        function [posVal,frontPos]=insert2ParentAttribute(obj,newParentPos)
            if nargin~=2
                error('Nicht genug Parameter für Funktion');
            elseif isempty(obj) || isempty(newParentPos)
                error('Eingabe ungültig');
            elseif (obj.id~=3) || ~isnumeric(newParentPos)
                error('Eingabe ungültig');
            else
                maxEl = numel(obj.parent);
                if maxEl == 0
                    obj.parent = [newParentPos];
                else
                    beginIdx = 1;
                    endIdx = maxEl;
                    %pivot = 0;
                    while(beginIdx~=endIdx)
                        pivot = ceil((beginIdx+endIdx)/2);
                        temp = obj.parent(pivot);
                        if temp < newParentPos
                            beginIdx = pivot;
                        else
                            endIdx = pivot-1;
                        end
                    end

                    posVal = beginIdx;
                    if newParentPos>obj.parent(beginIdx)
                        %hinter dem Index hinzugefügt
                        obj.parent = [obj.parent(1:beginIdx) newParentPos obj.parent((beginIdx+1):end)];
                        frontPos = 0;%für hintere Position kennzeichen
                    else
                        %vorne dem Index hinzugefügt
                        obj.parent = [obj.parent(1:(beginIdx-1)) newParentPos obj.parent(beginIdx:end)];
                        frontPos = 1;%für vorne Position kennzeichen
                    end
                end
            end
        end

        function obj=insert2NodeAttribute(obj,node,pos,front)
                if nargin~=4
                    error('Nicht genug Parameter für Funktion');
                %TODO Fehlerfall zu trennen
                else
                    temp0 = obj.node;
                    if front==0
                        if pos == numel(temp0)
                            obj.node(1,(pos+1)) = node;
                        else
                            temp1 = temp0(1:pos);
                            temp2 = temp0((pos+1):end);
                            obj.node = [temp1 node temp2];
                        end
                    else
                        if pos==1
                            temp1 = obj.node;
                            obj.node = node;
                            obj.node(1,2) = temp1;
                        else
                            temp1 = temp0(1:(pos-1));
                            temp2 = temp0(pos:end);
                            obj.node = [temp1 node temp2];
                        end
                    end
                end
        end

        function [row,col]=searchNode(obj,node)
            if nargin~=2
                error('Nicht genug Parameter für Funktion');
            elseif isempty(obj) || isempty(node)
                error('Eingabe ungültig');
            elseif (obj.id~=3) || (node.id~=2)
                error('Eingabe ungültig');
            else
                row = 1;
                col = 0;
                nrOfLevel = obj.depth;
                foundNode = false;
                for i=1:nrOfLevel
                    temp = obj.level(i,:);
                    beginPos = temp(1);
                    endPos = temp(2);
                    for j=beginPos:endPos
                        temp = obj.node(j);
                        nodesCmp = quadnodeCompare(temp,node);
                        if strcmp(nodesCmp,'coincide')
                            col = j;
                            foundNode = true;
                            break;
                        end
                    end
                    if foundNode==true
                        break;
                    end
                end
            end
        end

        % Beim Aufruf wird 4 Kinder für das Node hinzugefügt
        function obj=addChildrenForNode(obj,node)
            if nargin~=2
                error('Eingabe ungültig');
            elseif isempty(obj) || isempty(node)
                error('Eingabe ungültig');
            elseif (obj.id~=3) || (node.id~=2)
                error('Eingabe ungültig');
            else
                [~,nodePos]=obj.searchNode(node);
                if nodePos~=0
                    subNodeLvl = node.level + 1;
                    avgX = (node.xValMin + node.xValMax)/2;
                    avgY = (node.yValMin + node.yValMax)/2;
                    % Nord-West-Bereich
                    top = [avgX,node.yValMax];
                    bot = [node.xValMin,avgY];
                    nordWestNode = quadtreeNodeClass();
                    nordWestNode = nordWestNode.quadNodeInit(bot,top);
                    nordWestNode = nordWestNode.setLevel(subNodeLvl);
                    [insertPos,front] = obj.insert2ParentAttribute(nodePos);
                    obj = obj.insert2NodeAttribute(nordWestNode,insertPos,front);
                    % Nord-Ost-Bereich
                    top = [node.xValMax,node.yValMax];
                    bot = [avgX,avgY];
                    nordEastNode = quadtreeNodeClass();
                    nordEastNode = nordEastNode.quadNodeInit(bot,top);
                    nordEastNode = nordEastNode.setLevel(subNodeLvl);
                    [insertPos,front] = obj.insert2ParentAttribute(nodePos);
                    obj = obj.insert2NodeAttribute(nordEastNode,insertPos,front);
                    %Süd-Ost-Bereich
                    top = [node.xValMax,avgY];
                    bot = [avgX,node.yValMin];
                    southEastNode = quadtreeNodeClass();
                    southEastNode = southEastNode.quadNodeInit(bot,top);
                    southEastNode = southEastNode.setLevel(subNodeLvl);
                    [insertPos,front] = obj.insert2ParentAttribute(nodePos);
                    obj = obj.insert2NodeAttribute(southEastNode,insertPos,front);
                    %Süd-West OK
                    top = [avgX,avgY];
                    bot = [node.xValMin,node.yValMin];
                    southWestNode = quadtreeNodeClass();
                    southWestNode = southWestNode.quadNodeInit(bot,top);
                    southWestNode = southWestNode.setLevel(subNodeLvl);
                    [insertPos,front] = obj.insert2ParentAttribute(nodePos);
                    obj = obj.insert2NodeAttribute(southWestNode,insertPos,front);
                    obj.node(nodePos).child = 4;%TODO über getter/setter festlegen
                    treeDepth = obj.depth;
                    if subNodeLvl==(treeDepth+1)
                        beginPos = obj.level(treeDepth,2)+1;
                        endPos = beginPos + 4;
                        obj.level(treeDepth+1,1)=beginPos;
                        obj.level(treeDepth+1,2)=endPos;
                        obj.depth = treeDepth+1;
                    elseif subNodeLvl<=treeDepth
                        obj.level(subNodeLvl,2) = obj.level(subNodeLvl,2)+4;
                    end
                else
                    error('Eingabe ungültig');
                end
            end
        end

    end

    methods
        function obj = setRoot(obj,top,bot)
            if nargin~=3
                error('nicht genug Parameter für Funktion');
            elseif isempty(obj) || (obj.id~=3)
                error('Eingabe ungültig');
            else
                checkPoints = obj.checkTopAndBotPoints(top,bot);
                if checkPoints
                    root = quadtreeNodeClass();
                    root = root.quadNodeInit(bot,top);
                    root = root.setLevel(1);
                    obj.parent = [0];
                    obj.level = [1,1];
                    obj.depth = 1;
                    obj.node = [root];
                else
                    error('Eingabe ungültig');
                end 
            end
        end

        function arrayVal=getLeaf(obj)
            arrayVal = [];
            if nargin~=1
                error('nicht genug Parameter für Funktion');
            elseif isempty(obj) || (obj.id~=3)
                error('Eingabe ungültig');
            else
                if ~isempty(obj.node)
                    for i=1:numel(obj.node)
                end
            end
        end

        function obj = addPoints(obj, points)
            if nargin~=2
                error('nicht genug Parameter für Funktion');
            elseif isempty(obj) || (obj.id~=3)
                error('Eingabe ungültig');
            elseif isempty(points) || isnumeric(points)
                error('Eingabe ungültig');
            else
                pointsNum = numel(points);
            end
        end
    end
end