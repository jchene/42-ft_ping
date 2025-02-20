/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchene <jchene@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/19 16:25:59 by jchene            #+#    #+#             */
/*   Updated: 2025/02/20 15:49:13 by jchene           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../ft_ping.h"

unsigned get_oldest_packet(t_context* context) {
	unsigned oldest = 0;
	unsigned i = 0;
	int min = context->packet_list[0].send_time.tv_sec + 1000000 * context->packet_list[0].send_time.tv_usec;
	while (i < context->packet_list_size) {
		if (context->packet_list[i].send_time.tv_sec + 1000000 * context->packet_list[i].send_time.tv_usec < min) {
			min = context->packet_list[i].send_time.tv_sec + 1000000 * context->packet_list[i].send_time.tv_usec;
			oldest = i;
		}
		i++;
	}
	return oldest;
}

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