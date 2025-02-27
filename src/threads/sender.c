/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sender.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchene <jchene@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 14:57:08 by jchene            #+#    #+#             */
/*   Updated: 2025/02/27 18:32:43 by jchene           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../ft_ping.h"

static void build_packet(char* packet, int packet_size, int sequence) {
	struct icmphdr icmp_hdr;

	memset(packet, 0, packet_size);
	memset(&icmp_hdr, 0, sizeof(icmp_hdr));
	icmp_hdr.type = ICMP_ECHO;
	icmp_hdr.code = 0;
	icmp_hdr.un.echo.id = getpid() & 0xFFFF;
	icmp_hdr.un.echo.sequence = sequence;
	icmp_hdr.checksum = checksum(&icmp_hdr, sizeof(icmp_hdr));
	memcpy(packet, &icmp_hdr, sizeof(icmp_hdr));
}

void* send_thread(void* arg) {
	t_context* context = (t_context*)arg;
	int packet_size = context->opts.packet_size + sizeof(struct icmphdr);
	char packet[packet_size];
	t_packet_info packet_info;

	while (mutex_get_running(context)) {
		build_packet(packet, packet_size, context->stats.sent);
		if (sendto(context->sockfd, packet, packet_size, 0, (struct sockaddr*)&context->target_addr, sizeof(context->target_addr)) <= 0) {
			mutex_set_running(context, false);
			mutex_set_ctx_error(context, ERR_SENDTO_FAIL);
			break;
		}
		gettimeofday(&packet_info.send_time, NULL);
		packet_info.sequence = context->stats.sent;
		mutex_set_packet_info(context, packet_info);
		context->stats.sent++;
		usleep(1000000);
	}
	return NULL;
}