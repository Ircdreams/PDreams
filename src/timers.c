/* src/timers.c - fonctions de controle appellées periodiquement
 * Copyright (C) 2004 ircdreams.org
 *
 * contact: bugs@ircdreams.org
 * site web: http://ircdreams.org
 *
 * Services pour serveur IRC. Supporté sur IrcDreams V.2
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
 * $Id: timers.c,v 1.1.1.1 2005/05/28 20:44:12 bugs Exp $
 */

#include "main.h"
#include "config.h"
#include "outils.h"
#include "hash.h"
#include "del_info.h"
#include "fichiers.h"
#include "os_cmds.h"
#include "debug.h"

void exec_timers(void) 
{ 
        bot.lasttime = CurrentTS;       /* mise à jour des infos  */ 
        bot.lastbytes = bot.dataQ;      /* pour les stats traffic */ 
}
