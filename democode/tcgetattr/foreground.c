/* https://www.gnu.org/software/libc/manual/html_mono/libc.html#Implementing-a-Shell */
/* under section: 28.6.4 Foreground and Background */


/* Put job j in the foreground.  If cont is nonzero,
   restore the saved terminal modes and send the process group a
   SIGCONT signal to wake it up before we block.  */


/* The foreground job may have left the terminal in a strange state, so the shell should 
restore its own saved terminal modes before continuing. In case the job is merely stopped, 
the shell should first save the current terminal modes so that it can restore them later 
if the job is continued. */


pid_t shell_pgid;
int shell_terminal = STDIN_FILENO;
struct termios shell_tmodes; /* set by tcgetattr (shell_terminal, &shell_tmodes) when init the shell*/

void
put_job_in_foreground (job *j, int cont)
{
  /* Put the job into the foreground.  */
  tcsetpgrp (shell_terminal, j->pgid);


  /* Send the job a continue signal, if necessary.  */
  if (cont)
    {
      /* restore the terminal attributes when the job stopped last time */
      tcsetattr (shell_terminal, TCSADRAIN, &j->tmodes);
      if (kill (- j->pgid, SIGCONT) < 0)
        perror ("kill (SIGCONT)");
    }


  /* Wait for it to report. Either stopped or completed */
  wait_for_job (j);

  /* Put the shell back in the foreground.  */
  tcsetpgrp (shell_terminal, shell_pgid);

  /* store the cuurent terminal status in the job */
  tcgetattr (shell_terminal, &j->tmodes); 

  /* Restore the shell’s terminal modes.  */
  tcsetattr (shell_terminal, TCSADRAIN, &shell_tmodes); 
}
