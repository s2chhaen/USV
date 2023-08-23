function compareVal = quadnodeCompare(node1,node2)
%NODECOMPARE Summary of this function goes here
%   Detailed explanation goes here
    if nargin~=2
        error('nicht genug Parameter für Funktion');
    elseif isempty(node1)||isempty(node2)
        error('Eingabe nicht gültig');
    elseif (node1.id~=2)||(node2.id~=2)
        error('Eingabe nicht gültig');
    else
        compareXMax = numComp(node1.xValMax,node2.xValMax);
        compareXMin = numComp(node1.xValMin,node2.xValMin);
        compareYMax = numComp(node1.yValMax,node2.yValMax);
        compareYMin = numComp(node1.yValMin,node2.yValMin);
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
        %getrennt
        case5X = (case1X~=false)&&(case2X~=false)&&( ...
                  case3X~=false)&&(case4X~=false);
        
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
        %getrennt/apart
        case5Y = (case1Y~=false)&&(case2Y~=false)&&( ...
                  case3Y~=false)&&(case4Y~=false);
        if case1X==true || case2Y==true
            compareVal='intersect';
        elseif case4X==true && case4Y==true
            compareVal='coincide';
        elseif case2X==true && case2Y==true
            compareVal='include';
        elseif case3X==true && case3Y==true
            compareVal='included';
        else
            compareVal='apart';
        end
    end
end

