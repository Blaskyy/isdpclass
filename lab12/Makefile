ifeq ($(shell pkg-config --exists libipq; echo $$?), 0)
	IPQ_CFLAGS = $(shell pkg-config --cflags libipq)
	IPQ_LIBS = $(shell pkg-config --libs libipq)
else
	IPQ_CFLAGS = 
	IPQ_LIBS = -lipq
endif

ifeq ($(shell pkg-config --exists libnetfilter_queue; echo $$?), 0)
	NFQ_CFLAGS = $(shell pkg-config --cflags libnetfilter_queue)
	NFQ_LIBS = $(shell pkg-config --libs libnetfilter_queue)
else
	NFQ_CFLAGS = 
	NFQ_LIBS = -lnetfilter_queue
endif


all:nfq_firewall ipq_firewall

nfq_firewall:nfq_firewall.c
	gcc -o nfq_firewall nfq_firewall.c $(NFQ_CFLAGS) $(NFQ_LIBS)

ipq_firewall:ipq_firewall.c
	gcc -o ipq_firewall ipq_firewall.c $(IPQ_CFLAGS) $(IPQ_LIBS)

clean:
	rm -f nfq_firewall ipq_firewall

