/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sorkim <sorkim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/31 10:54:17 by sorkim            #+#    #+#             */
/*   Updated: 2022/04/07 16:01:49 by sorkim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void	child_process(char **argv, char **envp, int *fd)
{
	int		infile;

	infile = open(argv[1], O_RDONLY, 0777); //입력대신 읽어 올 파일을 open
	if (infile == -1) //파일 없으면 err
		error();
	//동일한 파일(pipe)에 대해 참조하는 fd로 표준 입출력 변경
	dup2(fd[1], STDOUT_FILENO); //표준 출력을 file descriptor가 가리키는 파일로 변경 
	//모니터에 출력될 내용이 fd[1]이 가리키는 파일로 출력 됨
	dup2(infile, STDIN_FILENO); //표준 입력을 infile로 변경
	//키보드로 입력될 내용이 infile로부터 읽어오도록 변경됨
	close(fd[0]);
	//읽기용 file descriptor은 사용하지 않으므로 close
	
	//argv[2] == "ls -l"
	execute(argv[2], envp);
	//자식프로세스의 명령어로서 받은 인자와 이 명령어를 실행시킬 env(시스템 환경 변수)의 목록을 인자로 보내
	//execute함수를 실행
}

void	parent_process(char **argv, char **envp, int *fd)
{
	int		outfile;

	//출력할 파일에 대해서 open //없으면 생성하고 기존에 있는 파일이면 내용을 새로 작성하기 위해 지움
	outfile = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, 0777);
	if (outfile == -1) //open error
		error();
	//동일한 파일(pipe)에 대해 참조하는 fd로 표준 입출력 변경
	dup2(fd[0], STDIN_FILENO); //표준 입력을 file descriptor가 가리키는 파일로 변경 
	//키보드가 입력할 내용이 fd[0]이 가리키는 파일을 통해 입력됨
	dup2(outfile, STDOUT_FILENO); //표준 출력을 outfile로 변경
	//모니터에 출력할 내용이 outfile로 출력됨 
	close(fd[1]);
	//쓰기용 file descriptor은 사용하지 않으므로 close
	execute(argv[3], envp);
	//부모프로세스의 명령어로서 받은 인자와 이 명령어를 실행시킬 env(시스템 환경 변수)의 목록을 인자로 보내
	//execute함수를 실행
}

//./pipex inputfile "ls -l" "wc -l" "outputfile"
// < inputfile {cmd1} {cmd2} > outputfile
//envp 는 env 명령 실행했을때 한줄씩 나누어서 받음
int	main(int argc, char **argv, char **envp)
{
	int		fd[2];
	pid_t	pid1;

	if (argc == 5)
	{
		//프로세스 간의 통신을 하기 위해서는 데이터를 주고 받기 위한 특정공간 필요
		//하지만 프로세스는 메모리 상에 각자 독립된 공간을 할당받아 위치하며 서로 공유하기 어려움
		//프로세스 통신 방법 - 공유 메모리 기법 / 메세지 교환 기법
		//메세지 교환 기법 중에서도 fork를 이용해 자식과 부모 프로세스 간에 통신하는데 이용하는 방식이 파이프
		//파이프는 파일 디스크립터를 이용해 자식과 부모간 데이터를 주고 받을 수 있음
		//파이프는 단방향 통신이기 때문에 프로세스 간 파이프를 개설하면 한쪽에서는 read 작업만 가능하고 한쪽에서는 write 작업만 가능.
		//파일 디스크립터를 이용한 통신이 가능한건 파일 디스크립터가 참조하는 파일 자체는 디스크 상의 별도의 공간으로 유지되면서 
		//프로세스의 메모리 공간을 침해하지 않고, 이들은 어떤 프로세스에서도 접근이 가능하기 때문이다.
		//즉, 각각의 프로세스에서 사용하고 있는 read 용도의 파일 디스크립터와 write용도의 파일 디스크립터가 동일한 파일을 참조하도록 되어있다면,
		//해당 파일 디스크립터를 읽고 쓰면서 통신이 가능한 것
		if (pipe(fd) == -1)
			error();
		pid1 = fork();
		//자식프로세스 생성 
		//자식프로세스 -> 0반환
		//실패 -> -1반환
		//부모프로세스(원본 프로세스) -> 자식 pid반환
		if (pid1 == -1)
			error();
		if (pid1 == 0)
			child_process(argv, envp, fd);
		//자식 프로세스 종료까지 기다리지 않음.
		//비동기적으로 실행
		//-> pipe 하면 파일에 쓰여지지 않는 경우 쓰여질때까지 읽기를 하지 않고 대기
		//자식의 자원을 회수하기는 해야해서 waitpid, WNOHANG 옵션을 통해 자식을 기다리지 않고 비동기적으로 수행
		waitpid(pid1, NULL, WNOHANG);
		parent_process(argv, envp, fd);
	}
	else
	{
		ft_putstr_fd("Error : argument\n", 2);
		ft_putstr_fd("argument : ./pipex <file1> <cmd1> <cmd2> <file2>\n", 1);
	}
	return (0);
}