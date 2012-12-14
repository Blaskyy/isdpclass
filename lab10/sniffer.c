#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pcap.h>
#include <sys/time.h>
#include <time.h>
#include <linux/tcp.h>
#include <linux/ip.h>
#include <linux/if_ether.h>

#define BUFFSIZE 1518;
#define ETHERTYPE_IP 0x0800

void print_mes(char *data,int len);
void my_callback(u_char *useless,const struct pcap_pkthdr* pkthdr,const u_char*   packet);
void print(char *line);
int count;
int main(int argc, char *argv[]){
	char *dev,errbuf[PCAP_ERRBUF_SIZE];
	char *net_c = NULL,*mask_c = NULL;
	bpf_u_int32 mask;
	bpf_u_int32 net;
	struct in_addr addr;
	struct pcap_pkthdr header;
	pcap_t *handle;
	const u_char *packet;
	struct bpf_program fp;
	char filter_exp[] = "ip";
	dev = pcap_lookupdev(errbuf);
	if(dev == NULL) {
		perror("can't find default dev!\n");
		exit(-1);
	}
	printf("DEV:%s\n",dev);
	if(pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
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
	handle = pcap_open_live(dev,1500,1,0,errbuf);
	if(handle == NULL) {
		perror("couldn't get handle!\n");
		exit(-1);
	}
	if(pcap_compile(handle,&fp,filter_exp,0,mask)== -1) {
		perror("Couldn't parse filter\n");
		exit(-1);
	}
	if(pcap_setfilter(handle,&fp)==-1) {
		perror("Couldn't install filter\n");
		exit(-1);
	}
	pcap_loop(handle,-1,my_callback,NULL);
	pcap_freecode(&fp);
	pcap_close(handle);
	printf("\nCapture complete.\n");
}
void my_callback(u_char *useless,const struct pcap_pkthdr* pkthdr,const u_char* packet) {
	struct ethhdr *eptr;
	eptr = (struct ethhdr*)packet;
	char *data;
	int len;
	struct in_addr saddr,daddr;
	if((ntohs(eptr->h_proto) !=ETHERTYPE_IP)) {
		return;
	}
	int i;
	printf("Pcaket %d\n",count);
	count++;
	printf("MAC src: ");
	for(i=0;i<6;i++) {
		printf("%1x",eptr->h_source[i]);
		if(i < 5) {
			printf(":");
		}
	}
	printf("\nMAC dest: ");
	for(i=0;i<6;i++) {
		printf("%1x",eptr->h_dest[i]);
		if(i < 5) {
			printf(":");
		}
	}
	printf("\n");
	struct iphdr *p_iphdr;
	p_iphdr = (struct iphdr *)(packet + 14);
	char *src=NULL,*dest=NULL;
	saddr.s_addr = p_iphdr->saddr;
	src=inet_ntoa(saddr);
	printf("src ip:%s ",src);
	daddr.s_addr = p_iphdr->daddr;
	dest=inet_ntoa(daddr);
	printf("dest ip:%s\n",dest);
	if(p_iphdr->protocol==6) {
		printf("Protocol TCP:\n");
		struct tcphdr *p_tcphdr;
		p_tcphdr = (struct tcphdr *)(packet + 14 + 20);
		printf("TCP: src port:%d dest port:%d\n",ntohs(p_tcphdr->source),(char*)ntohs(p_tcphdr->dest));
		data = (char *)(packet + 14 + 20 + 20);
		len = pkthdr->len - 54;
		print_mes(data,len);
		printf("\n\n");
		return ;
	} else if(p_iphdr->protocol==17) {
		printf("Protocol UDP:\n");
		data = (char *)(packet + 14 + 20 + 20);
		len = pkthdr->len - 54;
		print_mes(data,len);
		printf("\n\n");
	}else if(p_iphdr->protocol==1) {
		printf("Protocol ICMP:\n");
		data = (char *)(packet + 14 + 20 + 20);
		len = pkthdr->len - 54;
		print_mes(data,len);
		printf("\n\n");
	}else{
		printf("Protocol unknow\n");
		data = (char *)(packet + 14 + 20 + 20);
		len = pkthdr->len - 54;
		print_mes(data,len);
		printf("\n\n");
	}
}
void print_mes(char *data,int len) {
	int i,line_width = 16,len_rem;
	char *p =NULL;
	p = data;
	len_rem = len;
	i=0;
	while(len!=0) {
		if(len_rem < line_width) {
			break;
		}
		printf("%5d  ",i);
		len_rem = len_rem -line_width;
		print(p);
		p = p+16;
		i +=16;
	}
}
void print(char *line) {
	char *p =line;
	int i;
	for(i=0;i<16;i++) {
		printf("%.2X ",(unsigned char)*p);
		FILE *fp = fopen("captured.txt","a");
		fprintf(fp,"%c",(unsigned char)*p);
		fclose(fp);
		p++;
	}
	p = line;
	printf(" ");
	for(i=0;i<16;i++) {
		if(!isprint(*p)) {
			printf(".");
			FILE *fp = fopen("captured.txt","a");
			char cp[] = ".";
			fprintf(fp,"%s",cp);
			fclose(fp);
		} else {
			printf("%c",*p);
			FILE *fp = fopen("captured.txt","a");
			fprintf(fp,"%c",*p);
			fclose(fp);
		}
		p++;
	}
	printf("\n");
}
