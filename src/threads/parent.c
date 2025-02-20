/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parent.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchene <jchene@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 14:54:58 by jchene            #+#    #+#             */
/*   Updated: 2025/02/20 16:15:08 by jchene           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../ft_ping.h"

static t_context init_context(t_options opts) {
	t_context context;
	struct sockaddr_in target_addr;
	int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

	if (sockfd < 0) {
		context.net_error = ERR_SOCK_CREAT_FAIL;
		return context;
	}
	if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &opts.ttl, sizeof(opts.ttl)) < 0) {
		context.net_error = ERR_SETSOCKOPT_FAIL;
		return context;
	}
	context.sockfd = sockfd;
	memset(&target_addr, 0, sizeof(target_addr));
	target_addr.sin_family = AF_INET;
	if (inet_pton(AF_INET, opts.host, &target_addr.sin_addr) <= 0) {
		struct hostent* host = gethostbyname(opts.host);
		if (!host) {
			close(sockfd);
			context.net_error = ERR_UNKNOWN_HOST;
			return context;
		}
		memcpy(&target_addr.sin_addr, host->h_addr, host->h_length);
	}
	context.target_addr = target_addr;
	context.opts = opts;
	context.running = true;
	context.packet_sent = 0;
	context.packet_received = 0;
	pthread_mutex_init(&context.running_lock, NULL);
	pthread_mutex_init(&context.err_lock, NULL);
	pthread_mutex_init(&context.list_lock, NULL);
	return context;
}

t_context create_threads(t_options opts) {
	t_context context;
	pthread_t send_thread_id, recv_thread_id;
	struct timeval first_send_time;
	char test = 0;

	(void)first_send_time;
	context = init_context(opts);
	if (context.net_error)
		return context;

	if (pthread_create(&send_thread_id, NULL, send_thread, &context) != 0) {
		mutex_set_running(&context, false);
		mutex_set_net_error(&context, ERR_THREAD_CREAT_FAIL);
		return context;
	}
	if (pthread_create(&recv_thread_id, NULL, receive_thread, &context) != 0) {
		mutex_set_running(&context, false);
		mutex_set_net_error(&context, ERR_THREAD_CREAT_FAIL);
		pthread_join(send_thread_id, NULL);
		return context;
	}
	while (mutex_get_running(&context))
	{
		if (!test && mutex_get_list_size(&context)){
			first_send_time = mutex_get_packet_info_by_index(&context, 0).send_time;
			test = 1;
		}
	}
	
	return context;
}