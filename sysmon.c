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

void display_memory(int no_of_samples, int delay) {
	printf("Nbr of samples: %d -- every %d secs\n", no_of_samples, delay);
	struct rusage *usage = (struct rusage *)malloc(sizeof(struct rusage));
	getrusage(RUSAGE_SELF, usage);
        printf(" Memory usage: %ld kilobytes\n", usage -> ru_maxrss);
	free(usage);
	print_line();
	printf("### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)\n");
	struct sysinfo *info = (struct sysinfo *)malloc(sizeof(struct sysinfo));
	sysinfo(info);
	for(int i = 0; i < no_of_samples; i++) {
		float totalram = ((float)info -> totalram / info -> mem_unit) / 1073741824;
		float freeram = ((float)info -> freeram / info -> mem_unit) / 1073741824;
    		float totalswap = ((float)info -> totalswap / info -> mem_unit) / 1073741824;
    		float freeswap = ((float)info -> freeswap / info -> mem_unit) / 1073741824;
		float usedram = totalram - freeram;
		float usedswap = totalswap - freeswap;
		printf("%.2f GB / %.2f GB  -- %.2f GB / %.2f GB\n", usedram, totalram, usedswap, totalswap);
		sleep((unsigned int)delay);
	}
	free(info);
}

void display_session() {
	printf("### Sessions/users ###\n");
	setutent();
	struct utmp *users = getutent();
	while(users != NULL) {
   		printf(" %s       %s (%s)\n", users -> ut_user, users -> ut_line, users -> ut_host);
    		users = getutent();
  	}
	free(users);
}

void display_no_of_cores() {
	printf("Number of cores: %ld\n", sysconf(_SC_NPROCESSORS_ONLN));
	printf(" total cpu use = \n");
}

void display_sysinfo() {
	struct utsname *buf = (struct utsname *)malloc(sizeof(struct utsname));
        uname(buf);
        printf("### System Information ###\n");
	printf("System Name = %s\n", buf -> sysname);
        printf("Machine Name = %s\n", buf -> nodename);
        printf("Version Name = %s\n", buf -> version);
        printf("Release = %s\n", buf -> release);
        printf("Architecture = %s\n", buf -> machine);
	free(buf);
}

void display(int no_of_samples, int delay) {
	display_memory(no_of_samples, delay);
	print_line();
	display_session();
	print_line();
	display_no_of_cores();
	print_line();
	display_sysinfo();
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
