/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sender.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchene <jchene@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 14:57:08 by jchene            #+#    #+#             */
/*   Updated: 2025/02/28 15:28:33 by jchene           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../ft_ping.h"

uint32_t get_src_ip(void) {
	struct ifaddrs* ifaddr, * ifa;
	uint32_t src_ip = 0;
	char addr[INET_ADDRSTRLEN];

	if (getifaddrs(&ifaddr) == -1)
		return 0x7F000001;
	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr == NULL)
			continue;
		if (ifa->ifa_addr->sa_family == AF_INET) {
			struct sockaddr_in* sa = (struct sockaddr_in*)ifa->ifa_addr;
			inet_ntop(AF_INET, &(sa->sin_addr), addr, INET_ADDRSTRLEN);
			if (strcmp(ifa->ifa_name, "lo") != 0) {
				src_ip = sa->sin_addr.s_addr;
				break;
			}
		}
	}
	freeifaddrs(ifaddr);
	return src_ip;
}

static void build_packet(char* packet, int packet_size, t_context *context,
	uint32_t src_addr)
{
	struct iphdr* ip_hdr = (struct iphdr*)packet;
	struct icmphdr* icmp_hdr = (struct icmphdr*)(packet + sizeof(struct iphdr));

	memset(packet, 0, packet_size);
	ip_hdr->ihl = 5;
	ip_hdr->version = 4;
	ip_hdr->tos = 0;
	ip_hdr->tot_len = htons(packet_size);
	ip_hdr->id = htons((getpid() + context->stats.sent) & 0xFFFF);
	ip_hdr->frag_off = 0;
	ip_hdr->ttl = context->opts.ttl ? context->opts.ttl : DEFAULT_TTL;
	ip_hdr->protocol = IPPROTO_ICMP;
	ip_hdr->saddr = src_addr;
	ip_hdr->daddr = context->target_addr.sin_addr.s_addr;
	ip_hdr->check = 0;
	ip_hdr->check = checksum(ip_hdr, sizeof(struct iphdr));

	icmp_hdr->type = ICMP_ECHO;
	icmp_hdr->code = 0;
	icmp_hdr->un.echo.id = getpid() & 0xFFFF;
	icmp_hdr->un.echo.sequence = context->stats.sent;
	icmp_hdr->checksum = 0;
	icmp_hdr->checksum = checksum(icmp_hdr, sizeof(struct icmphdr));
}


void* send_thread(void* arg) {
	t_context* context = (t_context*)arg;
	int packet_size = sizeof(struct iphdr) + context->opts.packet_size + sizeof(struct icmphdr);
	char packet[packet_size];
	t_packet_info packet_info;

	while (mutex_get_running(context)) {
		build_packet(packet, packet_size, context, get_src_ip());
		packet_info.ip_header = (struct iphdr*)packet;
		packet_info.icmp_header = (struct icmphdr*)(packet + sizeof(struct iphdr));
		if (sendto(context->sockfd, packet, packet_size, 0, (struct sockaddr*)&context->target_addr, sizeof(context->target_addr)) <= 0) {
			mutex_set_running(context, false);
			mutex_set_ctx_error(context, ERR_SENDTO_FAIL);
			break;
		}
		gettimeofday(&packet_info.send_time, NULL);
		packet_info.sequence = context->stats.sent;
		mutex_set_packet_info(context, packet_info);
		context->stats.sent++;
		usleep(1000000);
	}
	return NULL;
}