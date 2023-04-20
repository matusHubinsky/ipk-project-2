# Version 1.0.0

### New Features

- UDP port scannig: program will send UDP packet to selected port. After waiting for ICMP packet port unreached reply, marks the port as closed. Otherwise, the port is considered as open
- New linked list data structure named list
- Added `#include <argp.h>` for argument parsing:
    - `./ipk-l4-scan --usage` prints usage
    - `./ipk-l4-scan --help` pritnts help
    - running `./ipk-l4-scan` or running program with the wrong number of argumets prints little hint
- program will end properly after recieving ^C on stdin

### Known Problems

- IPv6 is not implemented yes
- TCP port scanning is not implemented yet

# Version 1.0.1

### New Features

- transforming domain name to adress 
- IPv6 support
- detecting os

### Known Problems

- TCP port scanning is not implemented yet
