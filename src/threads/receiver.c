/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   receiver.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchene <jchene@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 14:59:24 by jchene            #+#    #+#             */
/*   Updated: 2025/02/27 18:32:28 by jchene           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../ft_ping.h"

static t_err read_socket(t_context* context, t_packet_context* packet_context) {
	int recv_len = recvfrom(context->sockfd, packet_context->buffer, sizeof(packet_context->buffer), 0, &packet_context->sender_addr, &packet_context->sender_addr_len);
	if (recv_len < 0) {
		mutex_set_running(context, false);
		mutex_set_ctx_error(context, ERR_RECVFROM_FAIL);
		return ERR_RECVFROM_FAIL;
	}
	packet_context->recv_len = recv_len;
	return ERR_NO_ERR;
}

static void update_stats(t_packet_stats* stats, double time) {
	double delta, delta2;

	if (time < stats->min_time || !stats->received)
		stats->min_time = time;
	if (time > stats->max_time || !stats->received)
		stats->max_time = time;
	stats->received++;
	delta = time - stats->avg_time;
	stats->avg_time += delta / stats->received;
	delta2 = time - stats->avg_time;
	stats->dif_squares_sum += delta * delta2;
}

static t_err read_response(t_context* context) {
	t_packet_context packet_context = { 0 };
	struct ip* ip_header;
	int ip_header_len;
	struct icmphdr* icmp_header;
	struct timeval send_time;
	struct timeval recieve_time;
	double round_trip_time = 0.0;
	t_err err;
	char host[NI_MAXHOST];
	int getname_ret;

	packet_context.sender_addr.sin_family = AF_INET;
	packet_context.sender_addr_len = sizeof(packet_context.sender_addr);
	err = read_socket(context, &packet_context);
	if (err != ERR_NO_ERR)
		return err;

	ip_header = (struct ip*)packet_context.buffer;
	ip_header_len = ip_header->ip_hl * 4;
	if (packet_context.recv_len >= ip_header_len + (int)sizeof(struct icmphdr)) {
		icmp_header = (struct icmphdr*)(packet_context.buffer + ip_header_len);
		if (context->opts.num_output)
			printf("%d bytes from %s: ", packet_context.recv_len - ip_header_len, inet_ntoa(ip_header->ip_src));
		else {
			getname_ret = getnameinfo((struct sockaddr*)&packet_context.sender_addr, packet_context.sender_addr_len,
				host, sizeof(host),
				NULL, 0,
				NI_NAMEREQD);
			if (getname_ret) {
				printf("err: %s\n", gai_strerror(getname_ret));
				printf("%d bytes from %s: ", packet_context.recv_len - ip_header_len, inet_ntoa(ip_header->ip_src));
			}
			else
				printf("%d bytes from %s (%s): ", packet_context.recv_len - ip_header_len, host, inet_ntoa(context->target_addr.sin_addr));
		}
		if (icmp_header->type == ICMP_ECHOREPLY) {
			gettimeofday(&recieve_time, NULL);
			send_time = mutex_get_packet_info_by_seq(context, icmp_header->un.echo.sequence).send_time;
			round_trip_time = (recieve_time.tv_sec - send_time.tv_sec) * 1000.0 + (recieve_time.tv_usec - send_time.tv_usec) / 1000.0;
			if (round_trip_time <= (double)context->opts.packet_timeout * 1000.0) {
				printf("icmp_seq=%d ttl=%d time time=%.3f ms\n", icmp_header->un.echo.sequence, ip_header->ip_ttl, round_trip_time);
				update_stats(&context->stats, round_trip_time);
			}
		}
		else if (icmp_header->type == ICMP_DEST_UNREACH || icmp_header->type == ICMP_TIME_EXCEEDED) {
			printf("%s\n", icmp_header->type == ICMP_DEST_UNREACH ? "Destination Net Unreachable" : "Time Exceeded");
		}
		else
			printf("Unknown ICMP type: %d\n", icmp_header->type);
	}
	return ERR_NO_ERR;
}

void* receive_thread(void* arg)
{
	t_context* context = (t_context*)arg;
	struct timeval select_timeout = { 0, 1000 };
	int select_ret;
	fd_set read_fds;

	printf("FT_PING %s (%s): %d data bytes\n", context->opts.host, inet_ntoa(context->target_addr.sin_addr), context->opts.packet_size);
	while (mutex_get_running(context)) {
		FD_ZERO(&read_fds);
		FD_SET(context->sockfd, &read_fds);

		select_ret = select(context->sockfd + 1, &read_fds, NULL, NULL, &select_timeout);
		if (!select_ret)
			continue;
		if (select_ret < 0) {
			mutex_set_running(context, false);
			mutex_set_ctx_error(context, ERR_SELECT_FAIL);
			break;
		}
		if (FD_ISSET(context->sockfd, &read_fds)) {
			if (read_response(context) != ERR_NO_ERR)
				break;
		}
		usleep(PRGM_SLEEP);
	}
	return NULL;
}