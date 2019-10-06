#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h> // open() syscall 사용
#define  BUFF_SIZE   1024

void main(void) {
    int server_socket;
    int client_socket;
    int client_addr_size;

    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;

    char buff[BUFF_SIZE];
    char file_name[BUFF_SIZE]; // 파일 이름용

    memset(&server_addr, 0, sizeof(server_addr)); // 0으로 초기화
    server_addr.sin_family = AF_INET; // AF_INET은 주소 체계에서 IPv4를 의미
    server_addr.sin_port = htons(4000); // htons 함수를 통해서 Little Endian일 경우 Big Endian으로 변경하여 포트 번호 대입
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // htonl 함수를 통해서 Little Endian일 경우 Big Endian으로 변경하여 INADDR_ANY로 설정하여 모든 IP의 요청을 받음

    server_socket = socket(PF_INET, SOCK_STREAM, 0); // PF_INET은 프로토콜 체계에서 IPv4, SOCK_STREAM은 TCP를 의미

    if (-1 == server_socket) {
        printf("server socket 생성 실패\n");
        exit(1);
    }

    if (-1 == bind(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr))) {
        printf("bind() 실행 에러\n");
        exit(1);
    }

    if (-1 == listen(server_socket, 5)) { // backlog를 5로 설정하여 큐의 길이 할당
        printf("listen() 실행 실패\n");
        exit(1);
    }

    while (1) { // 클라이언트의 요청을 반복적으로 받기 위한 Outer 루프
                // Outer 루프가 없는 경우 1회만 통신이 됨
       
	memset(buff, 0, sizeof(buff)); // 0으로 초기화

        memset(&client_addr, 0, sizeof(client_addr)); // 0으로 초기화
        client_addr_size = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &client_addr_size); // 클라이언트의 요청을 받아들임

        if (-1 == client_socket) { // accept 함수가 -1을 리턴한 경우 클라이언트와의 연결 실패
            printf("클라이언트 연결 수락 실패\n");
            close(server_socket);
            exit(1);
        }
	
	read(client_socket, buff, BUFF_SIZE); // 클라이언트 소켓으로부터 파일 이름을 먼저 읽어옴
	strcpy(file_name, buff); // 버퍼에 읽어들인 파일 이름을 따로 저장

	int fd; // 파일 디스크립터 생성
	fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, 0644); // 받아온 파일 이름으로 파일 생성. 모드는 쓰기 전용이고 해당 파일 이름이 이미 있는 경우 내용을 지우고 크기를 0으로 함
	if(!fd) { // 파일 디스크립터 오류 처리
	    printf("파일을 생성하지 못했습니다.\n");
	    exit(0);
	}
	
	while(1) { // 반복문을 통해 파일 내용을 받아옴
	    memset(buff, 0, BUFF_SIZE); // 초기화
	    if(read(client_socket, buff, BUFF_SIZE) == 0){ // 파일 내용을 클라이언트 소켓에서 버퍼로 읽어들임. 성공 시에 read()는 0 반환하고 반복문 나감
		printf("finish file\n");
		break;
	    }
	    write(fd, buff, BUFF_SIZE); // 버퍼 내용을 새로 만든 파일 디스크립터를 통해 적어넣음
	}
	close(client_socket); // 클라이언드 소켓 닫기
	close(fd); // 파일 닫기
	break; //연결을 위한  반복문 나감
    }
    close(server_socket); // 서버 소켓 닫기
}
