#include<stdio.h>
#include<stdlib.h>
#include<getopt.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<dirent.h>

#define DEBUG 1
#define HI if(DEBUG)printf("HI\n");
#define XD if(DEBUG)

void print_usage();
void show_proc();
void show_one_stat( int32_t pid );

int main( int argc , char *argv[] )
{
    // show all files in /proc
    show_proc();
    // show special pid status in /proc
    show_one_stat(1);
}

void print_usage()
{
    printf("2 Usage: hw0401 [options]\n");
    printf(" -t, --time-interval=time Update the information every [time] seconds. Default: 5 seconds.\n");
    printf(" -c, --count Update the information [count] times. Default: infinite.\n");
    printf(" -p, --pid=pid Only display the given process information.\n");
    printf(" -h, --help Display this information and exit.\n");
}

// show all files in /proc
void show_proc()
{
    DIR *dir;
    struct dirent *ptr;
    dir = opendir("/proc");
    while((ptr = readdir(dir)) != NULL)
    {
        printf("%s\n", ptr->d_name);
    }
    closedir(dir);
}

// show special pid status in /proc
void show_one_stat( int32_t pid )
{
    char path[100];
    sprintf(path, "/proc/%d/status", pid);
    FILE *fp = fopen(path, "r");
    if(fp == NULL)
    {
        printf("Error: cannot open file\n");
        return;
    }
    char buf[100];
    while(fgets(buf, 100, fp) != NULL)
    {
        printf("%s", buf);
    }
    fclose(fp);
}




