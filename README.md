# CPorts


This is a simple command-line tool written in C that allows you to scan for open ports on a given IP address within a specified range of port numbers. It utilizes multi-threading to improve scanning efficiency.

## Prerequisites

Make sure you have the following libraries installed before compiling and running the program:

- `stdio.h`
- `stdlib.h`
- `string.h`
- `unistd.h`
- `arpa/inet.h`
- `netinet/in.h`
- `pthread.h`
- `sys/select.h`
- `sys/time.h`
- `errno.h`
- `fcntl.h`

## Usage

To use the port scanner, follow these steps:

1. Compile the program by running the following command:
```Bash
gcc -o scan cports.c -pthread
```

- `<IP_ADDRESS>`: The IP address you want to scan.
- `<START_PORT>`: The starting port number of the range to scan.
- `<END_PORT>`: The ending port number of the range to scan.

For example, to scan ports 80 to 100 on the IP address 192.168.0.1, you would run:
```Bash
./scan 192.168.0.1 80 100
```

**Note:** The IP address must be a valid IPv4 address.

3. The program will start scanning the specified range of ports on the given IP address. If a port is open, it will be displayed in green color and underlined.

## Program Structure

The program consists of the following components:

- `main`: The entry point of the program. It validates the command-line arguments, creates multiple threads for scanning ports, and waits for the threads to complete.
- `is_ip_adress`: A helper function to validate the format of an IP address.
- `scan_port`: The function executed by each thread. It attempts to connect to a port and checks if it is open.
- `is_port_open`: A helper function to check if a specific port is open on a given IP address.

## Multi-Threading

The program utilizes multi-threading to enhance the scanning process. It creates a separate thread for each port within the specified range. The number of threads created is limited to `MAX_THREADS`, which is set to 32 in this program. If the maximum number of threads is reached, the program waits for all threads to complete before creating new ones. This helps in optimizing resource usage and improving overall performance.

## Note

- This program only supports IPv4 addresses.
- The program does not perform any port scanning techniques beyond attempting to establish a TCP connection to each port. It does not perform any deep inspection or fingerprinting of open ports.
- Scanning a large range of ports or scanning hosts without proper authorization may violate the terms of service of your network provider or local laws. Use this tool responsibly and ensure you have the necessary permissions to perform port scanning activities.
