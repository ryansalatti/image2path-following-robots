/* File: multi_task_example/client.c */
#include <stdio.h>
#include <stdlib.h>
#include <libmc.h>
#include <string.h>

int main() 
{
  MCAgency_t agency;
  MCAgencyOptions_t options;
  MCAgent_t agent;
  int *agent_return_value;
  int task_num;
  int local_port=5051;

  MC_InitializeAgencyOptions(&options);
  MC_SetThreadOff(&options, MC_THREAD_CP); /* Turn off command prompt */
  agency = MC_Initialize(local_port, &options);

  /* Compose the agent from a task source file */
  agent = MC_ComposeAgentFromFile(
      "mobagent1",      /* Name */
      "raspberrypi3.local:5051", /* Home */
      "IEL",            /* Owner */
      "attachfile.c",  /* Filename */
      NULL,             /* Return var name. NULL for no return */
      "raspberrypi7.local:5050", /* Server to execute task on */
      0 );
    
    MC_AgentAddTaskFromFile(
      agent,
      "retrievefile.ch",
      "results_task1",
      "raspberrypi3.local:5051",
      0 );
  
  /* Add the agent */
  MC_AddAgent(agency, agent);

  /* Wait for return-agent arrival signal */
  MC_WaitSignal(agency, MC_RECV_RETURN);

  /* Make sure we caught the correct agent */
  agent = MC_FindAgentByName(agency, "mobagent1");
  if (agent == NULL) {
    fprintf(stderr, "Did not receive correct agent. \n");
    exit(1);
  }

  task_num = 1; /* Get return value from first task */
  agent_return_value = (int*)MC_AgentReturnDataGetSymbolAddr(agent, task_num);
  printf("%return: d\n",*agent_return_value);
  
  if(*agent_return_value == 1) {
      printf("Running runrobot.ch");
      system("./runrobot.ch");
  }
  task_num++; /* Get the return value from the second (and last) task. */

  /* We must reset the signal that we previously caught with the 
   * MC_WaitSignal() function with MC_ResetSignal() */
  MC_ResetSignal(agency);

  MC_End(agency);

  return 0;
}
