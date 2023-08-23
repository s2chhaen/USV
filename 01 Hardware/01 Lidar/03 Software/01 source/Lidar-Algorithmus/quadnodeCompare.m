function strVal = quadnodeCompare(node1,node2)
%NODECOMPARE Summary of this function goes here
%   Detailed explanation goes here
    if nargin~=2
        error('nicht genug Parameter für Funktion');
    elseif isempty(node1)||isempty(node2)
        error('Eingabe nicht gültig');
    elseif (node1.id~=2)||(node2.id~=2)
        error('Eingabe nicht gültig');
    elseif (node1.init~=1) || (node2.init~=1)
        error('Koordinaten von Node1 oder Node2 oder beide sind leer');
    else
        compareXMax = numCmp(node1.xValMax,node2.xValMax);
        compareXMin = numCmp(node1.xValMin,node2.xValMin);
        compareYMax = numCmp(node1.yValMax,node2.yValMax);
        compareYMin = numCmp(node1.yValMin,node2.yValMin);
        %Für X
        %Schneiden/intersect
        case1X = ((compareXMax==1)&&(compareXMin==1))||( ...
                  (compareXMax==-1)&&(compareXMin==-1));
        % 1 Umfasst 2/contain
        case2X = (compareXMax==1)&&(compareXMin==-1);
        % 2 Umfasst 1/contain
        case3X = (compareXMax==-1)&&(compareXMin==1);
        %Übereinstimmen/coincide
        case4X = (compareXMax==0)&&(compareXMin==0);
        
        %Für Y
        %Schneiden/intersect
        case1Y = ((compareYMax==1)&&(compareYMin==1))||( ...
                  (compareYMax==-1)&&(compareYMin==-1));
        % 1 Umfasst 2/contain
        case2Y = (compareYMax==1)&&(compareYMin==-1);
        % 2 Umfasst 1/contain
        case3Y = (compareYMax==-1)&&(compareYMin==1);
        %Übereinstimmend/coincide
        case4Y = (compareYMax==0)&&(compareYMin==0);

        if case1X==true || case1Y==true
            strVal='intersect';
        elseif case4X==true && case4Y==true
            strVal='coincide';
        elseif case2X==true && case2Y==true
            strVal='include';
        elseif case3X==true && case3Y==true
            strVal='included';
        else
            strVal='apart';
        end
    end
end

