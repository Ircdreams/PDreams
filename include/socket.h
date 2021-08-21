/* include/socket.h
 * Copyright (C) 2021 ircdreams.org - bugsounet.fr
 *
 * contact: bugsounet@bugsounet.fr
 * site web: http://www.bugsounet.fr
 *
 * Services pour serveur IRC. Supporté sur IrcDreams V3
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef HAVEINC_socket
#define HAVEINC_socket

#define TCPWINDOWSIZE 513 /* à augmenter si le réseau augmente de taille */
#define MAXPARA 16

#define SOCK_REGISTER(fd) do { 	\
		if((fd) > highsock) highsock = (fd); \
		FD_SET((fd), &global_fd_set);		\
} while(0)
  
extern fd_set global_fd_set; 
extern int highsock; 


extern int init_bot(unsigned long int);
extern int run_bot(int);

#endif /*HAVEINC_socket*/
