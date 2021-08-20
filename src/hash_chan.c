/* src/hash_chan.c - gestion des hash chan
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
#include "hash.h"
#include "outils.h"
#include "debug.h"
#include "del_info.h"
#include "os_cmds.h"
#include "fichiers.h"

aChan *getchaninfo(const char *chan)
{
	unsigned int hash = do_hashc(chan);
	register aChan *tmp = chan_tab[hash];
	for(;tmp && strcasecmp(chan, tmp->chan);tmp = tmp->next);
	return tmp;
}
