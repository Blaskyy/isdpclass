#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char const *argv[])
{
  int f_des[2];
  int pid;
  char msg[4000];

  if (pipe(f_des) == -1) {
    perror("Can not create the IPC pipe!");
    return 1;
  }
  pid = fork();
  if (pid == -1) {
    perror("Can not create new process!");
    return 1;
  } else if (pid == 0) {
    close(f_des[1]);
    wait(0);
    if (read(f_des[0], msg, 4000) ==-1) {
      perror("Child process cannot read data from pipe!");
      return 1;
    } else {
      fprintf(stdout, "In child process, receiver message: \n%s\n", msg);
      _exit(0);
    }
  } else {
    close(f_des[0]);
    dup2(f_des[1],STDOUT_FILENO);
    execlp(argv[1],argv[1],argv[2],(char*)NULL);
    _exit(0);
  }
  return 0;
}