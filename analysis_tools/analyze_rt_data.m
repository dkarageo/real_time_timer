clear
clc

dataFile = 'timestamps_1527357665';
timestampsCount = 100;  % Number of timestamps.
duration = 10;          % Duration in secs.

file = fopen(dataFile);
dataRaw = fread(file, timestampsCount * 2, 'int64', 'ieee-be');

%for i = 1:timestampsCount*2
 %   fprintf("%d\n", i);
  %  dataRaw(i) = fread(file, 'int64');
%end
data = zeros(timestampsCount, 1);

for i = 1:timestampsCount
    rawIndex = i * 2;
    fprintf("i: %d - %ld secs %ld usecs\n", rawIndex, dataRaw(rawIndex - 1), dataRaw(rawIndex));
    % Store separate secs and usecs representation into a single value.
    data(i) = dataRaw(rawIndex - 1) + dataRaw(rawIndex) / 1000000;
end

%% Normalize timestamps.
data = data - data(1);

%% Print timestamps.
%for i = 1:timestampsCount
%    fprintf("%.3f\n", data(i));
%end

%% Calculate distances
distances = zeros(timestampsCount - 1, 1);
for i = 1:timestampsCount-1
    distances(i) = data(i+1) - data(i);
end

%% Extract statistics
minDist = min(distances);
maxDist = max(distances);
avg = mean(distances);
med = median(distances);
stddev = std(distances);

fprintf('Minimum: %f\n', minDist);
fprintf('Maximum: %f\n', maxDist);
fprintf('Average: %f\n', avg);
fprintf('Median: %f\n', med);
fprintf('Standard Deviation: %f\n', stddev);
    
%% Plot deviation
period = duration / 100;
deviations = distances - period;
ts = timeseries(distances, data(2:end));
ts.Name = 'Period';
plot(ts);