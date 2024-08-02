# SSH Brute Force Tool

This tool is a multi-threaded SSH brute force utility designed for speed and efficiency. It attempts to authenticate to an SSH server using either a single password or a list of passwords. The tool is optimized for faster password attempts and includes features to help terminate the process once the correct credentials are found.

## Features

- Multi-threaded password attempts for faster execution.
- Efficient socket handling using `select` to speed up connection.
- Global flag to stop all threads once the correct password is found.
- Verbose output to show progress and details of attempted credentials.

## Prerequisites

- **libssh2** library
- **gcc** compiler
- **pthread** library

## Installation on OpenBSD

To set up and use the SSH brute force tool on OpenBSD, follow these instructions:

### 1. Install Dependencies

OpenBSD's package manager **pkg_add** can be used to install the required dependencies. Ensure you have `libssh2` and `gcc` installed:

```
$ sudo pkg_add libssh2 gcc
```

### 2. Clone the Repository

Clone the repository where the SSH brute force tool is hosted:

```
$ git clone https://github.com/yourusername/ssh-brute-force.git
$ cd ssh-brute-force
```

### 3. Compile the Code

Compile the source code with gcc:

```
$ gcc -o ssh_brute_force ssh_brute_force.c -lssh2 -lpthread
```

This command will generate an executable named ssh_brute_force.
Installation on Linux

To set up and use the SSH brute force tool on Linux, follow these instructions:
## 1. Install Dependencies

On Linux, you'll need to install the libssh2 development library and gcc compiler. The exact package names can vary by distribution:
Debian/Ubuntu-based Systems

```
$ sudo apt-get update
$ sudo apt-get install libssh2-1-dev gcc
```

RedHat/CentOS-based Systems

```
sudo yum install libssh2-devel gcc
```

## 2. Clone the Repository

Clone the repository where the SSH brute force tool is hosted:

```
$ git clone https://github.com/yourusername/ssh-brute-force.git
$ cd ssh-brute-force
```

## 3. Compile the Code

Compile the source code with gcc:

```
$ gcc -o ssh_brute_force ssh_brute_force.c -lssh2 -lpthread
```

This command will create an executable named ssh_brute_force.
Usage

Run the tool using the following command syntax:

```
$ ./ssh_brute_force -h <hostname> -p <port> -u <username> [-p <password> | -P <password_list>] [-t <thread_count>] [-V]
```

## Parameters

    -h <hostname>: The hostname or IP address of the SSH server.
    -p <port>: The port number on which the SSH server is running.
    -u <username>: The SSH username to authenticate with.
    -p <password>: A single password to attempt.
    -P <password_list>: A file containing a list of passwords to attempt.
    -t <thread_count>: Number of concurrent threads to use (default is 4).
    -V: Enable verbose output to show detailed progress.

## Examples

  ### Single Password Attempt:


```
$ ./ssh_brute_force -h 192.168.1.1 -p 22 -u root -p password123 -t 10 -V
```

 ### Password List Attempt:

```
$ ./ssh_brute_force -h 192.168.1.1 -p 22 -u root -P passwords.txt -t 20 -V
```

 ### Performance Optimization

This tool incorporates several optimizations to enhance performance:

    Non-Blocking Sockets: Uses non-blocking sockets to improve connection handling efficiency.
    Select-Based Socket Management: Employs the select function for managing socket connections, reducing the overhead of blocking operations.
    Global Flag for Early Termination: Implements a global flag to terminate all threads once the correct password is found, reducing unnecessary operations.

## License
This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments
This tool utilizes the [libssh2](https://libssh2.org/) library for SSH functionality.
Inspired by various open-source SSH brute force tools.

## Disclaimer

This tool is intended for educational purposes only. Unauthorized use against systems that you do not own or have explicit permission to test is illegal and unethical. Ensure you have the necessary permissions before conducting any tests.
