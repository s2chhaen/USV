rMax = max(rSample);
rMin = min(rSample);

%Filter param
order = filterOrder(rMin,rMax);
fs = filterSampleFreq(sample);

%FIR - Filter
ffir = filterWindowKaiser_p10;%Eingetragen ist die Filter Parameter
result0 = rSample;

l = length(ffir.Numerator);
%Dummy-Sample erzeugen um die Phasenverschiebung zu vermeiden
rSample = [rSample,ones(1,fix(l/2))*rSample(end)];
%FIR Filter
result1 = filter(ffir.Numerator,1,rSample);
%Wegwerfen vom Dummy-Sample
result1 = result1(ceil(l/2):end-1);

figure(1)
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

