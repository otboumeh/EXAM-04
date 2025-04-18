/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   micro.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: otboumeh <otboumeh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/14 17:36:23 by dangonz3          #+#    #+#             */
/*   Updated: 2025/02/21 16:26:02 by otboumeh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

void	ft_error(char *str1, char *str2)
{
	while (*str1)
		write(2, str1++, 1);
	if (str2)
		while (*str2)
			write(2, str2++, 1);
	write(2, "\n", 1);	
}

void	ft_execute(char **argv, int	i, int tmp_fd, char **env)
{
	argv[i] = NULL;
	dup2(tmp_fd, STDIN_FILENO);
	close(tmp_fd);
	execve(argv[0], argv, env);
	ft_error("Cannot execve: ", argv[0]);
	exit(1);
}

int	main(int argc, char **argv, char **env)
{
	int	i = 0;
	int	fd[2];
	int	tmp_fd = dup(STDIN_FILENO);
	(void)argc;

	while (argv[i] && argv[i + 1])
	{
		argv = &argv[i + 1];
		i = 0;
		while(argv[i] && strcmp(argv[i], "|") && strcmp(argv[i], ";"))
			i++;
		if (!strcmp(argv[0], "cd"))
		{
			if (i != 2)
				ft_error("Incorrect cd arguments", NULL);
			else if (chdir(argv[1]))
				ft_error("Cannot cd to: ", argv[1]);
		}
		else if (i && (!argv[i] || !strcmp(argv[i], ";")))
		{
			if (!fork())
				ft_execute(argv, i, tmp_fd, env);
			else
			{
				close(tmp_fd);
				while (waitpid(-1, NULL, WUNTRACED) != -1)
					continue;
				tmp_fd = dup(STDIN_FILENO);				
			}
		}
		else if (i && !strcmp(argv[i], "|"))
		{
			pipe(fd);
			if (!fork())
			{
				dup2(fd[1], STDOUT_FILENO);
				close(fd[0]);
				close(fd[1]);
				ft_execute(argv, i, tmp_fd, env);
			}
			else
			{
				close(fd[1]);
				close(tmp_fd);
				tmp_fd = fd[0];				
			}			
		}
		
	}
	close(tmp_fd);
		return (0);
}
