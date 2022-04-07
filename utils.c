/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sorkim <sorkim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/28 13:35:00 by sorkim            #+#    #+#             */
/*   Updated: 2022/04/07 16:48:16 by sorkim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

static char	*ft_strnstr(const char	*big, const char *little, size_t len)
{
	size_t	i;
	size_t	j;

	i = 0;
	if (little[i] == '\0')
		return ((char *)big);
	while (big[i] && i < len)
	{
		j = 0;
		while (big[i + j] == little[j] && i + j < len)
		{
			if (little[j + 1] == '\0')
				return ((char *)big + i);
			j++;
		}
		i++;
	}
	return (0);
}

char	*is_exist_path(char *cmd, char **envp)
{
	char	**paths;
	char	*path;
	int		i;
	char	*to_absolute_path;

	i = 0;
	while (ft_strnstr(envp[i], "PATH", 4) == 0)
		i++;
	paths = ft_split(envp[i] + 5, ':');
	i = 0;
	while (paths[i])
	{
		to_absolute_path = ft_strjoin(paths[i], "/");
		path = ft_strjoin(to_absolute_path, cmd);
		free(to_absolute_path);
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

void	handling_error(void)
{
	perror("Error");
	exit(1);
}

void	execute_cmd(char *argv, char **envp)
{
	int		i;
	char	**cmd;
	char	*path;

	i = -1;
	cmd = ft_split(argv, ' ');
	if (access(cmd[0], F_OK) == 0)
	{
		if (execve(cmd[0], cmd, envp) == -1)
			handling_error();
	}
	path = is_exist_path(cmd[0], envp);
	if (!path)
	{
		while (cmd[++i])
			free(cmd[i]);
		free(cmd);
		handling_error();
	}
	if (execve(path, cmd, envp) == -1)
		handling_error();
}
