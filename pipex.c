/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sorkim <sorkim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/27 10:54:17 by sorkim            #+#    #+#             */
/*   Updated: 2022/04/07 16:47:17 by sorkim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void	child_process(char **argv, char **envp, int *fd)
{
	int		infile;

	infile = open(argv[1], O_RDONLY, 0777);
	if (infile == -1)
		handling_error();
	dup2(fd[1], STDOUT_FILENO);
	dup2(infile, STDIN_FILENO);
	close(fd[0]);
	execute_cmd(argv[2], envp);
}

void	parent_process(char **argv, char **envp, int *fd)
{
	int		outfile;

	outfile = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, 0777);
	if (outfile == -1)
		handling_error();
	dup2(fd[0], STDIN_FILENO);
	dup2(outfile, STDOUT_FILENO);
	close(fd[1]);
	execute_cmd(argv[3], envp);
}

int	main(int argc, char **argv, char **envp)
{
	int		fd[2];
	pid_t	pid;

	if (argc == 5)
	{
		if (pipe(fd) == -1)
			handling_error();
		pid = fork();
		if (pid == -1)
			handling_error();
		if (pid == 0)
			child_process(argv, envp, fd);
		waitpid(pid, NULL, WNOHANG);
		parent_process(argv, envp, fd);
	}
	else
	{
		ft_putstr_fd("Error: argument\n", 2);
		ft_putstr_fd("argument: ./pipex <file1> <cmd1> <cmd2> <file2>\n", 1);
	}
	return (0);
}
