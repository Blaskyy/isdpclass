实验题目
========
要求
-----------
1. 设置iptables过滤规则为: 所有从本机发出的icmp包全部到自己编写的应用程序
2. 编写应用程序, 功能如下:
 1. 允许从本机出发, 目的地址为win xp ip的icmp包;
 2. 丢弃其他任何icmp包;
 3. 当出现错误时, 做错误处理, 能够清理占用资源, 退出程序.

 `ps. nfq_firewall.c 是针对 nfnetlink_queue 机制用 libnetfilter_queue 重写的`