#! /bin/sh
#2
iptables -I OUTPUT -j DROP
iptables -A INPUT -p icmp -j ACCEPT
iptables -I OUTPUT -p icmp -j ACCEPT
#3
iptables -I INPUT -p tcp --dport 22 -j DROP

