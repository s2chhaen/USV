rMax = max(rSample);
rMin = min(rSample);

%Filter param
order = filterOrder(rMin,rMax);
fs = filterSampleFreq(180,0.5);

draw = 0;
filtered = 1;

%FIR - Filter
if strcmp(choosedFIR,'wk')
    ffir = filterWindowKaiser_p10;%Eingetragen ist die Filter Parameter
elseif strcmp(choosedFIR,'lsq')
    ffir = filterLeastSquares_p11;%Eingetragen ist die Filter Parameter
elseif strcmp(choosedFIR,'eq')
    ffir = filterEquiripple_p3;%Eingetragen ist die Filter Parameter
elseif strcmp(choosedFIR,'wb')
    ffir = filterWindowBarlett_p2;
elseif strcmp(choosedFIR,'wr')
    ffir = filterWindowRectangular_p3;
elseif strcmp(choosedFIR,'wt')
    ffir = filterWindowTriangular_p1;
else
    filtered = 0;
    draw = 0;
end

if filtered == 1
    result0 = rSample;
    tempR = rSample;
    l = length(ffir.Numerator);
    %Dummy-Sample erzeugen um die Phasenverschiebung zu vermeiden
    tempR = [tempR,ones(1,fix(l/2))*tempR(end)];
    %FIR Filter
    result1 = filter(ffir.Numerator,1,tempR);
    %Wegwerfen vom Dummy-Sample
    result1 = result1(fix(l/2)+1:end);
end

if draw == 1
    figure(figureNo)
    figureNo = figureNo + 1;
    ax(1) = subplot(2,1,1);
    plot(tVal,result0,'-o',tVal,result1,'-o');
    legend('Original Data','Filtered Data');
    grid on
    xlabel('Winkel/Grad');
    ylabel('Radius/cm');
    ax(2) = subplot(2,1,2);
    plot(tVal,20.*log10(result1./result0),'-o');
    legend('magnitude/dB');
    grid on
    xlabel('Winkel/Grad');
    ylabel('Radius/cm');
    linkaxes(ax,'x')
end

clear draw filtered tempR