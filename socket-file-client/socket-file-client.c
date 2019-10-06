#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h> // open() syscall 사용
#define BUFF_SIZE 1024

void main(int argc, char **argv) {
    int client_socket;
    struct sockaddr_in server_addr;

    char buff[BUFF_SIZE];
    char file_name[BUFF_SIZE]; // 파일 이름용
    int file_read_len; // 파일 길이 확인

    if (argv[1] == NULL) {
        printf("메세지 입력 필요\n");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr)); // 0으로 초기화
    server_addr.sin_family = AF_INET; // AF_INET은 주소 체계에서 IPv4를 의미
    server_addr.sin_port = htons(4000); // htons 함수를 통해서 Little Endian일 경우 Big Endian으로 변경하여 포트 번호 대입
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // 32bit IPv4 주소로 초기화

    client_socket = socket(PF_INET, SOCK_STREAM, 0); // PF_INET은 프로토콜 체계에서 IPv4, SOCK_STREAM은 TCP를 의미

    if (-1 == client_socket) {
        printf("socket 생성 실패\n");
        exit(1);
    }

    if (-1 == connect(client_socket, (struct sockaddr *) &server_addr, sizeof(server_addr))) { // 연결 요청
        printf("접속 실패\n");
        exit(1);
    }

    write(client_socket, argv[1], strlen(argv[1]) + 1); // 파일 이름을 소켓에 write
    strcpy(file_name,argv[1]); // 파일 이름을 복사해서 저장
    
    int fd; // 파일 디스크립터 생성
    fd = open(file_name, O_RDONLY); // 저장한 파일 이름의 파일을 읽기 전용으로 열기

    if(fd == -1) { // 파일 열기 오류 처리
        printf("파일을 열지 못했습니다.\n");
        exit(1);
    }

    while(1) { // 반복문을 통해 파일 내용 보내기 
	memset(buff, 0, BUFF_SIZE); // 초기화
	file_read_len = read(fd, buff, BUFF_SIZE); // 파일 내용을 버퍼에 옮겨 적음
	if (write(client_socket, buff, BUFF_SIZE) == -1){ // 버퍼에 적은 파일 내용을 소켓으로 옮김. write() 함수 에러 처리
	    printf("파일을 쓰지 못했습니다.\n");
	    exit(1);
	}
	if(file_read_len == EOF | file_read_len == 0) { // 파일의 끝까지 읽었는지 확인 후 반복문 나감
	    printf("finish file\n");
    	    break;
	}	
    }
    close(client_socket); // 소켓 닫기
    close(fd); // 파일 닫기
 }
