#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
int debug = 1;  //Test
int ppos = 0;
/*int write_device()
{
        int write_length = 0;
        ssize_t ret;
        char *data = (char *)malloc(1024 * sizeof(char));

        printf("please enter the data to write into device\n");
        scanf(" %[^\n]", data); 
        write_length = strlen(data);
        if (debug)
                printf("the length of dat written = %d\n", write_length);
        ret = write(fd, data, write_length, &ppos);
        if (ret == -1)
                printf("writting failed\n");
        else
                printf("writting success\n");
        if (debug)fflush(stdout); 
        free(data);
        return 0;
}*/
int lseek_device(int fd)
{
        int lseek_offset = 0, seek_value = 0;
        int counter = 0; /* to check if function called multiple times or loop*/
        counter++;
        printf("counter value = %d\n", counter);
        printf("enter the seek offset\n");
        scanf("%d", &lseek_offset);
        if (debug)
                printf("seek_offset selected is %d\n", lseek_offset);
        printf("1 for SEEK_SET, 2 for SEEK_CUR and 3 for SEEK_END\n");
        scanf("%d", &seek_value);
        printf("seek value = %d\n", seek_value);

        switch (seek_value) {
        case 1: lseek(fd, lseek_offset, SEEK_SET);
                return 0;
                break;
        case 2: lseek(fd, lseek_offset, SEEK_CUR);
                return 0;
                break;
        case 3: lseek(fd, lseek_offset, SEEK_END);
                return 0;
                break;
        default :
                printf("unknown  option selected, please enter right one\n");
                break;
        }
        if (debug)
                fflush(stdout);/*not to miss any log*/
        return 0;
}
/*int lseek_write(int fd)
{
        lseek_device(fd);
        //write_device();
        return 0;
}*/
int main ()
{
        int i,fd;
        char ch, write_buf[100]={0}, read_buf[100];
        fd = open("/dev/char_dev", O_RDWR);
        if (fd == -1)
        {
                printf("Error in opening file \n");
                exit(-1);
        }
        //while(1)
        //{
        printf ("Press r to read from device or w to write the device ");
        scanf ("%c", &ch);

        switch (ch) {

                case 'w':
                        lseek_device(fd);
                        printf ("Enter the data to be written into device\n");
                        scanf (" %[^\n]", write_buf);
                        printf("len:%ld\n",strlen(write_buf));
                        write(fd, write_buf, strlen(write_buf));
                        //lseek_write();
                        break;

                case 'r':
                        read(fd, read_buf, sizeof(read_buf));
                        printf ("The data in the device is %s\n", read_buf);
                        break;

                default:
                        printf("Wrong choice \n");
                        break;

        }
        //}
        close(fd);
        return 0;
}
