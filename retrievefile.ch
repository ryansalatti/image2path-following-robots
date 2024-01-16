#!/bin/ch

char** files;
int num_files;
int results_task1 = 1;
int i;
int status;
printf("Retrieving file \n");

mc_AgentListFiles(mc_current_agent, 0, &files, &num_files);
printf("%d saved files:\n", num_files);
for(i = 0; i < num_files; i++) {
    printf("%s\n", files[i]);
}
status = mc_AgentRetrieveFile(mc_current_agent, 0, "data", "points.txt");
if(status){
    printf("Error retrieving file.\n");
}

for (i = 0; i < num_files; i++) {
    free(files[i]);  // free memory allocated for each file name
}
free(files);  // free array of file names
