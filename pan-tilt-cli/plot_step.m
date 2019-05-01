 function read_plot_file(file_name)

 clc;

 % is the filename a string?
 if 0 == ischar(file_name)

     error(' Input must be a char array ');

 end

 % display that the file is about to be opened
 disp('Opening file');

 % open the file for reading
 fileID = fopen(file_name,'r');

 % how is the format spec
 formatSpec = '%d ; %f';

 % the formation is preallocated
 sizeA = [2 Inf];

 % read from the file
 A = fscanf(fileID,formatSpec,sizeA);

 % display that you read from file
 disp('Getting Data');

 % close the file after it is read
 fclose(fileID);

 % closed the file
 disp('Closing file');

 % transpose
 A = A';

 % plot
 disp('Plotting data');
 plot(A(:,1),A(:,2));

 % Should save the figure

 end
