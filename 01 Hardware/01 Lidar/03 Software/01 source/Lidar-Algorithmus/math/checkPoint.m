function boolVal = checkPoint(point)
%CHECKPOINT Summary of this function goes here
%   Detailed explanation goes here
    boolVal = false;
    if nargin == 1
        checkEmpty = ~isempty(point);
        checkElementNo = numel(point)==2;
        checkElementType = isnumeric(point);
        if checkEmpty && checkElementType && checkElementNo
            boolVal = true;    
        end
    end
end

