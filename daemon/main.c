/*
 * main.c
 *
 *  Created on: Oct 20, 2015
 *      Author: zhaolixiang
 */
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <syslog.h>
#include <sys/wait.h>

int main(int argc,char *argv[])
{
    if(argc <= 1)
    {
        printf("usage: %s <server/program/target,etc>\n",argv[0]);
        return 0;
    }

    //reset file create mode
    umask(0);

    //fork, and exit parent
    pid_t pid;
    if((pid = fork()) < 0)
    {
    	//fork lost
        exit(-1);
    }
    else if(pid > 0)
    {
    	//this is parent, exit
        exit(0);
    }

    //create new session leader
    setsid();
    signal(SIGHUP, SIG_IGN);
    //chdir("/"); //not use it right now

    //close file descriptors
    struct rlimit rl;
    getrlimit(RLIMIT_NOFILE, &rl);
    if(rl.rlim_max == RLIM_INFINITY)
    {
        rl.rlim_max = 1024;
    }
    unsigned int i;
    for(i = 0; i < rl.rlim_max; ++i)
    {
        close(i);
    }

    //redirect fd0/fd1/fd2 to /dev/null
    open("/dev/null", O_RDONLY);
    open("/dev/null", O_RDWR);
    open("/dev/null", O_RDWR);

    // use syslogd
    openlog(argv[0], LOG_PID|LOG_PERROR, LOG_USER);
    int status = 0;
    while( 1 )
    {
        if((pid = fork()) > 0)
        {
            syslog(LOG_INFO, "%s start %s ok, %s pid: %u\n",argv[0],argv[1],argv[1],pid);
            wait(&status);  // parent wait here
            if(WIFEXITED(status))
            {
                syslog(LOG_INFO,"%s exit normally,status:%d,%s will exit too",argv[1],status,argv[0]);
                continue;
            }

            if(WIFSIGNALED(status))
            {
                syslog(LOG_ERR, "%s exit with signal, status: %d, %s will restart it",argv[1], status,argv[0]);
            }
            else
            {
                syslog(LOG_ERR, "%s exit with unkown reason, %s will restart it",argv[1],argv[0]);
                continue;
            }
        }
        else if(pid < 0)
        {
            syslog(LOG_ERR, "%s fork error: %d, %s, %s will exit",argv[0],errno, strerror(errno),argv[0]);
            exit(-1);       // fork failed, daemon exit
        }
        else if(pid == 0)
        {
            if(execl(argv[1], argv[1], (char*)0) < 0)
            {
                syslog(LOG_ERR, "%s exec error: %d, %s, %s daemon will exit",argv[0], errno, strerror(errno),argv[1]);
                exit(-1);   // let parent exit
            }
        }
        sleep(2);
    }
    return 0;
}
