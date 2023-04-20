
# Projekt 2. OMEGA: L4 Skenner
UDP: každý port ktorý nepošle odpoveď ICMP je braný ako otvorený. 
Táto metóda má za následok veľa faľośnej pozitivity, teda portov ktoré skener vyhodnotí nesprávne ako otvorené. Ak port pošle  odpoveď ICMP typu 3 (port closed), uvažujeme o ňom ako o zatvorenom
Inak ho pokladámu za otvorený, ale v skutočnosti nemusí byť. 
Rozlišuje teda medzi dvoma stavmi portov: `( open | closed )`

TCP: todo

# Manuál
`./ipk-l4-scan --usage` vypíše použitie \
`./ipk-l4-scan --help`  vypíše nápovedu \
`./ipk-l4-scan`         alebo spustenie so zlým počtom portov vypíše krátku nápovedu 

# Dokumnetácia
viz. `html/index.html`

# Testovanie
Program som otestoval porovnávaním výsledkov s programom `nmap`. Pre `nmap` som sa rozhodol na základe skúseností s jeho používaním. Ako ciel som si vybral `scanme.nmap.org`, ktorého skenovanie pár krát za deń je, podľa tvorcov, v poriadku.

Citácia z scanme.nmap.org:
```
Hello, and welcome to Scanme.Nmap.Org, a service provided by the Nmap Security Scanner Project.

We set up this machine to help folks learn about Nmap and also to test and make sure that their Nmap installation (or Internet connection) is working properly. You are authorized to scan this machine with Nmap or other port scanners. Try not to hammer on the server too hard. A few scans in a day is fine, but don't scan 100 times a day or use this site to test your ssh brute-force password cracking tool.

Thanks
-Fyodor 
```

## UDP
Testoval som stav portov 22,68,80,123,443 a 8080. 
Predpokladám že porty 68 a 123 budú otvorené a ostatné porsty budú zatvorené. 
Testovanie priebehalo vo virtuálnom prostedí `nix` a `Manjaro Linux x86_64`

### 1. test IPv4
vstup pre nmap:
`sudo nmap -sU -p U:22,68,80,123,443,8080 scanme.nmap.org`

výsledok nmap (teda predpokladaný výsledok Ipk-l4-scan):
```
Starting Nmap 7.93 ( https://nmap.org ) at 2023-04-17 08:52 CEST
Nmap scan report for scanme.nmap.org (45.33.32.156)
Host is up (0.22s latency).
Other addresses for scanme.nmap.org (not scanned): 2600:3c01::f03c:91ff:fe18:bb2f

PORT     STATE         SERVICE
22/udp   closed        ssh
68/udp   open|filtered dhcpc
80/udp   closed        http
123/udp  open          ntp
443/udp  closed        https
8080/udp closed        http-alt

Nmap done: 1 IP address (1 host up) scanned in 3.04 seconds
```

vstup `ipk-l4-scan`:
`sudo ./ipk-l4-scan -i wlp3s0 -w 1000 -u 22,68,80,123,443,8080 45.33.32.156`

výsledok `ipk-l4-scan`:
```
Interesting ports on (45.33.32.156):
PORT STATE
   22/udp closed
   68/udp open
   80/udp closed
  123/udp open
  443/udp closed
 8080/udp closed
```

záver: výsledok `ipk-l4-scan` sa zhoduje s výsledkom `nmap` (stav portu `filtered` nebol pre UDP v zadaní)

### 2. test IPv6
vstup pre `nmap`:
`sudo nmap -sU -p U:5353,8080 localhost -6`

výsledok `nmap` (teda predpokladaný výsledok Ipk-l4-scan):
```
Starting Nmap 7.93 ( https://nmap.org ) at 2023-04-17 19:09 CEST
Nmap scan report for localhost (::1)
Host is up (0.000087s latency).
Other addresses for localhost (not scanned): 127.0.0.1

PORT     STATE         SERVICE
5353/udp open|filtered zeroconf
8080/udp closed        http-alt

Nmap done: 1 IP address (1 host up) scanned in 1.33 seconds
```

vstup `ipk-l4-scan`: 
`./ipk-l4-scan -i lo -w 1000 --pu 5353,8080 0:0:0:0:0:0:0:1`

výsledok `ipk-l4-scan`:
```
Interesting ports on (0:0:0:0:0:0:0:1):
PORT STATE
 5353/udp open
 8080/udp closed
```

záver: výsledky sa zhodujú
