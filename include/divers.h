/* include/divers.h
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

#ifndef HAVEINC_divers
#define HAVEINC_divers

extern int uptime(aNick *, aChan *, int, char **);
extern int ctcp_ping(aNick *, aChan *, int, char **);
extern int ctcp_version(aNick *, aChan *, int, char **);
extern int version(aNick *, aChan *, int, char **);
extern int show_admins(aNick *, aChan *, int, char **);

#endif /*HAVEINC_divers*/
