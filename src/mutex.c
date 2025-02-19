/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mutex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchene <jchene@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/19 16:11:38 by jchene            #+#    #+#             */
/*   Updated: 2025/02/19 16:46:13 by jchene           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"

bool mutex_get_running(t_context *context) {
	bool running;

	pthread_mutex_lock(&context->running_lock);
	running = context->running;
	pthread_mutex_unlock(&context->running_lock);
	return running;
}

void mutex_set_running(t_context *context, bool running) {
	pthread_mutex_lock(&context->running_lock);
	context->running = running;
	pthread_mutex_unlock(&context->running_lock);
}

t_err mutex_get_net_error(t_context *context) {
	t_err net_error;

	pthread_mutex_lock(&context->err_lock);
	net_error = context->net_error;
	pthread_mutex_unlock(&context->err_lock);
	return net_error;
}

void mutex_set_net_error(t_context *context, t_err net_error) {
	pthread_mutex_lock(&context->err_lock);
	context->net_error = net_error;
	pthread_mutex_unlock(&context->err_lock);
}

t_packet_info mutex_get_packet_info(t_context *context, int sequence){
	t_packet_info packet_info;

	pthread_mutex_lock(&context->list_lock);
	for (int i = 0; i < context->packet_list_size; i++){
		if (context->packet_list[i].sequence == sequence){
			packet_info = context->packet_list[i];
			break;
		}
	}
	pthread_mutex_unlock(&context->list_lock);
	return packet_info;
}

void mutex_set_packet_info(t_context *context, int sequence, t_packet_info packet_info){
	unsigned oldest;
	pthread_mutex_lock(&context->list_lock);
	if (context->packet_list_size == PACKET_LIST_SIZE){
		oldest = get_oldest_packet(context);
		context->packet_list[oldest] = packet_info;
	}
	else{
		context->packet_list[context->packet_list_size] = packet_info;
		context->packet_list_size++;
	}
	pthread_mutex_unlock(&context->list_lock);
}