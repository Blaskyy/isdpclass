all:nfq ipq

nfq:nfq_firewall.c
	gcc -o nfq_firewall nfq_firewall.c $(shell pkg-config --libs --cflags libnetfilter_queue)

ipq:ipq_firewall.c
	gcc -o ipq_firewall ipq_firewall.c $(shell pkg-config --libs --cflags libipq)

clean:
	rm -f nfq_firewall ipq_firewall

