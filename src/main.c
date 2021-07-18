/* src/main.c - Fichier principal
 * Copyright (C) 2005 ircdreams.org
 *
 * contact: bugs@ircdreams.org
 * site web: http://ircdreams.org
 *
 * Services pour serveur IRC. Support� sur IrcDreams V.2
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
 * $Id: main.c,v 1.3 2005/06/19 11:24:05 bugs Exp $
 */

#include "main.h"
#include "config.h"
#include "debug.h"
#include "hash.h"
#include "outils.h"
#include "serveur.h"
#include "fichiers.h"
#include "admin_cmds.h"
#include "admin_manage.h"
#include "divers.h"
#include "showcommands.h"
#include "aide.h"
#include "os_cmds.h"
#include "timers.h"
#include "socket.h"
#include "management.h"
#include "proxy.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>

int ConfFlag = 0;
int running = 1;
int deconnexion = 0;
time_t CurrentTS = 0;
int complete = 0;

struct ignore *ignorehead = NULL;
struct trusted *trusthead = NULL;

struct robot bot;
struct bots os;

aChan *chan_tab[CHANHASHSIZE] = {0}; 	/* hash chan*/
anUser *user_tab[USERHASHSIZE] = {0}; 	/* hash username*/
aNick **num_tab[MAXNUM] = {0}; 			/* table num -> struct nick*/
aServer *serv_tab[MAXNUM] = {0}; 		/* table numserv -> serv info*/
aNick *nick_tab[NICKHASHSIZE] = {0}; 	/* hash nick*/

void sig_die(int c)
{
	Debug(W_WARN|W_MAX, "SIGTERM re�u! -- abandon..");
	running = 0;
}

void sig_restart(int c) 
{ 
        Debug(W_WARN|W_MAX, "SIGINT re�u! -- Restarting.."); 
        running = 0; 
        ConfFlag |= CF_RESTART; 
 
        putserv("%s "TOKEN_QUIT" :Restarting", os.num); 
        putserv("%s "TOKEN_SQUIT" %s 0 :SIGINT/RESTART", bot.servnum, bot.server); 
} 

void sig_reload (int c)
{
	Debug(W_WARN, "Signal HUP re�u!");
	signal(SIGHUP, &sig_reload);
}

int main(int argc, char **argv)
{
	int restarted = 0, silent = 0, tmp = 0, background = 1;
	FILE *fd; /* pid & uptime */
    struct rlimit rlim; /* used for core size */

	CurrentTS = time(NULL);
	memset(&bot, 0, sizeof bot);

	while((tmp = getopt(argc, argv, "hn")) != EOF)
		switch(tmp)
		{
			case 'n':
				background = 0;
				break;
			case 'h':
				silent = 1;
				break;
			default:
				printf("Syntaxe: %s [-hn]\n", argv[0]);
				exit(EXIT_FAILURE);
		}

	chdir(BINDIR); /* on se place dans le r�pertoire principal de PDreams */

	if((fd = fopen(PDREAMS_PID, "r")))
	{
		if(fscanf(fd, "%d", &tmp) == 1)
		{
			fprintf(stderr, "PDreams est d�j� lanc� sur le pid %d.\n"
			"Si ce n'est pas le cas, supprimez le fichier '"PDREAMS_PID"' et recommencez.\n", tmp);
		}
		fclose(fd);
		exit(EXIT_FAILURE);
	}

	if(load_config(FICHIER_CONF) == -1)
	{
		fputs("Erreur lors de la lecture de la configuration\n", stderr);
		exit(EXIT_FAILURE);
	}

	if((fd = fopen("uptime.tmp", "r")))
	{
		fscanf(fd, "%lu", (unsigned long *) &bot.uptime);
		fclose(fd);
		remove("uptime.tmp");
		restarted = 1;
	}
	else bot.uptime = CurrentTS;

	RegisterCmd("REGISTER",              0, CMD_NEEDNOAUTH, 1, first_register);
	RegisterCmd("USER",              3, CMD_ADMIN, 1, user);
	RegisterCmd("DIE", 		5, CMD_ADMIN, 0, die);
	RegisterCmd("REHASH",		5, CMD_ADMIN, 0, rehash_conf);
	RegisterCmd("RESTART", 		5, CMD_ADMIN, 0, restart_bot);
	RegisterCmd("CHCOMNAME", 	4, CMD_ADMIN, 2, chcomname);
	RegisterCmd("CHLEVEL", 		4, CMD_ADMIN, 2, chlevel);
	RegisterCmd("DISABLE", 		4, CMD_ADMIN, 1, disable_cmd);
	RegisterCmd("INVITEME",  	1, CMD_ADMIN, 0, inviteme);
	RegisterCmd("WRITE", 		4, CMD_ADMIN, 0, write_files);
	RegisterCmd("SHOWCONFIG", 	3, CMD_NEEDNOAUTH|CMD_ADMIN, 0, showconfig);
	RegisterCmd("AIDE",             0, CMD_NEEDNOAUTH, 0, aide);
	RegisterCmd("SHOWCOMMANDS",	0, CMD_NEEDNOAUTH, 0, showcommands);
	RegisterCmd("ADMIN", 		0, CMD_NEEDNOAUTH, 0, show_admins);
	RegisterCmd("UPTIME", 		0, CMD_NEEDNOAUTH, 0, uptime);
	RegisterCmd("\1PING\1",		0, CMD_NEEDNOAUTH, 0, ctcp_ping);
	RegisterCmd("\1VERSION\1", 	0, CMD_NEEDNOAUTH, 0, ctcp_version);
	RegisterCmd("VERSION",      	0, CMD_NEEDNOAUTH, 0, version);
	RegisterCmd("TRUST",            2, CMD_ADMIN, 1, trust);
	RegisterCmd("IPCHECK",		2, CMD_ADMIN, 1, dnscheck);

	tmp = load_cmds();
	if(!silent) printf("Chargement des commandes IRC... OK (%d)\n", tmp);
	BuildCommandsTable(0);
	tmp = db_load_users();
	if(!silent) printf("Base de donn�e User charg�e (%d)\n", tmp);
	tmp = load_trusted();
	if(!silent) printf("Chargement de trust... OK (%d)\n", tmp);


	help_load();

	signal(SIGPIPE, SIG_IGN);
	signal(SIGHUP, &sig_reload);
	signal(SIGINT, &sig_restart);
	signal(SIGTERM, &sig_die);

	if(!restarted && !silent) puts("Proxy Dreams " SPVERSION " � 2005 IrcDreams.org");
	if(background && (tmp = fork()) == -1)
	{
		Debug(W_TTY, "Impossible de se lancer en background.");
		exit(EXIT_FAILURE);
	}
	else if(background && tmp > 1) /* fork ok */
	{
		if(!restarted && !silent) puts("Lancement en background...");
		exit(0);
	}

	if(!getrlimit(RLIMIT_CORE, &rlim) && rlim.rlim_cur != RLIM_INFINITY)
	{
		Debug(W_TTY, "Core size limit�e � %ldk, changement en illimit�.", rlim.rlim_cur);
		rlim.rlim_cur = RLIM_INFINITY;
		rlim.rlim_max = RLIM_INFINITY;
		setrlimit(RLIMIT_CORE, &rlim);
	}

	if(GetConf(CF_PREMIERE))
		{
		printf("Premier lancement de PDreams. Merci de votre choix.\n");
		printf("\n");
		printf("Lorsque les services seront sur votre r�seau IRC,\n tapez : "
			"/%s %s <username>\n", os.nick, RealCmd("register"));
		printf("\n");
		printf("ATTENTION: PDreams utilise les comptes de SDreams. Il est donc imp�ratif d'utiliser les memes logins!\n");
		}

	FD_ZERO(&global_fd_set);

sprintf(os.num, "%sAAA", bot.servnum);

	if((fd = fopen(PDREAMS_PID, "w")))
	{
		fprintf(fd, "%d\n", (int) getpid());
		fclose(fd);
	}
	else Debug(W_TTY, "Impossible d'�crire le fichier PID. [%s]", strerror(errno));

	while(running)
	{
		unsigned long int irc_ip = inet_addr(bot.ip);

		if(irc_ip == INADDR_NONE) putlog(LOG_ERREURS, "DNS lockup a �chou� pour [%s]", bot.ip);
		else if(init_bot(irc_ip) < 0) putlog(LOG_ERREURS, "Connexion �chou�e.");
		else run_bot(bot.sock);

		if(running)
		{
			putlog(LOG_ERREURS, "D�connect� du serveur.");

			purge_nickandserv();

			deconnexion = 1;

			sleep(WAIT_CONNECT);
		}
	}
	write_cmds();
	remove(PDREAMS_PID);
	CleanUp();
	putlog(LOG_PARSES, "Fermeture normale du programme");
	if(GetConf(CF_RESTART)) execlp(argv[0], argv[0], "-h", NULL); /* restarting.. */
	return 0;
}
