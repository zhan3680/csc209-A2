#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ptree.h"


int main(int argc, char **argv) {
    // TODO: Update error checking and add support for the optional -d flag
    if(argc !=2 && argc!=4){
       fprintf(stderr,"insufficient or too many arguments\n");
       fprintf(stderr,"Usage:\n\tptree [-d N] PID\n");
       return 1;
    }
    if(argc==4){
       char *option="-d";
       if(strcmp(argv[1],option)!=0){
          fprintf(stderr,"Wrong option given\n");
          fprintf(stderr,"Usage:\n\tptree [-d N] PID\n");
          return 1;
       }else{
          struct TreeNode *root = NULL;
          if(generate_ptree(&root, (pid_t)strtol(argv[3], NULL, 10))!=0){
	     //fprintf(stderr,"failed to generate tree\n");should finally remove this line
	     return 2;
	  }
	  int max_width=strtol(argv[2],NULL,10);
          print_ptree(root, max_width);
          return 0;
       }
    } 
    if(argc==2){
      // NOTE: This only works if no -d option is provided and does not
      // error check the provided argument or generate_ptree. Fix this!
      struct TreeNode *root = NULL;
      if(generate_ptree(&root, (pid_t)strtol(argv[1], NULL, 10))!=0){
         //fprintf(stderr,"failed to generate tree\n");should finally remove this line
	 return 2;
      }
      print_ptree(root, 0);
      return 0;
    }
    
}

