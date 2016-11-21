close all; clear; clc

ParentFolder = pwd;

for VarSize = 1000 : 100 : 5000
    CurrDirName = ['Problem_Set_' num2str(VarSize)];
    cd(fullfile(ParentFolder, CurrDirName));
    unix('rm *.o*');
    unix('mv Instances/*.lp .');
    cd('..');
end