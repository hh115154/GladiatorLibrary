/*
 * input - nested input source file reader
 *
 * Copyright (C) 1999  David I. Bell
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
 * @(#) $Id: input.c,v 29.6 2002/03/12 09:40:57 chongo Exp $
 * @(#) $Source: /usr/local/src/cmd/calc/RCS/input.c,v $
 *
 * Under source code control:	1990/02/15 01:48:16
 * File existed as early as:	before 1990
 *
 * Share and enjoy!  :-)	http://www.isthe.com/chongo/tech/comp/calc/
 */

/*
 * Nested input source file reader.
 * For terminal input, this also provides a simple command stack.
 */


#include <stdio.h>
#include <ctype.h>
#if !defined(_WIN32)
# include <pwd.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>

#include "have_unistd.h"
#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif

#if defined(__MSDOS__)
#include <limits.h>
#define _fullpath(f,n,s) (_fixpath(n,f),f)
#define _MAX_PATH PATH_MAX
#endif

#include "calc.h"
#include "conf.h"
#include "hist.h"

#ifdef ENABLE_RELOCATABLE
# include <relocate.h>
#else
# define relocatex (dir,path) (path)
#endif

extern int stdin_tty;		/* TRUE if stdin is a tty */
extern FILE *f_open(char *name, char *mode);
extern FILE *curstream(void);


#define TTYSIZE		100	/* reallocation size for terminal buffers */
#define MAXDEPTH		10	/* maximum depth of input */
#define IS_READ		1	/* reading normally */
#define IS_REREAD	2	/* reread current character */
#define chartoint(ch)	((ch) & 0xff)	/* make sure char is not negative */
#define READSET_ALLOC	8	/* readset to allocate chunk size */


typedef struct {
	int i_state;		/* state (read, reread) */
	int i_char;		/* currently read char */
	long i_line;		/* line number */
	char *i_cp;		/* pointer to string character to be read */
	char *i_str;		/* start of string copy to be read, or NULL */
	long i_num;		/* number of string characters remaining */
	char *i_ttystr;		/* current character of tty line (or NULL) */
	FILE *i_fp;		/* current file for input (if not NULL) */
	char *i_name;		/* file name if known */
} INPUT;


/* files that calc has read or included */
typedef struct {
	int active;		/* != 0 => active entry, 0 => unused entry */
	char *name;		/* name used to read file */
	char *path;		/* real path used to open file */
	struct stat inode;	/* inode information for file */
} READSET;

static READSET *readset = NULL;		/* array of files read */
static int maxreadset = 0;		/* length of readset */

static int linesize;		/* current max size of input line */
static char *linebuf;		/* current input line buffer */
static char *prompt;		/* current prompt for terminal */
static BOOL noprompt;		/* TRUE if should not print prompt */

static int depth;		/* current input depth */
static INPUT *cip;		/* current input source */
static INPUT inputs[MAXDEPTH];	/* input sources */


static int openfile(char *name);
static int ttychar(void);
static int isinoderead(struct stat *sbuf);
static int findfreeread(void);
static int addreadset(char *name, char *path, struct stat *sbuf);
static char *homeexpand(char *name);


/*
 * Open an input file by possibly searching through a path list
 * and also possibly applying the specified extension.	For example:
 * opensearchfile("barf", ".:/tmp", ".c", once) searches in order
 * for the files "./barf", "./barf.c", "/tmp/barf", and "/tmp/barf.c".
 *
 * Returns -1 if we could not open a file or error.
 * Returns 1 if file was opened and added to/updated in the readset
 * Returns 0 if file was already in the readset and reopen was 0.
 *
 * given:
 *	name		file name to be read
 *	pathlist	list of colon separated paths (or NULL)
 *	extension	extra extension to try (or NULL)
 *	rd_once		TRUE => do not reread a file
 */
int
opensearchfile(char *name, char *pathlist, char *extension, int rd_once)
{
	int i;
	char *cp;
	char *path;		/* name being searched for */
	struct stat statbuf;	/* stat of the path */
	unsigned long namelen;	/* length of name */
	unsigned long extlen;	/* length of the extension if non-NULL or 0 */
	unsigned long pathlen;	/* length of the pathlist if non-NULL or 0 */

	/*
	 * allocate storage for the longest name being searched for
	 *
	 * We will allocate more than we could ever want/need.
	 * The longest we could ever need would be:
	 *
	 *	pathlist (as a single long string)
	 *	/
	 *	name
	 *	.
	 *	extension
	 *	\0
	 *	guard byte
	 */
	namelen = strlen(name);
	if (extension != NULL) {
	    extlen = strlen(extension);
	} else {
	    extlen = 0;
	}
	if (pathlist != NULL) {
	    pathlen = strlen(pathlist);
	} else {
	    pathlen = 0;
	}
	path = malloc(pathlen+1 + 1 + namelen+1 + extlen+1 + 1 + 1);
	if (path == NULL) {
		math_error("Cannot allocate filename path buffer");
		/*NOTREACHED*/
	}

	/*
	 * Don't try the extension if the filename already contains it.
	 */
	if (extension != NULL && namelen >= extlen &&
	    strcmp(&name[namelen-extlen], extension) == 0) {
		extension = NULL;
	}

	/*
	 * If the name is absolute, or if there is no path list, then
	 * make one which just searches for the name straight.	Then
	 * search through the path list for the file, without and with
	 * the specified extension.
	 */
	if (name[0] == PATHCHAR ||
	    name[0] == HOMECHAR ||
	    (name[0] == DOTCHAR && name[1] == PATHCHAR) ||
	    pathlist == NULL) {
		pathlist = "";
	}
	pathlist--;
	do {
		pathlist++;
		cp = path;
		while (*pathlist && (*pathlist != LISTCHAR))
			*cp++ = *pathlist++;
		if (cp != path)
			*cp++ = PATHCHAR;
		strcpy(cp, name);
		i = openfile(path);
#ifdef ENABLE_RELOCATABLE
		if (i < 0) {
			char *relocpath = relocatex (INSTALLDIR, path);
			i = openfile (relocpath);
			free (relocpath);
		}
#endif /* ENABLE_RELOCATABLE */
		if ((i < 0) &&
		    (extension != NULL && extension[0] != '\0')) {
			strcat(path, extension);
			i = openfile(path);
#ifdef ENABLE_RELOCATABLE
			if (i < 0) {
				char *relocpath = relocatex (INSTALLDIR, path);
				i = openfile (relocpath);
				free (relocpath);
			}
#endif /* ENABLE_RELOCATABLE */
		}
	} while ((i < 0) && *pathlist);

	/* examine what our search produced */
	if (i < 0) {
		free(path);
		return i;
	}
	if (cip->i_fp == NULL) {
		/* cannot find a file to open */
		free(path);
		return -3;
	}
	if (fstat(fileno(cip->i_fp), &statbuf) < 0) {
		/* unable to fstat the open file */
		free(path);
		return -4;
	}

	/* note if we will reopen a file and if that is allowed */
	if (rd_once == TRUE && isinoderead(&statbuf) >= 0) {
		/* file is in readset and reopen is false */
		closeinput();
		free(path);
		return 1;
	}

	/* add this name to the readset if allowed */
	if (addreadset(name, path, &statbuf) < 0) {
		/* cannot add to readset */
		closeinput();
		free(path);
		return -1;
	}

	/* file was added to/updated in readset */
	free(path);
	return 0;
}


/*
 * Given a filename with a leading ~, expand it into a home directory for
 * that user.  This function will malloc the space for the expanded path.
 *
 * If the path is just ~, or begins with ~/, expand it to the home
 * directory of the current user.  If the environment variable $HOME
 * is known, it will be used, otherwise the password file will be
 * consulted.
 *
 * If the path is just ~username, or ~username/, expand it to the home
 * directory of that user by looking it up in the password file.
 *
 * If the password file must be consulted and the username is not found
 * a NULL pointer is returned.
 *
 * given:
 *	name		a filename with a leading ~
 */
static char *
homeexpand(char *name)
{
#if defined(_WIN32)

	return NULL;

#else /* Windoz free systems */

	struct passwd *ent;	/* password entry */
	char *home2;		/* fullpath of the home directory */
	char *fullpath;		/* the malloced expanded path */
	char *after;		/* after the ~user or ~ */
	char *username;		/* extracted username */

	/* firewall */
	if (name[0] != HOMECHAR)
		return NULL;

	/*
	 * obtain the home directory component
	 */
	switch (name[1]) {
	case PATHCHAR:		/* ~/... */
	case '\0':		/* ~ */
		home2 = home;
		after = name+1;
		break;
	default:		/* ~username or ~username/... */

		/* extract the username after the ~ */
		after = (char *)strchr(name+2, PATHCHAR);
		if (after == NULL) {
			/* path is just ~username */
			ent = (struct passwd *)getpwnam(name+1);
			if (ent == NULL) {
				/* unknown user */
				return NULL;
			}
			/* just malloc the home directory and return it */
			fullpath = (char *)malloc(strlen(ent->pw_dir)+1);
			if (fullpath == NULL) {
				return NULL;
			}
			strcpy(fullpath, ent->pw_dir);
			return fullpath;
		}
		username = (char *) malloc(after-name + 1 + 1);
		if (username == NULL) {
			/* failed to malloc username */
			return NULL;
		}
		strncpy(username, name+1, after-name-1);
		username[after-name-1] = '\0';

		/* get that user's home directory */
		ent = (struct passwd *)getpwnam(username);
		free(username);
		if (ent == NULL) {
			/* unknown user */
			return NULL;
		}
		home2 = ent->pw_dir;
		break;
	}

	/*
	 * build the fullpath given the home directory
	 */
	fullpath = (char *)malloc(strlen(home2)+strlen(after)+1);
	if (fullpath == NULL) {
		return NULL;
	}
	sprintf(fullpath, "%s%s", home2, after);
	return fullpath;
#endif /* Windoz free systems */
}


/*
 * f_open - ~-expand a filename and fopen() it
 *
 * given:
 *	name		the filename to open
 7	mode		the fopen mode to use
 */
FILE *
f_open(char *name, char *mode)
{
	FILE *fp;		/* open file descriptor */
	char *fullname;		/* file name with HOMECHAR expansion */

	/*
	 * be sore we are allowed to open a file in this mode
	 */
	if (!allow_read && !allow_write) {
		/* no reads and no writes means no opens! */
		if (run_state > RUN_BEGIN) {
			fprintf(stderr,
				"open of %s mode %s - %s\n", name, mode,
				"open for read or write disallowed by -m\n");
		}
		return NULL;
	} else if (!allow_read && strchr(mode, 'r') != NULL) {
		/* reading new files disallowed */
		if (run_state > RUN_BEGIN) {
			fprintf(stderr,
				"open of %s mode %s - %s\n", name, mode,
				"open for read disallowed by -m\n");
		}
		return NULL;
	} else if (!allow_write &&
		   (strchr(mode, 'w') != NULL ||
		    strchr(mode, 'a') != NULL ||
		    strchr(mode, '+') != NULL)) {
		/* writing new files disallowed */
		if (run_state > RUN_BEGIN) {
			fprintf(stderr,
				"open of %s mode %s - %s\n", name, mode,
				"open for write disallowed by -m\n");
		}
		return NULL;
	}

	/*
	 * expand ~ if needed
	 */
	if (name[0] == HOMECHAR) {
		fullname = homeexpand(name);
		if (fullname == NULL)
			return NULL;
		fp = fopen(fullname, mode);
		free(fullname);
	} else {
		fp = fopen(name, mode);
	}
	return fp;
}


/*
 * Setup for reading from a input file.
 * Returns -1 if file could not be opened.
 *
 * given:
 *	name		file name to be read
 */
static int
openfile(char *name)
{
	FILE *fp;		/* open file descriptor */

	if (depth >= MAXDEPTH)
		 return -2;
	fp = f_open(name, "r");
	if (fp == NULL)
		 return -1;
	cip = inputs + depth++;
	cip->i_state = IS_READ;
	cip->i_char = '\0';
	cip->i_str = NULL;
	cip->i_ttystr = NULL;
	cip->i_fp = fp;
	cip->i_line = 1;
	cip->i_name = (char *)malloc(strlen(name) + 1);
	if (cip->i_name == NULL) {
		 return -1;
	}
	strcpy(cip->i_name, name);
	return 0;
}


/*
 * Return the current input file stream, or NULL if none.
 */
FILE *
curstream(void)
{
	if (depth <= 0 || depth > MAXDEPTH)
		return NULL;
	return cip->i_fp;
}


/*
 * Open a string for scanning, num characters to be read.
 * String is copied into local memory so it can be trashed afterwards.
 * Returns -1 if cannot open string.
 *
 * given:
 *	str		string to be opened
 */
int
openstring(char *str, long num)
{
	char *cp;		/* copied string */

	if ((depth >= MAXDEPTH) || (str == NULL))
		 return -2;
	cp = (char *) malloc(num + 1);
	if (cp == NULL)
		 return -1;
	strcpy(cp, str);
	cip = inputs + depth++;
	cip->i_state = IS_READ;
	cip->i_char = '\0';
	cip->i_cp = cp;
	cip->i_str = cp;
	cip->i_num = num;
	cip->i_fp = NULL;
	cip->i_name = NULL;
	cip->i_ttystr = NULL;
	cip->i_line = 1;
	return 0;
}


/*
 * Set to read input from the terminal.
 * Returns -1 if there is no more depth for input.
 */
int
openterminal(void)
{
	if (depth >= MAXDEPTH)
		 return -2;
	cip = inputs + depth++;
	cip->i_state = IS_READ;
	cip->i_char = '\0';
	cip->i_str = NULL;
	cip->i_ttystr = NULL;
	cip->i_fp = NULL;
	cip->i_name = NULL;
	cip->i_line = 1;
	return 0;
}


/*
 * Close the current input source.
 */
void
closeinput(void)
{
	if (depth <= 0)
		return;
	if (cip->i_str)
		free(cip->i_str);
	if (cip->i_fp)
		fclose(cip->i_fp);
	if (cip->i_name)
		free(cip->i_name);
	depth--;
	cip = depth ? &inputs[depth - 1] : NULL;
}


/*
 * Reset the input sources back to the initial state.
 */
void
resetinput(void)
{
	while (depth > 0)
		closeinput();
	noprompt = FALSE;
}


/*
 * Set the prompt for terminal input.
 */
void
setprompt(char *str)
{
	prompt = str;
	noprompt = FALSE;
}


/*
 * Read the next character from the current input source.
 * End of file closes current input source, and returns EOF character.
 */
int
nextchar(void)
{
	int ch;			/* current input character */

	if (depth == 0)		/* input finished */
		 return EOF;
	if (cip->i_state == IS_REREAD) {	/* rereading current char */
		 ch = cip->i_char;
		 cip->i_state = IS_READ;
		 if (ch == '\n')
			cip->i_line++;
		 return ch;
	}
	if (cip->i_str) {		/* from string */
		if (cip->i_num) {
			ch = chartoint(*cip->i_cp++);
			cip->i_num--;
		} else {
			ch = EOF;
		}
	} else if (cip->i_fp) {		/* from file */
		ch = fgetc(cip->i_fp);
	} else if (!stdin_tty) {		/* from file */
		ch = fgetc(stdin);
	} else {			/* from terminal */
		ch = ttychar();
	}
	if (depth > 0)
		cip->i_char = ch;	/* save for rereads */
	if (ch == '\n')
		cip->i_line++;
	return ch;
}


/*
 * Read in the next line of input from the current input source.
 * The line is terminated with a null character, and does not contain
 * the final newline character.	 The returned string is only valid
 * until the next such call, and so must be copied if necessary.
 * Returns NULL on end of file.
 */
char *
nextline(void)
{
	char *cp;
	int ch;
	int len;

	cp = linebuf;
	if (linesize == 0) {
		cp = (char *)malloc(TTYSIZE + 1);
		if (cp == NULL) {
			math_error("Cannot allocate line buffer");
			/*NOTREACHED*/
		}
		linebuf = cp;
		linesize = TTYSIZE;
	}
	len = 0;
	for (;;) {
		noprompt = TRUE;
		ch = nextchar();
		noprompt = FALSE;
		if (ch == EOF)
			return NULL;
		if (ch == '\0')
			continue;
		if (ch == '\n')
			break;
		if (len >= linesize) {
			cp = (char *)realloc(cp, linesize + TTYSIZE + 1);
			if (cp == NULL) {
				math_error("Cannot realloc line buffer");
				/*NOTREACHED*/
			}
			linebuf = cp;
			linesize += TTYSIZE;
		}
		cp[len++] = (char)ch;
	}
	cp[len] = '\0';
	return linebuf;
}


/*
 * Read the next character from the terminal.
 * The routines in the history module are called so that the user
 * can use a command history and emacs-like editing of the line.
 */
static int
ttychar(void)
{
	int ch;			/* current char */
	int len;		/* length of current command */
	static char charbuf[1024];

	/*
	 * If we have more to read from the saved command line, then do that.
	 * When we see a newline character, then clear the pointer so we will
	 * read a new line on the next call.
	 */
	if (cip->i_ttystr) {
		ch = chartoint(*cip->i_ttystr++);
		if (ch == '\n')
			cip->i_ttystr = NULL;
		return ch;
	}

	/*
	 * We need another complete line.
	 */
	abortlevel = 0;
	inputwait = TRUE;
	len = hist_getline(noprompt ? "" : prompt, charbuf, sizeof(charbuf));
	if (len == 0) {
		inputwait = FALSE;
		return EOF;
	}
	inputwait = FALSE;

	/*
	 * Handle shell escape if present
	 */
	if (charbuf[0] == '!') {		/* do a shell command */
		char *cmd;

		cmd = charbuf + 1;
		if (*cmd == '\0' || *cmd == '\n')
			cmd = shell;
		if (allow_exec) {
			if (conf->calc_debug & CALCDBG_SYSTEM) {
				printf("%s\n", cmd);
			}
			system(cmd);
		} else {
			fprintf(stderr, "execution disallowed by -m flag\n");
		}
		return '\n';
	}
	hist_saveline(charbuf, len);

	/*
	 * Return the first character of the line, and set up to
	 * return the rest of it with later calls.
	 */
	ch = chartoint(charbuf[0]);
	if (ch != '\n')
		cip->i_ttystr = charbuf + 1;
	return ch;
}


/*
 * Return whether or not the input source is the terminal.
 */
BOOL
inputisterminal(void)
{
	return ((depth <= 0) || ((cip->i_str == NULL) && (cip->i_fp == NULL)));
}


/*
 * Return depth of current input source
 */
int
inputlevel(void)
{
	return depth - 1;
}


/*
 * Return the name of the current input file.
 * Returns NULL for terminal or strings.
 */
char *
inputname(void)
{
	if (depth <= 0)
		return NULL;
	return cip->i_name;
}


/*
 * Return the current line number.
 */
long
linenumber(void)
{
	if (depth > 0)
		return cip->i_line;
	return 1;
}


/*
 * Restore the next character to be read again on the next nextchar call.
 */
void
reread(void)
{
	if ((depth <= 0) || (cip->i_state == IS_REREAD))
		return;
	cip->i_state = IS_REREAD;
	if (cip->i_char == '\n')
		cip->i_line--;
}


/*
 * Process all startup files found in the $CALCRC path.
 */
void
runrcfiles(void)
{
	char path[MAX_CALCRC+1+1];	/* name being searched for */
	char *cp;
	char *p;

	/* execute each file in the list */
	while (calcrc != NULL && *calcrc) {
		cp = calcrc;
		calcrc = (char *) strchr(calcrc + 1, LISTCHAR);

		/* load file name into the path */
		if (calcrc == NULL) {
			strcpy(path, cp);
		} else {
			strncpy(path, cp, calcrc - cp);
			path[calcrc - cp] = '\0';
		}

		/* find the start of the path */
		p = (path[0] == ':') ? path + 1 : path;
		if (p[0] == '\0') {
			continue;
		}

		/* process the current file in the list */
		if (openfile(p) < 0) {
			/* Unable to open rcfile */
			if (c_flag && !d_flag)
				 fprintf(stderr,
					"Unable to open rcfile \"%s\"\n", p);
			continue;
		}
		getcommands(FALSE);
		closeinput();
	}
}


/*
 * isinoderead - determine if we have read a given dev/inode
 *
 * This function returns the index of the readset element that matches
 * a given device/inode, -1 otherwise.
 *
 *
 * WIN32 NOTE:
 *
 *	This function does not work under WIN32. The sbuf->st_ino is always
 *	zero because the FAT and NTFS filesystems do not support inodes.
 *	They also don't support links, which is why you need this function
 *	under UNIX. For WIN32, use _fullpath() to determine if you have
 *	already opened a file.
 *
 * given:
 *	sbuf		stat of the inode in question
 */

static int
isinoderead(struct stat *sbuf)
{
	int i;
#if defined(_WIN32) || defined(__MSDOS__)
	char fullpathname[_MAX_PATH];
#endif

	/* deal with the empty case */
	if (readset == NULL || maxreadset <= 0) {
		/* readset is empty */
		return -1;
	}

	/* scan the entire readset */
	for (i=0; i < maxreadset; ++i) {
#if defined(_WIN32) || defined(__MSDOS__)
		if (readset[i].active &&
		    strcasecmp(readset[i].path,
		    	       _fullpath(fullpathname,cip->i_name,
			       _MAX_PATH)) == 0) {
			/* found a match */
			return i;
		}
#else /* Windoz free systems */
		if (readset[i].active &&
		    sbuf->st_dev == readset[i].inode.st_dev &&
		    sbuf->st_ino == readset[i].inode.st_ino) {
			/* found a match */
			return i;
		}
#endif /* Windoz free systems */
	}

	/* no match found */
	return -1;
}


/*
 * findfreeread - find the next free readset element
 *
 * This function will return the index of the next free readset element.
 * If needed, this function will allocate new readset elements.
 *
 * This function returns the index of the next free element, or -1.
 */
static int
findfreeread(void)
{
	int i;

	/* deal with an empty readset case */
	if (readset == NULL || maxreadset <= 0) {

		/* malloc a new readset */
		readset = (READSET *)malloc((READSET_ALLOC+1)*sizeof(READSET));
		if (readset == NULL) {
			return -1;
		}
		maxreadset = READSET_ALLOC;
		for (i=0; i < READSET_ALLOC; ++i) {
			readset[i].active = 0;
		}

		/* return first entry */
		return 0;
	}

	/* try to find a free readset entry */
	for (i=0; i < maxreadset; ++i) {
		if (readset[i].active == 0) {
			/* found a free readset entry */
			return i;
		}
	}

	/* all readset entries are in use, allocate more */
	readset = (READSET *)realloc(readset,
	    (maxreadset+READSET_ALLOC) * sizeof(READSET));
	if (readset == NULL) {
		return -1;
	}
	for (i=0; i < READSET_ALLOC; ++i) {
		readset[i+maxreadset].active = 0;
	}
	maxreadset += READSET_ALLOC;

	/* return the first newly allocated free entry */
	return maxreadset-READSET_ALLOC;
}


/*
 * addreadset - add a entry to the readset array if it is not already there
 *
 * This function attempts to add a file into the readset.  If the readset
 * has an entry with a matching dev/inode, then that entry is updated with
 * the new name and path.  If no such readset entry is found, a new entry
 * is added.
 *
 * This function returns the index of the readset entry, or -1 if error.
 *
 * given:
 *	name		name given to read or include
 *	path		full pathname of file
 *	sbuf		stat of the path
 */
static int
addreadset(char *name, char *path, struct stat *sbuf)
{
	int ret;		/* index to return */

	/* find the inode */
	ret = isinoderead(sbuf);
	if (ret < 0) {
		/* not in readset, find a free node */
		ret = findfreeread();
		if (ret < 0) {
			/* cannot find/form a free readset entry */
			return -1;
		}
	} else {
		/* found an readset entry, free old readset data */
		if (readset[ret].name != NULL) {
			free(readset[ret].name);
		}
		if (readset[ret].path != NULL) {
			free(readset[ret].path);
		}
	}

	/* load our information into the readset entry */
	readset[ret].name = (char *)malloc(strlen(name)+1);
	if (readset[ret].name == NULL) {
		return -1;
	}
	strcpy(readset[ret].name, name);
#if defined(_WIN32) || defined(__MSDOS__)
	/*
	 * For WIN32, _fullpath expands the path to a fully qualified
	 * path name, which under WIN32 FAT and NTFS is unique, just
	 * like UNIX inodes. _fullpath also allocated the memory for
	 * this new longer path name.
	 */
 	 {
	 	char fullpathname[_MAX_PATH];

		readset[ret].path = _fullpath(fullpathname, path, _MAX_PATH);
	 }
#else /* Windoz free systems */
	readset[ret].path = (char *)malloc(strlen(path)+1);
	if (readset[ret].path == NULL) {
		return -1;
	}
	strcpy(readset[ret].path, path);
#endif /* Windoz free systems */
	readset[ret].inode = *sbuf;
	readset[ret].active = 1;

	/* return index of the newly added entry */
	return ret;
}
