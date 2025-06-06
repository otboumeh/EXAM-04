#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

void err(char *s)
{
	while (*s)
		write(2, s++, 1);
}

int print_err(char *s, char *key)
{
	err(s);
	if (key)
		err(key);
	err("\n");
	return (1);
}

int cd(char **argv, int i)
{
	if (i != 2)
		return (print_err("error: cd: bad arguments", NULL));
	else if (chdir(argv[1]) == -1)
		return (print_err("error: cd: cannot change directory to", argv[1]));
	return (0);
}

int exc(char **argv, char **env, int i)
{
	int fd[2];
	int g_status;
	int has_pipe = argv[i] && !strcmp(argv[i], "|");

	if (has_pipe && pipe(fd) == -1)
		return (print_err("error: fatal", NULL));
	int pid = fork();
	if (pid == 0)
	{
		argv[i] = NULL;
		if (has_pipe && (dup2(fd[1], 1) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1))
			return (print_err("error: fatal", NULL));
		execve(*argv, argv, env);
		return (print_err("error: cannot execute ", *argv));
	}
	waitpid(pid, &g_status, 0);
	if (has_pipe && (dup2(fd[0], 0) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1))
		return (print_err("error: fatal", NULL));
	return (g_status);
}

int main(int argc, char **argv, char **env)
{
	int i = 0;
	int g_status = 0;

	if (argc > 1)
	{
		while (argv[i] && argv[++i])
		{
			argv += i;
			i = 0;
			while (argv[i] && strcmp(argv[i], "|") && strcmp(argv[i], ";"))
				i++;
			if (!strcmp(*argv, "cd"))
				g_status = cd(argv, i);
			else if (i)
				g_status = exc(argv, env, i);
		}
	}
	return (g_status);
}
