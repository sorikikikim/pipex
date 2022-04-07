/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sorkim <sorkim@student.42seoul.kr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/02 09:54:02 by sorkim            #+#    #+#             */
/*   Updated: 2022/04/07 13:05:16 by sorkim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

/* Function that will look for the path line inside the environment, will
 split and test each command path and then return the right one. */
 //cmd =="ls"
char	*find_path(char *cmd, char **envp)
{
	char	**paths;
	char	*path;
	int		i;
	char	*part_path;

	i = 0;
	//env 명령에서 PATH있는 줄 찾음
	while (ft_strnstr(envp[i], "PATH", 4) == 0)
		i++;
	//PATH=  없애기 위해 5 더하고 콜론 기준으로 split
	paths = ft_split(envp[i] + 5, ':');
	i = 0;
	//모든 경로에 대해서 /ls 붙여서 access f_ok옵션으로 파일 존재하는 지 찾고 있으면 반환
	while (paths[i])
	{
		part_path = ft_strjoin(paths[i], "/");
		path = ft_strjoin(part_path, cmd);
		free(part_path);
		if (access(path, F_OK) == 0)
			return (path);
		free(path);
		i++;
	}
	i = -1;
	while (paths[++i])
		free(paths[i]);
	free(paths);
	return (0);
}

/* A simple error displaying function. */
void	error(void)
{
	perror("Error");
	exit(EXIT_FAILURE);
}

/* Function that take the command and send it to find_path
 before executing it. */
 //argv = "ls -l"
void	execute(char *argv, char **envp)
{
	char	**cmd;
	int 	i;
	char	*path;
	
	i = -1;
	//cmd = {"ls", "-l"}
	cmd = ft_split(argv, ' ');
	if (access(cmd[0], F_OK) == 0)
	{
		if (execve(cmd[0], cmd, envp) == -1)
			error();
	}
	//ls 모든 PATH경로 에 붙였을 때 파일있는지 찾음 없으면 0 반환
	path = find_path(cmd[0], envp);
	if (!path)	
	{
		while (cmd[++i])
			free(cmd[i]);
		free(cmd);
		error();
	}
	//path = "/bin/ls" , cmd = {"ls", "-l"}, envp는 env 했을 때 결과 /bin/ls 바이너리 파일을 이 프로세스에 덮어 씌움
	if (execve(path, cmd, envp) == -1)
		error();
}

/* Function that will read input from the terminal and return line. */
int	get_next_line(char **line)
{
	char	*buffer;
	int		i;
	int		r;
	char	ch;

	i = 0;
	r = 0;
	buffer = (char *)malloc(10000);
	if (!buffer)
		return (-1);
	r = read(0, &ch, 1);
	while (r && ch != '\n' && ch != '\0')
	{
		if (ch != '\n' && ch != '\0')
			buffer[i] = ch;
		i++;
		r = read(0, &ch, 1);
	}
	buffer[i] = '\n';
	buffer[++i] = '\0';
	*line = buffer;
	free(buffer);
	return (r);
}