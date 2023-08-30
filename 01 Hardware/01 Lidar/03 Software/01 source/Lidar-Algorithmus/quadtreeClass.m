% Autor: Thach
% Verwendungszweck: Modellierung des Quadtrees
% Erstellt am 17.08.2023
% Version: 1.00
% Revision: 1.12

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
    freeNodeId = 1
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

function []=updateLvlAtt(obj)
    if nargin~=1
        error('nicht genug Parameter für Funktion');
    elseif isempty(obj)||(obj.id~=3)
        error('ungültige Eingabe');
    elseif isempty(obj.node)
        error('keinen Knoten in Quadtree');
    else
        bufferMaxLen = numel(obj.node);%max. Anzahl des Knotens
        memIdxBuffer = zeros(1,bufferMaxLen);
        memIdxBufferLen = 0;
        maxLevel = obj.node(bufferMaxLen).level;
        nextIdx = 1;
        for j=1:maxLevel
            i = nextIdx;
            while i<=bufferMaxLen
                if obj.node(i).level == j
                    memIdxBuffer(1,memIdxBufferLen+1) = i;
                    memIdxBufferLen = memIdxBufferLen+1;
                else
                    nextIdx = i;
                    break;
                end
                i = i + 1;
            end
            if memIdxBufferLen==1
                beginPosIdx = memIdxBuffer(memIdxBufferLen);
                endPosIdx = memIdxBuffer(memIdxBufferLen);
            else
                beginPosIdx = memIdxBuffer(1);
                endPosIdx = memIdxBuffer(memIdxBufferLen);
            end
            memIdxBufferLen = 0;
            obj.level(j,:) = [beginPosIdx,endPosIdx];
        end
    end
end

function []=insert2NodeAttribute(obj,node)
    if nargin~=4
        error('Nicht genug Parameter für Funktion');
    else
        if node.level == obj.depth+1
            obj.depth = obj.depth+1;
            obj.node = [obj.node node];
        elseif node.level <= obj.depth
            lvl = node.level;
            %hinter einfügen
            insertPos = node.level(lvl,2);
            endIdx = numel(obj.node);
            obj.node = [obj.node(1:insertPos) node obj.node((insertPos+1):endIdx)];
        end
    end
end

function []=insert2ParentAtt(obj,pos)
    if nargin~=2
        error('Nicht genug Parameter für Funktion');
    elseif isempty(obj) || isempty(pos)
        error('Eingabe ungültig');
    elseif (obj.id~=3) || ~isnumeric(pos)
        error('Eingabe ungültig');
    else
        maxEl = numel(obj.parent);
        if maxEl == 0
            obj.parent = pos;
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
            nordWestNode = nordWestNode.setName('nord-west');
            nordWestNode.setNodeId(obj.freeNodeId);
            obj.freeNodeId = obj.freeNodeId +1;
            %[insertPos,front] = obj.insert2ParentAtt(nodePos);
            obj = obj.insert2NodeAttribute(nordWestNode,insertPos,front);
            % Nord-Ost-Bereich
            top = [node.xValMax,node.yValMax];
            bot = [avgX,avgY];
            nordEastNode = quadtreeNodeClass();
            nordEastNode = nordEastNode.quadNodeInit(bot,top);
            nordEastNode = nordEastNode.setLevel(subNodeLvl);
            nordEastNode = nordEastNode.setName('nord-east');
            %[insertPos,front] = obj.insert2ParentAtt(nodePos);
            obj = obj.insert2NodeAttribute(nordEastNode,insertPos,front);
            %Süd-Ost-Bereich
            top = [node.xValMax,avgY];
            bot = [avgX,node.yValMin];
            southEastNode = quadtreeNodeClass();
            southEastNode = southEastNode.quadNodeInit(bot,top);
            southEastNode = southEastNode.setLevel(subNodeLvl);
            southEastNode = southEastNode.setName('south-east');
            %[insertPos,front] = obj.insert2ParentAtt(nodePos);
            obj = obj.insert2NodeAttribute(southEastNode,insertPos,front);
            %Süd-West
            top = [avgX,avgY];
            bot = [node.xValMin,node.yValMin];
            southWestNode = quadtreeNodeClass();
            southWestNode = southWestNode.quadNodeInit(bot,top);
            southWestNode = southWestNode.setLevel(subNodeLvl);
            southWestNode = southWestNode.setName('south-west');
            %[insertPos,front] = obj.insert2ParentAtt(nodePos);
            obj = obj.insert2NodeAttribute(southWestNode,insertPos,front);
            obj.node(nodePos).child = 4;%TODO child-Attribute weg löschen
            
        else
            error('Eingabe ungültig');
        end
    end
end

function obj=updateChildAttNode(obj,nodeIdx)
    if nargin~=2
        error('Eingabe ungültig');
    elseif isempty(obj)||isempty(nodeIdx)||~isnumeric(nodeIdx)
        error('Eingabe ungültig');
    else
        if nodeIdx==0
            error('Eingabe ungültig');
        else
            lvl = obj.node(nodeIdx).level;
            if (obj.depth) > lvl
                lvlMemIdx = obj.level(lvl+1,:);
                beginIdx = lvlMemIdx(1,1);
                endIdx = lvlMemIdx(1,2);
                firstChildIdx = 0;
                for i=beginIdx:endIdx
                    parentIdx = obj.parent(i);
                    if nodeIdx == parentIdx
                        firstChildIdx = i;
                        break;%In diesem quadTree liegen die Kinder nebeneinander
                    end
                end
                if firstChildIdx~=0
                    nodeChildNum = 4;
                    obj.child(:,nodeIdx) = [firstChildIdx;(firstChildIdx+nodeChildNum-1)];
                else
                    obj.child(:,nodeIdx) = [0;0];
                end
            else
                %Wenn die vorne Knoten kein Kind haben,
                % dann wird es automatisch dafür hinzugefügt
                obj.child(:,nodeIdx) = [0;0];
            end
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
            endLevel = numel(obj.level)/2;
            for j = level:endLevel
                levelIdx = obj.level(j,:);
                for i= levelIdx(1):levelIdx(2)
                    obj = obj.updateChildAttNode(i);
                end
            end
        end
    end
end

function obj=updateChildAttAll(obj)
    if nargin~=1
        error('Eingabe ungültig');
    elseif isempty(obj)
        error('Eingabe ungültig');
    elseif obj.depth==0
        error('keinen Knoten im quadTree');
    else
        for i=1:(numel(obj.level)/2)
            obj = obj.updateChildAttLvl(i);
        end
    end
end

function boolVal = checkPoint(obj,point)
    boolVal = false;
    if nargin == 2
        checkEmpty = ~isempty(point);
        checkElementNo = numel(point)==2;
        checkElementType = isnumeric(point);
        checkObj = ~isempty(obj);
        if checkEmpty && checkElementType && checkElementNo && checkObj
            boolVal = true;    
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
            root.setNodeId(obj.freeNodeId);
            obj.freeNodeId = obj.freeNodeId + 1;
            obj.parent = 0;
            obj.level = [1,1];
            obj.depth = 1;
            obj.node = root;
            obj.updateLvlAtt();
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
            checkPointSet = obj.checkPoint(tempPoint);
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
            clear checkPointSet;
            stackNodeIdx = stackClass();
            %Root in stack hinzufügen
            stackNodeIdx = stackNodeIdx.push(1);
            stackNodeIdxLen = stackNodeIdx.stackLen();

            stackPointIdx = stackClass();
            stackPointIdxLen = stackPointIdx.stackLen();
            debugg = 0;
            while (stackNodeIdxLen~=0)
                remainedPoints = numel(pointSet)/2;
                fistElinNodeStack = stackNodeIdx.peek();
                if (remainedPoints==304) && (fistElinNodeStack==37)
                    debugg = 1;
                end
                [stackNodeIdx,tempNodeIdx] = stackNodeIdx.pop();
                tempNode = obj.node(tempNodeIdx);
                if stackPointIdxLen~=0
                    tempBuffer = zeros(1,stackPointIdxLen);
                    tempBufferLen = 0;
                    for i=1:stackPointIdxLen
                        [stackPointIdx,tempPointIdx] = stackPointIdx.pop();
                        tempPoint = pointSet(tempPointIdx,:);
                        checkPoint = tempNode.isInNode(tempPoint);
                        if checkPoint == true
                            tempBuffer(tempBufferLen+1) = tempPointIdx;
                            tempBufferLen = tempBufferLen+1;
                        end
                    end

                    if tempBufferLen>0
                        tempBuffer = tempBuffer(1:tempBufferLen);
                        if tempBufferLen>obj.maxPointsPerNode
                            obj = obj.addChildrenForNode(tempNode);
                            obj.updateLvlAtt();
                            obj = obj.updateChildAttAll();
                            %Nehmen der Beginn- und Endposition von Kinder in Node-Attribut
                            childIdx = obj.child(:,tempNodeIdx);
                            %WARNING hohe Bugwahrscheinlichkeit hier
                            stackNodeIdx = stackNodeIdx.push(childIdx(1):childIdx(2));
                            stackPointIdx = stackPointIdx.push(tempBuffer);
                        else
                            for i=1:tempBufferLen
                                tempBufferIdx = tempBuffer(i);
                                tempPoint = pointSet(tempBufferIdx,:);
                                obj.node(tempNodeIdx).addAPoint(tempPoint);
                            end
                            tempBuffer= flip(tempBuffer);%Kippen wegen des Stackes
                            %Separat löschen wegen des Indexproblems
                            pointSet(tempBuffer,:) = [];           
                        end
                    end
                    %TODO stop here weiter Implementation

                else
                    endOfLoop = numel(pointSet)/2;
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
                            obj.updateLvlAtt();
                            obj = obj.updateChildAttAll();
                            %Nehmen der Beginn- und Endposition von Kinder in Node-Attribut
                            childIdx = obj.child(:,tempNodeIdx);
                            stackNodeIdx = stackNodeIdx.push(childIdx(1):childIdx(2));
                        else
                            tempBuffer = zeros(1,stackPointIdxLen);
                            for i=1:stackPointIdxLen
                                [stackPointIdx,tempPointIdx]=stackPointIdx.pop();
                                tempPoint = pointSet(tempPointIdx,:);
                                obj.node(tempNodeIdx).addAPoint(tempPoint);
                                tempBuffer(i) = tempPointIdx;
                            end
                            tempBuffer = flip(tempBuffer);%Kippen wegen des Stackes
                            %Separat löschen wegen des Indexproblems
                            pointSet(tempBuffer,:) = [];
                        end
                    end
                end
                %Immer am Ende
                stackNodeIdxLen = stackNodeIdx.stackLen();
                stackPointIdxLen = stackPointIdx.stackLen();
                debugg = debugg-1;
            end
        end
    end
end

end

end