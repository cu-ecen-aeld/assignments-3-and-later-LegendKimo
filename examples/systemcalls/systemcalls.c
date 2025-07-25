#include "systemcalls.h"
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include<errno.h>
#include<string.h>
 #include <fcntl.h>
/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{

/*
 * TODO  add your code here
 *  Call the system() function with the command set in the cmd
 *   and return a boolean true if the system() call completed with success
 *   or false() if it returned a failure
*/
 int ret;
ret=system(cmd);
 if(ret ==-1){
    return false ;
 }

    return true;
}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

bool do_exec(int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);

    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];

/*
 * TODO:
 *   Execute a system command by calling fork, execv(),
 *   and wait instead of system (see LSP page 161).
 *   Use the command[0] as the full path to the command to execute
 *   (first argument to execv), and use the remaining arguments
 *   as second argument to the execv() command.
 *
*/
int pid;
int  wstatus;
int pids[5]={};
int child_err [5]={};

 pid= fork();
if(pid==0){
    //printf("Debug_kareeeeeeeeeeeeeeeeeeeeeeem child[%d]  \n" , getpid());
           //printf("Debug_kareeeeeeeeeeeeeeeeeeeeeeem command[] = %s \n",*command);
    int ex_ret =execv(command[0],command);
    if(ex_ret==-1)
   { 

   // printf("Debug_kareeeeeeeeeeeeeeeeeeeeeeem  there is an issue in child[%d] which is %d \n" , getpid(),errno);
    child_err[0] =  true;
    exit(EXIT_FAILURE);
    }
}
else{
pids[0]=pid;
    //printf("Debug_kareeeeeeeeeeeeeeeeeeeeeeem  there is an issue in child[%d] which is %d \n" , getpid(),child_err[0] );
}


            if (waitpid(pids[0], &wstatus, 0) == -1) {
            perror("waitpid failed");
            child_err[0] = true;
            //continue;
        }
        if (WIFEXITED(wstatus)) {
    int code = WEXITSTATUS(wstatus);
    if (code != 0) {
        child_err[0] = true;
        printf("the code is %d ",code);
        }
}

   // printf("pids[%d] child has status %d \n",pids[0],child_err[0]);
if(child_err[0]==true) {
   //printf("Debug_kareeeeeeeeeeeeeeeeeeeeeeem fail \n ");
return false ;
}
//}
    va_end(args);
//printf("Debug_kareeeeeeeeeeeeeeeeeeeeeeem  Pass  \n");
    return true;
}

/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char *outputfile, int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];


/*
 * TODO
 *   Call execv, but first using https://stackoverflow.com/a/13784315/1446624 as a refernce,
 *   redirect standard out to a file specified by outputfile.
 *   The rest of the behaviour is same as do_exec()
 *
*/

int pid;
int  wstatus;
int pids[5]={};
int child_err [5]={};
int fd = open(outputfile, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open");
        return 1;
    }
 pid= fork();
if(pid==0){
    printf("Debug_kareeeeeeeeeeeeeeeeeeeeeeem child[%d]  \n" , getpid());
           printf("Debug_kareeeeeeeeeeeeeeeeeeeeeeem command[] = %s \n",*command);
    dup2(fd, STDOUT_FILENO);  // redirect stdout to the file
        close(fd);
    int ex_ret =execv(command[0],command);
    if(ex_ret==-1)
   { 

    printf("Debug_kareeeeeeeeeeeeeeeeeeeeeeem  there is an issue in child[%d] which is %d \n" , getpid(),errno);
    child_err[0] =  true;
    //exit(EXIT_FAILURE);
    }
}
else{
close(fd);
pids[0]=pid;

}


            if (waitpid(pids[0], &wstatus, 0) == -1) {
            perror("waitpid failed");
            child_err[0] = true;
            //continue;
        }
        if (WIFEXITED(wstatus)) {
    int code = WEXITSTATUS(wstatus);
    if (code != 0) {
        child_err[0] = true;
        printf("the code is %d ",code);
        }
}

    printf("pids[%d] child has status %d \n",pids[0],child_err[0]);
if(child_err[0]==true) {
   printf("Debug_kareeeeeeeeeeeeeeeeeeeeeeem fail \n ");
return false ;
}
//}
    va_end(args);
printf("Debug_kareeeeeeeeeeeeeeeeeeeeeeem  Pass  \n");
    return true;

}
