/*--------------------------------------
----------------------------------------
----------------------------------------
Matias Cinera - U 6931_8506
COP 6611
Instructor: Dr. Ankur Mali

The following is my implementation of 
sort for xv6.


-----------------------------------------
-----------------------------------------
----------------------------------------*/
#include "types.h"
#include "stat.h"
#include "user.h"

char buf[5120];         // store the contents of the file
char cur_str[512];      // used as a temp string, later to be inserted into list
char list[500][512];    // array that holds all the strings
char eval[500][512];    // used for sort comparison

void sort(int fd, char* mode, char* outputfile)
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
                    for(j=0; j<512;j++) list[idx][j] = cur_str[j];
                    break;
                }

                // add a new line and end of string
                cur_str[j] = '\n';
                cur_str[j+1] = '\0';

                // append to list
                for(j=0; j<512;j++) list[idx][j] = cur_str[j];
                idx++;                      // increment the index for the next str
                memset(cur_str, 0, 512);    // clear the current string buffer for the next str
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

    memset(cur_str, 0, 512); // clear the buffer, used to swap
    int k;
    int z = 0;
    int copy_idx = 0;
    int eidx = 0;

    // truncated strings for sorting comparison
    for(i=0; i<=idx; i++)
    {
        // finding alpha, or numeric character index in current string
        for(z=0; z<512; z++)
        {
            if (list[i][z] >= 48 && list[i][z] <= 57) break;  // digits
            if (list[i][z] >= 65 && list[i][z] <= 90) break;  // cap
            if (list[i][z] >= 97 && list[i][z] <= 122) break; // lower
            if (list[i][z] == '\0') break;
        }
        if(z==512 || list[i][z] == '\0')    // if there is not a numeric || alpha char, copy all the string
        {
            for(k = 0; k<512; k++) cur_str[copy_idx] = list[i][copy_idx];
        }
        else // truncate the string until the desired character
        {
            copy_idx = 0;
            for(k = z; k<512; k++)
            {
                cur_str[copy_idx] = list[i][k];
                copy_idx++;
            }
        }
        // copy the string into eval
        for(j=0; j<512;j++) eval[eidx][j] = cur_str[j];
        eidx++; // increase index for next string
        memset(cur_str, 0, 512);
    }

    // bubble sort
    // Sorting based on the truncated strings while
    // keeping track of the original strings 
    for(i=0; i<=idx; i++)
    {
        for(j=i+1; j<=idx; j++)
        {
            if(strcmp(eval[i], eval[j]) > 0)
            {
                // original elements
                for(k=0; k<512;k++) cur_str[k] = list[i][k];
                for(k=0; k<512;k++) list[i][k] = list[j][k]; 
                for(k=0; k<512;k++) list[j][k] = cur_str[k];
                // truncated elements
                for(k=0; k<512;k++) cur_str[k] = eval[i][k];
                for(k=0; k<512;k++) eval[i][k] = eval[j][k]; 
                for(k=0; k<512;k++) eval[j][k] = cur_str[k];
                
            }
        }
    }

    if(strcmp("none", mode) == 0)
    {
        for(j=0; j<=idx; j++) printf(1, "%s", list[j]); // print the array
    }
    else if(strcmp("-r", mode) == 0)
    {
        for(j=idx; j>=0; j--) printf(1, "%s", list[j]); // reverse print
    }
    else if(strcmp("-o", mode) == 0)    // create output file
    {
        printf(1, "file name: %s", outputfile);
        printf(1, "-o branch");
    }

    if(n < 0)   // reading error
    {
        printf(1, "read error\n");
        exit();
    }
}

// --------------------------Sorting -n -------------------------------------
// Separate the strings into groups, negative ints, positive ints, and regular strings.  
// this process could be done with 2 arrays to be more memory efficient, or with some data structures.
// However, coding the right data structures for this is problem a whole different project and
// having arrays for each group makes it easier to code.

char positive_list[500][512];  // arrays that hold the strings which
int positive_eval[500][1];     // have negative or positive integers
                               // at index 0
char negative_list[500][512];  
int negative_eval[500][1];

char str_list[500][512];       // list for non digits at index 0
char str_eval[500][512];

void sort_n(int fd)
{
    int n, i;
    int j = 0;
    int idx = 0;

    // while loop will insert all the lines into list
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
                    for(j=0; j<512;j++) list[idx][j] = cur_str[j];
                    break;
                }

                // add a new line and end of string
                cur_str[j] = '\n';
                cur_str[j+1] = '\0';

                // append to list
                for(j=0; j<512;j++) list[idx][j] = cur_str[j];
                idx++;                      // increment the index for the next str
                memset(cur_str, 0, 512);    // clear the current string buffer for the next str
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

    memset(cur_str, 0, 512); // clear the buffer, used to swap
    int k;
    int z = 0;
    int copy_idx = 0;
    int eidx = 0;
    int int_t = -1;

    // truncated strings for sorting comparison
    for(i=0; i<=idx; i++)
    {
        // finding alpha, or numeric character index in current string
        for(z=0; z<512; z++)
        {
            if (list[i][z] >= 48 && list[i][z] <= 57) // digits
            {
                if(list[i][z-1] == 45)  // keep negative number for later sorting
                {
                    z--;
                    break;
                }
                else
                    break;
            }  
            if (list[i][z] >= 65 && list[i][z] <= 90) break;  // cap
            if (list[i][z] >= 97 && list[i][z] <= 122) break; // lower
            if (list[i][z] == '\0') break;
        }
        if(z==512 || list[i][z] == '\0')    // if there is not a numeric || alpha char, copy all the string
        {
            for(k = 0; k<512; k++) cur_str[copy_idx] = list[i][copy_idx];
        }
        else // truncate the string until the desired character
        {
            copy_idx = 0;
            for(k = z; k<512; k++)
            {
                cur_str[copy_idx] = list[i][k];
                copy_idx++;
            }
        }
        // copy the string into eval
        for(j=0; j<512;j++) eval[eidx][j] = cur_str[j];
        eidx++; // increase index for next string
        memset(cur_str, 0, 512);
    }
    
    int positive_idx = 0;
    int negative_idx = 0;
    int str_idx = 0;

    for(i=0; i<=idx; i++)
    {
        if(eval[i][0] == 45)  // negative ints
        {
            for(j=0; j<512;j++) 
                negative_list[negative_idx][j] = list[i][j];
            
            // have to parse the integer without "-"
            memset(cur_str, 0, 512);
            for(j=1; j<512;j++)
            {
                if (list[i][j] >= 48 && list[i][j] <= 57)
                    cur_str[j-1] = eval[i][j];
                else
                {
                    cur_str[j-1] = '\0';
                    break;
                }
            }   
            // ^^this old version of atoi cant parse the string
            // "-123", it has to be reomved manually
            negative_eval[negative_idx][0] = atoi(cur_str)*-1;
            negative_idx++;
        }
        else if(eval[i][0] >= 48 && eval[i][0] <= 57) // positive ints
        {
            for(j=0; j<512;j++) 
                positive_list[positive_idx][j] = list[i][j];

            positive_eval[positive_idx][0] = atoi(eval[i]);
            positive_idx++;
        }
        else    // else they are regular ints
        {
            for(j=0; j<512;j++) 
            {
                str_list[str_idx][j] = list[i][j];
                str_eval[str_idx][j] = eval[i][j];
            }
            str_idx++;
        }
    }

    // BUBBLE SORT X3 :) !!
    // 3 times the sh!t complexity :))))
    if(negative_idx >= 2)
    {
        for(i=0; i<=negative_idx; i++)
        {
            for(j=i+1; j<=negative_idx; j++)
            {
                if(negative_eval[i][0] > negative_eval[j][0])
                {
                    // original elements
                    for(k=0; k<512;k++) cur_str[k] = negative_list[i][k];
                    for(k=0; k<512;k++) negative_list[i][k] = negative_list[j][k]; 
                    for(k=0; k<512;k++) negative_list[j][k] = cur_str[k];
                    // truncated elements
                    int_t = negative_eval[i][0];
                    negative_eval[i][0] = negative_eval[j][0]; 
                    negative_eval[j][0] = int_t;
                }
            }
        }
    }

    if(positive_idx >= 2)
    {
        for(i=0; i<=positive_idx; i++)
        {
            for(j=i+1; j<=positive_idx; j++)
            {
                if(positive_eval[i][0] > positive_eval[j][0])
                {
                    // original elements
                    for(k=0; k<512;k++) cur_str[k] = positive_list[i][k];
                    for(k=0; k<512;k++) positive_list[i][k] = positive_list[j][k]; 
                    for(k=0; k<512;k++) positive_list[j][k] = cur_str[k];
                    // truncated elements
                    int_t = positive_eval[i][0];
                    positive_eval[i][0] = positive_eval[j][0]; 
                    positive_eval[j][0] = int_t;
                }
            }
        }
    }

    if(str_idx >= 2)
    {
        for(i=0; i<=str_idx; i++)
        {
            for(j=i+1; j<=str_idx; j++)
            {
                if(strcmp(str_eval[i], str_eval[j]) > 0)
                {
                    // original elements
                    for(k=0; k<512;k++) cur_str[k] = str_list[i][k];
                    for(k=0; k<512;k++) str_list[i][k] = str_list[j][k]; 
                    for(k=0; k<512;k++) str_list[j][k] = cur_str[k];
                    // truncated elements
                    for(k=0; k<512;k++) cur_str[k] = str_eval[i][k];
                    for(k=0; k<512;k++) str_eval[i][k] = str_eval[j][k]; 
                    for(k=0; k<512;k++) str_eval[j][k] = cur_str[k];
                }
            }
        }
    }

    // output
    for(j=0; j<=negative_idx; j++) printf(1, "%s", negative_list[j]);
    for(j=0; j<=str_idx; j++) printf(1, "%s", str_list[j]);
    for(j=0; j<=positive_idx; j++) printf(1, "%s", positive_list[j]);
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
            printf(1, "sort: cannot open file %s\n", argv[1]);
            exit();
        }
        sort(fd, "none", "");   
        printf(1,"\n");     // extra line in the output
        close(fd);
    }
    else if (param_size == 2)
    {
        if(strcmp(argv[1], "-r") == 0)
        {
            if((fd = open(argv[2], 0)) < 0)
            {
                printf(1, "sort: cannot open file %s\n", argv[2]);
                exit();
            }
            sort(fd, "-r", "");
            printf(1,"\n");     // extra line in the output
            close(fd);
        }
        else if (strcmp(argv[1], "-n") == 0)
        {
            if((fd = open(argv[2], 0)) < 0)
            {
                printf(1, "sort: cannot open file %s\n", argv[2]);
                exit();
            }
            sort_n(fd);
            printf(1,"\n");     // extra line in the output
            close(fd);
        }
        else
        {
            printf(1, "\nError: Missing args \n");
        }
        
    }
    else if (param_size == 3)
    {
        if(strcmp(argv[1], "-o") != 0)
        {
            printf(1, "\nError: Missing args \n");
        }
        if((fd = open(argv[3], 0)) < 0)
        {
            printf(1, "sort: cannot open file %s\n", argv[3]);
            exit();
        }
        sort(fd, "-o", argv[2]);
        printf(1,"\n");     // extra line in the output
        close(fd);
    }
    else
    {
        printf(1, "\nError: Missing args \n");
    }
    
    exit();
}