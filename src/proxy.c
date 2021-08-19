/* src/proxy.c - Diverses commandes proxyscan
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
 * $Id: proxy.c,v 1.4 2005/07/23 09:12:51 bugs Exp $
 */

#include "main.h"
#include "os_cmds.h"
#include "outils.h"
#include "add_info.h"
#include "fichiers.h"
#include "hash.h"
#include "del_info.h"
#include "aide.h"
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>

int dnscheck(aNick *nick, aChan *chaninfo, int parc, char **parv)
{
        FILE* flux;
        time_t tmt = CurrentTS;
        char params[70], buff[500], iptest[500], enculeraison[300];
        const char *ip = parv[1];
        unsigned char *a2, *b2 , *c2;
        int a=0, b=0, c=0, d=0, end = CurrentTS + 3600;
        struct tm *ntime = localtime(&tmt);

        if(!is_ip(ip)) return osntc(nick, "%s n'est pas une adresse IP valide", ip);

        a2 = strchr(ip,'.');
        a = atoi(ip);

        ip = a2 + 1;
        b2 = strchr(ip,'.');
        b = atoi(ip);

        ip = b2+ 1;
        c2 = strchr(ip,'.');
        c = atoi(ip);

        ip = c2 + 1;
        d = atoi(ip);

	snprintf(iptest, 500, "host %d.%d.%d.%d.dnsbl.dronebl.org", d, c, b, a);
        flux = popen(iptest, "r");
        fgets(buff, 500, flux);
        pclose(flux);
        if (!match("*127.0.0.*", buff)) {
		strcpy(enculeraison, "You have a host listed in the DroneBL. For more information, visit http://dronebl.org/lookup?ip=");
		strcat(enculeraison, parv[1]);
		putserv("%s " TOKEN_PRIVMSG " %s :[%02d:%02d:%02d] < \2\0036PROXY GLINE\2\3 *!*@%s",
                        os.num, bot.pchan, ntime->tm_hour, ntime->tm_min, ntime->tm_sec, parv[1]);
                putserv("%s GL * +*@%s 3600 %d %d :%s [Expire %s]", bot.servnum, parv[1], tmt, end, enculeraison,
                        get_time(NULL,end));

                strcpy(params, "*@");
                strcat(params, parv[1]);

		osntc(nick,"Proxy Anomyme: %s", params);
	} else {
		osntc(nick,"Aucune entrée pour l'IP %s", parv[1]);
	}
        return 0;
}

int testdns(const char *parv)
{
        FILE* flux;
        time_t tmt = CurrentTS;
        char params[70], buff[500], iptest[500], enculeraison[300];
        const char *ip = parv;
        unsigned char *a2, *b2 , *c2;
        int a=0, b=0, c=0, d=0, end = CurrentTS + 3600;
        struct tm *ntime = localtime(&tmt);

        a2 = strchr(ip,'.');
        a = atoi(ip);

        ip = a2 + 1;
        b2 = strchr(ip,'.');
        b = atoi(ip);

        ip = b2+ 1;
        c2 = strchr(ip,'.');
        c = atoi(ip);

        ip = c2 + 1;
        d = atoi(ip);

        snprintf(iptest, 500, "host %d.%d.%d.%d.dnsbl.dronebl.org", d, c, b, a);
        flux = popen(iptest, "r");
        fgets(buff, 500, flux);
        pclose(flux);
        if (!match("*127.0.0.*", buff)) {
		strcpy(enculeraison, "You have a host listed in the DroneBL. For more information, visit http://dronebl.org/lookup?ip=");
                strcat(enculeraison, parv);
                putserv("%s " TOKEN_PRIVMSG " %s :[%02d:%02d:%02d] < \2\0036PROXY GLINE\2\3 *!*@%s",
                        os.num, bot.pchan, ntime->tm_hour, ntime->tm_min, ntime->tm_sec, parv);
                putserv("%s GL * +*@%s 3600 %d %d :%s [Expire %s]", bot.servnum, parv, tmt, end, enculeraison,
                        get_time(NULL,end));

                strcpy(params, "*@");
                strcat(params, parv);

		oswallops("Proxy Anonyme: %s", params);
        }
        return 0;
}

int trust(aNick *nick, aChan *chaninfo, int parc, char **parv)
{
	char *ip = parv[2], *arg = parv[1];
	int count = 0;
	struct trusted *trust, *tmp = trusthead, *save;
	
        if(!strcasecmp(arg, "ADD")) {

		if(parc < 2)
                return osntc(nick, "Veuillez preciser une adresse ip.");

		if (!is_ip(ip) || strlen(ip) > 15)
		return osntc(nick, "L'adresse ip \2%s\2 n'est pas valide.", ip);

        	if((trust = gettrusted(ip)))
        	       	return osntc(nick, "L'adresse ip \2%s\2 est déjà couverte.", ip);

		add_trusted(ip);
		append_file(TRUSTED_FILE, ip);
		write_trusted();
		return osntc(nick, "L'adresse ip \2%s\2 a bien été ajoutée", ip);
	}
	if(!strcasecmp(arg, "DEL")) {
	
		if(!trusthead) return osntc(nick, "La liste des Ips-Trusted est vide!");
		if(parc < 2) return osntc(nick, "Veuillez preciser une adresse ip.");

		for(;tmp;tmp = save) {
			save = tmp->next;
                        if(!match(parv[2],tmp->ip)) {
				osntc(nick, "Ip-Trusted retiré: %s", tmp->ip);
				del_trusted(tmp->ip);
				count++;
				write_trusted();
			}
		}
		if(count) osntc(nick, "Total: %d effacé%s.", count, count > 1 ? "s" : "");
		else osntc(nick, "L'Ips \2%s\2 n'est pas dans la liste", ip);
		return 1;
	}
	if(!strcasecmp(arg, "LIST")) {

	        if(!trusthead) return osntc(nick, "La liste des Ips-Trusted est vide!");

        	osntc(nick, "Liste des Ips-Ttrusted :");
        	for(;tmp;tmp = tmp->next)
                	osntc(nick, "  \002%s\2", tmp->ip);
    		return 1;
	}
	if(!strcasecmp(arg, "RAZ")) {

                if(!trusthead) return osntc(nick, "La liste des Ips-Trusted est vide!");

                for(;tmp;tmp = save) {
			save = tmp->next;
                        osntc(nick, "Ips-Trusted supprimé:  \002%s\2", tmp->ip);
			del_trusted(tmp->ip);
			count++;
		}
		osntc(nick, "Total: %d effacé%s.", count, count > 1 ? "s" : "");
		write_trusted();		
                return 1;
        }
	return syntax_cmd(nick, FindCoreCommand("trust"));
}

