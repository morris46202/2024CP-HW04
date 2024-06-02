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

typedef struct return_data{
    int32_t pid;
    char state;
    unsigned long utime;
    unsigned long stime;
    int64_t cutime;
    int64_t cstime;
    u_int64_t starttime;
    u_int32_t vsize;

    

    double uptime;
}rd;


void print_usage();
void show_proc();
rd *get_cpu_data( int32_t pid );
int find_pid( int32_t pid );

int main( int argc , char *argv[] )
{
    // show all files in /proc
    //show_proc();
    // show special pid status in /proc

    // get options
    int opt;
    int time_interval = 5;
    int count = -1;
    int pid = 0;

    static struct option long_options[] = {
        {"time-interval", required_argument, 0, 't'},
        {"count", required_argument, 0, 'c'},
        {"pid", required_argument, 0, 'p'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };
    int option_index = 0;
    int t_flag = 0, c_flag = 0, p_flag = 0;
    while((opt = getopt_long(argc, argv, "t:c:p:h", long_options, &option_index)) != -1)
    {
        switch(opt)
        {
            case 't':
                time_interval = atoi(optarg);
                if(time_interval <= 0)
                {
                    printf("Error: time interval should be positive\n");
                    return 0;
                }
                t_flag = 1;
                break;
            case 'c':
                count = atoi(optarg);
                if(count <= 0)
                {
                    printf("Error: count should be positive\n");
                    return 0;
                }
                c_flag = 1;
                break;
            case 'p':
                pid = atoi(optarg);
                if(find_pid(pid) == 0)
                {
                    printf("Error: pid not found\n");
                    return 0;
                }
                p_flag = 1;
                break;
            case 'h':
                print_usage();
                return 0;
            default:
                print_usage();
                return 0;
        }
    }
    
    rd *cpu_data = malloc(sizeof(rd));
    if(p_flag == 1)
    {
        cpu_data = get_cpu_data(pid);
    }

    int32_t HZ = sysconf(_SC_CLK_TCK);


    // print data
    for(int i = 0; i < count || count == -1; i++)
    {
        printf("PID   NAME      state   CPU     MEM\n");
        if(p_flag == 1)
        {
            printf("pid: %d\n", cpu_data->pid);
            printf("state: %c\n", cpu_data->state);
            printf("utime: %lu\n", cpu_data->utime);
            printf("stime: %lu\n", cpu_data->stime);
            printf("cutime: %ld\n", cpu_data->cutime);
            printf("cstime: %ld\n", cpu_data->cstime);
            printf("vsize: %u\n", cpu_data->vsize);
        }
        else
        {
            // show all
            DIR *dir;
            struct dirent *ptr;
            dir = opendir("/proc");
            int index = 1;
            while((ptr = readdir(dir)) != NULL)
            {
                if(ptr->d_name[0] >= '0' && ptr->d_name[0] <= '9')
                {
                    XD printf("pid: %d\n", atoi(ptr->d_name));
                    cpu_data = get_cpu_data(atoi(ptr->d_name));
                    HI
                    double all_time = cpu_data->utime + cpu_data->stime + cpu_data->cutime + cpu_data->cstime;
                    double sec = (cpu_data->uptime - (cpu_data->starttime / HZ)) ;
                    double cpu_usage = ((all_time / HZ) / sec) *100;
                    printf("%d     %s      %c      %f     %f\n", cpu_data->pid, ptr->d_name, cpu_data->state, cpu_usage, cpu_data->vsize/1024.0);
                }
            }
            closedir(dir);
        }
        sleep(time_interval);
        // clear screen
        printf("\033[2J");
    }


}

void print_usage()
{
    printf("Usage: hw0401 [options]\n");
    printf(" -t, --time-interval=time Update the information every [time] seconds. Default: 5 seconds.\n");
    printf(" -c, --count=count Update the information [count] times. Default: infinite.\n");
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


// get some data in /proc
rd *get_cpu_data( int32_t mypid )
{
    char path[100];
    sprintf(path, "/proc/%d/stat",mypid);
    FILE *fp = fopen(path, "r");
    if(fp == NULL)
    {
        printf("Error: cannot open file\n");
        return NULL;
    }
    
    // read file
    int32_t pid, ppid, pgrp, session, tty_nr, tpgid, flags;
    char comm[100], state;
    unsigned long minflt, cminflt, majflt, cmajflt, utime, stime;
    int64_t cutime, cstime;
    int32_t priority, nice, num_threads;
    int32_t itrealvalue;
    unsigned long starttime;
    unsigned int vsize;
    
    fscanf(fp, "%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu %ld %ld %d %d %d %d %ld %d %d %lu %u",
            &pid, comm, &state, &ppid, &pgrp, &session, &tty_nr, &tpgid, &flags,
            &minflt, &cminflt, &majflt, &cmajflt, &utime, &stime, &cutime, &cstime,
            &priority, &nice, &num_threads, &itrealvalue, &starttime, &vsize);
    
    XD printf("pid: %d\n", pid);

    XD printf("state: %c\n", state);

    XD printf("utime: %lu\n", utime);
    XD printf("stime: %lu\n", stime);
    XD printf("cutime: %ld\n", cutime);
    XD printf("cstime: %ld\n", cstime);
    XD printf("starttime: %lu\n", starttime);
    XD printf("vsize: %u\n", vsize);

    fclose(fp);

    rd *ret = malloc(sizeof(rd));
    ret->pid = pid;
    ret->state = state;
    ret->utime = utime;
    ret->stime = stime;
    ret->cutime = cutime;
    ret->cstime = cstime;
    ret->starttime = starttime;
    ret->vsize = vsize;

    //get uptime
    fp = fopen("/proc/uptime", "r");
    if(fp == NULL)
    {
        printf("Error: cannot open file\n");
        return NULL;
    }
    double uptime;
    fscanf(fp, "%lf", &uptime);
    ret->uptime = uptime;

    return ret;
}

// find if the pid is in /proc
int find_pid( int32_t pid )
{
    char path[100];
    sprintf(path, "/proc/%d", pid);
    DIR *dir = opendir(path);
    if(dir == NULL)
        return 0;
    else
        return 1;
}




