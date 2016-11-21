% File Name     : CCKP_AllSubmission.m
% Author        : Harsh Gazula
% Created       : 07/30/2016 
% Description   : Contains code to submit the job submission 
%                 files (*.sh) in UNIX

close all; clear; clc;

MinVarSize  = 1000;
MaxVarSize  = 5000;
VarStepSize = 100;
VarRange    = MinVarSize : VarStepSize : MaxVarSize;

for CurrVarSize = VarRange
	CurrDir = ['Problem_Set_' num2str(CurrVarSize)];
	cd(CurrDir);
	system(['rm -f Problem_Set_' num2str(CurrVarSize) '.*']);
	system(['. ./All_' num2str(CurrDir) '.sh']);
%	system(['. ./Set_' num2str(VarSize) '.sh']);
%	system(['. qsub ' num2str(VarSize) '.sh']);
	cd('..');
end