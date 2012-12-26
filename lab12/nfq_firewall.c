#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <linux/netfilter.h>
#include <linux/ip.h>

#include <libnetfilter_queue/libnetfilter_queue.h>

#define TO "114.114.114.114"
static int cb(struct nfq_q_handle *qh, struct nfgenmsg *nfmsg, struct nfq_data *nfa, void *data){
    int id = 0;
    struct nfqnl_msg_packet_hdr *ph;
    ph = nfq_get_msg_packet_hdr(nfa);
    if(ph) {
        id = ntohl(ph->packet_id);
    }
    unsigned char *pdata;
    int pdata_len;
    pdata_len = nfq_get_payload(nfa, &pdata);
    if(pdata_len == -1) {
        pdata_len = 0;
    }
    struct iphdr *iphdrp = (struct iphdr *)pdata;
    if(iphdrp->daddr == inet_addr(TO)){
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
    struct nfq_handle *h;
    struct nfq_q_handle *qh;
    int fd;
    int rv;
    char buf[4096] __attribute__ ((aligned));
    h = nfq_open();
    if(!h) {
        fprintf(stderr, "error during nfq_open()\n");
        exit(1);
    }

    printf("unbinging existing nf_queue handler for PE_INET(if any)\n");
    if(nfq_unbind_pf(h, PF_INET) < 0) {
        fprintf(stderr, "error during nfq_unbind_pf()\n");
        exit(1);
    }

    printf("binding nfnetlink_queue as nf_queue handler for PF_INET\n");
    if(nfq_bind_pf(h, PF_INET) < 0) {
        fprintf(stderr, "error during nfq_bind_pf()\n");
        exit(1);
    }

    printf("binding this socket to queue '0'\n");
    qh = nfq_create_queue(h, 0, &cb, NULL);
    if(!qh) {
        fprintf(stderr,"error during nfq_create_queue()\n");
        exit(1);
    }

    printf("setting copy_packet mode\n");
    if(nfq_set_mode(qh, NFQNL_COPY_PACKET, 0xffff) < 0) {
        fprintf(stderr, "can't set packet_copy_mode\n");
        exit(1);
    }
    while (1) {
        nfq_handle_packet(h, buf, rv);
    }

    nfq_destroy_queue(qh);
    nfq_close(h);
    return 0;
}
