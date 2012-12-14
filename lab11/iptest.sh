#! /bin/sh
#1
iptables -t filter -F
iptables -t nat -F
iptables -t mangle -F
#iptables -t raw -F
#2
iptables -A INPUT -p tcp --dport 22 -j ACCEPT
#3
iptables -A OUTPUT -p icmp -j DROP
#4
iptables -P INPUT DROP
#5
iptables -P OUTPUT ACCEPT
#6
iptables -P FORWARD DROP
