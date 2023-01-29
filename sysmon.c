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

// Stores RAM and CPU data
struct info_node {
	float used_ram;
	float total_ram;
	float used_swap;
	float total_swap;
	unsigned long long cpu_busy;
	unsigned long long cpu_total;
	float cpu_usage;
	struct info_node *prev;
	struct info_node *next;
};

// Creates a new node of doubly-linked list 
struct info_node *create_new_node(float used_ram, float total_ram, float used_swap, float total_swap, \
		unsigned long long cpu_busy, unsigned long long cpu_total, float cpu_usage) {
	struct info_node *new_node = (struct info_node *) malloc(sizeof(struct info_node));
	if (new_node == NULL) {
		return NULL;
	}
	new_node -> used_ram = used_ram;
	new_node -> total_ram = total_ram;
	new_node -> used_swap = used_swap;
	new_node -> total_swap = total_swap;
	new_node -> cpu_busy = cpu_busy;
	new_node -> cpu_total = cpu_total;
	new_node -> cpu_usage = cpu_usage;
	new_node -> prev = NULL;
	new_node -> next = NULL;
	return new_node;
}

// Performs the insertion of new_node at the tail of the list with O(1) complexity
void insert_at_tail(struct info_node **head, struct info_node **tail, struct info_node *new_node) {
	if(new_node == NULL) return;
	if (*head == NULL && *tail == NULL) {
		*head = new_node;
		*tail = new_node;
		return;
	}
	if (*head == *tail) {
		*tail = new_node;
		(*head) -> next = *tail;
		(*tail) -> prev = *head;
		return;
	}
	(*tail) -> next = new_node;
	new_node -> prev = *tail;
	*tail = new_node;
}

// Frees the memory used by the list
void free_list(struct info_node *head) {
    struct info_node *tmp;
    while (head != NULL) {
        tmp = head -> next;
        free(head);
        head = tmp;
    }
}

// Prints a line of '-' symbols to separate different sections of the program
void print_line() {
	printf("---------------------------------------\n");
}

// Checks whether all characters in the string are 0-9
int is_number(char *number) {
	int result = 1;
	while(*number != '\0') {
		if(*number < 48 || *number > 57) result = 0;
		number++;
	}
	return result;
}

// Retrieve information about memory and store it in the current node
void retrieve_meminfo(struct info_node *current) {
	struct sysinfo *info = (struct sysinfo *)malloc(sizeof(struct sysinfo));
        sysinfo(info);
        float totalram = ((float)(info -> totalram) / (info -> mem_unit)) / 1073741824;
        float freeram = ((float)(info -> freeram) / (info -> mem_unit)) / 1073741824;
        float totalswap = ((float)(info -> totalswap) / (info -> mem_unit)) / 1073741824;
        float freeswap = ((float)(info -> freeswap) / (info -> mem_unit)) / 1073741824;
	current -> used_ram = totalram - freeram;
	current -> total_ram = totalram;
	current -> used_swap = totalswap - freeswap;
	current -> total_swap = totalswap;
	free(info);
}

// Prints out memory graphical output, called only when `--graphics` flag is used
void print_memory_graphics(struct info_node *head, struct info_node *current) {
	float change = 0;
	if(head != current) change = (current -> used_ram / current -> prev -> used_ram) - 1;
	printf("   |");
	for(int i = 0; i < (int)(change * 100) || i < -(int)(change * 100); i++, (change >= 0) ? printf("#") : printf(":"));
	(change >= 0) ? ((change == 0) ? printf("o") : printf("*")) : printf("@");
	printf(" %.2f (%.2f)", change, current -> used_ram);
}

// Prints 1 line of memory information
void print_memory(int graphics, struct info_node *head, struct info_node *current) {
	printf("%.2f GB / %.2f GB -- %.2f GB / %.2f GB", current -> used_ram, current -> total_ram, \
		current -> used_swap, current -> total_swap);
	if(graphics == 1) print_memory_graphics(head, current);
	printf("\n");
}

// Calculates CPU utilization and stores it in current -> cpu_busy and current -> cpu_total
void calculate_cpu_util(struct info_node *current) {
	char util_data[STRING_LEN]; char cpu_name[5];
	unsigned long long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
	FILE *file = fopen("/proc/stat", "r");
	if (file == NULL) fprintf(stderr, "Error opening /proc/stat\n");
	fgets(util_data, STRING_LEN, file);
	sscanf(util_data, "%s %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu", cpu_name, &user, \
		&nice, &system, &idle, &iowait, &irq, &softirq, &steal, &guest, &guest_nice);
	if(fclose(file) != 0) fprintf(stderr, "Error closing /proc/stat\n");
	current -> cpu_busy = user + nice + system + iowait + irq + softirq + steal;
	current -> cpu_total = user + nice + system + idle + iowait + irq + softirq + steal + guest + guest_nice;
}

// Prints CPU utililization graphics, called only when `--graphics` flag is used
void print_cpu_graphics(struct info_node *head, int no_of_samples, int sample_no) {
	while (head != NULL) {
		printf("         ");
		for(int i = 0; i < (int)((head -> cpu_usage) / 3); i++, printf("|"));
		printf("| %.2f\n", head -> cpu_usage);
		head = head -> next;
	}
	for(int i = 0; i < no_of_samples - sample_no - 1; i++, printf("\n"));
}

// Displays title with no_of_samples, delay, iteration, app memory usage
void display_title(int no_of_samples, int delay, int sequential, int sample_no) {
	if(sequential == 0) printf("Nbr of samples: %d -- every %d secs\n", no_of_samples, delay);
	if(sequential == 1) printf(">>> iteration %d\n", sample_no);
        struct rusage *usage = (struct rusage *)malloc(sizeof(struct rusage));
        getrusage(RUSAGE_SELF, usage);
        printf(" Memory usage: %ld kilobytes\n", usage -> ru_maxrss);
        free(usage);
	print_line();
}

// Displays RAM and swap usage with option for graphics, sequential
void display_memory(int no_of_samples, int sample_no, int sequential, int graphics, struct info_node *head, struct info_node *current) {
	retrieve_meminfo(current);
        printf("### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)\n");
	if(sequential == 0) {
		struct info_node *tmp = head;
		while(tmp != NULL) {
			print_memory(graphics, head, tmp);
			tmp = tmp -> next;
		}
	}
	if(sequential == 1) {
		for(int i = 0; i < sample_no; i++, printf("\n"));
		print_memory(graphics, head, current);
	}
	for(int i = 0; i < no_of_samples - sample_no - 1; i++, printf("\n"));	
	print_line();
}

// Displays the current users using the system using setutent and getutent
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

// Displays number of cores and cpu utilization, with option for graphics
void display_cpu(int graphics, int no_of_samples, int sample_no, struct info_node *head, struct info_node *current) {
	calculate_cpu_util(current);
	if(head != current) {
		current -> cpu_usage = (float)(current -> cpu_busy - current -> prev -> cpu_busy) \
			/ (float)(current -> cpu_total - current -> prev -> cpu_total) * 100;
	}
	printf("Number of cores: %ld\n", sysconf(_SC_NPROCESSORS_ONLN));
	printf(" total cpu use = %.2f%%\n", current -> cpu_usage);
	if(graphics == 1) print_cpu_graphics(head, no_of_samples, sample_no);
	print_line();
}

// Displays system information using uname
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

// Driver function of the program
void display(int no_of_samples, int delay, int mode, int sequential, int graphics) {
	struct info_node *head = NULL;
	struct info_node *tail = NULL;
	printf("\033c\033[1H");
	for(int i = 0; i < no_of_samples; i++) {
		struct info_node *current = create_new_node(0, 0, 0, 0, 0, 0, 0);
		insert_at_tail(&head, &tail, current);
		if(sequential == 0) printf("\033c\033[1H");
		display_title(no_of_samples, delay, sequential, i);
		if(mode != 2) display_memory(no_of_samples, i, sequential, graphics, head, current);
		if(mode != 1) display_session();
		if(mode != 2) display_cpu(graphics, no_of_samples, i, head, current);
		sleep(delay);
	}
	display_sysinfo();
	free_list(head);
}

// If user input is valid, calls display() driver function with appropriate argument values
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
