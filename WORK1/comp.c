// Names + Id: 
// Adir Edri 206991762
// Ido Gutman 325314698

#include <fcntl.h>
#include <stdio.h>

int main(int argc,char * argv[]){
  int fd1 = open (argv[1], O_RDONLY);
  int fd2 = open (argv[2], O_RDONLY);
  if(argc != 3){
    printf("please enter two files.\n");
    close(fd1);
  close(fd2);
    return -1;
  }
  if (fd1 == -1 || fd2 == -1){
    printf("error opening one of the files.\n");
    close(fd1);
  close(fd2);
    return -1;
  }
  char buffer_1[4096];
  char buffer_2[4096];
  int bytesize_fd1 = read(fd1,buffer_1,sizeof(buffer_1));
  int bytesize_fd2 = read(fd2,buffer_2,sizeof(buffer_2));
    if (bytesize_fd1 == 0 || bytesize_fd1 == -1 || bytesize_fd2 == 0 | bytesize_fd2 == -1){
        printf("error reading files.\n");
        close(fd1);
  close(fd2);
        return -1;
    }
    if(bytesize_fd1 != bytesize_fd2)
    return 1;
    else {for(int i = 0; i< bytesize_fd1;i++){
        if (buffer_1[i] !=buffer_2[i])
        return 1;
    }
  }
  close(fd1);
  close(fd2);
    return 2;
}
