/*
 * shelldon interface program

KUSIS ID: 59752 PARTNER NAME: Arun Ekin Özkan
KUSIS ID: 60066 PARTNER NAME: Doruk Taneli

 */

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>

#define MAX_LINE 80 /* 80 chars per line, per command, should be enough. */
static size_t count, histIndex;
char *history[10];

int parseCommand(char inputBuffer[], char *args[], int *background, int *redirection, char *line);

int main(void)
{
  char inputBuffer[MAX_LINE];   /* buffer to hold the command entered */
  int background;               /* equals 1 if a command is followed by '&' */
  char *args[MAX_LINE / 2 + 1]; /* command line (of 80) has max of 40 arguments */
  pid_t child;                  /* process id of the child process */
  int status;                   /* result from execv system call*/
  int shouldrun = 1;

  int i, upper;

  char line[MAX_LINE];

  int redirection = 0;
  char *outputname;
  int outfile;

  char *possiblePaths = getenv("PATH");
  char *pathsToken;

  char path[100];
  FILE *file;
  char *min, *hour;
  char full_path[MAX_LINE];
  char command[100];
  char music[40];

  while (shouldrun)
  { /* Program terminates normally inside setup */
    background = 0;
    redirection = 0;
    shouldrun = parseCommand(inputBuffer, args, &background, &redirection, line); /* get next command */

    if (strncmp(inputBuffer, "exit", 4) == 0)
      shouldrun = 0; /* Exiting from shelldon*/

    //PART 3 OUR IMPLEMENTATION
    if (strncmp(inputBuffer, "bd", 2) == 0)
    {
      time_t t = time(NULL);
      struct tm tm = *localtime(&t);

      //printf("atoi: %d-%d\n", atoi(args[1]), atoi(args[2]));

      int day = tm.tm_mday;
      int mon = tm.tm_mon + 1;

      //printf("int: %d-%d\n", day, mon);

      if (args[1] != NULL && args[2] != NULL)
      {
        if (atoi(args[1]) == day &&
            atoi(args[2]) == mon)
        {
          printf("Happy Birthday to you!!\n");
        }
      }
    }

    if (strncmp(inputBuffer, "history", 7) == 0)
    {
      printHistory();
    }
    historyCommands(line);
    addHistory(line);

    if (strcmp(args[0], "birdakika") == 0)
    {
      if (args[1] != NULL && args[2] != NULL)
      {

        file = fopen("crontabFile", "wt");

        hour = strtok(args[1], ".");
        min = strtok(NULL, ".");

        strcpy(command, min);
        strcat(command, " ");
        strcat(command, hour);
        strcpy(music, args[2]);
        realpath(music, full_path);
        strcat(command, " * * * play ");
        strcat(command, full_path);
        strcat(command, "\n");
        printf("%s", command);
        fprintf(file, "SHELL=/bin/sh\n");
        fprintf(file, "PATH=/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin\n");
        fprintf(file, "%s", command);
        fclose(file);
        for (i = 0; i < MAX_LINE / 2 + 1; i++)
        {
          args[i] = NULL;
        }

        args[0] = "crontab";
        args[1] = "crontabFile";

        strcpy(path, "/usr/bin/");
        strcat(path, args[0]);
      }
    }

    if (redirection != 0)
    {
      int ctr = 0;
      while (args[ctr] != NULL)
      {
        ctr++;
      }
      args[ctr - 2] = NULL;

      outputname = args[ctr - 1];

      if (redirection == 1)
      {
        outfile = open(outputname, O_RDWR | O_CREAT | O_TRUNC, 0666);
      }
      if (redirection == 2)
      {
        outfile = open(outputname, O_RDWR | O_CREAT | O_APPEND, 0666);
      }

      if (outfile == -1)
      {
        printf("Opening outputfile file failed.");
        exit(1);
      }
    }

    if (shouldrun)
    {
      child = fork();

      if (child == 0) //child
      {
        if (redirection != 0)
        {
          dup2(outfile, STDOUT_FILENO);
          dup2(outfile, STDERR_FILENO);
          close(outfile);
        }
        pathsToken = strtok(possiblePaths, ":");

        while (pathsToken != NULL)
        {

          char command[MAX_LINE];
          strcpy(command, pathsToken);
          strcat(command, "/");
          strcat(command, args[0]);

          execv(command, args);

          pathsToken = strtok(NULL, ":");
        }

        exit(0);
      }

      if (child > 0) //parent
      {
        waitpid(child, &status, 0);
        if (background == 1)
        {
          wait();
        }
      }
      /*
	After reading user input, the steps are 
	(1) Fork a child process using fork()
	(2) the child process will invoke execv()
	(3) if command included &, parent will invoke wait()
       */
    }
  }
  return 0;
}

/** 
 * The parseCommand function below will not return any value, but it will just: read
 * in the next command line; separate it into distinct arguments (using blanks as
 * delimiters), and set the args array entries to point to the beginning of what
 * will become null-terminated, C-style strings. 
 */

int parseCommand(char inputBuffer[], char *args[], int *background, int *redirection, char line[])
{
  int length,         /* # of characters in the command line */
      i,              /* loop index for accessing inputBuffer array */
      start,          /* index where beginning of next command parameter is */
      ct,             /* index of where to place the next parameter into args[] */
      command_number; /* index of requested command number */

  ct = 0;

  /* read what the user enters on the command line */
  do
  {
    printf("shelldon>");
    fflush(stdout);
    length = read(STDIN_FILENO, inputBuffer, MAX_LINE);
    strcat(inputBuffer, "\0");
    strcpy(line, inputBuffer);
  } while (inputBuffer[0] == '\n'); /* swallow newline characters */

  /**
     *  0 is the system predefined file descriptor for stdin (standard input),
     *  which is the user's screen in this case. inputBuffer by itself is the
     *  same as &inputBuffer[0], i.e. the starting address of where to store
     *  the command that is read, and length holds the number of characters
     *  read in. inputBuffer is not a null terminated C-string. 
     */
  start = -1;
  if (length == 0)
    exit(0); /* ^d was entered, end of user command stream */

  /** 
     * the <control><d> signal interrupted the read system call 
     * if the process is in the read() system call, read returns -1
     * However, if this occurs, errno is set to EINTR. We can check this  value
     * and disregard the -1 value 
     */

  if ((length < 0) && (errno != EINTR))
  {
    perror("error reading the command");
    exit(-1); /* terminate with error code of -1 */
  }

  /**
     * Parse the contents of inputBuffer
     */

  for (i = 0; i < length; i++)
  {
    /* examine every character in the inputBuffer */

    switch (inputBuffer[i])
    {
    case ' ':
    case '\t': /* argument separators */
      if (start != -1)
      {
        args[ct] = &inputBuffer[start]; /* set up pointer */
        ct++;
      }
      inputBuffer[i] = '\0'; /* add a null char; make a C string */
      start = -1;
      break;

    case '\n': /* should be the final char examined */
      if (start != -1)
      {
        args[ct] = &inputBuffer[start];
        ct++;
      }
      inputBuffer[i] = '\0';
      args[ct] = NULL; /* no more arguments to this command */
      break;

    default: /* some other character */
      if (start == -1)
        start = i;
      if (inputBuffer[i] == '&')
      {
        *background = 1;
        inputBuffer[i - 1] = '\0';
      }
      if (inputBuffer[i] == '>')
      {
        if (*redirection != 2)
        {
          *redirection = 1;
        }

        if (inputBuffer[i + 1] == '>' && i + 1 < length)
        {
          *redirection = 2;
        }
        inputBuffer[i - 1] = '\0';
      }
    } /* end of switch */
  }   /* end of for */

  /**
     * If we get &, don't enter it in the args array
     */

  if (*background)
    args[--ct] = NULL;

  args[ct] = NULL; /* just in case the input line was > 80 */

  return 1;

} /* end of parseCommand routine */

void addHistory(char *line)
{
  if (count < 10)
    count++;
  free(history[histIndex]);
  history[histIndex] = strdup(line);
  histIndex = (((histIndex) + (1)) % 10);
}

void printHistory(void)
{
  for (size_t i = count; i; i--)
    printf("%lu %s\n", i, history[(((histIndex) + 10 - (i)) % 10)]);
}

int historyCommands(char *line)
{
  size_t index, id;

  if (line[0] != '!')
    return 0;

  if (line[1] == '!')
  {
    if (count == 0)
    {
      fprintf(stderr, "No commands in history.\n");
      return -1;
    }
    index = (((histIndex) + 10 - (1)) % 10);
  }
  else if (isdigit(line[1]))
  {
    sscanf(&line[1], "%lu", &id);
    if (id == 0 || id > count)
    {
      fprintf(stderr, "No such command in history\n");
      return -1;
    }
    index = (((histIndex) + 10 - (id)) % 10);
  }
  else
  {
    fprintf(stderr, "Invalid syntax for history");
    return -1;
  }

  strcpy(line, history[index]);
  printf("%s\n", line);
  return 0;
}
