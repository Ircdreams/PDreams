/* src/debug.c
 * Copyright (C) 2004-2005 ircdreams.org
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
 * $Id: debug.c,v 1.1.1.1 2005/05/28 20:44:12 bugs Exp $
 */

#include "main.h"
#include "debug.h"
#include "os_cmds.h"
#include "outils.h"

int Debug(int flag, const char *format, ...)
{
	FILE *fp = fopen(LOG_ERREURS, "a");
	char buf[512];
	int i = 0;
	va_list va;

	if(flag & W_PROTO) buf[i++] = 'P';
	if(flag & W_DESYNCH) buf[i++] = 'D';
	if(flag & W_MAX) buf[i++] = 'M';
	buf[i++] = ' ';

	va_start(va, format);
	vsnprintf(buf + i, sizeof buf - i, format, va);
	va_end(va);
	if(flag & W_WARN && bot.sock) oswallops("%s", buf);
	if(flag & W_TTY) puts(buf);
	if(fp)
	{
		fputc('[', fp);
		fputs(get_time(NULL, CurrentTS), fp);
		fputs("] ", fp);
		fputs(buf, fp);
		fputc('\n', fp);
		fclose(fp);
	}
	return 0;
}
