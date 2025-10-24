#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

int	count_cmd(char **cmds[])
{
	int	i;

	i = 0;
	while (cmds[i])
		i++;
	return (i);
}

int	picoshell(char **cmds[])
{
	int	i;
	int	num;
	int	fd[2];
	pid_t	pid;
	int	status;
	int	in_fd;
	int	ret;

	num = count_cmd(cmds);
	ret = 0;
	in_fd = 0;
	i = 0;
	while (cmds[i])
	{
		if (i < num - 1)
		{
			if (pipe(fd) == -1)
			{
				if (in_fd != 0)
					close(in_fd);
				while (wait(&status) > 0)
					;
				return (1);
			}
		}
		pid = fork();
		if (pid == -1)
		{
			if (i < num - 1)
			{
				close(fd[0]);
				close(fd[1]);
			}
			if (in_fd != 0)
				close(in_fd);
			while (wait(&status) > 0)
				;
			return (1);
		}
		else if (!pid)
		{
			if (i < num - 1)
			{
				if (dup2(fd[1], 1) == -1)
					exit (1);
				close(fd[1]);
				close(fd[0]);
			}
			if (i > 0)
			{
				if (dup2(in_fd, 0) == -1)
					exit (1);
				close(in_fd);
			}
			if (execvp(cmds[i][0], cmds[i]) == -1)
				exit (1);
		}
		if (i > 0)
			close(in_fd);
		if (i < num - 1)
		{	
			in_fd = fd[0];
			close(fd[1]);
		}
		i++;
	}
	while (wait(&status) > 0)
	{
		if (WIFEXITED(status) && WEXITSTATUS(status))
			ret = 1;
		else if (!WIFEXITED(status))
			ret = 1;
	}
	return (ret);
}

int	main(void)
{
	char	*cmd1[] = {"ls", "-l", NULL};
	char	*cmd2[] = {"wc", "-l", NULL};
	char	**cmds[] = {cmd1, cmd2, NULL};
	int	ret;

	ret = picoshell(cmds);
	printf("%d\n", ret);
	return (0);
}
