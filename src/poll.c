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

#include "dnsmasq.h"

/* Wrapper for poll(). Allocates and extends array of struct pollfds,
   keeps them in fd order so that we can set and test conditions on
   fd using a simple but efficient binary chop. */

/* poll_reset()
   poll_listen(fd, event)
   .
   .
   poll_listen(fd, event);

   hits = do_poll(timeout);

   if (poll_check(fd, event)
    .
    .

   if (poll_check(fd, event)
    .
    .

    event is OR of POLLIN, POLLOUT, POLLERR, etc
*/

//两分法查找fd数组（数组内按fd从小到大排列）
static struct pollfd *pollfds = NULL;
static nfds_t nfds/*pollfds数组有效大小*/, arrsize = 0/*pollfds数组实际内存大小*/;

/* Binary search. Returns either the pollfd with fd, or
   if the fd doesn't match, or return equals nfds, the entry
   to the left of which a new record should be inserted. */
//查找fd的索引（采用二分法查找）
static nfds_t fd_search(int fd)
{
  nfds_t left, right, mid;
  
  if ((right = nfds) == 0)
    return 0;//无注册的fd,直接返回0
  
  left = 0;
  
  while (1)
    {
	  /*此返回值返回的值，并不保证索引对应的为fd对应索引*/
      if (right == left + 1)
	return (pollfds[left].fd >= fd) ? left : right;
      
      mid = (left + right)/2;//两分法，取中间
      
      if (pollfds[mid].fd > fd)
	right = mid;//正确的值在左侧
      else 
	left = mid;//正确的值在右侧
    }
}

//清空pollfds
void poll_reset(void)
{
  nfds = 0;
}

//对已注册的fd执行poll,超时时间为timeout
int do_poll(int timeout)
{
  return poll(pollfds, nfds, timeout);
}

//检查给定fd是否触发了event事件
int poll_check(int fd, short event)
{
  nfds_t i = fd_search(fd);
  
  //fd索引有效，且查找正确，则检查event是否已发生
  if (i < nfds && pollfds[i].fd == fd)
    return pollfds[i].revents & event;

  return 0;
}

//注册fd的事件event
void poll_listen(int fd, short event)
{
   nfds_t i = fd_search(fd);
  
   //fd已注册，仅或上关注的事件
   if (i < nfds && pollfds[i].fd == fd)
     pollfds[i].events |= event;
   else
     {
	   //fd未注册，如果内存足够，则将i位置后的fd后移一格，将i位置空开
       if (arrsize != nfds)
	 memmove(&pollfds[i+1], &pollfds[i], (nfds - i) * sizeof(struct pollfd));
       else
	 {
    	//fd未注册，且内存不足，需要realloc，再完成i位置空开
	   /* Array too small, extend. */
	   struct pollfd *new;

	   arrsize = (arrsize == 0) ? 64 : arrsize * 2;

	   //重新申请一块内存
	   if (!(new = whine_malloc(arrsize * sizeof(struct pollfd))))
	     return;

	   if (pollfds)
	     {
		   //之前有内存，则复制旧数组，空开i位置
	       memcpy(new, pollfds, i * sizeof(struct pollfd));
	       memcpy(&new[i+1], &pollfds[i], (nfds - i) * sizeof(struct pollfd));
	       free(pollfds);
	     }
	   
	   pollfds = new;
	 }
       
       //将要注册的fd存放在i位置处，并初始化
       pollfds[i].fd = fd;
       pollfds[i].events = event;
       nfds++;
     }
}
