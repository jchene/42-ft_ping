/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mutex_get.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchene <jchene@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 14:51:29 by jchene            #+#    #+#             */
/*   Updated: 2025/02/20 16:11:08 by jchene           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../ft_ping.h"

bool mutex_get_running(t_context* context) {
	bool running;

	pthread_mutex_lock(&context->running_lock);
	running = context->running;
	pthread_mutex_unlock(&context->running_lock);
	return running;
}

t_err mutex_get_net_error(t_context* context) {
	t_err net_error;

	pthread_mutex_lock(&context->err_lock);
	net_error = context->net_error;
	pthread_mutex_unlock(&context->err_lock);
	return net_error;
}

unsigned mutex_get_list_size(t_context* context) {
	unsigned size;

	pthread_mutex_lock(&context->list_lock);
	size = context->packet_list_size;
	pthread_mutex_unlock(&context->list_lock);
	return size;
}

t_packet_info mutex_get_packet_info_by_seq(t_context* context, unsigned sequence) {
	t_packet_info packet_info;

	pthread_mutex_lock(&context->list_lock);
	for (unsigned i = 0; i < context->packet_list_size; i++) {
		if (context->packet_list[i].sequence == sequence) {
			packet_info = context->packet_list[i];
			break;
		}
	}
	pthread_mutex_unlock(&context->list_lock);
	return packet_info;
}

t_packet_info mutex_get_packet_info_by_index(t_context* context, unsigned index) {
	t_packet_info packet_info;

	pthread_mutex_lock(&context->list_lock);
	packet_info = context->packet_list[index];
	pthread_mutex_unlock(&context->list_lock);
	return packet_info;
}