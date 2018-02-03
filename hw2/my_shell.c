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
  int i = 0;
  while (should_run)
  {
    i = 0;
    //args_idx = 0;
    printf("osh>\n");
    fflush(stdout);
    gets(input_text);
//    fgets(input_text,args_max_size,stdin); // = "Test one two";
    // We need to null-terminate the input_text
//    input_text[strlen(input_text) -1] = '\0';
    //gets(input_text);
    printf("%s\n",input_text);
    char* word;
    // Note: Referred to need to include <string.h> here: https://gist.github.com/efeciftci/9120921
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

    int should_wait = 0;
    printf("Should wait: %s\n",args[args_idx-1]);
    should_wait = (args[args_idx-1] == "&");
    printf("Should wait: %d\n",should_wait);

    args[args_idx] = NULL;

    for(;i<args_idx;i++)
    {
      printf("Parsed: %s\n",args[i]);
    }
    i = args_idx + 1;
    for(; args_idx < args_max_size;args_idx++)
    {
      args[args_idx] = NULL;
    }
    // Now let's fork a process
    pid_t pid = fork();
    if (pid < 0)
    {
      fprintf(stderr,"Error while trying to fork\n");
    }
    else if (pid == 0)
    {
      // This is the child process. Run the job
      printf("Running the job\n");
      
      printf("%s\n",args[0]);
      //execlp("/bin/ls","ls",NULL);
      execvp(args[0],args);
      exit(errno);
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
