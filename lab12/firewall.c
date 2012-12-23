/*
 * This code is GPL.
 */
#include <netinet/in.h>
#include <linux/netfilter.h>
#include <libipq.h>
#include <stdio.h>
#include<netinet/ip.h>
#define BUFSIZE 2048 

static void die(struct ipq_handle *h)
{
	ipq_perror("passer");
	ipq_destroy_handle(h);
	exit(1);
}

int main(int argc, char **argv)
{
	int status;
	unsigned char buf[BUFSIZE];
	struct ipq_handle *h;
	struct iphdr *iphead;
	h = ipq_create_handle(0, PF_INET);
	if (!h)
		die(h);
		
	status = ipq_set_mode(h, IPQ_COPY_PACKET, BUFSIZE);
	if (status < 0)
		die(h);
		
	do{
		status = ipq_read(h, buf, BUFSIZE, 0);
		if (status < 0)
			die(h);
			
		switch (ipq_message_type(buf)) {
			case NLMSG_ERROR:
				fprintf(stderr, "Received error message %d\\n",
				        ipq_get_msgerr(buf));
				break;
				
			case IPQM_PACKET: {
				ipq_packet_msg_t *m = ipq_get_packet(buf);
				char *dest = NULL;
				struct in_addr daddr;
				iphead = (struct iphdr *)m->payload;
				daddr.s_addr = iphead->daddr;
				dest = inet_ntoa(daddr);

				if(strcmp(dest, "192.168.254.1") == 0)
				{
					status = ipq_set_verdict(h,m->packet_id,
							NF_ACCEPT,0,NULL);
					if(status < 0)
						die(h);
					break;
				}
				else
				{
					status = ipq_set_verdict(h,m->packet_id,
							NF_DROP,0,NULL);
					if(status < 0)
						die(h);
					break;
				}			


			//	status = ipq_set_verdict(h, m->packet_id,
			//	                         NF_ACCEPT, 0, NULL);
			//	if (status < 0)
			//		die(h);
			//	break;
			}
			
			default:
				fprintf(stderr, "Unknown message type!\\n");
				break;
		}
	} while (1);
	
	ipq_destroy_handle(h);
	return 0;
}
