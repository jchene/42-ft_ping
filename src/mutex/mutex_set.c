/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mutex_set.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchene <jchene@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 14:51:08 by jchene            #+#    #+#             */
/*   Updated: 2025/02/21 12:33:59 by jchene           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../ft_ping.h"

void mutex_set_running(t_context *context, bool running) {
	pthread_mutex_lock(&context->running_lock);
	context->running = running;
	pthread_mutex_unlock(&context->running_lock);
}

void mutex_set_ctx_error(t_context *context, t_err ctx_error) {
	pthread_mutex_lock(&context->err_lock);
	context->ctx_error = ctx_error;
	pthread_mutex_unlock(&context->err_lock);
}

void mutex_set_packet_info(t_context *context, t_packet_info packet_info){
	unsigned oldest;
	pthread_mutex_lock(&context->list_lock);
	if (context->packet_list_size == PACKET_LIST_MAX_SIZE){
		oldest = get_oldest_packet(context);
		context->packet_list[oldest] = packet_info;
	}
	else{
		context->packet_list[context->packet_list_size] = packet_info;
		context->packet_list_size++;
	}
	pthread_mutex_unlock(&context->list_lock);
}
