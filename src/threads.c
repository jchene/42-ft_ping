/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchene <jchene@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/19 14:41:57 by jchene            #+#    #+#             */
/*   Updated: 2025/02/19 16:36:25 by jchene           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"

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
	pthread_mutex_init(&context.running_lock, NULL);
	pthread_mutex_init(&context.err_lock, NULL);
	pthread_mutex_init(&context.list_lock, NULL);
	return context;
}

static void *send_thread(void *arg) {
	int sequence = 0;
	struct icmphdr icmp_hdr;

	t_context *context = (t_context *)arg;
	int packet_size = context->opts.packet_size + sizeof(struct icmphdr);
	char packet[packet_size];

	memset(&icmp_hdr, 0, sizeof(icmp_hdr));
	icmp_hdr.type = ICMP_ECHO;
	icmp_hdr.code = 0;
	icmp_hdr.un.echo.id = getpid() & 0xFFFF;
	icmp_hdr.un.echo.sequence = sequence++;
	icmp_hdr.checksum = checksum(&icmp_hdr, sizeof(icmp_hdr));
	memcpy(packet, &icmp_hdr, sizeof(icmp_hdr));
	while (mutex_get_running(context)) {
		if (sendto(context->sockfd, packet, packet_size, 0, (struct sockaddr *)&context->target_addr, sizeof(context->target_addr)) <= 0) {
			mutex_set_running(context, false);
			mutex_set_net_error(context, ERR_SENDTO_FAIL);
			break;
		}
		usleep(1000000);
		icmp_hdr.un.echo.sequence = sequence++;
		icmp_hdr.checksum = checksum(&icmp_hdr, sizeof(icmp_hdr));
		memcpy(packet, &icmp_hdr, sizeof(icmp_hdr));
	}
	return NULL;
}

static void *receive_thread(void *arg) {
	t_context *context = (t_context *)arg;
	struct sockaddr_in sender_addr;
	socklen_t sender_addr_len = sizeof(sender_addr);
	char buffer[1024];
	int recv_len;

	
	while (mutex_get_running(context)) {
		recv_len = recvfrom(context->sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&sender_addr, &sender_addr_len);
		if (recv_len < 0) {
			mutex_set_running(context, false);
			mutex_set_net_error(context, ERR_RECVFROM_FAIL);
			break;
		}
	}
	return NULL;
}

t_context create_threads(t_options opts) {
	t_context context;
	pthread_t send_thread_id, recv_thread_id;

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
	return context;
}