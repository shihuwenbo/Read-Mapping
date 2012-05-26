#include "misc.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[])
{
    if(argc != 4)
    {
        printf("Usage: [convert] [FILE NAME] [NUM] [OPTION]\n");
        return 0;
    }
    else
    {
        char* ptr;
        size_t num = (size_t) strtol(argv[2], &ptr, 10);
        if(strcmp(argv[3], "rev3") == 0)
        {
            char *src, *dst;
            read_file(argv[1], &src);
            size_t dst_size = (num * 3 - 1)/8 + 1;
            dst = (char*) malloc(dst_size*sizeof(char));
            memset(dst, 0, dst_size*sizeof(char));
            for(unsigned int i = 0; i < num; i++)
            {
                char bp = get_bp_3bit(src, num - i - 1);
                write_bp_3bit(dst, i, bp);
            }
            char rev_fn[100];
            memset(rev_fn, 0, 100);
            sprintf(rev_fn, "%s_rev", argv[1]);
            printf("%s\n", rev_fn);
            write_file(rev_fn, dst, dst_size);
            free(src);
            free(dst);
        }
        else if(strcmp(argv[3], "rev2") == 0)
        {
            char *src, *dst;
            read_file(argv[1], &src);
            size_t dst_size = (num * 2 - 1)/8 + 1;
            dst = (char*) malloc(dst_size*sizeof(char));
            memset(dst, 0, dst_size*sizeof(char));
            for(unsigned int i = 0; i < num; i++)
            {
                char bp = get_bp_2bit(src, num - i - 1);
                write_bp_2bit(dst, i, bp);
            }
            char rev_fn[100];
            memset(rev_fn, 0, 100);
            sprintf(rev_fn, "%s_rev", argv[1]);
            printf("%s\n", rev_fn);
            write_file(rev_fn, dst, dst_size);
            free(src);
            free(dst);
        }
        else if(strcmp(argv[3], "2to3") == 0)
        {
            char *src, *dst;
            read_file(argv[1], &src);
            size_t dst_size = (num * 3 - 1)/8 + 1;
            dst = (char*) malloc(dst_size*sizeof(char));
            memset(dst, 0, dst_size*sizeof(char));
            for(unsigned int i = 0; i < num; i++)
            {
                char bp = get_bp_2bit(src, i);
                write_bp_3bit(dst, i, bp);
            }
            char rev_fn[100];
            memset(rev_fn, 0, 100);
            sprintf(rev_fn, "%s_3bit", argv[1]);
            printf("%s\n", rev_fn);
            write_file(rev_fn, dst, dst_size);
            free(src);
            free(dst);
        }
        else if(strcmp(argv[3], "3to2") == 0)
        {
            char *src, *dst;
            read_file(argv[1], &src);
            size_t dst_size = (num * 2 - 1)/8 + 1;
            dst = (char*) malloc(dst_size*sizeof(char));
            memset(dst, 0, dst_size*sizeof(char));
            for(unsigned int i = 0; i < num; i++)
            {
                char bp = get_bp_3bit(src, i);
                write_bp_2bit(dst, i, bp);
            }
            char rev_fn[100];
            memset(rev_fn, 0, 100);
            sprintf(rev_fn, "%s_2bit", argv[1]);
            printf("%s\n", rev_fn);
            write_file(rev_fn, dst, dst_size);
            free(src);
            free(dst);
        }
        else if(strcmp(argv[3], "2to8") == 0)
        {
            char *src, *dst;
            read_file(argv[1], &src);
            size_t dst_size = num;
            dst = (char*) malloc(dst_size*sizeof(char));
            memset(dst, 0, dst_size*sizeof(char));
            for(unsigned int i = 0; i < num; i++)
            {
                char bp = get_bp_2bit(src, i);
                dst[i] = bp;
            }
            char rev_fn[100];
            memset(rev_fn, 0, 100);
            sprintf(rev_fn, "%s_8bit", argv[1]);
            printf("%s\n", rev_fn);
            write_file(rev_fn, dst, dst_size);
            free(src);
            free(dst);
        }
        else if(strcmp(argv[3], "3to8") == 0)
        {
            char *src, *dst;
            read_file(argv[1], &src);
            size_t dst_size = num;
            dst = (char*) malloc(dst_size*sizeof(char));
            memset(dst, 0, dst_size*sizeof(char));
            for(unsigned int i = 0; i < num; i++)
            {
                char bp = get_bp_3bit(src, i);
                printf("%c\n", bp);
                dst[i] = bp;
            }
            char rev_fn[100];
            memset(rev_fn, 0, 100);
            sprintf(rev_fn, "%s_8bit", argv[1]);
            printf("%s\n", rev_fn);
            write_file(rev_fn, dst, dst_size);
            free(src);
            free(dst);
        }
        else
        {
            printf("Invalid option\n");
            return 0;
        }
    }
}
