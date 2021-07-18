/* src/lang.c - Gestion du multilangage
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
 * $Id: lang.c,v 1.1.1.1 2005/05/28 20:44:12 bugs Exp $
 */

#include "main.h"
#include "outils.h"
#include "debug.h"

int LangCount = 0;

Lang *lang_isloaded(const char *name)
{
	Lang *lang = DefaultLang;
	for(;lang && strcasecmp(lang->langue, name);lang=lang->next);
	return lang;
}

int lang_add(char *name)
{
	Lang *lang = lang_isloaded(name);
	FILE *f;
	char path[501];
	int i = 0, items = 0;

	if(!lang)
	{
		Lang *temp = DefaultLang;
		lang = calloc(1, sizeof(Lang));
		if(!(lang = calloc(1, sizeof *lang)))
		{
			Debug(W_MAX, "lang_add, malloc a échoué pour Lang %s", name);
			return -1;
		}
		lang->next = NULL;
		Strncpy(lang->langue, name, 15);

		if(!DefaultLang) DefaultLang = lang;
		else
		{
			for(i = 1;temp->next;temp = temp->next, ++i); /* ajout à la fin */
			if(temp) temp->next = lang;
		}
		LangCount = lang->id = i;
	}
	strlwr(name);
	snprintf(path, sizeof path, LANG_PATH "/%s.lang", name);
	if(!(f = fopen(path, "r")))
	{
		Debug(W_TTY|W_WARN, "lang: fichier %s.lang non trouvé à '%s'", name, path);
		return -1;
	}

	while(fgets(path, sizeof path, f))
	{
		char *msg = strchr(path, ' '); /* découpe 'id msg'*/
		int msgid = 0, size;

		if(path[0] == '#' || !msg) continue;
		else *msg++ = 0;

		if(!is_num(path)) continue;
		msgid = atoi(path);
		if(msgid >= LANGMSGNB)
		{
			printf("lang: ID de msg %d inconnu lors du chargement du langage %s\n", msgid, name);
			continue;
		}
		strip_newline(msg);
		if(lang->msg[msgid] && !strcmp(msg, lang->msg[msgid])) continue;
		size = strlen(msg);
		if(size + 1 > LANGMSGMAX)
		{
			printf("lang: langage %s: msg %d trop long (%d) a été tronqué,"
				"cela peut causer des erreurs au runtime\n", name, msgid, size);
			size = LANGMSGMAX;
		}
		if(lang != DefaultLang && lang->msg[msgid] == DefaultLang->msg[msgid]) 
                           lang->msg[msgid] = NULL; /* it's an alias to the default one if was missing. */ 

		if(!(lang->msg[msgid] = realloc(lang->msg[msgid], size + 1)))
		{
			Debug(W_MAX, "lang_add, malloc a échoué pour Lang %s id %d", name, msgid);
			fclose(f);
			return -1;
		}
		Strncpy(lang->msg[msgid], msg, size);
		items++;
	}

	if(items < LANGMSGNB) /* hum uncomplet language set.. */ 
		for(i = 0;i < LANGMSGNB;++i)
                        if(!lang->msg[i]) lang->msg[i] = DefaultLang->msg[i]; 
                        /* .. alias all missing replies to default language. */ 
	fclose(f);
	return 1;
}
