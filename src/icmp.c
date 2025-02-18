/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   icmp.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchene <jchene@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/17 13:45:49 by jchene            #+#    #+#             */
/*   Updated: 2025/02/17 14:06:48 by jchene           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

unsigned short checksum(void *b, int len) {
	unsigned short *buf = b;
	unsigned int sum = 0;
	unsigned short result;

	for (sum = 0; len > 1; len -= 2)
		sum += *buf++;
	if (len == 1)
		sum += *(unsigned char *)buf;
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	result = ~sum;
	return result;
}

int create_icmp_socket() {
	int sockfd;

	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
		printf("[ICMP]Failed to create socket\n"); 
        perror("Raw socket creation failed");
        return sockfd;
    }
	printf("[ICMP]Socket created, fd: %d\n", sockfd); 
	return sockfd;
}


t_host_status get_response_status(int sockfd) {
	struct timeval tv;
    fd_set readfds;
	struct iphdr *ipptr;
	struct icmp *icmpptr;

    tv.tv_sec = TIMEOUT_SEC;
    tv.tv_usec = 0;
    FD_ZERO(&readfds);
    FD_SET(sockfd, &readfds);

	//Waiting for a response
    if (DEBUG_LEVEL >= MEDIUM) printf("[ICMP]Waiting for ICMP reply...\n");
    if (select(sockfd + 1, &readfds, NULL, NULL, &tv) > 0) {
        char buffer[1024] = {0};
        struct sockaddr_in reply_addr;
        socklen_t addr_len = sizeof(reply_addr);
		//Reading response from the socket
        if (recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&reply_addr, &addr_len) > 0) {
			ipptr = (struct iphdr *)buffer;

			if (DEBUG_LEVEL >= MEDIUM) {
				printf("[ICMP]Received ICMP response from: %s\n", inet_ntoa(reply_addr.sin_addr));
				printf("[ICMP]IP Header: ");
				printf("Version: %d\n", ipptr->version);
				printf("Header Length: %d\n", ipptr->ihl);
				printf("Type of Service: %d\n", ipptr->tos);
				printf("Total Length: %d\n", ntohs(ipptr->tot_len));
				printf("Identification: %d\n", ntohs(ipptr->id));
				printf("Fragment Offset: %d\n", ntohs(ipptr->frag_off));
				printf("Time to Live: %d\n", ipptr->ttl);
				printf("Protocol: %d\n", ipptr->protocol);
				printf("Checksum: %d\n", ntohs(ipptr->check));
				printf("Source IP: %s\n", inet_ntoa(*(struct in_addr *)&ipptr->saddr));
				printf("Destination IP: %s\n", inet_ntoa(*(struct in_addr *)&ipptr->daddr));

				icmpptr = (struct icmp *)(buffer + ipptr->ihl * 4);
				printf("[ICMP]ICMP Header: ");
				printf("Type: %d\n", icmpptr->icmp_type);
				printf("Code: %d\n", icmpptr->icmp_code);
				printf("Checksum: %d\n", ntohs(icmpptr->icmp_cksum));
				printf("ID: %d\n", ntohs(icmpptr->icmp_id));
				printf("Sequence: %d\n", ntohs(icmpptr->icmp_seq));

			}

            if (DEBUG_LEVEL >= LOW) printf("[ICMP]Received ICMP response\n");
            close(sockfd);
            return HOST_UP;
        }
    }
	//No response recieved before timeout
	if (DEBUG_LEVEL >= LOW) printf("[ICMP]Did not recieve ICMP response\n");
	close(sockfd);
    return HOST_DOWN;
}

//Sends an ICMP request to check the status of an IPv4 address
t_host_status icmp_probe(const char *ip) {
    int sockfd;
    struct sockaddr_in target_addr;
    struct icmp icmp_hdr;
    char packet[64];

	sockfd = create_icmp_socket();
    if (sockfd < 0) return HOST_DOWN;

	//Building ICMP header
    memset(&icmp_hdr, 0, sizeof(icmp_hdr));
    icmp_hdr.icmp_type = ICMP_ECHO;
    icmp_hdr.icmp_code = 0;
    icmp_hdr.icmp_id = getpid() & 0xFFFF;
    icmp_hdr.icmp_seq = 0;
    icmp_hdr.icmp_cksum = checksum(&icmp_hdr, sizeof(icmp_hdr));
    memcpy(packet, &icmp_hdr, sizeof(icmp_hdr));

	//Filling out target info
    target_addr.sin_family = AF_INET;
    target_addr.sin_addr.s_addr = inet_addr(ip);

	//Sending packet to target using socket
    if (sendto(sockfd, packet, sizeof(icmp_hdr), 0, (struct sockaddr *)&target_addr, sizeof(target_addr)) <= 0) {
        if (DEBUG_LEVEL >= LOW) printf("[ICMP]Failed to send packet\n");
		perror("Sendto failed");
        close(sockfd);
        return HOST_DOWN;
    }
	if (DEBUG_LEVEL >= MEDIUM) printf("[ICMP]Packet sent\n");

	//Returnin the host status based on the response
    return get_response_status(sockfd);
}