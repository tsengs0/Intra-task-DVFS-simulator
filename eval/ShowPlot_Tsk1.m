M1=csvread('Total_csv_1/Total_NonDVFS.1.csv');
M2=csvread('Total_csv_1/Total_DVFS.NonOverhead.1.csv');
M3=csvread('Total_csv_1/Total_DVFS.Overhead.1.csv');
x_axis = M1(:, 1);
y_axis1 = M1(:, 3);
y_axis2 = M2(:, 3);
y_axis3 = M3(:, 3);
plot(x_axis, y_axis1, x_axis, y_axis2, x_axis, y_axis3);
xlabel("Target Response Time");
ylabel("Absolute Finish Time Jitter");
legend({'NonDVFS','DVFS','DVFSOverhead'});


