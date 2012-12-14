#! /bin/sh
#2
iptables -I INPUT -p icmp -j ACCEPT
#3
iptables -I INPUT -p tcp --dport 22 -j DROP

