M1=csvread('Total_csv_4/Total_NonDVFS.2.csv');
M2=csvread('Total_csv_4/Total_DVFS.NonOverhead.2.csv');
M3=csvread('Total_csv_4/Total_DVFS.Overhead.2.csv');

x_axis = M1(:, 1);

RFJ_Env1 = M1(:, 2);
RFJ_Env2 = M2(:, 2);
RFJ_Env3 = M3(:, 2);

AFJ_Env1 = M1(:, 3);
AFJ_Env2 = M2(:, 3);
AFJ_Env3 = M3(:, 3);

TargetResponse_Env1 = M1(:, 4);
TargetResponse_Env2 = M2(:, 4);
TargetResponse_Env3 = M3(:, 4);

AverageResponse_Env1 = M1(:, 5);
AverageResponse_Env2 = M2(:, 5);
AverageResponse_Env3 = M3(:, 5);

Energy_Env1 = M1(:, 6);
Energy_Env2 = M2(:, 6);
Energy_Env3 = M3(:, 6);

subplot(2,2,1);
plot(x_axis, RFJ_Env1, x_axis, RFJ_Env2, x_axis, RFJ_Env3);
xlabel("Target Response Time(%)");
ylabel("Relative Finish Time Jitter(us)");
legend({'NonDVFS','DVFS','DVFSOverhead'});

subplot(2,2,2);
plot(x_axis, AFJ_Env1, x_axis, AFJ_Env2, x_axis, AFJ_Env3);
xlabel("Target Response Time(%)");
ylabel("Absolute Finish Time Jitter(us)");
legend({'NonDVFS','DVFS','DVFSOverhead'});

subplot(2,2,3);
plot(TargetResponse_Env1, AverageResponse_Env1, TargetResponse_Env2, AverageResponse_Env2, TargetResponse_Env3, AverageResponse_Env3);
xlabel("Target Response Time(us)");
ylabel("Average Response Time(us)");
legend({'NonDVFS','DVFS','DVFSOverhead'});

subplot(2,2,4);
plot(x_axis, Energy_Env1, x_axis, Energy_Env2, x_axis, Energy_Env3);
xlabel("Target Response Time(%)");
ylabel("Energy Consumption(uJ)");
legend({'NonDVFS','DVFS','DVFSOverhead'});


