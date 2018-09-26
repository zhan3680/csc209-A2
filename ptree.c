#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
// Add your other system includes here.

#include "ptree.h"

//define a new struct we are going to use later;
struct child{
   pid_t pid;
   struct child *next;
};
// Defining the constants described in ptree.h
const unsigned int MAX_PATH_LENGTH = 1024;

// If TEST is defined (see the Makefile), will look in the tests 
// directory for PIDs, instead of /proc.
#ifdef TEST
    const char *PROC_ROOT = "tests";
#else
    const char *PROC_ROOT = "/proc";
#endif


 


/*
 * Creates a PTree rooted at the process pid.
 * The function returns 0 if the tree was created successfully 
 * and 1 if the tree could not be created or if at least
 * one PID was encountered that could not be found or was not an 
 * executing process.
 */
int generate_ptree(struct TreeNode **root, pid_t pid) {
     /*Here's a way to generate a string representing the name of
     a file to open. Note that it uses the PROC_ROOT variable.*/
    
    

   
    //check if the given pid exists;
    char pid_path[MAX_PATH_LENGTH + 1];
    if (sprintf(pid_path, "%s/%d", PROC_ROOT, pid) < 0) {
        fprintf(stderr, "sprintf failed to produce a path for pid\n");
        return 1;
    }    
    struct stat Pid;
    if(lstat(pid_path,&Pid)<0){
       //fprintf(stderr,"given pid does not exist\n");
       //perror("lstat");
       return 1;   
    }
    
    //if exists, chexk if the given pid refers to a running process;
    char exe_path[MAX_PATH_LENGTH + 1];
    if (sprintf(exe_path, "%s/%d/exe", PROC_ROOT, pid) < 0) {
        fprintf(stderr, "sprintf failed to produce a path for exe\n");
        return 1;
    }    
    struct stat Exe;
    if(lstat(exe_path,&Exe)<0){
       //fprintf(stderr,"this is not a running process\n");this will finally be removed
       return 1;  
    }
    
    //if exists and running, then create a new Node;
    struct TreeNode *return_root=(struct TreeNode *)malloc(sizeof(struct TreeNode));
    if(return_root==NULL){
       fprintf(stderr,"not enough space for a new TreeNode\n");
       return 1;
    }
    return_root->pid=pid;
    return_root->name=(char*)malloc(sizeof(char)*(MAX_PATH_LENGTH + 1));
    if(return_root->name==NULL){
       fprintf(stderr,"not enough space for a new string (name)\n");
       return 1;
    }
    return_root->child=NULL;
    return_root->sibling=NULL;    
    
    //initialize the name;
    char cmdline_path[MAX_PATH_LENGTH + 1];
    if (sprintf(cmdline_path, "%s/%d/cmdline", PROC_ROOT, pid) < 0) {
        fprintf(stderr, "sprintf failed to produce a path for cmdline\n");
        return 1;
    }   
    struct stat Cmd;
    if(lstat(cmdline_path,&Cmd)<0){
       //fprintf(stderr,"this pid does not have an executable name\n");//this will finally be removed
       free(return_root->name);
       return_root->name=NULL;   
    }else{
       FILE *Cmdline=fopen(cmdline_path,"r");
       if(Cmdline==NULL){
          fprintf(stderr,"can not open the cmdline file for name\n");
          return 1; 
       }
       char executable_name[MAX_PATH_LENGTH + 1];
       if(fgets(executable_name,MAX_PATH_LENGTH + 1,Cmdline)!=NULL){
          strncpy(return_root->name,executable_name,strlen(executable_name));
          return_root->name[strlen(executable_name)]='\0';
       }else{
          free(return_root->name);
          return_root->name=NULL;
       }
       if(fclose(Cmdline)!=0){
          fprintf(stderr,"Cmdline can not be closed\n");
       return 1;
       }
    }
    //start build children(sibling), using linked list;
    char children_path[MAX_PATH_LENGTH + 1];
    if (sprintf(children_path, "%s/%d/task/%d/children", PROC_ROOT, pid, pid) < 0) {
        fprintf(stderr, "sprintf failed to produce a path for children\n");
        return 1;
    } 
    FILE *Children=fopen(children_path,"r");
    if(Children==NULL){
       fprintf(stderr,"cannot open children file for children");
       return 1;
    }
    char line[MAX_PATH_LENGTH + 1];
    struct child *head,*cur,*new1;
    pid_t i;
    char *leftover;
    int initialized=0;
    if(fgets(line,MAX_PATH_LENGTH + 1,Children)!=NULL){
          //printf("Line: %s\n",line);  
          i=(pid_t)strtol(line,&leftover,10);
          head=(struct child *)malloc(sizeof(struct child));
	  if(head==NULL){
             fprintf(stderr,"not enough space for a new child node1\n");
             return 1;
          }
	  head->pid=i;
	  head->next=NULL;
	  cur=head;
	  while(strcmp(leftover,"\n")!=0){
	        i=(pid_t)strtol(leftover,&leftover,10);
		if((int)i==0){
		   break;
		}
		new1=(struct child*)malloc(sizeof(struct child));
		//printf("%d\n",strcmp(leftover,"\n"));
		//printf("%s\n",leftover);
		if(new1==NULL){
                   fprintf(stderr,"not enough space for a new child node2\n");
                   return 1;
                }
		new1->pid=i;
		//printf("new's pid is set to :%d\n",new1->pid);
		new1->next=cur->next;
		cur->next=new1;
		cur=cur->next;
	  }
	  initialized=1;
    }
    while(fgets(line,MAX_PATH_LENGTH + 1,Children)!=NULL){
	  while(strcmp(leftover,"\n")!=0){
	        i=(pid_t)strtol(leftover,&leftover,10);
		if((int)i==0){
		   break;
		}
		new1=(struct child*)malloc(sizeof(struct child));
		if(new1==NULL){
                   fprintf(stderr,"not enough space for a new child node3\n");
                   return 1;
                }
		new1->pid=i;
		//printf("new's pid is set to :%d\n",new1->pid);
		new1->next=cur->next;
		cur->next=new1;
		cur=cur->next;
	  }
    }
    if(initialized){
       struct child *cur2=head;
       struct TreeNode **Sibling_pointer=NULL;
       int child_built=0;
       while(cur2!=NULL){
             //printf("root: %d; pid: %d;\n",(int)pid,(int)(cur2->pid));
             if(!child_built){
	        if(generate_ptree(&(return_root->child),cur2->pid)==0){
	           child_built=1;
	           Sibling_pointer=&((return_root->child)->sibling);	       
	        }
             }else{
		   if(generate_ptree(Sibling_pointer,cur2->pid)==0){
		      Sibling_pointer=&((*Sibling_pointer)->sibling); 
		   }
	     }	         
	     cur2=cur2->next;
        }
    }
       //close opened files

       if(fclose(Children)!=0){
          fprintf(stderr,"Children can not be closed\n");
          return 1;
       }
    
       *root=return_root;
       return 0;
    
}


/*
 * Prints the TreeNodes encountered on a preorder traversal of an PTree
 * to a specified maximum depth. If the maximum depth is 0, then the 
 * entire tree is printed.
 */
void print_ptree(struct TreeNode *root, int max_depth) {
    // Here's a way to keep track of the depth (in the tree) you're at
    // and print 2 * that many spaces at the beginning of the line.  
    static int depth = 0;

    //printf("%d\n",Max_depth(root)-max_depth);
    if(root!=NULL && max_depth>=-1){
    //printf("%*s", depth * 2,"");
    //printf("max depth of %s-subtree is: %d\n",root->name,Max_depth(root));
       /*if(max_depth==-1){
          if(root->name==NULL){
             printf("%*s", depth * 2,"");
             printf("%ld\n",(long)(root->pid));
          }else{
             printf("%*s", depth * 2,"");
             printf("%ld: %s\n",(long)(root->pid),root->name);
          } 
       }*/if(max_depth==0){        	
	  //print whole tree,don't forget to change max_depth to non_zero except at the root.
	     if(root->name==NULL){
                printf("%*s", depth * 2,"");
		printf("%ld\n",(long)(root->pid));
             }else{
                printf("%*s", depth * 2,"");
		printf("%ld: %s\n",(long)(root->pid),root->name);
             }
	     depth+=1;
             print_ptree(root->child,max_depth);
	     depth-=1;
             print_ptree(root->sibling,max_depth);	     
       }else{
          if(root->name==NULL){
            printf("%*s", depth * 2,"");
	    printf("%ld\n",(long)(root->pid));
          }else{
            printf("%*s", depth * 2,"");
	    printf("%ld: %s\n",(long)(root->pid),root->name);
          }
	  if(max_depth==1){
	     depth+=1;
             print_ptree(root->child,max_depth-2);
	     depth-=1;
             print_ptree(root->sibling,max_depth);
	  }else{
	    depth+=1;
	    print_ptree(root->child,max_depth-1);
	    depth-=1;
            print_ptree(root->sibling,max_depth);
	  }
       }
    } 
}


