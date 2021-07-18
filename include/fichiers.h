/* include/fichiers.h - Lecture/Écriture des données
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
 * $Id: fichiers.h,v 1.2 2005/06/19 11:24:05 bugs Exp $
 */

#ifndef HAVEINC_fichier
#define HAVEINC_fichier

#define DBVERSION_U 1

extern int db_load_users(void);
extern int db_write_users(void);

extern int write_anuser(anUser *);
extern void write_cmds(void);
extern int load_cmds(void);
extern void write_trusted(void);
extern int load_trusted(void);

extern void conv_char(const char *, char *);    
extern void File_UserDel(char *); 
extern int write_files(aNick *, aChan *, int, char **);
extern void append_file(const char *, const char *);

#endif /*HAVEINC_fichier*/
