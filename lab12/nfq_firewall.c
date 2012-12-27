#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <linux/netfilter.h>
#include <linux/ip.h>

#include <libnetfilter_queue/libnetfilter_queue.h>

#define TO "114.114.114.114"
#define LENGTH 4096

static int cb(struct nfq_q_handle *qh, struct nfgenmsg *nfmsg, struct nfq_data *nfa, void *data){
    int id = 0, pload_len;
    unsigned char *pload;
    struct nfqnl_msg_packet_hdr *ph;

    //get unique ID of packet in queue
    ph = nfq_get_msg_packet_hdr(nfa);
    if(ph) {
        id = ntohl(ph->packet_id);
    }

    //get payload
    pload_len = nfq_get_payload(nfa, &pload);
    if(pload_len == -1) {
        pload_len = 0;
    }

    /* get the dest addr of this packet;
     * compare the dest addr with TO
     * if they are the same,then return nfq_set_verdict(qh, id, NF_ACCEPT, 0, NULL)
     * else return nfq_set_verdict(qh, id, NF_DROP, 0, NULL)
     */
    struct iphdr *ip_head = (struct iphdr *)pload;
    if(ip_head->daddr == inet_addr(TO)){
        printf("Access ping to ");
        printf(TO);
        printf("\n");
        return nfq_set_verdict(qh, id, NF_ACCEPT, 0, NULL);
    } else {
        return nfq_set_verdict(qh, id, NF_DROP, 0, NULL);
    }
}

int main(int argc, const char *argv[])
{
    int len, fd;
    char buf[LENGTH];
    struct nfq_handle *h;
    struct nfq_q_handle *qh;

    //call nfq_open() to open a NFQUEUE handler
    h = nfq_open();
    if(!h) {
        fprintf(stderr, "error during nfq_open()\n");
        exit(1);
    }

    //unbinging existing nf_queue handler for PE_INET(if any)
    if(nfq_unbind_pf(h, PF_INET) < 0) {
        fprintf(stderr, "error during nfq_unbind_pf()\n");
        exit(1);
    }

    //binding nfnetlink_queue as nf_queue handler for PF_INET
    if(nfq_bind_pf(h, PF_INET) < 0) {
        fprintf(stderr, "error during nfq_bind_pf()\n");
        exit(1);
    }

    //binding this socket to queue 
    qh = nfq_create_queue(h, 0, &cb, NULL);
    if(!qh) {
        fprintf(stderr,"error during nfq_create_queue()\n");
        exit(1);
    }

    //setting copy_packet mode
    if(nfq_set_mode(qh, NFQNL_COPY_PACKET, 0xffff) < 0) {
        fprintf(stderr, "can't set packet_copy_mode\n");
        exit(1);
    }

    //get the file descriptor associated with the nfqueue handler
    fd = nfq_fd(h);

    //handle a packet received from the nfqueue subsystem
    while ((len = recv(fd, buf, sizeof(buf), 0)) && len >= 0) {
        nfq_handle_packet(h, buf, len);
    }

    nfq_destroy_queue(qh);
    nfq_close(h);
    return 0;
}
