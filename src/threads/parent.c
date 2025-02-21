/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parent.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchene <jchene@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 14:54:58 by jchene            #+#    #+#             */
/*   Updated: 2025/02/21 16:27:31 by jchene           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../ft_ping.h"

t_context *get_context(){
	static t_context context;
	return &context;
}

static int create_socket(t_options opts) {
	int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

	if (sockfd < 0)
		return ERR_SOCK_CREAT_FAIL * -1;
	if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &opts.ttl, sizeof(opts.ttl)) < 0)
		return ERR_SETSOCKOPT_FAIL * -1;
	return sockfd;
}

static t_context init_context(t_options opts) {
	t_context context;
	struct sockaddr_in target_addr;
	
	context.sockfd = create_socket(opts);
	if (context.sockfd < 0) {
		context.ctx_error = ERR_SOCK_CREAT_FAIL;
		return context;
	}
	memset(&target_addr, 0, sizeof(target_addr));
	target_addr.sin_family = AF_INET;
	if (inet_pton(AF_INET, opts.host, &target_addr.sin_addr) <= 0) {
		struct hostent* host = gethostbyname(opts.host);
		if (!host) {
			close(context.sockfd);
			context.ctx_error = ERR_UNKNOWN_HOST;
			return context;
		}
		memcpy(&target_addr.sin_addr, host->h_addr, host->h_length);
	}
	context.target_addr = target_addr;
	context.opts = opts;
	context.running = true;
	context.stats.sent = 0;
	context.stats.received = 0;
	pthread_mutex_init(&context.running_lock, NULL);
	pthread_mutex_init(&context.err_lock, NULL);
	pthread_mutex_init(&context.list_lock, NULL);
	return context;
}

static void create_threads(t_context* context) {
	if (pthread_create(&context->send_thread_id, NULL, send_thread, context) != 0) {
		mutex_set_running(context, false);
		mutex_set_ctx_error(context, ERR_THREAD_CREAT_FAIL);
		return;
	}
	if (pthread_create(&context->recv_thread_id, NULL, receive_thread, context) != 0) {
		mutex_set_running(context, false);
		mutex_set_ctx_error(context, ERR_THREAD_CREAT_FAIL);
		pthread_join(context->send_thread_id, NULL);
		return;
	}
}

static void stop_threads(t_context* context) {
	pthread_join(context->send_thread_id, NULL);
	pthread_join(context->recv_thread_id, NULL);
	close(context->sockfd);
}

t_context *parent_thread(t_options opts) {
	t_context *context = get_context();
	struct timeval first_send_time;
	bool init_ok = FALSE;
	struct sigaction sa;

    sa.sa_handler = handle_sigint;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        context->ctx_error = ERR_SIGACTION_FAIL;
		return context;
    }

	(void)first_send_time;
	*context = init_context(opts);
	if (context->ctx_error)
		return context;

	create_threads(context);
	if (context->ctx_error)
		return context;
	
	while (mutex_get_running(context)){
		if (!init_ok && mutex_get_list_size(context)){
			first_send_time = mutex_get_packet_info_by_index(context, 0).send_time;
			init_ok = TRUE;
		}
	}
	stop_threads(context);
	return context;
}