/* src/hash_cmd.c - gestion de la hash des commandes
 * Copyright (C) 2021 ircdreams.org - bugsounet.fr
 *
 * contact: bugsounet@bugsounet.fr
 * site web: http://www.bugsounet.fr
 *
 * Services pour serveur IRC. Support? sur IrcDreams V3
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
#include "outils.h"
#include "debug.h"
#include "hash.h"
#include <ctype.h>

aHashCmd *cmd_hash[CMDHASHSIZE] = {0};
int CmdsCount = 0;

static inline unsigned int do_hashcmd(const char *cmd)
{
	unsigned int checksum = 0;
	while(*cmd) checksum += (checksum << 3) + tolower(*cmd++);
	return checksum & (CMDHASHSIZE-1);
}

static int hashcmd_add(aHashCmd *cmd)
{
	unsigned int hash = do_hashcmd(cmd->name);
	cmd->next = cmd_hash[hash];
	cmd_hash[hash] = cmd;
	return 0;
}

static int hashcmd_del(unsigned int hash, aHashCmd *cmd)
{
	register aHashCmd *tmp = cmd_hash[hash];

	if(tmp == cmd) cmd_hash[hash] = tmp->next;
	else
	{
		for(;tmp && tmp->next != cmd;tmp = tmp->next);
		if(tmp) tmp->next = cmd->next;
		else Debug(W_WARN|W_MAX, "hashcmd_del %s non trouv? ? l'offset %u ?!", cmd->corename, hash);
	}
	return 0;
}

int HashCmd_switch(aHashCmd *cmd, const char *newname)
{
	hashcmd_del(do_hashcmd(cmd->name), cmd);
	Strncpy(cmd->name, newname, CMDLEN);
	hashcmd_add(cmd);
	return 0;
}

int RegisterCmd(const char *corename, int level, int flag, int args,
	int (*func) (aNick *, aChan *, int, char **))
{
	aHashCmd *cmd = calloc(1, sizeof *cmd);

	if(!cmd || !(cmd->help = calloc(1, sizeof *cmd->help)))
		return Debug(W_MAX|W_WARN, "RegisterCmd, malloc a ?chou? pour aHashCmd %s", corename);
	Strncpy(cmd->corename, corename, CMDLEN);
	strcpy(cmd->name, cmd->corename); /* load, on met name = corename*/
	cmd->flag = flag;
	cmd->args = args;
	cmd->level = level;
	cmd->func = func;
	hashcmd_add(cmd);
	if(*corename != 1) ++CmdsCount;
	return 0;
}

aHashCmd *FindCommand(const char *name)
{
	register aHashCmd *tmp = cmd_hash[do_hashcmd(name)];

	for(;tmp && strcasecmp(name, tmp->name);tmp = tmp->next);
	return tmp;
}

aHashCmd *FindCoreCommand(const char *name)
{
	register aHashCmd *tmp;
	int i = 0;

	for(;i < CMDHASHSIZE;++i)
		for(tmp = cmd_hash[i];tmp;tmp = tmp->next)
			if(!strcasecmp(tmp->corename, name)) return tmp;

	return NULL;
}
