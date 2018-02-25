#include <sys/types.h>
  #include <termios.h>
  #include <unistd.h>
  #include <stdio.h>
  #include <stdlib.h>

  #define NOTATTY 1

  main()
  {
     int ttyDevice = STDOUT_FILENO;
     struct termios termAttributes;

        /* Make sure file descriptor is for a TTY device.         */
     if ( ! isatty(ttyDevice) )
        return(NOTATTY);

        /* Get terminal attributes and then determine if terminal */
        /* start and stop is enabled and if terminal is in        */
        /* canonical mode.                                        */
     else {
        if (tcgetattr(ttyDevice, &termAttributes) != 0)
           perror("tcgetattr error");
        else {
           if (termAttributes.c_iflag & IXON)
               printf("Terminal start and stop is enabled\n");
           if (termAttributes.c_lflag & ICANON)
               printf("Terminal is in canonical mode\n");
        }
     }
     return(EXIT_SUCCESS);
  }
