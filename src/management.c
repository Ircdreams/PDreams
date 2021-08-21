/* src/operserv.c - Diverses commandes operserv pour admins
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
 */

#include <ctype.h>
#include "main.h"
#include "debug.h"
#include "os_cmds.h"
#include "hash.h"
#include "outils.h"
#include "management.h"
#include "add_info.h"
#include "del_info.h"
#include "fichiers.h"
#include "aide.h"
#include "divers.h"
#include "admin_manage.h"
#include "config.h"
#include "showcommands.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int first_register(aNick *nick, aChan *chaninfo, int parc, char **parv)
{
        anUser *user = NULL;
	aNick *n;
	aHashCmd *cmdp = FindCommand("REGISTER");

	if (!(n = getnickbynick(parv[1])))
                return osntc(nick, "%s n'est pas connecté.", parv[1]);

        if(!(n->flag & N_OPER))
		return osntc(nick, "Commande réservé aux Ircops !");

 	if(!GetConf(CF_PREMIERE))
                return osntc(nick, "Commande réservé au premier lancement!");

        if((user = getuserinfo(parv[1])))
		return osntc(nick, "%s est déjà enregistré.", user->nick);

        user = add_regnick(parv[1], 5);

        osntc(nick, "Bienvenue sur les Services PDreams [" SPVERSION "] !");
        osntc(nick, "Vous êtes Administrateurs de niveau maximum.");

	ConfFlag &= ~CF_PREMIERE;
        cmdp->flag |= CMD_DISABLE;

	load_cmds();
        BuildCommandsTable(1);
        write_cmds();
	db_write_users();

	user = getuserinfo(parv[1]);
	nick->user = user;
        nick->user->n = nick;

        return 1;
}

int user(aNick *nick, aChan *chaninfo, int parc, char **parv)
{
	char *cmd = parv[1];
	anUser *user = NULL;
	int level = 0, i = 0;

	if(!strcasecmp(cmd, "ADD")) {
		if (parc < 3)
			return osntc(nick, "Syntaxe: USER ADD <username> <level>");
		if((user = getuserinfo(parv[2])))
			return osntc(nick, "%s est déjà enregistré.", user->nick);

		if(!is_num(parv[3]) || (level = atoi(parv[3])) > MAXADMLVL || level < 1)
			return osntc(nick, "Le level doit être compris entre 1 et %d", MAXADMLVL);

		if(level >= nick->user->level && nick->user->level != MAXADMLVL)
                	return osntc(nick, "Vous ne pouvez pas donner un niveau supérieur au votre.");

		user = add_regnick(parv[2], atoi(parv[3]));
		db_write_users();

		osntc(nick, "%s a été enregistré au level %d", user->nick, user->level);
	}
	else if(!strcasecmp(cmd, "DEL")) {
		if (parc < 2)
                        return osntc(nick, "Syntaxe: USER DEL <username>");
		if(!(user = getuserinfo(parv[2])))
                        return osntc(nick, "%s n'est pas dans la base de donnée", parv[2]);
		if(level >= nick->user->level && nick->user->level != MAXADMLVL)
                        return osntc(nick, "Vous ne pouvez pas donner un niveau supérieur au votre.");
		if(user->n)
			osntc(user->n, "Votre compte vient d'être supprimé par l'Administrateur %s", nick->nick);

		osntc(nick, "Le compte %s a bien été supprimé.", user->nick);
		del_regnick(user);
	}
	else if(!strcasecmp(cmd, "LIST")) {
		osntc(nick, "\2Niveau  Username\2");
		for(;i < USERHASHSIZE;++i) for(user = user_tab[i];user;user = user->next)
		osntc(nick, "%d       %-13s", user->level, user->nick);
	}
	else if(!strcasecmp(cmd, "LEVEL")) {
		if (parc < 3)
                        return osntc(nick, "Syntaxe: USER LEVEL <username> <level>");
		if(!is_num(parv[3]) || (level = atoi(parv[3])) > MAXADMLVL || level < 0)
		return osntc(nick, "Veuillez préciser un level valide.");

		if(!(user = getuserinfo(parv[2])))
			return osntc(nick, "\2%s\2 n'est pas un UserName enregistré.", parv[2]);

		if(level >= nick->user->level && nick->user->level != MAXADMLVL)
			return osntc(nick, "Vous ne pouvez pas donner un niveau supérieur au votre.");

		if(level == 0)
                        return osntc(nick, "Un Administrateur doit avoir un niveau > 0");
 
		if(level == user->level)
			return osntc(nick, "%s est déjà au niveau %d.", user->nick, level);
	
		if(level < ADMINLEVEL)
		{
			osntc(nick, "%s n'est plus Administrateur.", user->nick);
			if(user->n) adm_active_del(user->n);
		}
		else if(user->level < ADMINLEVEL)
		{
			osntc(nick, "%s est maintenant Administrateur au niveau\2 %d\2.", user->nick, level);
			if(user->n) adm_active_del(user->n);
		}
		else osntc(nick, "Vous avez modifié le niveau Administrateur de \2%s\2 en\2 %d\2.",
			user->nick, level);

		user->level = level;
		db_write_users();
	}
	else return osntc(nick, "Option inconnue: %s", cmd);
	return 1;
}

