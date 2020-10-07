# System monitoring server

Multithreaded TCP/UDP server monitors system status: CPU load, RAM usage, disk space used and available network interfaces.

## Using 

At client's request - message `report`, in response the server gives the description of the current state of the system in [JSON](https://www.json.org/json-en.html) format.

Example of server response:

```json
{"CPU, %": [11, 11, 13, 9, 8], "RAM": {"Total": 3908840, "Free": 232132, "Available": 1505048, "Buffers": 169024, "Cached": 1506540, "Total swap": 2097148, "Free swap": 2087676, "Unit": "kB"}, "Storage": [{"Filesystem": "/dev/sda5", "Mount Point": "/", "Total": 30507428, "Free": 26878422, "Available": 25317974, "Block size, kB": 4}, {"Filesystem": "/dev/sda1", "Mount Point": "/boot/efi", "Total": 130812, "Free": 130811, "Available": 130811, "Block size, kB": 4}], "Network": [{"Interface": "lo", "IP": "127.0.0.1"}, {"Interface": "wlp2s0", "IP": "192.168.41.193"}], "Time": "Wed Oct  7 10:32:17 2020"}
```

### Required pre-requisites for building (tested in Linux Mint 20):

1. happycoders-libsocket-dev - Generic C++ library implementing Udp/Tcp socket interface - development files
2. libjansson-dev - C library for encoding, decoding and manipulating JSON data (dev)

#### Compiling

```shell
cd ./server/
make
cd ./client/
make
```

#### Running

On server side:

```bash
$ ./server 
Usage: ./server <TCP/UDP> <port> <max_connections>
$ ./server TCP 10000 10
Using TCP on port 10000 with no more than 10 clients
New TCP connection accepted: now there are 1 clients
Client 1 stops observation
```

On client side:

```bash
$ ./client
Usage: ./client host port time_lag
$ ./client 127.0.0.1 10000 1
Total usage of  4 CPUs:   5%, Memory: 1937.8 MB used, 1879.4 MB free
Total usage of  4 CPUs:   6%, Memory: 1935.9 MB used, 1881.4 MB free
Total usage of  4 CPUs:   5%, Memory: 1935.8 MB used, 1881.4 MB free
Total usage of  4 CPUs:   6%, Memory: 1935.8 MB used, 1881.4 MB free
^C
$
```

#### Acknowledgements

The code is based on the [Harry Wong template](https://github.com/RedAndBlueEraser/c-multithreaded-client-server) (RedAndBlueEraser).

#### Warranty

There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. Use at your own risk.

#### License

This project is licensed under the MIT License - see the LICENSE file for
details.

####  Author

Sergei Malykhin, s.e.malykhin@gmail.com

