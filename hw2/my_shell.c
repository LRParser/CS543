#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#define MAX_LINE 80
#define DEBUG 1

int main(void)
{
 const int args_max_size = MAX_LINE/2 + 1;
 //int args_idx;
 int should_run = 1;
 char *delimiters = " ";
 const int history_size = 10;
 char command_history[history_size][args_max_size];
 char alias_tos[history_size][args_max_size];
 char alias_froms[history_size][args_max_size];
 int commands_run = 0;
 int aliases_set = 0;
 while (should_run)
 {
  char input_text[args_max_size];
  int repeat_history_command = 0;
  printf("osh>");
  fflush(stdout);
  fgets(input_text,sizeof input_text,stdin);
  if(DEBUG)
    printf("Got: %s\n",input_text);
  // Remove the newline
  input_text[strcspn(input_text,"\n")] = '\0';
  
  // The first thing to check is if this is a command like ! or !!. Then, we do not need to tokenize it and instead tokenize the element in history

  if(input_text[0] == '!')
  {
   int last_idx = commands_run < history_size ? commands_run : history_size;
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
       continue;
     }
     else
     {
       command_num = input_text[1] - '0';
     }
   }
   if (commands_run == 0)
   {
     printf("No commands in history\n");
     continue;
   }
   else if(command_num <= commands_run - history_size || command_num > commands_run)
   {
     printf("No such command in history\n");
     continue;
   }
   printf("Take from history\n");
   char *command = command_history[last_idx-1];
   char command_copy[args_max_size];
   strcpy(command_copy,command);
   printf("Going to re-run command: %s\n",command_copy);
   repeat_history_command = 1;
   strcpy(input_text,command_copy);
  }
  char input_text_copy[args_max_size];
  // Before we tokenize our input string, we make a copy for history storage, as tokenization mutates original string
  strcpy(input_text_copy,input_text);

  int args_idx = 0;
  char* args[args_max_size];
  char* word;
  word = strtok(input_text, " ");
  args[args_idx] = word;
  args_idx++;
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
  else if(strcmp("history",args[0]) == 0)
  {
    int l;
    l = commands_run < history_size ? commands_run : history_size;
    for(;l>0;l--)
    {
     printf("%d %s\n",l,command_history[l-1]);
    }
    continue;
  }
  else if(strcmp("alias",args[0]) == 0)
  {
    if(args_idx < 2)
    {
      printf("You must provide at least three arguments to an alias command\n");
      printf("For instance, alias dir = \"ls -la\"\n");
      continue;
    }
    else if(aliases_set >= history_size)
    {
      printf("This shell only supports setting %d aliases, please restart it to set new ones\n",history_size);
      continue;
    }
    else
    {
      char alias_from[args_max_size / 2];
      char alias_to[args_max_size / 2];
      strcpy(alias_from,args[1]);
      char arg_parsed[args_max_size];
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
      strcpy(alias_froms[aliases_set],alias_from);
      strcpy(alias_tos[aliases_set],arg_parsed);
      aliases_set++;
      continue;
    }
  }

  if (!repeat_history_command)
  {
  int idx_written;
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
    strcpy(command_history[idx_written],input_text_copy);
  }
  else
  {
    idx_written = commands_run;
    strcpy(command_history[idx_written],input_text_copy);
  }
  commands_run++;
  }


  if (DEBUG)
    printf("Command to now run: %s\n",args[0]);

  int should_wait = 0;
  int path_provided = 0;
  int set_command = 0;
  int set_path_command = 0;
  int dirs_in_path = 0;
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
       char path_segment[args_max_size];
       strcpy(path_segment,args[i]);
       printf("Segment is: %s\n",path_segment);
       char path_segment_processed[2 * args_max_size];
       // We validate that the first segment contains a left paren, and remove it
       int new_char_idx = 0;
       int j=0;
       int path_segment_len = strlen(path_segment);
       printf("Segment len is: %d\n",path_segment_len);
       for(;j<path_segment_len;j++)
       {
         if(path_segment[j] == '.')
         {
           char cwd[4 * args_max_size];
           if(getcwd(cwd,sizeof(cwd)) != NULL)
           {
             printf("Resolve cwd as: %s\n",cwd);
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
      //printf("Initial pathvar=%s\n",pathvar);
      //printf("Full desired path: %s\n",desired_path);
      putenv(desired_path);
      pathvar = getenv("PATH");
      //printf("New pathvar=%s\n",pathvar);
    }
    else {
      printf("I see you entered a set command, but it was not in the form set path = (path1 path2)\n");
      printf("Please resubmit\n");
      continue;
    }
  }
  //args[args_idx] = NULL;

  for(int i = 0;i<args_idx;i++)
  {
   printf("Parsed: %s\n",args[i]);
  }

  // See if the program name has been aliases by anything
  int match_found = 0;
  int args_idx_new = 0;
  char* args_new[args_max_size];
  for(int i = 0; i < aliases_set; i++)
  {
    if(!match_found && strcmp(alias_froms[i],args[0]) == 0)
    {
      printf("Match found %s\n",alias_tos[i]);
      char alias_to_tokenize[args_max_size];
      strcpy(alias_to_tokenize,alias_tos[i]);
      //alias_to_tokenize[strlen(alias_tos[i])] = '\0';
      char* word;
      word = strtok(alias_to_tokenize, " ");
      args_new[args_idx_new] = word;
      args_idx_new++;
      while((word = strtok(NULL, " ")) != NULL)
      {
       args_new[args_idx_new] = word;
       args_idx_new++;
      }
      match_found = 1;
      printf("args_idx_new is: %d\n",args_idx_new);
    }
  }

  if (match_found)
  {
      for(int i = 0; i < args_idx_new; i++)
      {
        strcpy(args[i],args_new[i]);
      }
      args_idx = args_idx_new;
  }
  printf("args_idx is: %d\n",args_idx);

  //args[args_idx] = NULL;

  for(int i = 0;i<args_idx;i++)
  {
   printf("Args: %s\n",args[i]);
  }

  if (args_idx >= 2 && strcmp("&",args[args_idx -1]) == 0)
  {
    printf("Strip last character%s\n",args[args_idx-1]);
    args[args_idx-1] = NULL;
  }
  for(int i = args_idx; i < args_max_size;i++)
  {
   args[i] = NULL;
  }

  for(int i = 0; i < args_idx; i++)
  {
    printf("Last hurrah: %s\n",args[i]);
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
   printf("Running the job: \n");
   if (match_found)
   {
     printf("%s\n",args_new[0]);
     ret = execvp(args_new[0],args_new);
   }
   else
   {
     printf("%s\n",args[0]);
     ret = execvp(args[0],args);
   }
   if(ret != 0)
   {
     if (match_found)
     {
       printf("Unable to find the command: %s in the PATH\n",args_new[0]);
     }
     else
     {
       printf("Unable to find the command: %s in the PATH\n",args[0]);
     }
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
     continue;
   }
  }
  
 }
 return 0;
}
