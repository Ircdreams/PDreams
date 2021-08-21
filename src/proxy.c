/* src/proxy.c - Diverses commandes proxyscan
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
  char params[70], buff[500], iptest[500], reason[300];
  const char *ip = parv[1];
  unsigned char *a2, *b2 , *c2;
  int a=0, b=0, c=0, d=0, end = CurrentTS + 3600;
  struct tm *ntime = localtime(&tmt);
  
  if(!is_ip(ip)) return osntc(nick, "%s is not a valid IP address", ip);
  
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
    strcpy(reason, "You have a host listed in the DroneBL. For more information, visit http://dronebl.org/lookup?ip=");
    strcat(reason, parv[1]);
    putserv("%s " TOKEN_PRIVMSG " %s :[%02d:%02d:%02d] < \2\0036PROXY GLINE\2\3 *!*@%s",
                        os.num, bot.pchan, ntime->tm_hour, ntime->tm_min, ntime->tm_sec, parv[1]);
    putserv("%s GL * +*@%s 3600 %d %d :%s [Expire: %s]", bot.servnum, parv[1], tmt, end, reason,
            get_time(NULL,end));

    strcpy(params, "*@");
    strcat(params, parv[1]);

    osntc(nick,"Anonymous proxy: %s", params);
  } else {
    osntc(nick,"%s is not listed in DroneBL", parv[1]);
  }
    return 0;
}

int testdns(const char *parv)
{
  FILE* flux;
  time_t tmt = CurrentTS;
  char params[70], buff[500], iptest[500], reason[300];
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
    strcpy(reason, "You have a host listed in the DroneBL. For more information, visit http://dronebl.org/lookup?ip=");
    strcat(reason, parv);
    putserv("%s " TOKEN_PRIVMSG " %s :[%02d:%02d:%02d] < \2\0036PROXY GLINE\2\3 *!*@%s",
            os.num, bot.pchan, ntime->tm_hour, ntime->tm_min, ntime->tm_sec, parv);
    putserv("%s GL * +*@%s 3600 %d %d :%s [Expire: %s]", bot.servnum, parv, tmt, end, reason,
            get_time(NULL,end));

    strcpy(params, "*@");
    strcat(params, parv);

    oswallops("Anonymous proxy: %s", params);
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
      return osntc(nick, "Please specify an ip address.");

    if (!is_ip(ip) || strlen(ip) > 15)
      return osntc(nick, "\2%s\2 is not a valid IP address.", ip);

    if((trust = gettrusted(ip)))
      return osntc(nick, "The ip address \2%s\2 is already registered.", ip);

    add_trusted(ip);
    append_file(TRUSTED_FILE, ip);
    write_trusted();
    return osntc(nick, "The ip address \2%s\2 has been added.", ip);
  }
  if(!strcasecmp(arg, "DEL")) {
  
    if(!trusthead) return osntc(nick, "Trusted IP list is empty");
    if(parc < 2) return osntc(nick, "Please specify an ip address.");

    for(;tmp;tmp = save) {
      save = tmp->next;
                        if(!match(parv[2],tmp->ip)) {
        osntc(nick, "Ip-Trusted deleted: %s", tmp->ip);
        del_trusted(tmp->ip);
        count++;
        write_trusted();
      }
    }
    if(count) osntc(nick, "Total: %d deleted.");
    else osntc(nick, "Ip \2%s\2 is not on the list", ip);
    return 1;
  }
  if(!strcasecmp(arg, "LIST")) {

    if(!trusthead) return osntc(nick, "Trusted IP list is empty");

    osntc(nick, "Ip Trusted list:");
    for(;tmp;tmp = tmp->next)
     osntc(nick, "  \002%s\2", tmp->ip);
    return 1;
  }
  if(!strcasecmp(arg, "RAZ")) {

    if(!trusthead) return osntc(nick, "Trusted IP list is empty");

    for(;tmp;tmp = save) {
      save = tmp->next;
      osntc(nick, "Ip Trusted deleted:  \002%s\2", tmp->ip);
      del_trusted(tmp->ip);
      count++;
    }
    osntc(nick, "Total: %d deleted.");
    write_trusted();    
    return 1;
  }
  return syntax_cmd(nick, FindCoreCommand("trust"));
}

