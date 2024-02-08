function outVal = slopeArray(inY,inX)
outVal = zeros(1,numel(inY)-1);

if (numel(inY) == numel(inX)) && (numel(inY) > 1)
    for i=2:numel(inY)
        outVal(i-1) = (inY(i) - inY(i-1)) / (inX(i) - inX(i-1));
    end
end
end

