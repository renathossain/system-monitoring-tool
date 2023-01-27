#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/resource.h>
#include<sys/utsname.h>
#include<sys/sysinfo.h>
#include<sys/types.h>
#include<utmp.h>
#include<unistd.h>

#define STRING_LEN 256

struct info_node {
	char mem_data[STRING_LEN];
	float cpu_data;
	float used_ram;
	struct info_node *next;
};

struct info_node *create_node(char *mem_data, float cpu_data, float used_ram) {
	struct info_node *new_node = (struct info_node *)malloc(sizeof(struct info_node));
	if(new_node == NULL) return NULL;
	strcpy(new_node -> mem_data, mem_data);
	new_node -> cpu_data = cpu_data;
	new_node -> used_ram = used_ram;
	new_node -> next = NULL;
	return new_node;
}

struct info_node *insert_at_tail(struct info_node *head, struct info_node *new_node) {
	if(head == NULL) return new_node;
	struct info_node *tmp = head;
	while(tmp -> next != NULL) {
		tmp = tmp -> next;
	}
	tmp -> next = new_node;
	return head;
}

void print_list(struct info_node *head) {
	while (head != NULL) {
		printf("%s\n", head -> mem_data);
		head = head -> next;
	}
}

void free_linked_list(struct info_node *head) {
	struct info_node *current = head;
	struct info_node *next;
	while (current != NULL) {
		next = current -> next;
		free(current);
		current = next;
	}
}

void print_cpu_graphics(struct info_node *head, int no_of_samples, int sample_no) {
	while (head != NULL) {
		printf("         ");
		for(int i = 0; i < (int)((head -> cpu_data) / 3); i++, printf("|"));
		printf("| %.2f\n", head -> cpu_data);
		head = head -> next;
	}
	for(int i = 0; i < no_of_samples - sample_no - 1; i++, printf("\n"));
}

float ram_usage(struct info_node *head, int index) {
    struct info_node *current = head;
    for (int i = 0; i < index; i++) {
        if (current == NULL) {
            return (float)-1;
        }
        current = current -> next;
    }
    return current -> used_ram;
}

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

void calculate_cpu_util(unsigned long long *cpu_util) {
	cpu_util[0] = cpu_util[2]; cpu_util[1] = cpu_util[3];
	char util_data[STRING_LEN]; char cpu_name[5];
	unsigned long long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
	FILE *file = fopen("/proc/stat", "r");
	if (file == NULL) fprintf(stderr, "Error opening /proc/stat\n");
	fgets(util_data, STRING_LEN, file);
	sscanf(util_data, "%s %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu", cpu_name, &user, \
		&nice, &system, &idle, &iowait, &irq, &softirq, &steal, &guest, &guest_nice);
	if(fclose(file) != 0) fprintf(stderr, "Error closing /proc/stat\n");
	cpu_util[2] = user + nice + system + iowait + irq + softirq + steal;
	cpu_util[3] = user + nice + system + idle + iowait + irq + softirq + steal + guest + guest_nice;
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

void display_memory(int no_of_samples, int sample_no, int sequential, struct info_node **head, int graphics) {
	struct sysinfo *info = (struct sysinfo *)malloc(sizeof(struct sysinfo));
        sysinfo(info);
        float totalram = ((float)(info -> totalram) / (info -> mem_unit)) / 1073741824;
        float freeram = ((float)(info -> freeram) / (info -> mem_unit)) / 1073741824;
        float totalswap = ((float)(info -> totalswap) / (info -> mem_unit)) / 1073741824;
        float freeswap = ((float)(info -> freeswap) / (info -> mem_unit)) / 1073741824;
        float usedram = totalram - freeram;
        float usedswap = totalswap - freeswap;
        printf("### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)\n");
	char info_string[STRING_LEN];
	char graphics_string[STRING_LEN / 2] = "";
	if(graphics == 1) {
		float change = 0;
		if(sample_no > 0) {
			change = usedram / ram_usage(*head, sample_no - 1) - 1;
		}
		strcat(graphics_string, "   |");
		if(change >= 0) {
			for(int i = 0; i < (int)(change * 100); i++, strcat(graphics_string, "#"));
			if(change == 0) {
				strcat(graphics_string, "o");
			} else {
				strcat(graphics_string, "*");
			}
		} else {
			for(int i = 0; i < -(int)(change * 100); i++, strcat(graphics_string, ":"));
			strcat(graphics_string, "@");
		}
		char extra_info[20];
		sprintf(extra_info, " %.2f (%.2f)", change, usedram);
		strcat(graphics_string, extra_info);
	}
	sprintf(info_string, "%.2f GB / %.2f GB -- %.2f GB / %.2f GB%s", usedram, totalram, usedswap, totalswap, graphics_string);
	*head = insert_at_tail(*head, create_node(info_string, (float)0, usedram));
	if(sequential == 0) {
		print_list(*head);
	}
	if (sequential == 1) {
		for(int i = 0; i < sample_no; i++, printf("\n"));
		printf("%s\n", info_string);
	}
	for(int i = 0; i < no_of_samples - sample_no - 1; i++, printf("\n"));
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

void display_no_of_cores(unsigned long long *cpu_util, int graphics, struct info_node **head, int no_of_samples, int sample_no) {
	calculate_cpu_util(cpu_util);
	float util = (float)(cpu_util[2] - cpu_util[0]) / (float)(cpu_util[3] - cpu_util[1]) * 100;
	printf("Number of cores: %ld\n", sysconf(_SC_NPROCESSORS_ONLN));
	printf(" total cpu use = %.2f%%\n", util);
	if(graphics == 1) {
		*head = insert_at_tail(*head, create_node("", util, (float)0));
		print_cpu_graphics(*head, no_of_samples, sample_no);
	}
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

void display(int no_of_samples, int delay, int mode, int sequential, int graphics) {
	unsigned long long cpu_util[4] = {0, 0, 0, 0}; // prev_busy, prev_total, curr_busy, curr_total
	calculate_cpu_util(cpu_util); sleep(1);
	struct info_node *mem_list = NULL;
	struct info_node *cpu_list = NULL;
	printf("\033c\033[1H"); // Clear screen
	for(int i = 0; i < no_of_samples; i++) {
		if(sequential == 0) printf("\033c\033[1H");
		display_title(no_of_samples, delay, sequential, i);
		if(mode != 2) display_memory(no_of_samples, i, sequential, &mem_list, graphics);
		if(mode != 1) display_session();
		if(mode != 2) display_no_of_cores(cpu_util, graphics, &cpu_list, no_of_samples, i);
		sleep(delay);
	}
	display_sysinfo();
	free_linked_list(mem_list);
	free_linked_list(cpu_list);
}

int main(int argc, char **argv) {
	int mode = 0; //0 - default, 1 - system, 2 - user
	int graphics = 0; //0 - no graphics (default), 1 - use graphics
	int sequential = 0; //0 - not sequential (default), 1 - sequential
	int no_of_samples = 10; //default value
	int delay = 1; //default value
	int samples_changed = 0; // prevent multiple of the same arguments
	int tdelay_changed = 0; // prevent multiple of the same arguments	

	if (argc <= 0 || 7 <= argc) {
		fprintf(stderr, "Invalid or duplicate argument(s).\n");
                return 1;
	}

	for(int i = 1; i < argc; i++) {
		if (mode == 0 && strcmp(argv[i], "--system") == 0) {
			mode = 1;
		} else if (mode == 0 && strcmp(argv[i], "--user") == 0) {
			mode = 2;
		} else if (graphics == 0 && (strcmp(argv[i], "--graphics") == 0 || strcmp(argv[i], "-g") == 0)) {
			graphics = 1;
		}  else if (sequential == 0 && strcmp(argv[i], "--sequential") == 0) {
                	sequential = 1;
        	}  else if (samples_changed == 0 &&
				strncmp(argv[i], "--samples=", 10) == 0 &&
				is_number(argv[i] + 10) == 1 &&
				0 <= atoi(argv[i] + 10) &&
				atoi(argv[i] + 10) <= 2147483647) {
			if(*(argv[i] + 10) != '\0') no_of_samples = atoi(argv[i] + 10);
			samples_changed = 1;
        	}  else if (tdelay_changed == 0 &&
				strncmp(argv[i], "--tdelay=", 9) == 0 &&
				is_number(argv[i] + 9) == 1 &&
				0 <= atoi(argv[i] + 9) &&
				atoi(argv[i] + 9) <= 2147483647) {
			if(*(argv[i] + 9) != '\0') delay = atoi(argv[i] + 9);
			tdelay_changed = 1;
        	} else if (samples_changed == 0 && tdelay_changed == 0 && 1 <= i && i <= 4 && i < argc - 1 &&
				is_number(argv[i]) == 1 && is_number(argv[i + 1]) == 1 &&
				0 <= atoi(argv[i]) && atoi(argv[i]) <= 2147483647 &&
				0 <= atoi(argv[i + 1]) && atoi(argv[i + 1]) <= 2147483647) {
			no_of_samples = atoi(argv[i]); delay = atoi(argv[i + 1]);
			samples_changed = 1; tdelay_changed = 1; i++;
		} else {
			fprintf(stderr, "Invalid or duplicate argument(s).\n");
			return 1;
		}
	}

	display(no_of_samples, delay, mode, sequential, graphics);
	return 0;
}
