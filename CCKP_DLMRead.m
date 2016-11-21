function myExcelData = CCKP_DLMRead(FileList, ParentFolder, folderString)

myExcelData = [];

for i = 1 : length(FileList)
    C = strsplit(FileList(i).name, '_');
    C1 = str2double(C{1});
    C2 = str2double(C{2});
    C3 = str2double(C{3});
    
    bufferData = NaN(17, 12);
    bufferData(:,2:4) = repmat([C1 C2 C3], 17, 1) ;
    
    try
        currData = dlmread(fullfile(ParentFolder, folderString, FileList(i).name));
        %         bufferData(1:size(currData, 1), :) = currData;
        if size(currData, 1) == 17
            bufferData = currData;
        end
        myExcelData = [myExcelData; bufferData];
    catch
        myExcelData = [myExcelData; bufferData];
    end
end

end