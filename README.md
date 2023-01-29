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
 * To get CPU utilization data, `/proc/stat` was read using fgets and parsed with sscanf to obtain various CPU utilization numbers, which were added to get cpu_busy and cpu_total.
 * To get the app's memory usage `getrusage` from `sys/resource.h` was used.
 * To get list of users `setutent` and `getutent`from `utmp.h` was used.
 * To get number of CPU cores `sysconf(_SC_NPROCESSORS_ONLN)` from `unistd.h` was used.
 * To get system information `uname` from `sys/utsname.h` was used.

### Other
 * `printf("\033c\033[1H");` - Control Sequence Introducer (CSI) was used to clear the terminal screen and reset the cursor position back to 1,1.
 * Defensive programming was done to make sure that user input is valid - i.e. checking all characters are digits 0-9 before using `atoi` to convert a string to an int.
 * Each function had one responsibility - e.g. one function displays memory usage, one function displays system information, one function retrieves CPU utilization

## Overview of functions

