/* include/config.h
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

#ifndef HAVEINC_config
#define HAVEINC_config

/* flags utilisés pour la config */
#define CF_PREMIERE 	0x1
#define CF_RESTART	0x2

#define EndOfTab(x) (x && *x == '}' && (*(x+1) == '\0' || *(x+1) == '\n' || *(x+1) == '\r'))

extern int clonemax;
extern char *quit_msg;
extern char *pasdeperm;
extern char *network;
extern char *mailprog;
extern char *scanmsg;
extern char *defraison;

extern int load_config(const char *);

#endif /*HAVEINC_config*/

