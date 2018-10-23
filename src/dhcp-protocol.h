/* dnsmasq is Copyright (c) 2000-2018 Simon Kelley

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 dated June, 1991, or
   (at your option) version 3 dated 29 June, 2007.
 
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
     
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#define DHCP_SERVER_PORT 67
#define DHCP_CLIENT_PORT 68
#define DHCP_SERVER_ALTPORT 1067
#define DHCP_CLIENT_ALTPORT 1068
#define PXE_PORT 4011

/* These each hold a DHCP option max size 255
   and get a terminating zero added */
#define DHCP_BUFF_SZ 256

#define BOOTREQUEST              1
#define BOOTREPLY                2
#define DHCP_COOKIE              0x63825363

/* The Linux in-kernel DHCP client silently ignores any packet 
   smaller than this. Sigh...........   */
#define MIN_PACKETSZ             300

//dhcp选项字段
#define OPTION_PAD               0
//网络掩码
#define OPTION_NETMASK           1
//网关地址选项
#define OPTION_ROUTER            3
//dns服务器地址选项
#define OPTION_DNSSERVER         6
#define OPTION_HOSTNAME          12
#define OPTION_DOMAINNAME        15
#define OPTION_BROADCAST         28
#define OPTION_VENDOR_CLASS_OPT  43
#define OPTION_REQUESTED_IP      50 
//租约时间选项
#define OPTION_LEASE_TIME        51
#define OPTION_OVERLOAD          52
//dhcp消息类型选项（例如discover,request等）
#define OPTION_MESSAGE_TYPE      53
#define OPTION_SERVER_IDENTIFIER 54
#define OPTION_REQUESTED_OPTIONS 55
#define OPTION_MESSAGE           56
#define OPTION_MAXMESSAGE        57
#define OPTION_T1                58
#define OPTION_T2                59
#define OPTION_VENDOR_ID         60
#define OPTION_CLIENT_ID         61
#define OPTION_SNAME             66
//为客户端分配的启动文件名称
#define OPTION_FILENAME          67
#define OPTION_USER_CLASS        77
#define OPTION_RAPID_COMMIT      80
#define OPTION_CLIENT_FQDN       81
#define OPTION_AGENT_ID          82
#define OPTION_ARCH              93
#define OPTION_PXE_UUID          97
#define OPTION_SUBNET_SELECT     118
#define OPTION_DOMAIN_SEARCH     119
#define OPTION_SIP_SERVER        120
#define OPTION_VENDOR_IDENT      124
#define OPTION_VENDOR_IDENT_OPT  125
#define OPTION_END               255

#define SUBOPT_CIRCUIT_ID        1
#define SUBOPT_REMOTE_ID         2
#define SUBOPT_SUBNET_SELECT     5     /* RFC 3527 */
#define SUBOPT_SUBSCR_ID         6     /* RFC 3393 */
#define SUBOPT_SERVER_OR         11    /* RFC 5107 */

#define SUBOPT_PXE_BOOT_ITEM     71    /* PXE standard */
#define SUBOPT_PXE_DISCOVERY     6
#define SUBOPT_PXE_SERVERS       8
#define SUBOPT_PXE_MENU          9
#define SUBOPT_PXE_MENU_PROMPT   10

#define DHCPDISCOVER             1
#define DHCPOFFER                2
#define DHCPREQUEST              3
#define DHCPDECLINE              4
#define DHCPACK                  5
#define DHCPNAK                  6
#define DHCPRELEASE              7
#define DHCPINFORM               8

#define BRDBAND_FORUM_IANA       3561 /* Broadband forum IANA enterprise */

#define DHCP_CHADDR_MAX 16

struct dhcp_packet {
  /**
   * op 报文的操作类型。分为请求报文和响应报文。1：请求报文，2：应答报文。即client送给server的封包，设为1，反之为2。
   * 请求报文：DHCP Discover、DHCP Request、DHCP Release、DHCP Inform和DHCP Decline。
   * 应答报文：DHCP Offer、DHCP ACK和DHCP NAK。
   * htype DHCP客户端的MAC地址类型。MAC地址类型其实是指明网络类型，Htype值为1时表示为最常见的以太网MAC地址类型。
   * Hlen：DHCP客户端的MAC地址长度。以太网MAC地址长度为6个字节，即以太网时Hlen值为6。
   * Hops：DHCP报文经过的DHCP中继的数目，默认为0。DHCP请求报文每经过一个DHCP中继，该字段就会增加1。
   *       没有经过DHCP中继时值为0。(若数据包需经过router传送，每站加1，若在同一网内，为0。)
   * Xid：客户端通过DHCP Discover报文发起一次IP地址请求时选择的随机数，相当于请求标识。
   *      用来标识一次IP地址请求过程。在一次请求中所有报文的Xid都是一样的。
   * Secs：DHCP客户端从获取到IP地址或者续约过程开始到现在所消耗的时间，以秒为单位。
   *       在没有获得IP地址前该字段始终为0。(DHCP客户端开始DHCP请求后所经过的时间。目前尚未使用，固定为0。)
   * Flags：标志位，只使用第0比特位，是广播应答标识位，用来标识DHCP服务器应答报文是采用单播还是广播发送，
   *        0表示采用单播发送方式，1表示采用广播发送方式。其余位尚未使用。(即从0-15bits，
   *        最左1bit为1时表示server将以广播方式传送封包给client。)
   *       【注意】在客户端正式分配了IP地址之前的第一次IP地址请求过程中，所有DHCP报文都是以广播方式发送的，
   *       包括客户端发送的DHCP Discover和DHCP Request报文，以及DHCP服务器发送的DHCP Offer、DHCP ACK
   *       和DHCP NAK报文。当然，如果是由DHCP中继器转的报文，则都是以单播方式发送的。另外，IP地址续约、IP
   *       地址释放的相关报文都是采用单播方式进行发送的。
   * Ciaddr：DHCP客户端的IP地址。仅在DHCP服务器发送的ACK报文中显示，在其他报文中均显示0，因为在得到DHCP
   *        服务器确认前，DHCP客户端是还没有分配到IP地址的。只有客户端是Bound、Renew、Rebinding状态，并且能响应ARP请求时，才能被填充。
   * Yiaddr：DHCP服务器分配给客户端的IP地址。仅在DHCP服务器发送的Offer和ACK报文中显示，其他报文中显示为0。
   * Siaddr：下一个为DHCP客户端分配IP地址等信息的DHCP服务器IP地址。仅在DHCP Offer、DHCP ACK报文中显示，其他报文中显示为0。(用于bootstrap过程中的IP地址)
   * giaddr：DHCP客户端发出请求报文后经过的第一个DHCP中继的IP地址。如果没有经过DHCP中继，则显示为0。(转发代理（网关）IP地址)
   * Chaddr：DHCP客户端的MAC地址。在每个报文中都会显示对应DHCP客户端的MAC地址。
   * Sname：为DHCP客户端分配IP地址的DHCP服务器名称（DNS域名格式）。在Offer和ACK报文中显示发送报文的DHCP服务器名称，其他报文显示为0。
   * File：DHCP服务器为DHCP客户端指定的启动配置文件名称及路径信息。仅在DHCP Offer报文中显示，其他报文中显示为空。
   * Options：可选项字段，长度可变，格式为"代码+长度+数据"。
   */
  u8 op, htype, hlen, hops;
  u32 xid;
  u16 secs, flags;
  struct in_addr ciaddr, yiaddr, siaddr, giaddr;
  u8 chaddr[DHCP_CHADDR_MAX], sname[64], file[128];
  //options的前４个字节是dhcp magic cookie,且是个常量值
  u8 options[312];
};
