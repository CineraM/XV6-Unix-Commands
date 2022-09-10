/*--------------------------------------
----------------------------------------
----------------------------------------

Matias Cinera - U 6931_8506
COP 6611
Instructor: Dr. Ankur Mali

The following is my implementation of 
uniq & cat for xv6.

Template parameters of the program:
    - uniq arg1 arg2(optional) filename
    - uniq cat filename | uniq

-----------------------------------------
-----------------------------------------
----------------------------------------*/
#include "fcntl.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#define STRING_SIZE 512         // array size for all strings

char buf[5120];                 // store the contents of the file
char cur_str[STRING_SIZE];      // used as a temp string, later to be inserted into list
char list[500][STRING_SIZE];    // array that holds all the strings

// based on the cat implementation in xv6
void cat(int fd)
{
    int n;
    while((n = read(fd, buf, sizeof(buf))) > 0) 
    {
        if (write(1, buf, n) != n)
        {
            printf(1, "cat: write error\n");
            exit();
        }
    }
    if(n < 0)
    {
        printf(1, "cat: read error\n");
        exit();
    }
}

void uniq(int fd, int eval)
{
    int n, i;
    int j = 0;
    int idx = 0;

    while((n = read(fd, buf, sizeof(buf))) > 0)     // store the contents of the file into buf 
    {   
        // this loop will create strings and appends them into "list"
        for(i=0; buf[i]!='\0';i++)                  // loop through each char of buf
        {
            if(buf[i] == '\n' || buf[i+1] == '\0')  // check for new line
            {                                       // or end of line
                // edge case
                if(buf[i+1] == '\0')
                {
                    cur_str[j] = buf[i];
                    j++;
                    if(cur_str[0]=='\n')        // last line is new line
                        cur_str[j] = '\0';
                    else                        // last line is not a new line
                        cur_str[j+1] = '\0';

                    // append to list
                    for(j=0; j<STRING_SIZE;j++) list[idx][j] = cur_str[j];
                    break;
                }

                // add a new line and end of string
                cur_str[j] = '\n';
                cur_str[j+1] = '\0';

                // append to list
                for(j=0; j<STRING_SIZE;j++) list[idx][j] = cur_str[j];
                idx++;                      // increment the index for the next str
                memset(cur_str, 0, STRING_SIZE);    // clear the current string buffer for the next str
                j=0;                        // reset the counter
                
                // next char is \n
                if(buf[i+1] == '\n')
                {
                    list[idx][0] = '\n';
                    list[idx][1] = '\0';
                    idx++;
                    // if two or more \n, loop until the char != '\n'
                    if(buf[i+1] == '\n')
                    {
                        i++;
                        while(buf[i] == '\n')
                        {
                            list[idx][0] = '\n';
                            list[idx][1] = '\0';
                            i++; idx++;
                        }
                        i--; idx--;
                    }
                }
            }
            else
            {
                // add character to the curr string
                cur_str[j] = buf[i];
                j++;
            }
        }
    }

    // if last line is \n remove it --> following the behaviour of uniq
    if(list[idx][0] == '\n' && list[idx][0] == '\n') memset(list[idx], 0, STRING_SIZE);

    // printing uniq
    int count = 1;
    char* cur;
    for(i=0; list[i][0]!='\0';)
    {
        cur = list[i];
        i++;
        // print as soon as you find a different string
        // reset the counter and re-assign cur;
        while(strcmp(list[i], cur) == 0) 
        {
            count++; 
            i++;
        }

        if(eval == 0)               // no unique args, or -i
            printf(1, "%s", cur);
        else if(eval == 1)          // -c, print frequency of each repeated line
            printf(1, "      %d %s",count,cur);
        else if(eval == 2)          // -d, print based on frequency
        {
            if(count > 1)           // only print if the str freq is larger than 2
                printf(1, "%s", cur);
        }
        
        count = 1;      // reset count for the next group
    }   
    if(n < 0)           // reading error
    {
        printf(1, "read error\n");
        exit();
    }
}

// save contents to a file
void save(char* contents)
{
    int fd, i;
    char copy_contens[10] = ""; 
    for (i=0; i<10; i++) copy_contens[i] = contents[i];

    fd = open("savestr", O_CREATE | O_RDWR);
    if(fd >= 0) {
    } else {
        exit();
    }
    int size = sizeof(copy_contens);
    if(write(fd, &copy_contens, size) != size){
        exit();
    }
    close(fd);
}

int main(int argc, char *argv[])
{
    // skip the 1st command
    int param_size = argc-1;
    int fd = 1; // initialize to aoivd bug

    int pid;            // pipe and pid will only be used for 
    int fdd[2];         // cat filename | uniq

    // only one conditino can create a child
    if(strcmp(argv[1], "cat") == 0) // whenever cat is called
    {                               // store the file arg
        pid = fork();
        pipe(fdd);

        close(fdd[0]);              // close the parent pipe
        char copy_contens[10];
        int i;
        for (i=0; i<10; i++) copy_contens[i] = argv[2][i];
        save(copy_contens);
        wait();
        
    }
    else if(param_size == 0)
    {
        pid = fork();
        pipe(fdd);
        
        if(pid!=0)          // if parent exit
        {                   // otherwise there will be 2 prints
            wait();
            exit();
        }
            
        int n;              // copy the file name
        char copy_str[10] = {'a'};
        if((n = open("savestr", 0)) < 0) exit();
        read(n, copy_str, 10);
        close(fdd[1]);      // load the string

        if((fd = open(copy_str, 0)) < 0)    // call uniq 
        {
            printf(1, "uniq: cannot open file %s\n", copy_str);
            exit();
        }
        uniq(fd, 0);
        close(fd);
        wait();
    }
    else if(param_size == 1)         // no args, same as -i
    {
        if((fd = open(argv[1], 0)) < 0)
        {
            printf(1, "uniq: cannot open file %s\n", argv[1]);
            exit();
        }
        uniq(fd, 0);
        close(fd);
    }
    else
    {

        if((fd = open(argv[2], 0)) < 0)
        {
            printf(1, "uniq: cannot open file %s\n", argv[2]);
            exit();
        }

        // different args for uniq
        if(strcmp(argv[1], "-c") == 0)
        {
            uniq(fd, 1);   
        }
        else if(strcmp(argv[1], "-i") == 0)
        {
            uniq(fd, 0);
        }
        else if(strcmp(argv[1], "-d") == 0)
        {
            uniq(fd, 2);
        }
        else
        {
            printf(1, "Error: unssuported unique extension\n");
            close(fd);
            exit();
        }
        close(fd);
    }
    printf(1, "\n");    // new line for output
    exit();
}