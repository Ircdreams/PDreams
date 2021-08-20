/* src/add_info.c - Ajout d'informations en mémoire
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

#include "main.h"
#include "debug.h"
#include "outils.h"
#include "config.h"
#include "hash.h"
#include "add_info.h"
#include "os_cmds.h"
#include "del_info.h"
#include "serveur.h"

int add_server(const char *name, const char *num, const char *hop, const char *proto, const char *from) 
{ 
        aServer *links = calloc(1, sizeof *links); 
        unsigned int servindex;
	
        if(!links) 
                return Debug(W_MAX|W_WARN, "m_server, malloc() a échoué pour le Link %s", name); 
    
        Strncpy(links->serv, name, HOSTLEN); 
        Strncpy(links->num, num, NUMSERV); 
        links->maxusers = base64toint(num + NUMSERV); 
        servindex = base64toint(links->num); 
 
        for(links->smask = 16;links->smask < links->maxusers; links->smask <<= 1); 
        links->smask--;/* on passe de 010000 à 001111 */ 
 
        if(serv_tab[servindex]) return Debug(W_DESYNCH|W_WARN, "m_server: collision, " 
                                                                        "le serveur %s[%s] existe déjà?", name, links->num); 
 
        links->hub = num2servinfo(from); /* ptr vers son Hub */ 
        serv_tab[servindex] = links; 
 
        if(!(num_tab[servindex] = calloc(1, (links->maxusers+1) * sizeof(aNick *)))) 
                return Debug(W_MAX|W_WARN, "m_server, malloc() a échoué pour les %d Offsets de %s", 
                                links->maxusers+1, name); 
        /* 1erhub = my uplink -> je suis enregistré! */ 
        if(atoi(hop) == 1 && strcasecmp(name, bot.server)) mainhub = links; 
 
        links->flag = (*proto == 'J') ? ST_BURST : ST_ONLINE; 
        return 0; 
} 

void add_trusted(const char *ip)
{
        struct trusted *ptr = malloc(sizeof *ptr);

        if(!ptr)
        {
	        Debug(W_MAX, "add_trusted, malloc a échoué pour l'ip %s", ip);
                return;
        }

        Strncpy(ptr->ip, ip, 15);
        ptr->next = trusthead;
        trusthead = ptr;
}

void add_join(aNick *nick, const char *chan, int status, time_t timestamp, aChan *c)
{
	aJoin *join = calloc(1, sizeof *join);

	if(!join)
	{
		Debug(W_MAX|W_WARN, "add_join, malloc a échoué pour aJoin de %s sur %s", nick->nick, chan);
		return;
	}

	Strncpy(join->channel, chan, CHANLEN);
	join->status = (status & ~J_BURST);
	join->timestamp = timestamp;
	join->nick = nick;
	join->next = nick->joinhead; /* insert at top of the linked list */
	nick->joinhead = join;

	if(status & J_BURST) return; /* it's a Burst, nothing more to do.. */

	/* Perform priviledged(access-ed) user checks */
	
	if(nick->flag & N_HIDE) return;        

	return;
}
