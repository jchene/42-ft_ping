/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sender.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchene <jchene@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 14:57:08 by jchene            #+#    #+#             */
/*   Updated: 2025/02/20 16:17:09 by jchene           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../ft_ping.h"

void* send_thread(void* arg) {
	int sequence = 0;
	struct icmphdr icmp_hdr;

	t_context* context = (t_context*)arg;
	int packet_size = context->opts.packet_size + sizeof(struct icmphdr);
	char packet[packet_size];
	t_packet_info packet_info;

	memset(&icmp_hdr, 0, sizeof(icmp_hdr));
	icmp_hdr.type = ICMP_ECHO;
	icmp_hdr.code = 0;
	icmp_hdr.un.echo.id = getpid() & 0xFFFF;

	while (mutex_get_running(context)) {
		icmp_hdr.un.echo.sequence = sequence++;
		icmp_hdr.checksum = checksum(&icmp_hdr, sizeof(icmp_hdr));
		memcpy(packet, &icmp_hdr, sizeof(icmp_hdr));
		if (sendto(context->sockfd, packet, packet_size, 0, (struct sockaddr*)&context->target_addr, sizeof(context->target_addr)) <= 0) {
			mutex_set_running(context, false);
			mutex_set_net_error(context, ERR_SENDTO_FAIL);
			break;
		}
		printf("Sent ICMP echo request to %s\n", context->opts.host);
		gettimeofday(&packet_info.send_time, NULL);
		packet_info.sequence = icmp_hdr.un.echo.sequence;
		mutex_set_packet_info(context, packet_info);
		context->packet_sent++;
		usleep(1000000);
	}
	return NULL;
}