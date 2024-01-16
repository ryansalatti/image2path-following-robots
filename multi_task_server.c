#include <stdio.h>
#include <stdlib.h>
#include <libmc.h>

int main() {
    MCAgency_t agency;
    int local_port=5050;
    setbuf(stdout, NULL);

    agency = MC_Initialize(local_port, NULL);
    
    /* wait for message arrival signal */
    //MC_WaitSignal(agency, MC_RECV_MESSAGE);
    
    //system("./testrun.ch");
    
    MC_MainLoop(agency);
 
   
    MC_End(agency);
    
}
