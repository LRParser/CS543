#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#define MAX_LINE 80


int DEBUG=0;
const int ARGS_MAX_SIZE = MAX_LINE/2 + 1;

/**
 * Executes the shell command entered
 * @param input_text - this is the text (either from shell or config file) that will be executed
 * @param should_run - this pointer is set to say if the REPL can exit
 * @param history_size - the number of commands to store in history. Usually set to 10
 * @param commands_run - a counter of the items in history, used to implment a ring buffer for history element storage
 * @param command_history - a pointer to strings of the last history_size commands
 * @param alias_froms - a pointer to strings where we will make an alias-based substitution
 * @param alias_tos - a pointer to strings that we will substitute in for an alias.
 * @param aliases_set - a counter of the number of aliases that we have set
 */
void process_command(char *input_text, int *should_run, const int history_size, int *commands_run, char **command_history, char **alias_froms, char **alias_tos, int *aliases_set)
{

  if(DEBUG)
      printf("Got: %s\n",input_text);
    // Remove the newline
    input_text[strcspn(input_text,"\n")] = '\0';
    printf("Removed newline\n");
    // The first thing to check is if this is a command like ! or !!. Then, we do not need to tokenize it and instead tokenize the element in history
    int repeat_history_command = 0;
    if(input_text[0] == '!')
    {
     int last_idx = *commands_run < history_size ? *commands_run : history_size;
     printf("Last_idx %d\n",last_idx);
     int command_num;
     if (input_text[1] == '!')
     {
       command_num = last_idx;
     }
     else
     {
       // Subtracting zero character will give us an integer
       if (!isdigit(input_text[1]))
       {
         printf("Cannot parse the digit for the repeat command\n");
         return;
       }
       else
       {
         command_num = input_text[1] - '0';
       }
     }
     if (*commands_run == 0)
     {
       printf("No commands in history\n");
       return;
     }
     else if(command_num <= *commands_run - history_size || command_num > *commands_run)
     {
       printf("No such command in history\n");
       return;
     }
     if(DEBUG)
       printf("Take from history\n");
     char *command = command_history[last_idx-1];
     char command_copy[ARGS_MAX_SIZE];
     strcpy(command_copy,command);
     if(DEBUG)
        printf("Going to re-run command: %s\n",command_copy);
     repeat_history_command = 1;
     strcpy(input_text,command_copy);
    }

    if(DEBUG)
      printf("Checking for alias match\n");

    // If we match an aliased command, we sub that in also, similar to how we sub in a history command
    int match_found = 0;
    for(int i = 0; i < *aliases_set; i++)
    {
      if(!match_found && strcmp(alias_froms[i],input_text) == 0)
      {
        if(DEBUG)
           printf("Match found %s\n",alias_tos[i]);
        strcpy(input_text,alias_tos[i]);
        match_found = 1;
      }
    }

    if(DEBUG)
      printf("Checked for alias match\n");


    char input_text_copy[ARGS_MAX_SIZE];
    // Before we tokenize our input string, we make a copy for history storage, as tokenization mutates original string
    strcpy(input_text_copy,input_text);



    int args_idx_new = 0;
    char* args_new[ARGS_MAX_SIZE];

    int args_idx = 0;
    char* args[ARGS_MAX_SIZE];
    char* word;
    word = strtok(input_text, " ");
    args[args_idx] = word;
    args_idx++;
    while((word = strtok(NULL, " ")) != NULL)
    {
     args[args_idx] = word;
     args_idx++;
    }

    if(DEBUG)
      printf("Tokenized\n");

    // See if we should exit the shell
    if(strcmp("exit",args[0]) == 0)
    {
      *should_run = 0;
      return;
    }
    else if(strcmp("history",args[0]) == 0)
    {
      int l;
      l = *commands_run < history_size ? *commands_run : history_size;
      for(;l>0;l--)
      {
       if(DEBUG)
         printf("%d %s\n",l,command_history[l-1]);
      }
      return;
    }
    else if(strcmp("alias",args[0]) == 0)
    {
      if(args_idx < 2)
      {
        printf("You must provide at least three arguments to an alias command\n");
        printf("For instance, alias dir = \"ls -la\"\n");
        return;
      }
      else if(*aliases_set >= history_size)
      {
        printf("This shell only supports setting %d aliases, please restart it to set new ones\n",history_size);
        return;
      }
      else
      {
        char alias_from[ARGS_MAX_SIZE / 2];
        char alias_to[ARGS_MAX_SIZE / 2];
        strcpy(alias_from,args[1]);
        char arg_parsed[ARGS_MAX_SIZE];
        int parsed_idx = 0;
        for(int i=2; i < args_idx; i++)
        {
           size_t arg_len = strlen(args[i]);
           for(int j=0; j < arg_len; j++)
           {
             if (args[i][j] != '"')
             {
               arg_parsed[parsed_idx] = args[i][j];
               parsed_idx++;
             }
           }

           if(i < args_idx - 1)
           {
              arg_parsed[parsed_idx] = ' ';
              parsed_idx++;
           }

           // Append string

        }
        arg_parsed[parsed_idx] = '\0';
        strcpy(alias_froms[*aliases_set],alias_from);
        strcpy(alias_tos[*aliases_set],arg_parsed);
        *aliases_set = *aliases_set + 1;
        return;
      }
    }
    if(DEBUG)
      printf("Writing entry in history\n");

    if (!repeat_history_command)
    {
    int idx_written;
    if(*commands_run > history_size -1)
    {
      if(DEBUG)
        printf("History full, making space\n");

      // Move everything one position left in the array
      int i;
      for(i=1;i<history_size;i++)
      {
        strcpy(command_history[i-1],command_history[i]);
      }
      // Copy new command into last index
      idx_written = history_size - 1;
      strcpy(command_history[idx_written],input_text_copy);
    }
    else
    {
      if(DEBUG)
        printf("Add directly to history\n");
      idx_written = *commands_run;
      strcpy(command_history[idx_written],input_text_copy);
    }
    *commands_run = *commands_run + 1;
    }


    if (DEBUG)
      printf("Command to now run: %s\n",args[0]);

    int should_wait = 0;
    int path_provided = 0;
    int set_command = 0;
    should_wait = (strcmp("&",args[args_idx-1]) != 0);
    path_provided = args[0][0] == '/';
    set_command = strcmp("set",args[0]) == 0;
    if (set_command && args_idx >= 2)
    {
      if(strcmp("path",args[1]) == 0 && strcmp("=",args[2]) == 0)
      {
        char desired_path[1024];
        const char *path_start = "PATH=";
        strcpy(desired_path,"PATH=");
        for(int i = 3; i < args_idx; i++)
        {
         char path_segment[ARGS_MAX_SIZE];
         strcpy(path_segment,args[i]);
         char path_segment_processed[2 * ARGS_MAX_SIZE];
         // We validate that the first segment contains a left paren, and remove it
         int new_char_idx = 0;
         int j=0;
         size_t path_segment_len = strlen(path_segment);
         for(;j<path_segment_len;j++)
         {
           if(path_segment[j] == '.')
           {
             char cwd[4 * ARGS_MAX_SIZE];
             if(getcwd(cwd,sizeof(cwd)) != NULL)
             {
               int cwd_len = strlen(cwd);
               int k = 0;
               for(; k < cwd_len; k++)
               {
                 path_segment_processed[new_char_idx] = cwd[k];
                 new_char_idx++;
               }
             }
           }
           else if(path_segment[j] != '(' && path_segment[j] != ')')
           {
             path_segment_processed[new_char_idx] = path_segment[j];
             new_char_idx++;
           }
         }
         path_segment_processed[new_char_idx] = '\0';
         j = 0;
         strcat(desired_path,path_segment_processed);
         if(i < args_idx - 1)
         {
           const char *path_sep = ":";
           strcat(desired_path,path_sep);
         }
        }
        char* pathvar = getenv("PATH");
        putenv(desired_path);
        pathvar = getenv("PATH");
      }
      else {
        printf("I see you entered a set command, but it was not in the form set path = (path1 path2)\n");
        printf("Please resubmit\n");
        return;
      }
    }

    if (DEBUG)
    {
      for(int i = 0;i<args_idx;i++)
      {
       printf("Parsed: %s\n",args[i]);
      }
    }


    if (args_idx >= 2 && strcmp("&",args[args_idx -1]) == 0)
    {
      if(DEBUG)
        printf("Strip last character%s\n",args[args_idx-1]);
      args[args_idx-1] = NULL;
    }
    for(int i = args_idx; i < ARGS_MAX_SIZE;i++)
    {
     args[i] = NULL;
    }

    // Here, we fork
    pid_t pid = fork();
    if (pid < 0)
    {
     fprintf(stderr,"Error while trying to fork\n");
    }
    else if (pid == 0)
    {
     // Here this is the child. In the context of the child process, run the command
     int ret;
     if(DEBUG)
     {
       printf("Running the job: \n");
       printf("%s\n",args[0]);
     }
     ret = execvp(args[0],args);
     if(ret != 0)
     {
       printf("Unable to find the command: %s in the PATH\n",args[0]);
     }
     exit(errno);
    }
    else
    {
     // Here this is the parent. Wait for child to finish, unless & was given
     if(should_wait)
     {
       wait(NULL);
     }
     else
     {
       return;
     }
    }

}

/**
 *
 * Ensures the arrays that store history and alias states are properly allocated on the heap
 *
 * @param history_size - used to determine number of aliases or commands to store at most
 * @param command_history - used to implement history function in shell
 * @param alias_tos - determines what the from string will be substituted to
 * @param alias_froms - used to store the from part of an alias
 */
void init_history_arrays(int history_size, char **command_history, char **alias_tos, char **alias_froms)
{
  for(int i = 0; i < history_size; i++)
  {
    command_history[i] = malloc(ARGS_MAX_SIZE * sizeof(char));
  }

  for(int i = 0; i < history_size; i++)
  {
    alias_tos[i] = malloc(ARGS_MAX_SIZE * sizeof(char));
  }

  for(int i = 0; i < history_size; i++)
  {
    alias_froms[i] = malloc(ARGS_MAX_SIZE * sizeof(char));
  }

}

/**
 * Main entry point the the program, implements a shell that first executes any commands in a .cs543rc file in the user's
 * home directory, and then loops until a user types "exit", executing one of the commands via the main process_command function
 * @return
 */
int main(void)
{
 int should_run = 1;
 char *delimiters = " ";
 const int history_size = 10;

 char** command_history = malloc(history_size * sizeof(char));
 char** alias_tos = malloc(history_size * sizeof(char));
 char** alias_froms = malloc(history_size * sizeof(char));

 init_history_arrays(history_size, command_history, alias_tos, alias_froms);

 int commands_run = 0;
 int aliases_set = 0;
 int init_file_processed = 0;
 while (should_run)
 {
  char input_text[ARGS_MAX_SIZE];

  if(!init_file_processed)
  {
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t chars;

    char* home_path = getenv("HOME");
    char file_path[strlen(home_path) + 20];
    strcpy(file_path,home_path);
    strcat(file_path,"/.cs543rc");
    fp = fopen(file_path, "r");
    if(fp == NULL)
    {
      init_file_processed = 1;
    }
    else
    {

      while((chars = getline(&line, &len, fp)) != -1)
      {
        process_command(line, &should_run, history_size, &commands_run, command_history, alias_froms, alias_tos, &aliases_set);
      }

      fclose(fp);
    }
    init_file_processed = 1;
  }
  else {

  printf("osh>");
  fflush(stdout);
  fgets(input_text,sizeof input_text,stdin);

  if(strlen(input_text) == 1)
  {
    continue;
  }

  process_command(input_text, &should_run, history_size, &commands_run, command_history, alias_froms, alias_tos, &aliases_set);

  }

 }
 return 0;
}
