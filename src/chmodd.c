#include "const.h"


int ch(bool x, char newname[]) {
    if(x==true){
        printf("File renamed successfully\n");
      printf("Removing Permissions....\n");
      printf("SUCCESS\n");
      char *cmd = "chmod";
      char *argz[3];
      argz[0] = "chmod";
      argz[1] = "000";
      argz[2] = newname;
      argz[3] = NULL;
      execvp(cmd, argz);

    }else {
        printf("Error: unable to rename the file");
    }
     return 0;
}
