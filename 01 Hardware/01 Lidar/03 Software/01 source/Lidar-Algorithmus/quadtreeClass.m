% Autor: Thach
% Verwendungszweck: Modellierung des Quadtrees
% Erstellt am 17.08.2023
% Version: 1.00
% Revision: 1.11

classdef quadtreeClass < handle
properties
    node=[]%Zeile-Vektor
    child=[]%2xn Matrix
    parent=[]%Zeile-Vektor
    level=[]%nx2 Matrix
end

properties (Access = private)
    maxPointsPerNode = 5
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

function [posVal,frontPos]=insert2ParentAtt(obj,newParentPos)
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
    elseif node.child==0
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
            nordWestNode = nordWestNode.setName('nord-west');
            [insertPos,front] = obj.insert2ParentAtt(nodePos);
            obj = obj.insert2NodeAttribute(nordWestNode,insertPos,front);
            % Nord-Ost-Bereich
            top = [node.xValMax,node.yValMax];
            bot = [avgX,avgY];
            nordEastNode = quadtreeNodeClass();
            nordEastNode = nordEastNode.quadNodeInit(bot,top);
            nordEastNode = nordEastNode.setLevel(subNodeLvl);
            nordEastNode = nordEastNode.setName('nord-east');
            [insertPos,front] = obj.insert2ParentAtt(nodePos);
            obj = obj.insert2NodeAttribute(nordEastNode,insertPos,front);
            %Süd-Ost-Bereich
            top = [node.xValMax,avgY];
            bot = [avgX,node.yValMin];
            southEastNode = quadtreeNodeClass();
            southEastNode = southEastNode.quadNodeInit(bot,top);
            southEastNode = southEastNode.setLevel(subNodeLvl);
            southEastNode = southEastNode.setName('south-east');
            [insertPos,front] = obj.insert2ParentAtt(nodePos);
            obj = obj.insert2NodeAttribute(southEastNode,insertPos,front);
            %Süd-West
            top = [avgX,avgY];
            bot = [node.xValMin,node.yValMin];
            southWestNode = quadtreeNodeClass();
            southWestNode = southWestNode.quadNodeInit(bot,top);
            southWestNode = southWestNode.setLevel(subNodeLvl);
            southWestNode = southWestNode.setName('south-west');
            [insertPos,front] = obj.insert2ParentAtt(nodePos);
            obj = obj.insert2NodeAttribute(southWestNode,insertPos,front);
            obj.node(nodePos).child = 4;%TODO über getter/setter festlegen
            treeDepth = obj.depth;
            if subNodeLvl==(treeDepth+1)
                beginPos = obj.level(treeDepth,2)+1;
                endPos = obj.level(treeDepth,2)+4;
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

function obj=updateChildAttNode(obj,node)
    if nargin~=2
        error('Eingabe ungültig');
    elseif isempty(obj)||isempty(node)||(node.id~=2)
        error('Eingabe ungültig');
    else
        [~,col] = obj.searchNode(node);
        %TODO return level not used col col is the real idx in obj.node Array
        if col~=0
            beginIdx0 = 1;
            tempSize = size(obj.level);
            endIdx0 = tempSize(1,1);
            while beginIdx0~=endIdx0
                pivot = ceil((beginIdx0+endIdx0)/2);
                temp = obj.level(pivot,:);
                if (col>=temp(1)) && (col<=temp(2))
                    beginIdx0 = pivot;
                    endIdx0 = pivot;
                elseif col < temp(1)
                    endIdx0 = pivot-1;
                elseif col > temp(2)
                    beginIdx0 = pivot;
                end
            end

            if obj.depth > beginIdx0
                lvlMemIdx = obj.level(beginIdx0+1,:);
                beginIdx = lvlMemIdx(1,1);
                endIdx = lvlMemIdx(1,2);
                firstChildIdx = 0;
                for i=beginIdx:endIdx
                    parentIdx = obj.parent(i);
                    if col == parentIdx
                        firstChildIdx = i;
                        break;%In diesem quadTree liegen die Kinder nebeneinander
                    end
                end
                if firstChildIdx~=0
                    nodeChildNum = obj.node(1,col).child;
                    obj.child(:,col) = [firstChildIdx;(firstChildIdx+nodeChildNum-1)];
                else
                    obj.child(:,col) = [0;0];
                end
            else
                %Wenn die vorne Knoten kein Kind haben, 
                % dann wird es automatisch dafür hinzugefügt
                obj.child(:,col) = [0;0];
            end
        else
            error('Node ist nicht zum Quadtree');
        end
    end
end

function obj=updateChildAttLvl(obj,level)
    if nargin~=2
        error('Eingabe ungültig');
    elseif isempty(obj)||isempty(level)||~isnumeric(level)
        error('Eingabe ungültig');
    else
        if level <= obj.depth
            levelIdx = obj.level(level,:);
            for i= levelIdx(1):levelIdx(2)
                tempNode = obj.node(i);
                obj = obj.updateChildAttNode(tempNode);
            end
        end
    end
end

end

methods (Access=public)
    
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
            root = root.setName('root');
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
        if numel(obj.node)>1
            for j=1:numel(obj.level)
                obj = obj.updateChildAttLvl(j);
            end
            childNum = 0;
            childrenSet = zeros(1,numel(obj.child));
            endOfLoop = size(obj.child);
            for i=1:endOfLoop(1,2)
                childBeginIdx = obj.child(1,i);
                %Wenn Knot kein Kind hat, dann die Beginn- und Endindex
                % der Kinder in obj.children-Array beide gleich 0
                if childBeginIdx==0
                    childrenSet(childNum+1) = i;
                    childNum = childNum+1;
                end
            end
            if childNum~=0
                arrayVal = childrenSet(1:childNum);
            end
        end
    end
end

function obj = addPoints(obj, pointSet)
    if nargin~=2
        error('nicht genug Parameter für Funktion');
    elseif isempty(obj) || (obj.id~=3)
        error('Eingabe ungültig');
    elseif isempty(pointSet) || ~isnumeric(pointSet)
        error('Eingabe ungültig');
    elseif isempty(obj.node)
        error('kein Element in quadTreeClass');
    else
        checkPointSet = true;
        tempNode = obj.node(1);
        %Ein Punkt hat 2 Elemente X und Y (X,Y)
        endOfLoop = numel(pointSet)/2;
        for i=1:endOfLoop
            tempPoint = pointSet(i,:);
            %WARNING hohe Bugwahrscheinlichkeit hier
            checkPointSet = checkPoint(tempPoint);
            if checkPointSet == false
                break;
            end
            checkPointSet = tempNode.isInNode(tempPoint);
            if checkPointSet==false
                break;
            end
        end
        if checkPointSet == false
            error('Punkte-Form ist nicht gültig');
        else
            stackNodeIdx = stackClass();
            stackNodeIdxLen = stackNodeIdx.stackLen();
            stackPointIdx = stackClass();
            stackPointIdxLen = stackPointIdx.stackLen();
            %Root in stack hinzufügen
            stackNodeIdx = stackNodeIdx.push(1);
            
            while (stackNodeIdxLen~=0)
                [stackNodeIdx,tempNodeIdx] = stackNodeIdx.pop();
                if stackPointIdxLen~=0
                    tempBuffer = zeros(stackPointIdxLen,1);
                    tempBufferLen = 0;
                    tempNode = obj.node(tempNodeIdx);
                    for i=1:stackPointIdxLen
                        tempPointIdx = stackPointIdx.pop();
                        tempPoint = pointSet(tempPointIdx,:);
                        checkPoint = tempNode.isInNode(tempPoint);
                        if checkPoint == true
                            tempBuffer(tempBufferLen+1) = tempPoint;
                            tempBufferLen = tempBufferLen+1;
                        end
                    end

                    if tempBufferLen>0
                        tempBuffer = tempBuffer(1:tempBufferLen);
                        if tempBufferLen>obj.maxPointsPerNode
                            obj = obj.addChildrenForNode(tempNode);
                            obj = obj.updateChildAttNode(tempNode);
                            %Nehmen der Beginn- und Endposition von Kinder in Node-Attribut
                            childIdx = obj.child(tempNodeIdx);
                            %WARNING hohe Bugwahrscheinlichkeit hier
                            stackNodeIdx = stackNodeIdx.push(childIdx(1):childIdx(2));
                            stackPointIdx = stackPointIdx.push(tempBuffer);
                        else
                            for i=1:tempBufferLen
                                tempPoint = pointSet(i,:);
                                obj.node(tempNodeIdx).addAPoint(tempPoint);
                                pointSet(i,:) = [];
                            end
                        end
                    end

                    %TODO stop here weiter Implementation
                else
                    tempNode = obj.node(tempNodeIdx);
                    for i=1:endOfLoop
                        tempPoint = pointSet(i,:);
                        checkPoint = tempNode.isInNode(tempPoint);
                        %Hinzufüge aller Positionen der Elemente,die
                        %im Knoten liegen in pointIdxStack
                        if checkPoint == true
                            stackPointIdx = stackPointIdx.push(i);
                        end
                    end
                    stackPointIdxLen = stackPointIdx.stackLen();
                    if stackPointIdxLen>0
                        if stackPointIdxLen > obj.maxPointsPerNode
                            obj = obj.addChildrenForNode(tempNode);
                            obj = obj.updateChildAttNode(tempNode);
                            %Nehmen der Beginn- und Endposition von Kinder in Node-Attribut
                            childIdx = obj.child(tempNodeIdx);
                            %WARNING hohe Bugwahrscheinlichkeit hier
                            stackNodeIdx = stackNodeIdx.push(childIdx(1):childIdx(2));
                        else
                            for i=1:stackPointIdxLen
                                tempPoint = pointSet(i,:);
                                obj.node(tempNodeIdx).addAPoint(tempPoint);
                                pointSet(i,:) = [];
                            end
                        end
                    end
                end
                %Immer am Ende
                stackNodeIdxLen = stackNodeIdx.stackLen();
                stackPointIdxLen = stackPointIdx.stackLen();
            end
        end
    end
end

end

end