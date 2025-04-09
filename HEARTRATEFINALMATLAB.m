% ========== SMART HELMET MATLAB VISUALIZATION ==========
clear;
clc;

% ESP32 Server IP
esp32_ip = '192.168.19.54';
esp32_port = 80;

% Create TCP client
try
    t = tcpclient(esp32_ip, esp32_port);
    disp("Connected to ESP32!");
catch
    error("Connection failed. Ensure ESP32 is running and Wi-Fi is connected.");
end

% Plot settings
figure('Name', 'Smart Helmet Monitoring', 'NumberTitle', 'off');

hrPlot = animatedline('Color', 'r', 'LineWidth', 2);
hold on;
alcoholPlot = animatedline('Color', 'b', 'LineWidth', 2);
legend('Heart Rate (BPM)', 'Alcohol Level');
xlabel('Time (s)');
ylabel('Values');
grid on;

% Axes limits
ylim([0 5000]);
xlim([0 60]);
drawnow;

% Initialize
startTime = datetime('now');
i = 0;

% Thresholds
BPM_LOW = 50;
BPM_HIGH = 120;
ALCOHOL_THRESH = 4000;

while true
    if t.NumBytesAvailable > 0
        rawData = readline(t);
        data = strsplit(strtrim(char(rawData)), ',');

        if length(data) == 2
            bpm = str2double(data{1});
            alcohol = str2double(data{2});
            i = i + 1;
            elapsed = seconds(datetime('now') - startTime);

            % Add points to plot
            addpoints(hrPlot, elapsed, bpm);
            addpoints(alcoholPlot, elapsed, alcohol);
            drawnow limitrate;

            % Display status
            statusMsg = "STATUS: ";
            if bpm < BPM_LOW || bpm > BPM_HIGH
                statusMsg = statusMsg + "Heart Rate NOT OK | ";
            else
                statusMsg = statusMsg + "Heart Rate OK | ";
            end

            if alcohol > ALCOHOL_THRESH
                statusMsg = statusMsg + "ALCOHOL DETECTED";
            else
                statusMsg = statusMsg + "NO Alcohol";
            end

            disp("Heart Rate: " + bpm + " BPM | Alcohol: " + alcohol + " | " + statusMsg);
        end
    end

    pause(0.01);
end
