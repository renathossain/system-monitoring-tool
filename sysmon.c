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

int is_number(char *number) {
	int result = 1;
	while(*number != '\0') {
		if(*number < 48 || *number > 57) result = 0;
		number++;
	}
	return result;
}

void display_title(int no_of_samples, int delay, int sequential, int sample_no) {
	if(sequential == 0) printf("Nbr of samples: %d -- every %d secs\n", no_of_samples, delay);
	if(sequential == 1) printf(">>> iteration %d\n", sample_no);
        struct rusage *usage = (struct rusage *)malloc(sizeof(struct rusage));
        getrusage(RUSAGE_SELF, usage);
        printf(" Memory usage: %ld kilobytes\n", usage -> ru_maxrss);
        free(usage);
	print_line();
}

void display_memory(int no_of_samples, int sample_no) {
	struct sysinfo *info = (struct sysinfo *)malloc(sizeof(struct sysinfo));
        sysinfo(info);
        float totalram = ((float)(info -> totalram) / (info -> mem_unit)) / 1073741824;
        float freeram = ((float)(info -> freeram) / (info -> mem_unit)) / 1073741824;
        float totalswap = ((float)(info -> totalswap) / (info -> mem_unit)) / 1073741824;
        float freeswap = ((float)(info -> freeswap) / (info -> mem_unit)) / 1073741824;
        float usedram = totalram - freeram;
        float usedswap = totalswap - freeswap;
        printf("### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)\n");
	printf("\e[%dB", sample_no);
	printf("%.2f GB / %.2f GB -- %.2f GB / %.2f GB\n", usedram, totalram, usedswap, totalswap);
	printf("\e[%dB", no_of_samples - sample_no - 1);
	free(info);
	print_line();
}

void display_session() {
	printf("### Sessions/users ###\n");
	setutent();
	struct utmp *users = getutent();
	while(users != NULL) {
   		printf(" %-16s %s (%s)\n", users -> ut_user, users -> ut_line, users -> ut_host);
    		users = getutent();
  	}
	free(users);
	print_line();
}

void display_no_of_cores() {
	printf("Number of cores: %ld\n", sysconf(_SC_NPROCESSORS_ONLN));
	printf(" total cpu use = \n");
	print_line();
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
	print_line();
}

void display(int no_of_samples, int delay, int mode, int sequential) {
	printf("\e[1;1H\e[2J"); // Clear screen
	for(int i = 0; i < no_of_samples; i++) {
		if(sequential == 0) printf("\e[1;1H");
		display_title(no_of_samples, delay, sequential, i);
		if(mode != 2) display_memory(no_of_samples, i);
		if(mode != 1) display_session();
		if(mode != 2) display_no_of_cores();
		sleep(delay);
	}
	display_sysinfo();
}

int main(int argc, char **argv) {
	int mode = 0; //0 - default, 1 - system, 2 - user
	int graphics = 0; //0 - no graphics (default), 1 - use graphics
	int sequential = 0; //0 - not sequential (default), 1 - sequential
	int no_of_samples = 10; //default value
	int delay = 1; //default value
	int samples_changed = 0; // prevent multiple of the same arguments
	int tdelay_changed = 0; // prevent multiple of the same arguments

	if (2 <= argc && argc <= 6) {
		for(int i = 1; i < argc; i++) {
			if (strcmp(argv[i], "--system") == 0 && mode == 0) {
				mode = 1;
			} else if (strcmp(argv[i], "--user") == 0 && mode == 0) {
				mode = 2;
			} else if (strcmp(argv[i], "--graphics") == 0 && graphics == 0) {
				graphics = 1;
			}  else if (strcmp(argv[i], "--sequential") == 0 && sequential == 0) {
                        	sequential = 1;
                	}  else if (strncmp(argv[i], "--samples=", 10) == 0 &&
					is_number(argv[i] + 10) == 1 &&
					samples_changed == 0) {
				if(*(argv[i] + 10) != '\0') no_of_samples = atoi(argv[i] + 10);
				samples_changed = 1;
                	}  else if (strncmp(argv[i], "--tdelay=", 9) == 0 &&
					is_number(argv[i] + 9) == 1 &&
					tdelay_changed == 0) {
				if(*(argv[i] + 9) != '\0') delay = atoi(argv[i] + 9);
				tdelay_changed = 1;
                	} else {
				fprintf(stderr, "Invalid or duplicate argument(s).\n");
				return 1;
			}
		}
	} else if (argc <= 0 || 7 <= argc) {
		fprintf(stderr, "Invalid or duplicate argument(s).\n");
                return 1;
	}

	display(no_of_samples, delay, mode, sequential);
	return 0;
}
