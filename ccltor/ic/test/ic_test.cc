
#include <stdio.h>
#include <stdlib.h>
#include "CcltorIC.h"

static CcltorIC ic;

static void recv()
{
	CcltorICMsg msg;
	CcltorICPeer from;
	int r = ic.receive(&msg, &from);
	printf("received r: %d, msg_head: %u\n", r, msg.head);
	printf("received from: %s:%d\n", inet_ntoa(from.saddr_in.sin_addr), ntohs(from.saddr_in.sin_port));
}

static void send()
{
	CcltorICPeer to;
	char host[64]; int port = 0; int head = 0;
	printf("Insert destination ip: ");
	scanf("%s\n", host);
	printf("Insert destination port: ");
	scanf("%d\n", &port);
	printf("Insert head of msg: ");
	scanf("%d\n", &head);
	printf("sending %d to %s:%d\n", head, host, port);
	int r = to.set(host, port);
	if (r != 0)
	{
		fprintf(stderr, "peer set failed with %d\n", r);
		return;
	}
	CcltorICMsg msg(head);
	r = ic.send(to, msg, 0);
	if (r != 0)
	{
		fprintf(stderr, "ic.send failed with %d\n", r);
		perror("ic.send\n");
	}
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s <port>\n\n", argv[0]);
		exit(-1);
	}

	int port = atoi(argv[1]);
	printf("using port %d\n", port);
	if (ic.init(port) == -1)
	{
		perror("ic.init");
		exit(-1);
	}
	
	for (;;)
	{
		switch (getchar())
		{
		case 'r': recv(); break;
		case 's': send(); break;
		case 'q': exit(0);
		case 'm':
			printf("r: receive\ns: send\nq: exit\n\n");
		}
	}
}
