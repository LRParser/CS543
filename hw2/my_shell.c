#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#define MAX_LINE 80

int main(void)
{
  const int args_max_size = MAX_LINE/2 + 1;
  char *args[args_max_size];
  int should_run = 1;
  char *delimiters = " ";
  int i = 0;
  int args_idx = 0;
  pid_t pid;
  while (should_run)
  {
    i = 0;
    args_idx = 0;
    printf("osh>\n");
    fflush(stdout);
    char input_text[80];
    fgets(input_text,80,stdin); // = "Test one two";
    //gets(input_text);
    printf("%s\n",input_text);
    char* word;
    // Note: Referred to need to include <string.h> here: https://gist.github.com/efeciftci/9120921
    word = strtok(input_text, " ");
    args[args_idx] = word;
    args_idx++;
    printf("Token: %s\n", word);
    while((word = strtok(NULL, " ")) != NULL)
    {
      args[args_idx] = word;
      args_idx++;
    }

    for(;i<args_idx;i++)
    {
      printf("Parsed: %s\n",args[i]);
    }
    i = args_idx + 1;
    for(; i < args_max_size;i++)
    {
      //args[i] = NULL;
    }
    // Now let's fork a process
    pid = fork();
    if (pid < 0)
    {
      fprintf(stderr,"Error while trying to fork\n");
    }
    else if (pid == 0)
    {
      // This is the child process. Run the job
      printf("Running the job\n");
      execvp(args[0],args);
      should_run = 0;
      continue;
    }
    else
    {
      // This is the parent. Wait for the child to finish
      wait(NULL);
      printf("Command run, waiting for next command\n");
      
    }
  }
  return 0;
}
