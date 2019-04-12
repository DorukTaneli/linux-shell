# Linux-Shell

This code was developed by **Doruk Taneli** and **Ekin Ozkan** as a project of Operating Systems course.

### Description

We implemented an interactive linux shell that can exucute common UNIX commands and some custom commands. 

The shell supports background execution of commands with an ampersand (&) at the end of the commandline.

We implemented an history function.
_history_ command shows the last 10 commands.
_!!_ command shows the last command,
_!n_ command shows the last nth command.

There is also I/O redirection function.
A sample terminal line for I/O redirection:
> program arg1 arg2 > outputfile

For the I/O redirection if the redirection character is >, the output file is created if it does not exist and truncated if it does. 
If the redirection symbol is >> then the output file is created if it does not exist and appended if it does.

There is a custom command called birdakika.
This command schedules a song specified by the user to play every day at a time specified by the user. It is implemented utilizing crontab.
Sample use of the command:
> birdakika 7.15 song_name.wav

There is another custom bd command.
It wishes you a happy birthday if your birthday is today.
Sample use of the command:
> bd 29 04
