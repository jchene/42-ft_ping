/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   receiver.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchene <jchene@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 14:59:24 by jchene            #+#    #+#             */
/*   Updated: 2025/02/20 16:12:51 by jchene           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../ft_ping.h"

void* receive_thread(void* arg) {
	t_context* context = (t_context*)arg;
	struct sockaddr_in sender_addr;
	socklen_t sender_addr_len = sizeof(sender_addr);
	char buffer[1024];
	int recv_len;
	struct timeval recieve_time;
	struct timeval send_time;

	while (mutex_get_running(context)) {
		recv_len = recvfrom(context->sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&sender_addr, &sender_addr_len);
		if (recv_len < 0) {
			mutex_set_running(context, false);
			mutex_set_net_error(context, ERR_RECVFROM_FAIL);
			break;
		}
		struct ip* ip_header = (struct ip*)buffer;
		int ip_header_len = ip_header->ip_hl * 4;
		if (recv_len >= ip_header_len + (int)sizeof(struct icmphdr)) {
			struct icmphdr* icmp_header = (struct icmphdr*)(buffer + ip_header_len);

			if (icmp_header->type == ICMP_ECHOREPLY) {
				printf("Received ICMP echo reply from %s\n", inet_ntoa(sender_addr.sin_addr));
				gettimeofday(&recieve_time, NULL);
				printf("packet seq: %d\n", icmp_header->un.echo.sequence);
				send_time = mutex_get_packet_info_by_seq(context, icmp_header->un.echo.sequence).send_time;
				if (recieve_time.tv_sec - send_time.tv_sec > context->opts.packet_timeout) {
					printf("Warning: Packet timeout\n");
				}
				else {
					printf("Round trip time: %ld ms\n", (recieve_time.tv_sec - send_time.tv_sec) * 1000 + (recieve_time.tv_usec - send_time.tv_usec) / 1000);
				}
			}
		}
		else {
			printf("Warning: Received incomplete ICMP packet\n");
		}
	}
	return NULL;
}