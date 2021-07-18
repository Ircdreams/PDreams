/* include/add_info.h - Ajout d'informations en mémoire
 * Copyright (C) 2004 ircdreams.org
 *
 * contact: bugs@ircdreams.org
 * site web: http://www.ircdreams.org
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
 * $Id: add_info.h,v 1.1.1.1 2005/05/28 20:44:12 bugs Exp $
 */

#ifndef HAVEINC_add_info
#define HAVEINC_add_info

extern int add_server(const char *, const char *, const char *, const char *, const char *);

extern void add_trusted(const char *);
extern void add_join(aNick *, const char *, int, time_t, aChan *);

#endif /*HAVEINC_add_info*/
