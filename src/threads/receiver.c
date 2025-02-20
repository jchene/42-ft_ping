/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   receiver.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchene <jchene@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 14:59:24 by jchene            #+#    #+#             */
/*   Updated: 2025/02/20 22:06:46 by jchene           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../ft_ping.h"

void* receive_thread(void* arg)
{
	t_context* context = (t_context*)arg;
	struct sockaddr_in sender_addr;
	socklen_t sender_addr_len = sizeof(sender_addr);
	char buffer[1024];
	int recv_len;
	int select_ret;
	int ip_header_len;
	fd_set read_fds;
	struct timeval timeout;
	struct timeval recieve_time;
	struct timeval send_time;
	struct ip* ip_header;
	struct icmphdr* icmp_header;

	while (mutex_get_running(context))
	{
		FD_ZERO(&read_fds);
		FD_SET(context->sockfd, &read_fds);
		timeout.tv_sec = 0;
		timeout.tv_usec = 1000;

		select_ret = select(context->sockfd + 1, &read_fds, NULL, NULL, &timeout);
		if (select_ret < 0) {
			mutex_set_running(context, false);
			mutex_ser_ctx_error(context, ERR_SELECT_FAIL);
			break;
		}
		else if (select_ret == 0) 
			continue;

		if (FD_ISSET(context->sockfd, &read_fds))
		{
			recv_len = recvfrom(context->sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&sender_addr, &sender_addr_len);
			if (recv_len < 0)
			{
				mutex_set_running(context, false);
				mutex_ser_ctx_error(context, ERR_RECVFROM_FAIL);
				break;
			}
			ip_header = (struct ip*)buffer;
			ip_header_len = ip_header->ip_hl * 4;
			if (recv_len >= ip_header_len + (int)sizeof(struct icmphdr))
			{
				icmp_header = (struct icmphdr*)(buffer + ip_header_len);
				if (icmp_header->type == ICMP_ECHOREPLY) {
					printf("Received ICMP echo reply from %s\n", inet_ntoa(sender_addr.sin_addr));
					gettimeofday(&recieve_time, NULL);
					printf("packet seq: %d\n", icmp_header->un.echo.sequence);
					send_time = mutex_get_packet_info_by_seq(context, icmp_header->un.echo.sequence).send_time;
					if (recieve_time.tv_sec - send_time.tv_sec <= context->opts.packet_timeout) {
						printf("Round trip time: %ld ms\n", (recieve_time.tv_sec - send_time.tv_sec) * 1000 + (recieve_time.tv_usec - send_time.tv_usec) / 1000);
						context->packet_received++;
					}
				}
			}
		}
		usleep(1000);
	}
	return NULL;
}