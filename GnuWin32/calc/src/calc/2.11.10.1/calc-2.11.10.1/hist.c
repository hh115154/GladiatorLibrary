/*
 * hist - interactive readline module
 *
 * Copyright (C) 1999-2004  David I. Bell
 *
 * Calc is open software; you can redistribute it and/or modify it under
 * the terms of the version 2.1 of the GNU Lesser General Public License
 * as published by the Free Software Foundation.
 *
 * Calc is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU Lesser General
 * Public License for more details.
 *
 * A copy of version 2.1 of the GNU Lesser General Public License is
 * distributed with calc under the filename COPYING-LGPL.  You should have
 * received a copy with calc; if not, write to Free Software Foundation, Inc.
 * 59 Temple Place, Suite 330, Boston, MA  02111-1307, USA.
 *
 * @(#) $Revision: 29.6 $
 * @(#) $Id: hist.c,v 29.6 2004/02/23 14:04:01 chongo Exp $
 * @(#) $Source: /usr/local/src/cmd/calc/RCS/hist.c,v $
 *
 * Under source code control:	1993/05/02 20:09:19
 * File existed as early as:	1993
 *
 * Share and enjoy!  :-)	http://www.isthe.com/chongo/tech/comp/calc/
 */

/*
 * Adapted from code written by Stephen Rothwell.
 *
 * GNU readline support added by Martin Buck <mbuck@debian.org>
 *
 * Interactive readline module.	 This is called to read lines of input,
 * while using emacs-like editing commands within a command stack.
 * The key bindings for the editing commands are (slightly) configurable.
 */


#include <stdio.h>
#include <ctype.h>
#if !defined(_WIN32)
# include <pwd.h>
#endif

#include "have_unistd.h"
#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif

#include "have_stdlib.h"
#if defined(HAVE_STDLIB_H)
#include <stdlib.h>
#endif

#include "calc.h"
#include "hist.h"
#include "have_string.h"

#include "have_strdup.h"
#if !defined(HAVE_STRDUP)
# define strdup(x) calc_strdup((CONST char *)(x))
#endif /* HAVE_STRDUP */

#ifdef HAVE_STRING_H
# include <string.h>
#endif

#include "have_unused.h"

#if !defined(USE_READLINE)

extern FILE *curstream(void);

#define STDIN		0
#define SAVE_SIZE	256		/* size of save buffer */
#define MAX_KEYS	60		/* number of key bindings */


#define CONTROL(x)		((char)(((int)(x)) & 0x1f))

static	struct {
	char	*prompt;
	char	*buf;
	char	*pos;
	char	*end;
	char	*mark;
	int	bufsize;
	int	linelen;
	int	histcount;
	int	curhist;
	BOOL	virgin_line;	/* 1 => never typed chars, 0 => chars typed */
} HS;


typedef void (*FUNCPTR)();

typedef struct {
	char	*name;
	FUNCPTR func;
} FUNC;

/* declare binding functions */
static void flush_input(void);
static void start_of_line(void);
static void end_of_line(void);
static void forward_char(void);
static void backward_char(void);
static void forward_word(void);
static void backward_word(void);
static void delete_char(void);
static void forward_kill_char(void);
static void backward_kill_char(void);
static void forward_kill_word(void);
static void kill_line(void);
static void new_line(void);
static void save_line(void);
static void forward_history(void);
static void backward_history(void);
static void insert_char(int key);
static void goto_line(void);
static void list_history(void);
static void refresh_line(void);
static void swap_chars(void);
static void set_mark(void);
static void yank(void);
static void save_region(void);
static void kill_region(void);
static void reverse_search(void);
static void quote_char(void);
static void uppercase_word(void);
static void lowercase_word(void);
static void ignore_char(void);
static void arrow_key(void);
static void quit_calc(void);


static	FUNC	funcs[] =
{
	{"ignore-char",		ignore_char},
	{"flush-input",		flush_input},
	{"start-of-line",	start_of_line},
	{"end-of-line",		end_of_line},
	{"forward-char",	forward_char},
	{"backward-char",	backward_char},
	{"forward-word",	forward_word},
	{"backward-word",	backward_word},
	{"delete-char",		delete_char},
	{"forward-kill-char",	forward_kill_char},
	{"backward-kill-char",	backward_kill_char},
	{"forward-kill-word",	forward_kill_word},
	{"uppercase-word",	uppercase_word},
	{"lowercase-word",	lowercase_word},
	{"kill-line",		kill_line},
	{"goto-line",		goto_line},
	{"new-line",		new_line},
	{"save-line",		save_line},
	{"forward-history",	forward_history},
	{"backward-history",	backward_history},
	{"insert-char",		insert_char},
	{"list-history",	list_history},
	{"refresh-line",	refresh_line},
	{"swap-chars",		swap_chars},
	{"set-mark",		set_mark},
	{"yank",		yank},
	{"save-region",		save_region},
	{"kill-region",		kill_region},
	{"reverse-search",	reverse_search},
	{"quote-char",		quote_char},
	{"arrow-key",		arrow_key},
	{"quit",		quit_calc},
	{NULL,			NULL}
};


typedef struct key_ent	KEY_ENT;
typedef struct key_map	KEY_MAP;

struct key_ent	{
	FUNCPTR		func;
	KEY_MAP		*next;
};


struct key_map {
	char		*name;
	KEY_ENT		default_ent;
	KEY_ENT		*map[256];
};


static char	base_map_name[] = "base-map";
static char	esc_map_name[] = "esc-map";


static KEY_MAP	maps[] = {
	{base_map_name, {NULL, NULL}, {NULL, NULL}},
	{esc_map_name, {NULL, NULL}, {NULL, NULL}},
};


#define INTROUND	(sizeof(int) - 1)
#define HISTLEN(hp)	((((hp)->len + INTROUND) & ~INTROUND) + sizeof(int))
#define HISTOFFSET(hp)	(((char *) (hp)) - histbuf)
#define FIRSTHIST	((HIST *) histbuf)
#define NEXTHIST(hp)	((HIST *) (((char *) (hp)) + HISTLEN(hp)))


typedef struct {
	int	len;		/* length of data */
	char	data[1];	/* varying length data */
} HIST;


static	int		inited;
static	int		canedit;
static	int		histused;
static	int		key_count;
static	int		save_len;
static	KEY_MAP		*cur_map;
static	KEY_MAP		*base_map;
static	KEY_ENT		key_table[MAX_KEYS];
static	char		histbuf[HIST_SIZE + 1];
static	char		save_buffer[SAVE_SIZE];

/* declare other static functions */
static	FUNCPTR find_func(char *name);
static	HIST	*get_event(int n);
static	HIST	*find_event(char *pat, int len);
static	void	read_key(void);
static	void	erasechar(void);
static	void	newline(void);
static	void	backspace(void);
static	void	beep(void);
static	void	echo_char(int ch);
static	void	echo_string(char *str, int len);
static	void	savetext(char *str, int len);
static	void	memrcpy(char *dest, char *src, int len);
static	int	read_bindings(FILE *fp);
static	int	in_word(int ch);
static	KEY_MAP *find_map(char *map);
static	void	unbind_key(KEY_MAP *map, int key);
static	void	raw_bind_key(KEY_MAP *map, int key,
			     FUNCPTR func, KEY_MAP *next_map);
static	KEY_MAP *do_map_line(char *line);
static	void	do_default_line(KEY_MAP *map, char *line);
static	void	do_bind_line(KEY_MAP *map, char *line);
static	void	back_over_char(int ch);
static	void	echo_rest_of_line(void);
static	void	goto_start_of_line(void);
static	void	goto_end_of_line(void);
static	void	remove_char(int ch);
static	void	decrement_end(int n);
static	void	insert_string(char *str, int len);


/*
 * Read a line into the specified buffer.  The line ends in a newline,
 * and is NULL terminated.  Returns the number of characters read, or
 * zero on an end of file or error.  The prompt is printed before reading
 * the line.
 */
int
hist_getline(char *prompt, char *buf, int len)
{
	/*
	 * initialize if we have not already done so
	 */
	if (!inited)
		(void) hist_init(calcbindings);

	/*
	 * establish the beginning of a line condition
	 */
	HS.prompt = prompt;
	HS.bufsize = len - 2;
	HS.buf = buf;
	HS.pos = buf;
	HS.end = buf;
	HS.mark = NULL;
	HS.linelen = -1;
	HS.virgin_line = TRUE;

	/*
	 * prep the I/O
	 */
	fputs(prompt, stdout);
	fflush(stdout);

	/*
	 * special case: non-interactive editing
	 */
	if (!canedit) {
		if (fgets(buf, len, stdin) == NULL)
			return 0;
		return strlen(buf);
	}

	/*
	 * get the line
	 */
	while (HS.linelen < 0) {

		/* get the next char */
		read_key();

		/* chars typed, no longer virgin */
		HS.virgin_line = FALSE;
	}

	/*
	 * return the line
	 */
	return HS.linelen;
}


/*
 * Initialize the module by reading in the key bindings from the specified
 * filename, and then setting the terminal modes for noecho and cbreak mode.
 * If the supplied filename is NULL, then a default filename will be used.
 * We will search the CALCPATH for the file.
 *
 * Returns zero if successful, or a nonzero error code if unsuccessful.
 * If this routine fails, hist_getline, hist_saveline, and hist_term can
 * still be called but all fancy editing is disabled.
 */
int
hist_init(char *filename)
{
	/*
	 * prevent multiple initializations
	 */
	if (inited) {
		if (conf->calc_debug & CALCDBG_TTY)
			printf("hist_init: inited already set\n");
		return HIST_INITED;
	}

	/*
	 * setup
	 */
	inited = 1;
	canedit = 0;
	if (conf->calc_debug & CALCDBG_TTY)
		printf("hist_init: Set inited, cleared canedit\n");

	/*
	 * open the bindings file
	 */
	if (filename == NULL)
		filename = HIST_BINDING_FILE;
	if (opensearchfile(filename, calcpath, NULL, FALSE) > 0)
		return HIST_NOFILE;

	/*
	 * load the bindings
	 */
	if (read_bindings(curstream()))
		return HIST_NOFILE;

	/*
	 * close the bindings
	 */
	closeinput();

	/*
	 * setup the calc TTY on STDIN
	 */
	if (!calc_tty(STDIN)) {
		return HIST_NOTTY;
	}

	canedit = 1;
	if (conf->calc_debug & CALCDBG_TTY)
		printf("hist_init: Set canedit\n");

	return HIST_SUCCESS;
}


/*
 * Reset the terminal modes just before exiting.
 */
void
hist_term(void)
{
	if (!inited || !canedit) {
		if (conf->calc_debug & CALCDBG_TTY) {
			if (!inited)
				printf("hist_term: inited already cleared\n");
			if (!canedit)
				printf("hist_term: canedit already cleared\n");
		}
		inited = 0;
		if (conf->calc_debug & CALCDBG_TTY)
			printf("hist_term: Cleared inited\n");
		return;
	}

	/*
	 * restore orignal tty state
	 */
	(void) orig_tty(STDIN);
}


static KEY_MAP *
find_map(char *map)
{
	unsigned int i;

	for (i = 0; i < sizeof(maps) / sizeof(maps[0]); i++) {
		if (strcmp(map, maps[i].name) == 0)
			return &maps[i];
	}
	return NULL;
}


static void
unbind_key(KEY_MAP *map, int key)
{
	map->map[key] = NULL;
}


static void
raw_bind_key(KEY_MAP *map, int key, FUNCPTR func, KEY_MAP *next_map)
{
	if (map->map[key] == NULL) {
		if (key_count >= MAX_KEYS)
			return;
		map->map[key] = &key_table[key_count++];
	}
	map->map[key]->func = func;
	map->map[key]->next = next_map;
}


static KEY_MAP *
do_map_line(char *line)
{
	char	*cp;
	char	*map_name;

	cp = line;
	while (isspace((int)*cp))
		cp++;
	if (*cp == '\0')
		return NULL;
	map_name = cp;
	while ((*cp != '\0') && !isspace((int)*cp))
		cp++;
	*cp = '\0';
	return find_map(map_name);
}


static void
do_bind_line(KEY_MAP *map, char *line)
{
	char		*cp;
	char		key;
	char		*func_name;
	char		*next_name;
	KEY_MAP		*next;
	FUNCPTR		func;

	if (map == NULL)
		return;
	cp = line;
	key = *cp++;
	if (*cp == '\0') {
		unbind_key(map, key);
		return;
	}
	if (key == '^') {
		if (*cp == '?') {
			key = 0177;
			cp++;
		} else {
			key = CONTROL(*cp++);
		}
	} else if (key == '\\') {
		key = *cp++;
	}

	while (isspace((int)*cp))
		cp++;
	if (*cp == '\0') {
		unbind_key(map, key);
		return;
	}

	func_name = cp;
	while ((*cp != '\0') && !isspace((int)*cp))
		cp++;
	if (*cp) {
		*cp++ = '\0';
		while (isspace((int)*cp))
			cp++;
	}
	func = find_func(func_name);
	if (func == NULL) {
		fprintf(stderr, "Unknown function \"%s\"\n", func_name);
		return;
	}

	if (*cp == '\0') {
		next = map->default_ent.next;
		if (next == NULL)
			next = base_map;
	} else {
		next_name = cp;
		while ((*cp != '\0') && !isspace((int)*cp))
			cp++;
		if (*cp) {
			*cp++ = '\0';
			while (isspace((int)*cp))
				cp++;
		}
		next = find_map(next_name);
		if (next == NULL)
			return;
	}
	raw_bind_key(map, key, func, next);
}


static void
do_default_line(KEY_MAP *map, char *line)
{
	char		*cp;
	char		*func_name;
	char		*next_name;
	KEY_MAP		*next;
	FUNCPTR		func;

	if (map == NULL)
		return;
	cp = line;
	while (isspace((int)*cp))
		cp++;
	if (*cp == '\0')
		return;

	func_name = cp;
	while ((*cp != '\0') && !isspace((int)*cp))
		cp++;
	if (*cp != '\0') {
		*cp++ = '\0';
		while (isspace((int)*cp))
			cp++;
	}
	func = find_func(func_name);
	if (func == NULL)
		return;

	if (*cp == '\0') {
		next = map;
	} else {
		next_name = cp;
		while ((*cp != '\0') && !isspace((int)*cp))
			cp++;
		if (*cp != '\0') {
			*cp++ = '\0';
			while (isspace((int)*cp))
				cp++;
		}
		next = find_map(next_name);
		if (next == NULL)
			return;
	}

	map->default_ent.func = func;
	map->default_ent.next = next;
}


/*
 * Read bindings from specified open file.
 *
 * Returns nonzero on error.
 */
static int
read_bindings(FILE *fp)
{
	char	*cp;
	KEY_MAP *input_map;
	char	line[100];

	base_map = find_map(base_map_name);
	cur_map = base_map;
	input_map = base_map;

	if (fp == NULL)
		return 1;

	while (fgets(line, sizeof(line) - 1, fp)) {
		cp = line;
		while (isspace((int)*cp))
			cp++;

		if ((*cp == '\0') || (*cp == '#') || (*cp == '\n'))
			continue;

		if (cp[strlen(cp) - 1] == '\n')
			cp[strlen(cp) - 1] = '\0';

		if (memcmp(cp, "map", 3) == 0)
			input_map = do_map_line(&cp[3]);
		else if (memcmp(cp, "default", 7) == 0)
			do_default_line(input_map, &cp[7]);
		else
			do_bind_line(input_map, cp);
	}
	return 0;
}


static void
read_key(void)
{
	KEY_ENT		*ent;
	int		key;

	fflush(stdout);
	key = fgetc(stdin);
	if (key == EOF) {
		HS.linelen = 0;
		HS.buf[0] = '\0';
		return;
	}

	ent = cur_map->map[key];
	if (ent == NULL)
		ent = &cur_map->default_ent;
	if (ent->next)
		cur_map = ent->next;
	if (ent->func)
		/* ignore Saber-C warning #65 - has 1 arg, expecting 0 */
		/*	  ok to ignore in proc read_key */
		(*ent->func)(key);
	else
		insert_char(key);
}


/*
 * Return the Nth history event, indexed from zero.
 * Earlier history events are lower in number.
 */
static HIST *
get_event(int n)
{
	register HIST * hp;

	if ((n < 0) || (n >= HS.histcount))
		return NULL;
	hp = FIRSTHIST;
	while (n-- > 0)
		hp = NEXTHIST(hp);
	return hp;
}


/*
 * Search the history list for the specified pattern.
 * Returns the found history, or NULL.
 */
static HIST *
find_event(char *pat, int len)
{
	register HIST * hp;

	for (hp = FIRSTHIST; hp->len; hp = NEXTHIST(hp)) {
		if ((hp->len == len) && (memcmp(hp->data, pat, len) == 0))
			return hp;
	}
	return NULL;
}


/*
 * Insert a line into the end of the history table.
 * If the line already appears in the table, then it is moved to the end.
 * If the table is full, then the earliest commands are deleted as necessary.
 * Warning: the incoming line cannot point into the history table.
 */
void
hist_saveline(char *line, int len)
{
	HIST *	hp;
	HIST *	hp2;
	int	left;

	if ((len > 0) && (line[len - 1] == '\n'))
		len--;
	if (len <= 0)
		return;

	/*
	 * See if the line is already present in the history table.
	 * If so, and it is already at the end, then we are all done.
	 * Otherwise delete it since we will reinsert it at the end.
	 */
	hp = find_event(line, len);
	if (hp) {
		hp2 = NEXTHIST(hp);
		left = histused - HISTOFFSET(hp2);
		if (left <= 0)
			return;
		histused -= HISTLEN(hp);
		memcpy(hp, hp2, left + 1);
		HS.histcount--;
	}

	/*
	 * If there is not enough room left in the history buffer to add
	 * the new command, then repeatedly delete the earliest command
	 * as many times as necessary in order to make enough room.
	 */
	while ((histused + len) >= HIST_SIZE) {
		hp = (HIST *) histbuf;
		hp2 = NEXTHIST(hp);
		left = histused - HISTOFFSET(hp2);
		histused -= HISTLEN(hp);
		memcpy(hp, hp2, left + 1);
		HS.histcount--;
	}

	/*
	 * Add the line to the end of the history table.
	 */
	hp = (HIST *) &histbuf[histused];
	hp->len = len;
	memcpy(hp->data, line, len);
	histused += HISTLEN(hp);
	histbuf[histused] = 0;
	HS.curhist = ++HS.histcount;
}


/*
 * Find the function for a specified name.
 */
static FUNCPTR
find_func(char *name)
{
	FUNC	*fp;

	for (fp = funcs; fp->name; fp++) {
		if (strcmp(fp->name, name) == 0)
			return fp->func;
	}
	return NULL;
}


static void
arrow_key(void)
{
	switch (fgetc(stdin)) {
		case 'A':
			backward_history();
			break;
		case 'B':
			forward_history();
			break;
		case 'C':
			forward_char();
			break;
		case 'D':
			backward_char();
			break;
	}
}


static void
back_over_char(int ch)
{
	backspace();
	if (!isprint(ch))
		backspace();
}


static void
remove_char(int ch)
{
	erasechar();
	if (!isprint(ch))
		erasechar();
}


static void
echo_rest_of_line(void)
{
	echo_string(HS.pos, HS.end - HS.pos);
}


static void
goto_start_of_line(void)
{
	while (HS.pos > HS.buf)
		back_over_char((int)(*--HS.pos));
}


static void
goto_end_of_line(void)
{
	echo_rest_of_line();
	HS.pos = HS.end;
}


static void
decrement_end(int n)
{
	HS.end -= n;
	if (HS.mark && (HS.mark > HS.end))
		HS.mark = NULL;
}


static void
ignore_char(void)
{
}


static void
flush_input(void)
{
	echo_rest_of_line();
	while (HS.end > HS.buf)
		remove_char((int)(*--HS.end));
	HS.pos = HS.buf;
	HS.mark = NULL;
}


static void
start_of_line(void)
{
	goto_start_of_line();
}


static void
end_of_line(void)
{
	goto_end_of_line();
}


static void
forward_char(void)
{
	if (HS.pos < HS.end)
		echo_char(*HS.pos++);
}


static void
backward_char(void)
{
	if (HS.pos > HS.buf)
		back_over_char((int)(*--HS.pos));
}


static void
uppercase_word(void)
{
	while ((HS.pos < HS.end) && !in_word((int)(*HS.pos)))
		echo_char(*HS.pos++);
	while ((HS.pos < HS.end) && in_word((int)(*HS.pos))) {
		if ((*HS.pos >= 'a') && (*HS.pos <= 'z'))
			*HS.pos += 'A' - 'a';
		echo_char(*HS.pos++);
	}
}


static void
lowercase_word(void)
{
	while ((HS.pos < HS.end) && !in_word((int)(*HS.pos)))
		echo_char(*HS.pos++);
	while ((HS.pos < HS.end) && in_word((int)(*HS.pos))) {
		if ((*HS.pos >= 'A') && (*HS.pos <= 'Z'))
			*HS.pos += 'a' - 'A';
		echo_char(*HS.pos++);
	}
}


static void
forward_word(void)
{
	while ((HS.pos < HS.end) && !in_word((int)(*HS.pos)))
		echo_char(*HS.pos++);
	while ((HS.pos < HS.end) && in_word((int)(*HS.pos)))
		echo_char(*HS.pos++);
}


static void
backward_word(void)
{
	if ((HS.pos > HS.buf) && in_word((int)(*HS.pos)))
		back_over_char((int)(*--HS.pos));
	while ((HS.pos > HS.buf) && !in_word((int)(*HS.pos)))
		back_over_char((int)(*--HS.pos));
	while ((HS.pos > HS.buf) && in_word((int)(*HS.pos)))
		back_over_char((int)(*--HS.pos));
	if ((HS.pos < HS.end) && !in_word((int)(*HS.pos)))
		echo_char(*HS.pos++);
}


static void
forward_kill_char(void)
{
	int	rest;
	char	ch;

	rest = HS.end - HS.pos;
	if (rest-- <= 0)
		return;
	ch = *HS.pos;
	if (rest > 0) {
		memcpy(HS.pos, HS.pos + 1, rest);
		*(HS.end - 1) = ch;
	}
	echo_rest_of_line();
	remove_char((int)ch);
	decrement_end(1);
	while (rest > 0)
		back_over_char((int)(HS.pos[--rest]));
}


static void
delete_char(void)
{
	/*
	 * quit delete_char (usually ^D) is at start of line and we are allowed
	 *
	 * We exit of start of line and config("ctrl_d", "empty") or
	 * if config("ctrl_d", "virgin") and we have never typed on the line.
	 */
	if ((HS.end == HS.buf) &&
	    (conf->ctrl_d == CTRL_D_EMPTY_EOF ||
	     (conf->ctrl_d == CTRL_D_VIRGIN_EOF && HS.virgin_line == TRUE))) {
		quit_calc();
	}

	/*
	 * normal case: just forward_kill_char
	 */
	if (HS.end > HS.buf)
		forward_kill_char();
}


static void
backward_kill_char(void)
{
	if (HS.pos > HS.buf) {
		HS.pos--;
		back_over_char((int)(*HS.pos));
		forward_kill_char();
	}
}


static void
forward_kill_word(void)
{
	char	*cp;

	if (HS.pos >= HS.end)
		return;
	echo_rest_of_line();
	for (cp = HS.end; cp > HS.pos;)
		remove_char((int)(*--cp));
	cp = HS.pos;
	while ((cp < HS.end) && !in_word((int)(*cp)))
		cp++;
	while ((cp < HS.end) && in_word((int)(*cp)))
		cp++;
	savetext(HS.pos, cp - HS.pos);
	memcpy(HS.pos, cp, HS.end - cp);
	decrement_end(cp - HS.pos);
	echo_rest_of_line();
	for (cp = HS.end; cp > HS.pos;)
		back_over_char((int)(*--cp));
}


static void
kill_line(void)
{
	if (HS.end <= HS.pos)
		return;
	savetext(HS.pos, HS.end - HS.pos);
	echo_rest_of_line();
	while (HS.end > HS.pos)
		remove_char((int)(*--HS.end));
	decrement_end(0);
}


/*
 * This is the function which completes a command line editing session.
 * The final line length is returned in the HS.linelen variable.
 * The line is NOT put into the edit history, so that the caller can
 * decide whether or not this should be done.
 */
static void
new_line(void)
{
	int	len;

	newline();
	fflush(stdout);

	HS.mark = NULL;
	HS.end[0] = '\n';
	HS.end[1] = '\0';
	len = HS.end - HS.buf + 1;
	if (len <= 1) {
		HS.curhist = HS.histcount;
		HS.linelen = 1;
		return;
	}
	HS.curhist = HS.histcount;
	HS.pos = HS.buf;
	HS.end = HS.buf;
	HS.linelen = len;
}


static void
save_line(void)
{
	int	len;

	len = HS.end - HS.buf;
	if (len > 0) {
		hist_saveline(HS.buf, len);
		flush_input();
	}
	HS.curhist = HS.histcount;
}


static void
goto_line(void)
{
	int	num;
	char	*cp;
	HIST	*hp;

	num = 0;
	cp = HS.buf;
	while ((*cp >= '0') && (*cp <= '9') && (cp < HS.pos))
		num = num * 10 + (*cp++ - '0');
	if ((num <= 0) || (num > HS.histcount) || (cp != HS.pos)) {
		beep();
		return;
	}
	flush_input();
	HS.curhist = HS.histcount - num;
	hp = get_event(HS.curhist);
	memcpy(HS.buf, hp->data, hp->len);
	HS.end = &HS.buf[hp->len];
	goto_end_of_line();
}


static void
forward_history(void)
{
	HIST	*hp;

	flush_input();
	if (++HS.curhist >= HS.histcount)
		HS.curhist = 0;
	hp = get_event(HS.curhist);
	if (hp) {
		memcpy(HS.buf, hp->data, hp->len);
		HS.end = &HS.buf[hp->len];
	}
	goto_end_of_line();
}


static void
backward_history(void)
{
	HIST	*hp;

	flush_input();
	if (--HS.curhist < 0)
		HS.curhist = HS.histcount - 1;
	hp = get_event(HS.curhist);
	if (hp) {
		memcpy(HS.buf, hp->data, hp->len);
		HS.end = &HS.buf[hp->len];
	}
	goto_end_of_line();
}


static void
insert_char(int key)
{
	int	len;
	int	rest;

	len = HS.end - HS.buf;
	if (len >= HS.bufsize) {
		beep();
		return;
	}
	rest = HS.end - HS.pos;
	if (rest > 0)
		memrcpy(HS.pos + 1, HS.pos, rest);
	HS.end++;
	*HS.pos++ = key;
	echo_char(key);
	echo_rest_of_line();
	while (rest > 0)
		back_over_char((int)(HS.pos[--rest]));
}


static void
insert_string(char *str, int len)
{
	int	rest;
	int	totallen;

	if (len <= 0)
		return;
	totallen = (HS.end - HS.buf) + len;
	if (totallen > HS.bufsize) {
		beep();
		return;
	}
	rest = HS.end - HS.pos;
	if (rest > 0)
		memrcpy(HS.pos + len, HS.pos, rest);
	HS.end += len;
	memcpy(HS.pos, str, len);
	HS.pos += len;
	echo_string(str, len);
	echo_rest_of_line();
	while (rest > 0)
		back_over_char((int)(HS.pos[--rest]));
}


static void
list_history(void)
{
	HIST	*hp;
	int	hnum;

	for (hnum = 0; hnum < HS.histcount; hnum++) {
		hp = get_event(hnum);
		printf("\n%3d: ", HS.histcount - hnum);
		echo_string(hp->data, hp->len);
	}
	refresh_line();
}


static void
refresh_line(void)
{
	char	*cp;

	newline();
	fputs(HS.prompt, stdout);
	if (HS.end > HS.buf) {
		echo_string(HS.buf, HS.end - HS.buf);
		cp = HS.end;
		while (cp > HS.pos)
			back_over_char((int)(*--cp));
	}
}


static void
swap_chars(void)
{
	char	ch1;
	char	ch2;

	if ((HS.pos <= HS.buf) || (HS.pos >= HS.end))
		return;
	ch1 = *HS.pos--;
	ch2 = *HS.pos;
	*HS.pos++ = ch1;
	*HS.pos = ch2;
	back_over_char((int)ch2);
	echo_char(ch1);
	echo_char(ch2);
	back_over_char((int)ch2);
}


static void
set_mark(void)
{
	HS.mark = HS.pos;
}


static void
save_region(void)
{
	int	len;

	if (HS.mark == NULL)
		return;
	len = HS.mark - HS.pos;
	if (len > 0)
		savetext(HS.pos, len);
	if (len < 0)
		savetext(HS.mark, -len);
}


static void
kill_region(void)
{
	char	*cp;
	char	*left;
	char	*right;

	if ((HS.mark == NULL) || (HS.mark == HS.pos))
		return;

	echo_rest_of_line();
	if (HS.mark < HS.pos) {
		left = HS.mark;
		right = HS.pos;
		HS.pos = HS.mark;
	} else {
		left = HS.pos;
		right = HS.mark;
		HS.mark = HS.pos;
	}
	savetext(left, right - left);
	for (cp = HS.end; cp > left;)
		remove_char((int)(*--cp));
	if (right < HS.end)
		memcpy(left, right, HS.end - right);
	decrement_end(right - left);
	echo_rest_of_line();
	for (cp = HS.end; cp > HS.pos;)
		back_over_char((int)(*--cp));
}


static void
yank(void)
{
	insert_string(save_buffer, save_len);
}


static void
reverse_search(void)
{
	int	len;
	int	count;
	int	testhist;
	HIST	*hp;
	char	*save_pos;

	count = HS.histcount;
	len = HS.pos - HS.buf;
	if (len <= 0)
		count = 0;
	testhist = HS.curhist;
	do {
		if (--count < 0) {
			beep();
			return;
		}
		if (--testhist < 0)
			testhist = HS.histcount - 1;
		hp = get_event(testhist);
	} while ((hp == NULL) || (hp->len < len) ||
		memcmp(hp->data, HS.buf, len));

	HS.curhist = testhist;
	save_pos = HS.pos;
	flush_input();
	memcpy(HS.buf, hp->data, hp->len);
	HS.end = &HS.buf[hp->len];
	goto_end_of_line();
	while (HS.pos > save_pos)
		back_over_char((int)(*--HS.pos));
}


static void
quote_char(void)
{
	int	ch;

	ch = fgetc(stdin);
	if (ch != EOF)
		insert_char(ch);
}


/*
 * Save data in the save buffer.
 */
static void
savetext(char *str, int len)
{
	save_len = 0;
	if (len <= 0)
		return;
	if (len > SAVE_SIZE)
		len = SAVE_SIZE;
	memcpy(save_buffer, str, len);
	save_len = len;
}


/*
 * Test whether a character is part of a word.
 */
static int
in_word(int ch)
{
	return (isalnum(ch) || (ch == '_'));
}


static void
erasechar(void)
{
	fputs("\b \b", stdout);
}


static void
newline(void)
{
	fputc('\n', stdout);
}


static void
backspace(void)
{
	fputc('\b', stdout);
}


static void
beep(void)
{
	fputc('\007', stdout);
}


static void
echo_char(int ch)
{
	if (isprint(ch)) {
		putchar(ch);
	} else {
		putchar('^');
		putchar((ch + '@') & 0x7f);
	}
}


static void
echo_string(char *str, int len)
{
	while (len-- > 0)
		echo_char(*str++);
}


static void
memrcpy(char *dest, char *src, int len)
{
	dest += len - 1;
	src += len - 1;
	while (len-- > 0)
		*dest-- = *src--;
}

#endif /* !USE_READLINE */

static void
quit_calc(void)
{
	hist_term();
	putchar('\n');
	libcalc_call_me_last();
	exit(0);
}

#if defined(USE_READLINE)


#define HISTORY_LEN (1024)	/* number of entries to save */


#include <readline/readline.h>
#include <readline/history.h>


/*
 * The readline/history libs do most of the dirty work for us, so we can
 * replace hist_init() and hist_term() with dummies when using readline.
 * For hist_getline() we have to add a newline that readline removed but
 * calc expects. For hist_saveline(), we have to undo this.  hist_getline()
 * also has to cope with the different memory management schemes of calc and
 * readline.
 */


/* name of history file */
char *my_calc_history = NULL;

int
hist_getline(char *prompt, char *buf, int len)
{
	char *line;

	buf[0] = '\0';
	line = readline(prompt);
	if (!line) {
		switch (conf->ctrl_d) {
			case CTRL_D_NEVER_EOF:
				return 0;
			case CTRL_D_VIRGIN_EOF:
			case CTRL_D_EMPTY_EOF:
			default:
				quit_calc();
				/*NOTREACHED*/
		}
	}
	strncpy(buf, line, len - 1);
	buf[len - 2] = '\0';
	len = strlen(buf);
	buf[len] = '\n';
	buf[len + 1] = '\0';
	free(line);
	return len + 1;
}


void
hist_term(void)
{
}


static void
my_stifle_history (void)
{
	/* only save last number of entries */
	stifle_history(HISTORY_LEN);

	if (my_calc_history)
		write_history(my_calc_history);
}


int
hist_init(char UNUSED *filename)
{
	/* used when parsing conditionals in ~/.inputrc */
	rl_readline_name = "calc";

	/* initialize interactive variables */
	using_history();

	/* name of history file */
	my_calc_history = tilde_expand("~/.calc_history");

	/* read previous history */
	read_history(my_calc_history);

	atexit(my_stifle_history);

	return HIST_SUCCESS;
}

void
hist_saveline(char *line, int len)
{
	static char *prev = NULL;

	if (len <= 1)
		return;

	/* ignore if identical with previous line */
	if (prev != NULL && strcmp(prev, line) == 0)
		return;

	free(prev);

	/* fail silently */
	prev = strdup(line);

	line[len - 1] = '\0';
	add_history(line);
	line[len - 1] = '\n';
}


#endif /* USE_READLINE */


#if defined(HIST_TEST)

/*
 * Main routine to test history.
 */
void
main(int argc, char **argv)
{
	char	*filename;
	int	len;
	char	buf[256];

	filename = NULL;
	if (argc > 1)
		filename = argv[1];

	switch (hist_init(filename)) {
		case HIST_SUCCESS:
			break;
		case HIST_NOFILE:
			fprintf(stderr, "Binding file was not found\n");
			break;
		case HIST_NOTTY:
			fprintf(stderr, "Cannot set terminal parameters\n");
			break;
		case HIST_INITED:
			fprintf(stderr, "Hist is already inited\n");
			break;
		default:
			fprintf(stderr, "Unknown error from hist_init\n");
			break;
	}

	do {
		len = hist_getline("HIST> ", buf, sizeof(buf));
		hist_saveline(buf, len);
	} while (len && (buf[0] != 'q'));

	hist_term();
	exit(0);
}

#endif /* HIST_TEST */
