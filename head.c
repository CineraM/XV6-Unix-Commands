/*--------------------------------------
----------------------------------------
----------------------------------------

Matias Cinera - U 6931_8506
COP 6611
Instructor: Dr. Ankur Mali

The following is my implementation of 
head for xv6.

Template parameters of the program:
    - head filename
    - head -n #lines filename

-----------------------------------------
-----------------------------------------
----------------------------------------*/
#include "types.h"
#include "stat.h"
#include "user.h"
#define STRING_SIZE 512         // array size for all strings

char buf[5120];                 // store the contents of the file
char cur_str[STRING_SIZE];              // used as a temp string, later to be inserted into list
char list[500][STRING_SIZE];            // array that holds all the strings

void head(int fd, int limit)
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

    for(i=0; i<limit; i++) 
    {
        if(list[i][0]=='\0') break; // break if end of file was reached
        printf(1, "%s",list[i]);    
    }
    if(n < 0)   // reading error
    {
        printf(1, "read error\n");
        exit();
    }
}

int main(int argc, char *argv[])
{
    // skip the 1st command
    int param_size = argc-1;
    int fd = 1; // initialize to aoivd bug

    if(param_size == 1)
    {
        if((fd = open(argv[1], 0)) < 0)
        {
            printf(1, "uniq: cannot open file %s\n", argv[1]);
            exit();
        }
        head(fd, 10);
        close(fd);
    }
    else if(param_size == 3)
    {
        if(strcmp(argv[1], "-n") == 0)  // parsing the number
        {
            int num_lines = atoi(argv[2]);
            if(num_lines == -1)
                printf(1, "Error: %s is not an integer\n", argv[2]);  
            else
            {
                if((fd = open(argv[3], 0)) < 0)
                {
                    printf(1, "uniq: cannot open file %s\n", argv[3]);
                    exit();
                }
                 
                head(fd, num_lines);
                close(fd);
            }  
            
        }
        else
            printf(1, "extension error: only -n is suopported\n");
    }
    else
    {
        printf(1, "Error: wrong args\n");
    }
    
    exit();
}