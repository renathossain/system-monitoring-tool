# system-monitoring-tool
Report different metrics about the system and resource utilization.
  * RAM usage and swap memory usage
  * Lists current users
  * CPU utilization
  * System Information
    * OS name
    * Machine name
    * Linux kernel version
    * System architecture
    * Number of cpu cores

## How to use the program
 * To compile the code, run the following: `gcc -std=c99 -Wall -o sysmon sysmon.c`.
 * To running the command with no args (produces 10 samples 1 sec apart): `./sysmon`.
 * To change the number of samples, use the `--samples=N` flag. For e.g. - `./sysmon --samples=20`.
 * To change the delay between the samples displayed, use the `--tdelay=T` flag. For e.g. - `./sysmon --tdelay=5`.
 * Passing `./sysmon --samples= --tdelay=` with no numbers results in the default setting of 10 samples 1 sec apart.
 * Additionally, you can pass 2 numbers separated by a space to set the `samples` and `tdelay` values. For e.g. - `./sysmon 25 3` means 25 samples 3 sec apart.
 * To only display system information use the `--system` flag. For e.g. - `./sysmon --system`.
 * To only display user information use the `--user` flag. For e.g. - `./sysmon --user`.
 * To display CPU and memory line bars, use the `--graphics` or `-g` flag. For e.g. - `./sysmon --graphics`.
 * To display the data sequentially, use the `--sequential` flag. For e.g. - `./sysmon --sequential`. This is useful for saving the results in a text file, which can be done with the following command: `./sysmon --sequential > out.txt`.
 * All of the flags can be used together, for e.g. - `/sysmon -g --system --sequential 30 2`, except for `--system` and `--user`, as they are conflicting modes.

## How did I solve the problem

### Doubly-linked list
To store the RAM and CPU data, a doubly-linked list was used, for the following reasons:
 * To store an arbitrary no of samples, in the order the samples were generated, and to print them in that order.
 * At each iteration, a new sample can be added at the tail of the list with O(1) complexity instead of O(N) of a linked list.
 * The previous sample node can easily be accessed with `current -> prev` which is crucial for calculating CPU usage and RAM change in memory graphics.

### 3rd-party libaries
 * To get information about current memory usage, `sysinfo` from `sys/sysinfo.h>` was used.
 * To get CPU utilization data, `/proc/stat` was read using `fgets` and parsed with `sscanf` to obtain various CPU utilization numbers, which were added and stored in `current -> cpu_busy` and `current -> cpu_total`.
 * To get the app's memory usage `getrusage` from `sys/resource.h` was used.
 * To get list of users `setutent` and `getutent`from `utmp.h` was used.
 * To get number of CPU cores `sysconf(_SC_NPROCESSORS_ONLN)` from `unistd.h` was used.
 * To get system information `uname` from `sys/utsname.h` was used.

### Other
 * `printf("\033c\033[1H");` - Control Sequence Introducer (CSI) was used to clear the terminal screen and reset the cursor position back to 1,1.
 * Defensive programming was done to make sure that user input is valid - i.e. checking all characters are digits 0-9 before using `atoi` to convert the user string to an int.
 * Each function had one responsibility - e.g. one function displays memory usage, one function displays system information, one function retrieves CPU utilization.
 * Read the documentation of 3rd-party functions to understand how to use them. E.g. - `int uname(struct utsname *buf)` would store kernel info data in the struct `buf` and I could access the operating system version using `buf -> version`.

## Overview of functions
 * `struct info_node *create_new_node(float used_ram, float total_ram, float used_swap, float total_swap, unsigned long long cpu_busy, unsigned long long cpu_total, float cpu_usage)` creates a new node of doubly-linked list to store RAM and CPU data.
 * `void insert_at_tail(struct info_node **head, struct info_node **tail, struct info_node *new_node)` - performs the insertion of new_node at the tail of the list with O(1) complexity. Note that the head and tail of the list are given as double pointers.
 * `void free_list(struct info_node *head)` - frees the memory used by the list by traversing it once.
 * `void print_line()` - prints a line of '-' symbols to separate different sections of the program.
 * `int is_number(char *number)` - checks whether all characters in the string are 0-9, by making sure the ASCII value is between 48 and 57 inclusive.
 * `void retrieve_meminfo(struct info_node *current)` retrieve information about memory and store it in the appropriate fields of the current node of the list.
 * `void print_memory_graphics(struct info_node *head, struct info_node *current)` prints out memory graphical output, called only when `--graphics` flag is used.
 * `void print_memory(int graphics, struct info_node *head, struct info_node *current)` prints 1 line of memory information using data from 1 node of list.
 * `void calculate_cpu_util(struct info_node *current)` calculates CPU utilization and stores it in current -> cpu_busy and current -> cpu_total.
 * `void print_cpu_graphics(struct info_node *head, int no_of_samples, int sample_no)` prints CPU utililization graphics, called only when `--graphics` flag is used.
 * `void display_title(int no_of_samples, int delay, int sequential, int sample_no)` displays title with no_of_samples, delay, iteration, app memory usage
 * `void display_memory(int no_of_samples, int sample_no, int sequential, int graphics, struct info_node *head, struct info_node *current)` displays RAM and swap usage with option for graphics, sequential.
 * `void display_session()` displays the current users using the system using setutent and getutent.
 * `void display_cpu(int graphics, int no_of_samples, int sample_no, struct info_node *head, struct info_node *current)` displays number of cores and cpu utilization, with option for graphics.
 * `void display_sysinfo()` displays system information using uname.
 * `void display(int no_of_samples, int delay, int mode, int sequential, int graphics)` driver function of the program.
 * `int main(int argc, char **argv)` - If user input is valid, calls display() driver function with appropriate argument values otherwise returns an "Invalid or duplicate argument(s)." error message.
