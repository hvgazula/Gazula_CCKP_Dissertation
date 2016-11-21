close all; clear; clc

% Part 1 - Copy all results into one folder
% ParentFolder    = '~/CCKP_Harsh/20160724_CCKP';
AllResultsFolder = 'All_Results_Set_02';
ParentFolder = pwd;
VarSizeRange = 1000 : 100 : 5000;
mkdir(ParentFolder, AllResultsFolder);

for VarSize = VarSizeRange
    CurrDirName = ['Problem_Set_' num2str(VarSize)];
    fprintf('Entering Folder: %s\n', CurrDirName);
    
    UnixSource      = fullfile(ParentFolder, CurrDirName, 'Results', '*.xls');
    UnixDestination = fullfile(ParentFolder, AllResultsFolder);
    UnixCommand     = ['cp ' UnixSource ' ' UnixDestination];
    
    unix(UnixCommand);
end