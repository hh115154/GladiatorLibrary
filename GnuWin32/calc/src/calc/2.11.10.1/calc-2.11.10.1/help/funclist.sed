s/VALUE/int/
s/NUMBER[	 ]*\*/int /
s/NUMBER/int/
s/STRINGHEAD/int/
s/\(".*",.*,.*\),.*,.*,.*,.*,/\1, 0, 0, 0, 0,/
s/[	 ][	 ]*$//
p

## Copyright (C) 1999  Landon Curt Noll
##
## Calc is open software; you can redistribute it and/or modify it under
## the terms of the version 2.1 of the GNU Lesser General Public License
## as published by the Free Software Foundation.
##
## Calc is distributed in the hope that it will be useful, but WITHOUT
## ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
## or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU Lesser General
## Public License for more details.
##
## A copy of version 2.1 of the GNU Lesser General Public License is
## distributed with calc under the filename COPYING-LGPL.  You should have
## received a copy with calc; if not, write to Free Software Foundation, Inc.
## 59 Temple Place, Suite 330, Boston, MA  02111-1307, USA.
##
## @(#) $Revision: 29.2 $
## @(#) $Id: funclist.sed,v 29.2 2000/06/07 14:02:33 chongo Exp $
## @(#) $Source: /usr/local/src/cmd/calc/help/RCS/funclist.sed,v $
##
## Under source code control:	1995/07/10 01:33:06
## File existed as early as:	1995
##
## chongo <was here> /\oo/\	http://www.isthe.com/chongo/
## Share and enjoy!  :-)	http://www.isthe.com/chongo/tech/comp/calc/
