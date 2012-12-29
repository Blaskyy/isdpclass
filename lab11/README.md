实验题目
========
要求一
-----------
1. 修改本机ip为10.3.1.(X+100), 其中X为WinXP IP的最后一位
2. 编写脚本iptest.sh, 功能如下:
 1. iptest.sh清空所有存在的规则;
 2. 只开放22号端口(ssh, tcp);
 3. 禁止发送icmp包;
 4. INPUT默认规则为DROP;
 5. OUTPUT默认规则为ACCEPT;
 6. FORWARD默认规则为DROP;
 7. iptest.sh能随系统启动。

要求二
---------
1. 编写脚本iptest2.sh, 功能如下:
 1. 不清空所有存在的规则的情况下;
 2. 只允许icmp通过;
 3. 禁止22号服务.