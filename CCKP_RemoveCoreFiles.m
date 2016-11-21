close all; clear; clc;

fprintf('Size Before Clean-up : ');
system('du -sh');

for VarSize = 1000 : 100 : 5000
    CurrDir = ['Problem_Set_' num2str(VarSize)];
    fprintf('Entering Directory %s \n', CurrDir);
    cd(CurrDir);
    system('rm -f core.*');
	system('rm -rf cardinality2.exe');
    % 	system(['. ./All_' num2str(CurrDir) '.sh']);
    %	system(['. ./Set_' num2str(VarSize) '.sh']);
    %	system(['. qsub ' num2str(VarSize) '.sh']);
    cd('..');
end
fprintf('Size After Clean-up : ');
system('du -sh');