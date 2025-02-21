/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchene <jchene@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/19 16:25:59 by jchene            #+#    #+#             */
/*   Updated: 2025/02/21 16:39:20 by jchene           ###   ########.fr       */
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

static double get_stddev(const t_packet_stats *stats) {
	double variance;

	if (stats->received > 1)
		variance = stats->dif_squares_sum / (stats->received - 1);
	else
		variance = 0.0;
    return sqrt(variance);
}

void print_stats(t_packet_stats* stats) {
	printf("--- %s ft_ping statistics ---\n", get_context()->opts.host);
	printf("%d packets transmitted, %d received, %.0f%% packet loss\n", 
		stats->sent, stats->received, 100.0 - (stats->sent/ stats->received * 100.0));
	printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n", 
		stats->min_time, stats->avg_time, stats->max_time, get_stddev(stats));
}

void handle_sigint(int sig){
	(void)sig;
	t_context* context = get_context();
	mutex_set_running(context, false);
	print_stats(&context->stats);
}