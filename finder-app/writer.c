#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    openlog("writer program", LOG_PID | LOG_CONS, LOG_USER);
    if (argc != 3) {

       syslog(LOG_ERR,"The arguments arenot entered as expected");
      // printf("The arguments arenot entered as expected");
        return 1;
    }

    const char *file_path = argv[1];
    const char *text = argv[2];
    int fd = open(file_path, O_WRONLY | O_CREAT , 0644);
    if (fd ==-1){
    syslog(LOG_ERR,"Error opening file and the error code is %d",errno);   
           //printf("Error opening file and the error code is %d /n",errno);

    }
    else{
    syslog(LOG_DEBUG,"File %s opened successfully ",file_path);  
           //printf("File %s opened successfully /n",file_path);
    }
    int nr = write(fd, text,strlen(text));
    if(nr==-1){
    syslog(LOG_ERR,"Error writing file and the error code is %d",errno);  
   // printf("Error writing file and the error code is %s /n",strerror(errno));
    }
    else{
    syslog(LOG_DEBUG,"The Data %s written successfully ",text);  
   //printf("The Data %s written successfully /n",text);
    }

if(close(fd)!=0){
     syslog(LOG_ERR,"Error closing file and the error code is %d",errno);      
}
else{
      syslog(LOG_DEBUG,"The File %s closed successfully ",file_path);    
}
 closelog();
    return 0;
}