// #include <fcntl.h>
// #include <unistd.h>



// // int main() {
     
// //      char buffer[100];
// //      // // if want read from input
// //      // int n = read(0, buffer, 100);
// //      // while (n > 0)
// //      // {
// //      //      write(1, buffer, n);
// //      //      n = read(0, buffer, 100);
// //      //      if (n == -1) {
// //      //           write(2, "Error\n", 6);
// //      //           return (1);
// //      //      }
// //      // }

// //      // if want read from file
// //     int fd = open("file.txt", O_RDONLY);
// //      if (fd < 0) {
// //           write(2, "Error\n", 6);
// //           return (1);
// //      }
// //      int n1;
// //      while ((n1 = read(fd, buffer, 100)) > 0)
// //           write(1, buffer, n1);
// //      if (n1 < 0) {
// //           write(2, "Error\n", 6);
// //           close(fd);
// //           return (1);
// //      }

// //      close(fd);
// //      return (0);

// // }




// #include <stdio.h>
// #include <string.h>

// int main(int argc, char **argv) {
//      char buffer[100];
//      int n;

//      if (argc == 2) {
//           int fd = open(argv[1], O_RDONLY);
//           while ((n = read(fd, buffer, 100)) > 0) {
//                write(1, buffer, n);
//                if (n < 0) {
//                     write(2, "Error\n", 6);
//                     close(fd);
//                     return (1);
//                }
//           }
//      }
//      else {
//           while ((n = read(0, buffer, 100)) > 0) {
//                write(1, buffer, n);
//                if (n < 0) {
//                     write(2, "Error\n", 6);
//                     return (1);
//                }
//           }
//      }
//      return (0);
// }




#include <pthread.h>
#include <stdio.h>

void *routine(void *n) {
     int *x = (int *)n;
     (*x)++;
     printf("coder %d is working\n", *x);
     return NULL;
}

int main() {
     pthread_t coder1, coder2;
     int x = 0;
     pthread_create(&coder1, NULL, routine, (void *)&x);
     pthread_create(&coder2, NULL, routine, (void *)&x);

     pthread_join(coder1, NULL);
     pthread_join(coder2, NULL);


     return (0);
}
