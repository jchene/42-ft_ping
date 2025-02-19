/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchene <jchene@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/19 16:25:59 by jchene            #+#    #+#             */
/*   Updated: 2025/02/19 16:56:56 by jchene           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"

unsigned get_oldest_packet(t_context* context) {
	unsigned oldest = 0;
	unsigned i = 0;
	int min = context->packet_list[0].send_time.tv_sec + 1000000 * context->packet_list[0].send_time.tv_usec;
	while (i < context->packet_list_size) {
		if (context->packet_list[i].send_time.tv_sec + 1000000 * context->packet_list[i].send_time.tv_usec < min) {
			//CLEAN OLD ENOUGH PACKETS?
			min = context->packet_list[i].send_time.tv_sec + 1000000 * context->packet_list[i].send_time.tv_usec;
			oldest = i;
		}
		i++;
	}
	return oldest;
}