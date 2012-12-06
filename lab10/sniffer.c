/****************************************************************************************
*
*
*	name:debris_廖偲
*	date:2011.11.11 buf_overflow
*	function: use libpcap 捕获分组实现在链路层进行抓包，区分数据.
*	isprint() 1 可打印
*
*
**********************************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pcap.h>
#include <sys/time.h>
#include <time.h>
#include <linux/tcp.h>
#include <linux/ip.h>
#include <linux/if_ether.h>

#define BUFFSIZE 1500;
#define ETHERTYPE_IP 0x0800

void print_mes(char *data,int len);

void my_callback(u_char *useless,const struct pcap_pkthdr* pkthdr,const u_char*   packet);
void print(char *line);

int main(int argc, char *argv[])
{
	//dev设备描述符，错误缓冲区，字符串形式网络地址，网络掩码地址
	char *dev,errbuf[PCAP_ERRBUF_SIZE];
	char *net_c = NULL,*mask_c = NULL;

	//bpf(BSD 分组过滤器)储存bpf形式的网络地址网络掩码地址
	bpf_u_int32 mask;
	bpf_u_int32 net;

	//网路的地址,pcap的头部,libpcap设备描述符
	struct in_addr addr;
	struct pcap_pkthdr header;
	pcap_t *handle;

	//实际包内容
	const u_char *packet;

	//过滤器
	struct bpf_program fp;
	char filter_exp[] = "port 80";//实际的过滤规则

	//获得设备
	dev = pcap_lookupdev(errbuf);
	if(dev == NULL)
	{
		perror("can't find default dev!\n");
		exit(-1);
	}
	dev = "wlan0";
	printf("DEV:%s\n",dev);


	if(pcap_lookupnet(dev, &net, &mask, errbuf) == -1)
	{
		perror("can't get netmask\n");
		net_c =0;
		mask_c = 0;
		exit(-1);
	}

	addr.s_addr = net;
	net_c = inet_ntoa(addr);
	printf("Net:%s\n",net_c);

	addr.s_addr = mask;
	mask_c = inet_ntoa(addr);
	printf("Mask:%s\n",mask_c);
	printf("==================================================\n");
	/*获取Libpcap bpf过滤设备*/
	/*设备最大捕获包的字节数为1518（针对以太网）*/
	/*将NIC设为混在模式，可以侦听局域网的所有内容len 64-1518 */
	handle = pcap_open_live(dev,1500,1,0,errbuf);
	if(handle == NULL)
	{
		perror("couldn't get handle!\n");
		exit(-1);
	}

	//制定规则
	if(pcap_compile(handle,&fp,filter_exp,0,net)== -1)
	{
		perror("Couldn't parse filter\n");
		exit(-1);
	}

	//在filter上加规则
	if(pcap_setfilter(handle,&fp)==-1)
	{
		perror("Couldn't install filter\n");
		exit(-1);
	}


	pcap_loop(handle,-1,my_callback,NULL);

}

void my_callback(u_char *useless,const struct pcap_pkthdr* pkthdr,const u_char* packet)
{
	struct ethhdr *eptr;
	eptr = (struct ethhdr*)packet;
	char *data;
	int len;
	struct in_addr saddr,daddr;

	if((ntohs(eptr->h_proto) !=ETHERTYPE_IP))
	{
		return;
	}


	int i;
	printf("MAC source: ");
	for(i=0;i<6;i++)
	{
		printf("%1x",eptr->h_source[i]);
		if(i < 5)
		{
			printf(":");
		}
	}
	printf("\nMAC dest: ");
	for(i=0;i<6;i++)
	{
		printf("%1x",eptr->h_dest[i]);
		if(i < 5)
		{
			printf(":");
		}
	}
	printf("\n");

	//取得ip头部
	struct iphdr *p_iphdr;
	p_iphdr = (struct iphdr *)(packet + 14);


	//打印ip地址
	char *src=NULL,*dest=NULL;
	saddr.s_addr = p_iphdr->saddr;
	src=inet_ntoa(saddr);
	printf("source ip:%s ",src);
	daddr.s_addr = p_iphdr->daddr;
	dest=inet_ntoa(daddr);
	printf("dest ip:%s\n",dest);

	//判断是不是tcp
	if(p_iphdr->protocol==6)
	{
		printf("TCP:\n");
		struct tcphdr *p_tcphdr;
		p_tcphdr = (struct tcphdr *)(packet + 14 + 20);
		printf("TCP: source port:%d dest port:%d\n",ntohs(p_tcphdr->source),(char*)ntohs(p_tcphdr->dest));
		data = (char *)(packet + 14 + 20 + 20);
		len = pkthdr->len - 54;

		print_mes(data,len);
		printf("\n\n");
		return ;
	}//判断是不是udp
	else if(p_iphdr->protocol==17)
	{
		printf("UDP:\n");
		data = (char *)(packet + 14 + 20 + 20);
		len = pkthdr->len - 54;
		print_mes(data,len);
		printf("\n\n");
	}


}

void print_mes(char *data,int len)
{
	int i,line_width = 16,len_rem;
	char *p =NULL;
	p = data;

	len_rem = len;
	i=0;
	while(len!=0)
	{
		printf("data+%d  ",i);
		if(len_rem < line_width)
		{
			break;
		}
		len_rem = len_rem -line_width;
		print(p);
		p = p+16;
		i +=16;
	}
}

void print(char *line)
{
	char *p =line;
	int i;
	for(i=0;i<16;i++)
	{
		printf("%.2X ",(unsigned char)*p);
		p++;
	}
	p = line;
	printf(" ");
	for(i=0;i<16;i++)
	{
		if(!isprint(*p))
		{
			printf(".",*p);
		}
		else
		{
			printf("%c",*p);
		}
		p++;
	}
	printf("\n");
}
