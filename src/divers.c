/* src/divers.c - Diverses commandes
 * Copyright (C) 2021 ircdreams.org - bugsounet.fr
 *
 * contact: bugsounet@bugsounet.fr
 * site web: http://www.bugsounet.fr
 *
 * Services pour serveur IRC. Support� sur IrcDreams V3
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
#include "divers.h"
#include "outils.h"
#include "hash.h"
#include "del_info.h"
#include "os_cmds.h"
#include "admin_manage.h"

int uptime(aNick *nick, aChan *chaninfo, int parc, char **parv)
{
	return osntc(nick, "Service are UP since %s", duration(CurrentTS - bot.uptime));
}

int ctcp_ping(aNick *nick, aChan *chaninfo, int parc, char **parv)
{
	if(parc) osntc(nick, "\1PING %s", parv[1]);
	return 1;
}

int ctcp_version(aNick *nick, aChan *chaninfo, int parc, char **parv)
{
    osntc(nick, "\1VERSION PDreams Service [" SPVERSION "] �bugsounet.fr (Build: " __DATE__ " "__TIME__ ")\1");
	return 1;
}

int version(aNick *nick, aChan *chaninfo, int parc, char **parv)
{
	osntc(nick, "PDreams Service [" SPVERSION "] �bugsounet (Build: " __DATE__ " "__TIME__ ")");
        return 1;
}

int show_admins(aNick *nick, aChan *chaninfo, int parc, char **parv)
{
	int i = 0;
	anUser *u;
	osntc(nick, "\2Here  Level  Username       Nickname\2");
	for(;i < USERHASHSIZE;++i) for(u = user_tab[i];u;u = u->next)
		if(IsAdmin(u)) osntc(nick, "\2\003%s\2\3 %d     %-13s    \0032%s\3",
				u->n ? "3YES" : "4NO", u->level, u->nick, u->n ? u->n->nick : "");
	return 1;
}
