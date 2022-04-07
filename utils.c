/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sorkim <sorkim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/31 13:35:00 by sorkim            #+#    #+#             */
/*   Updated: 2022/04/07 15:43:51 by sorkim           ###   ########.fr       */
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

/* Function that will look for the path line inside the environment, will
 split and test each command path and then return the right one. */
 //cmd =="ls"
char	*find_path(char *cmd, char **envp)
{
	char	**paths;
	char	*path;
	int		i;
	char	*to_absolute_path;

	i = 0;
	//env 명령에서 PATH있는 줄 찾음
	while (ft_strnstr(envp[i], "PATH", 4) == 0)
		i++;
	//PATH=  없애기 위해 5 더한 위치의 주소, 콜론 기준으로 split
	paths = ft_split(envp[i] + 5, ':');
	i = 0;
	//모든 경로에 대해서 /ls 붙여서 access f_ok옵션으로 파일 존재하는 지 찾고 있으면 반환
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

/* A simple error displaying function. */
void	error(void)
{
	perror("Error");
	exit(EXIT_FAILURE);
}

/* Function that take the command and send it to find_path
 before executing it. */
 //argv = "ls -l"
 //명령어와 시스템 환경 변수 목록을 받는 execute 함수
void	execute(char *argv, char **envp)
{
	char	**cmd;
	int 	i;
	char	*path;
	
	i = -1;
	//cmd = {"ls", "-l"}
	cmd = ft_split(argv, ' ');
	//명령어로 받은 인자를 공백 기준으로 split
	if (access(cmd[0], F_OK) == 0)
	//파일 권한 체크 함수
	//절대 경로로 받은 명령어의 경우 파일 존재 유무를 체크하고 성공하면 
	{
		//명령어(실행파일)와 옵션의 실행 코드를 현재 프로세스에 적재하여 기존의 실행코드와 교체하여 새로운 기능으로 실행.
		//즉 현재 실행되는 프로그램의 기능은 없어지고 cmd[0]의 파이너리 실행 파일을 메모리에 로딩하여 처음부터 실행함
		//env에서 PATH 환경에 등록된 환경변수를 내부적으로 이용해 프로그램을 실행
		if (execve(cmd[0], cmd, envp) == -1)
			error();
	}
	//상대경로로 받은 명령어를 모든 PATH 경로에 붙였을 때 파일 유무 확인
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




// /* Function that will read input from the terminal and return line. */
// int	get_next_line(char **line)
// {
// 	char	*buffer;
// 	int		i;
// 	int		r;
// 	char	ch;

// 	i = 0;
// 	r = 0;
// 	buffer = (char *)malloc(10000);
// 	if (!buffer)
// 		return (-1);
// 	r = read(0, &ch, 1);
// 	while (r && ch != '\n' && ch != '\0')
// 	{
// 		if (ch != '\n' && ch != '\0')
// 			buffer[i] = ch;
// 		i++;
// 		r = read(0, &ch, 1);
// 	}
// 	buffer[i] = '\n';
// 	buffer[++i] = '\0';
// 	*line = buffer;
// 	free(buffer);
// 	return (r);
// }