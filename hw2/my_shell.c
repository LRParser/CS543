#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#define MAX_LINE 80

int main(void)
{
  const int args_max_size = MAX_LINE/2 + 1;
  char input_text[args_max_size];
  char *args[args_max_size];
  int should_run = 1;
  char *delimiters = " ";
  int i;
  const int history_size = 3;
  char command_history[history_size][args_max_size];
  int commands_run = 0;
  while (should_run)
  {
    i = 0;
    printf("osh>");
    fflush(stdout);
    gets(input_text);
    printf("%s\n",input_text);
    char* word;
    word = strtok(input_text, " ");
    int args_idx = 0;
    args[args_idx] = word;
    args_idx++;
    printf("Token: %s\n", word);
    while((word = strtok(NULL, " ")) != NULL)
    {
      args[args_idx] = word;
      args_idx++;
    }

    // See if we should exit the shell
    if(strcmp("exit",args[0]) == 0)
    {
        should_run = 0;
        continue;
    }
    if(strcmp("history",args[0]) == 0)
    {
        printf("Print history\n");
        int l;
        l = commands_run < history_size ? commands_run : history_size;
        for(;l>0;l--)
        {
           printf("%d %s\n",l,command_history[l-1]);
        }
    }


    int idx_written = 0;
    if(commands_run > history_size -1)
    {
        // Move everything one position left in the array
        int i;
        for(i=1;i<history_size;i++)
        {
            strcpy(command_history[i-1],command_history[i]);
        }
        // Copy new command into last index
        idx_written = history_size - 1;
        strcpy(command_history[idx_written],args[0]);
    }
    else
    {
        idx_written = commands_run;
        strcpy(command_history[idx_written],args[0]);
    }
 
    printf("Added to history at idx: %d, %s\n",commands_run,command_history[idx_written]);
    commands_run++;

    int should_wait = 0;
    int path_provided = 0;
    int set_command = 0;
    int set_path_command = 0;
    int dirs_in_path = 0;
    should_wait = (strcmp("&",args[args_idx-1]) != 0);
    path_provided = args[0][0] == '/';
    set_command = strcmp("set",args[0]) == 0;
    printf("First char: %c\n",args[0][0]);
    printf("Should wait: %d\n",should_wait);
    printf("Path provided: %d\n",path_provided);
    printf("Set command: %d\n",set_command);
    if (set_command && args_idx >= 2)
    {
        if(strcmp("path",args[1]) == 0 && strcmp("=",args[2]) == 0)
        {
            char desired_path[1024];
            const char *path_start = "PATH=";
            strncpy(desired_path,"PATH=",sizeof(path_start));
            for(i = 3; i < args_idx; i++)
            {
               char* path_segment = args[i];
               printf("Segment is: %s\n",path_segment);
               char path_segment_processed[1024];
               // We validate that the first segment contains a left paren, and remove it
               int new_char_idx = 0;
               int j=0;
               int path_segment_len = strlen(path_segment);
               printf("Segment len is: %d\n",path_segment_len);
               for(;j<path_segment_len;j++)
               {
                   if(path_segment[j] == '.')
                   {
                      char cwd[1024];
                      if(getcwd(cwd,sizeof(cwd)) != NULL)
                      {
                          printf("Resolve cwd as: %s\n",cwd);
                          int cwd_len = strlen(cwd);
                          int k = 0;
                          for(; k < cwd_len; k++)
                          {
                              path_segment_processed[new_char_idx] = cwd[k];
                          }
                      }
                   }
                   else if(path_segment[j] != '(' && path_segment[j] != ')')
                   {
                      path_segment_processed[new_char_idx] = path_segment[j];
                      new_char_idx++; 
                   }
               }
               j = 0;

               printf("Adding to path: %s\n",args[i]);
               strncat(desired_path,path_segment_processed,sizeof(path_segment_processed));
               if(i < args_idx - 1)
               {
                   const char *path_sep = ":";
                   strcat(desired_path,path_sep);                
               }
            }
            char* pathvar = getenv("PATH");
            printf("Initial pathvar=%s\n",pathvar);
            printf("Full desired path: %s\n",desired_path);
            //putenv(desired_path);
            pathvar = getenv("PATH");
            printf("New pathvar=%s\n",pathvar);


            i = 0;
        }
        else {
            printf("I see you entered a set command, but it was not in the form set path = (path1 path2)\n");
            printf("Please resubmit\n");
            continue;
        }
    }
    args[args_idx] = NULL;

    for(;i<args_idx;i++)
    {
      printf("Parsed: %s\n",args[i]);
    }
    i = 0;
    if (args_idx >= 2 && strcmp("&",args[args_idx -1]) == 0)
    {
        printf("Strip last character%s\n",args[args_idx-1]);
        args[args_idx-1] = NULL;
    }
    for(; args_idx < args_max_size;args_idx++)
    {
      args[args_idx] = NULL;
    }

    // Here, we fork
    pid_t pid = fork();
    if (pid < 0)
    {
      fprintf(stderr,"Error while trying to fork\n");
    }
    else if (pid == 0)
    {
      // This is the child process. Run the job
      //printf("Running the job\n");
      //printf("%s\n",args[0]);
      int ret;
      ret = execvp(args[0],args);
      if(ret != 0)
      {
          printf("Unable to find the command: %s in the PATH\n",args[0]);
      }
      exit(errno);
    }
    else
    {
      // This is the parent. Wait for the child to finish
      if(should_wait)
      {
          wait(NULL);
      }
      else
      {
          continue;
      }
    }
    
  }
  return 0;
}
