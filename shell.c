#include "shell.h" 

char ** parse_args(char *line) {
  char **args = malloc(10 * sizeof(char*));
  int i = 0;
  for (char *part = strsep(&line, " "); part; part = strsep(&line, " ")) //parses 1 function
  {
    args[i++] = part;
  }
  return args;
}

void run(char **args){
 if(special_cases(args)) {
  return;
 } 
 child_run(args);
}

/*
 *Creates a child process to run args with execvp.
 *Parent process waits for child process to finish.
 */
void child_run(char **args) {
  pid_t child = fork();
  if(child == 0) {
    int status = execvp(args[0], args);
    if (status == -1) { //Built in error handling
      printf("ERROR: %s", strerror(errno));
      exit(-1);
    }
  }
  else {
    waitpid(child, NULL, 0);
  }
}

/*
 *This is an amalgamation of special cases that the shell DO NOT FORK.
 *"exit" - shell does not fork the process and terminates itself.
 *"cd" - shell does not fork and changes the directory of interest.
 *Returns 1 if there was a special case used; 0 otherwise.
 */
int special_cases(char **args) {
  if(strcmp("exit", args[0]) == 0) {
    exit(0);
  }
  if(strcmp("cd", args[0]) == 0) {
    if (chdir(args[1]) == -1) {
      printf("ERROR: %s\n", strerror(errno));
    }
    else {
      return 1;
    }
  }

  return 0;
}

/*
 *Strips off any trailing whitespaces such as: tabs, spaces, etc.
 */
char *strip_spaces(char *str){
  while (isspace(str[0])){//kill starting whitespace
    str++;
  }
  while (isspace(str[strlen(str)-1])){//kill ending whitespace
    str[strlen(str)-1] = '\0';
  }
  return str;
}

void redirect_out(char *fileName , char **args) {
  pid_t child = fork();
  if (child == 0) {
    int fd1 = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, 0755);
    int backup_stdout = dup(STDOUT_FILENO); // save stdout for later
    dup2(fd1, STDOUT_FILENO);          // sets STDOUT_FILENO's entry to the file for fd1.
    
    child_run(args);

    fflush(stdout); //reset stdout
    dup2(backup_stdout, STDOUT_FILENO); 
    
    close(fd1); //remove any memory leaks
    exit(0);
  } 
  else {
    waitpid(child, NULL, 0); 
  }
}

void redirect_out_append(char *fileName , char **args) {
  pid_t child = fork();
  if (child == 0) {
    int fd1 = open(fileName, O_WRONLY | O_APPEND | O_CREAT, 0755);
    int backup_stdout = dup(STDOUT_FILENO); // save stdout for later
    dup2(fd1, STDOUT_FILENO);          // sets STDOUT_FILENO's entry to the file for fd1.
    
    child_run(args);

    fflush(stdout); //reset stdout
    dup2(backup_stdout, STDOUT_FILENO); 
    
    close(fd1); //remove any memory leaks
    exit(0);
  } 
  else {
    waitpid(child, NULL, 0); 
  }
}

void redirect_in(char *fileName, char **args)
{
  pid_t child = fork();
  if (child == 0) {
    int fd1 = open(fileName, O_RDONLY, 0755); // source of idiocy
    int backup_stdin = dup(STDIN_FILENO); // save stdin for later
    dup2(fd1, STDIN_FILENO);               // sets STDIN_FILENO's entry to the file for fd1.

    child_run(args); 

    fflush(stdin); // reset stdin
    dup2(backup_stdin, STDIN_FILENO);

    close(fd1); // remove any memory leaks
    exit(0);
  }
  else {
    waitpid(child, NULL, 0);
  }
} 

void redirect_pipe(char **src_args, char **dest_args) { //I feel like this should work
  pid_t child = fork(); 
  if (child == 0) {
    int fd1 = open("tmp", O_CREAT | O_RDWR | O_TRUNC, 0755); 
    redirect_out("tmp", src_args); 
    redirect_in("tmp", dest_args); 
    execlp("rm", "rm", "tmp", NULL); //deletes temp file and exits child
  } 
  else {
    waitpid(child, NULL, 0);
  }
}

void redirect_inout(char *fileIn, char **args, char *fileOut){
  int backup_stdin = dup(STDIN_FILENO); 
  int backup_stdout = dup(STDOUT_FILENO); 
  pid_t child = fork(); 
  if (child == 0) {
    int fd_in = open(fileIn, O_RDONLY, 0755);
    int fd_out = open(fileOut, O_CREAT | O_WRONLY | O_TRUNC, 0755);
    dup2(fd_in, STDIN_FILENO);
    dup2(fd_out, STDOUT_FILENO);
    child_run(args); //I think this is what I'm supposed to do?
    //resets stdin and stdout
    fflush(stdin); 
    fflush(stdout);
    dup2(backup_stdin, STDIN_FILENO);
    dup2(backup_stdout, STDOUT_FILENO);
    close(fd_in);
    close(fd_out);
    exit(0);
  } 
  else {
    waitpid(child, NULL, 0);
  }
}