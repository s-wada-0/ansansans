<How To Use>

1.Compile
$ g++ -o checkServers checkServers.cc

2.usage
$ ./checkServers [inputfile]

3.Change parameter
If you want to change parameters (N, m, t), rewrite L8~10 of checkServers.cc


<Example>

(after compile)
$ ./checkServers test_case.txt

---result---
server 192.168.1.1 stopped (2020/10/19 13'33"34 - 2020/10/19 13'38"34 )
subnet 00000000110000001010100000000001 was broken (2020/10/19 13'37"35 - 2020/10/19 13'38"34 )
server 192.168.1.2 stopped (2020/10/19 13'35"35 - 2020/10/19 13'38"35 )
server 192.168.1.1 over loaded (2020/10/19 13'42"34 - 2020/10/19 13'44"34 )
server 10.20.30.2 over loaded (2020/10/19 13'46"25 - 2020/10/19 13'47"25 )
server 10.20.30.1 stopped (2020/10/19 13'46"24 - now )
server 10.20.30.2 stopped (2020/10/19 13'47"25 - now )
server 192.168.1.2 stopped (2020/10/19 13'48"35 - now )
server 192.168.1.1 over loaded (2020/10/19 13'50"34 - now )
subnet 00000000000000000000101000010100 is broken (2020/10/19 13'49"25 - now )
------------
