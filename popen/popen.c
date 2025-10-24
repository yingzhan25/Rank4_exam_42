#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int	ft_popen(const char *file, char *const argv[], char type)
{
	int	fd[2];
	pid_t	pid;

	if (!file || !argv || (type != 'w' && type != 'r'))
		return (-1);
	if (pipe(fd) == -1)
		return (-1);
	pid = fork();
	if (pid == -1)
	{
		close(fd[0]);
		close(fd[1]);
		return (-1);
	}
	else if (!pid)
	{
		if (type == 'w')
		{
			if (dup2(fd[0], 0) == -1)
			{
				close(fd[0]);
				close(fd[1]);
				exit(-1);
			}
		}
		else
		{
			if (dup2(fd[1], 1) == -1)
			{
				close(fd[0]);
				close(fd[1]);
				exit(-1);
			}
		}
		close(fd[0]);
		close(fd[1]);
		execvp(file, argv);
		exit(-1);
	}
	if (type == 'w')
	{
		close(fd[0]);
		return (fd[1]);
	}
	else
	{
		close(fd[1]);
		return (fd[0]);
	}
		
}

int	main(void)
{
	int	fd;
	int	bytes_read;
	char	buf[1024];

	fd = ft_popen("ls", (char *const[]){"ls", NULL}, 'r');
	if (fd != -1)
	{
		bytes_read = read(fd, buf, sizeof(buf) - 1);
		while (bytes_read > 0)
		{
			buf[bytes_read] = '\0';
			printf("%s", buf);
			bytes_read = read(fd, buf, sizeof(buf) - 1);	
		}
		close(fd);
	}
	return (0);
}
