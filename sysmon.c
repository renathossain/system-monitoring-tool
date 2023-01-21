#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/resource.h>
#include<sys/utsname.h>
#include<sys/sysinfo.h>
#include<sys/types.h>
#include<utmp.h>
#include<unistd.h>

/*
Nbr of samples: 10 -- every 1 secs
 Memory usage: 4092 kilobytes
---------------------------------------
### Memory ### (Phys.Used/Tot -- Virtual Used/Tot) 
9.78 GB / 15.37 GB  -- 9.78 GB / 16.33 GB
9.77 GB / 15.37 GB  -- 9.77 GB / 16.33 GB
9.77 GB / 15.37 GB  -- 9.77 GB / 16.33 GB
9.77 GB / 15.37 GB  -- 9.77 GB / 16.33 GB
9.77 GB / 15.37 GB  -- 9.77 GB / 16.33 GB
9.77 GB / 15.37 GB  -- 9.77 GB / 16.33 GB
9.77 GB / 15.37 GB  -- 9.77 GB / 16.33 GB
9.77 GB / 15.37 GB  -- 9.77 GB / 16.33 GB
9.77 GB / 15.37 GB  -- 9.77 GB / 16.33 GB
9.77 GB / 15.37 GB  -- 9.77 GB / 16.33 GB
---------------------------------------
### Sessions/users ### 
 marcelo       pts/0 (138.51.12.217)
 marcelo       pts/1 (tmux(3773782).%0)
 alberto       tty7 (:0)
 marcelo       pts/2 (tmux(3773782).%1)
 marcelo       pts/3 (tmux(3773782).%3)
 marcelo       pts/4 (tmux(3773782).%4)
---------------------------------------
Number of cores: 4 
 total cpu use = 0.00%
---------------------------------------
### System Information ### 
 System Name = Linux
 Machine Name = iits-b473-01
 Version = #99-Ubuntu SMP Thu Sep 23 17:29:00 UTC 2021
 Release = 5.4.0-88-generic
 Architecture = x86_64
---------------------------------------
*/

void print_line() {
	printf("---------------------------------------\n");
}

void display(int no_of_samples, int delay) {
	printf("Nbr of samples: %d -- every %d secs\n", no_of_samples, delay);
	printf(" Memory usage: 4092 kilobytes\n");
	print_line();
	print_line();
	print_line();
	
	print_line();
	
	struct sysinfo *info = (struct sysinfo *)malloc(sizeof(struct sysinfo));
	sysinfo(info);
	printf("%lu\n", info -> totalram);
	
	print_line();
}

int main(int argc, char **argv) {
	if (argc == 1) {
		display(10, 1);
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
