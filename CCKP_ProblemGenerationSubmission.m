close all; clear; clc

for i = 1000 : 100 : 5000
    folderName = ['Problem_Set_' num2str(i)];
    cd(folderName);
    unix('qsub prob_gen.sh');
    cd('..');
end