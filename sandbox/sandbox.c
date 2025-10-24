#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

void	signal_handler(int sig)
{
	(void)sig;
}

int	sandbox(void (*f)(void), unsigned int timeout, bool verbose)
{
	struct sigaction sa;
	struct sigaction old;
	pid_t	pid;
	int	status;
	int	ret;

	pid = fork();
	if (pid == -1)
		return (-1);
	else if(!pid)
	{
		f();
		exit(0);
	}	
	sa.sa_handler = signal_handler;
        sa.sa_flags = 0;
        sigemptyset(&sa.sa_mask);
        sigaction(SIGALRM, &sa, &old);
	alarm(timeout);
	ret = waitpid(pid, &status, 0);
	alarm(0);
	sigaction(SIGALRM, &old, NULL);
	if (ret == -1)
	{
		if (errno == EINTR)
		{
			kill(pid, SIGKILL);
			waitpid(pid, NULL, 0);
			if (verbose)
				printf("Bad function: timed out after %u seconds\n", timeout);
			return (0);
		}
		return (-1);

	}
	else
	{
		if (WIFEXITED(status))
		{	
			if (WEXITSTATUS(status))
			{
				if (verbose)
					printf("Bad function: exited with code %d\n", WEXITSTATUS(status));
				return (0);
			}
			else
			{
				if (verbose)
                                        printf("Nice function!\n");
				return (1);
			}
		}
		else if (WIFSIGNALED(status))
		{
			if (verbose)
				printf("Bad function: %s\n", strsignal(WTERMSIG(status)));
			return (0);
		}
		return (-1);
	}
}

void nice_func() {
    printf("1\n");
}

void bad_exit() {
    exit(1);
}

void bad_segfault() {
    int *p = NULL;
    *p = 42;
}

void bad_timeout() {
    while(1) {}  
}

int	main(void)
{
	int	ret;
	ret = sandbox(bad_timeout, 2, 1);
	printf("%d\n", ret);
	return (0);
}
