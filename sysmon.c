#include<stdlib.h>
#include<stdio.h>
#include<string.h>

int main(int argc, char **argv) {
	if (argc == 1) {
		printf("1 argument\n");
	} else if (argc == 2) {
		printf("2 arguments\n");
		if (strcmp(argv[1], "--system") == 0) {
			printf("--system\n");
		} else if (strcmp(argv[1], "--user") == 0) {
			printf("--user\n");
		} else if (strcmp(argv[1], "--graphics") == 0) {
			printf("--graphics\n");
		}  else if (strcmp(argv[1], "--sequential") == 0) {
                        printf("--sequential\n");
                }  else if (strcmp(argv[1], "--samples=N") == 0) {
                        printf("--samples=N\n");
                }  else if (strcmp(argv[1], "--tdelay=T") == 0) {
                        printf("--tdelay=T\n");
                } else {
			printf("Invalid argument\n");
		}
	}
	return 0;
}
