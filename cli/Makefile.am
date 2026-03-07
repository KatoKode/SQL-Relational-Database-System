#-----------------------------------------------------------------------------
# Desc: RedB client program Makefile
#
# Copyright © 2019 J McIntosh
#
#   This file is part of redB.
#
#   redB is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; version 3 of the License.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with redB.  If not, see <https://www.gnu.org/licenses/>.
#-----------------------------------------------------------------------------

bin_PROGRAMS = redcli
redcli_SOURCES = main.cpp

redcli_CPPFLAGS = -I$(top_srcdir)/eno\
									-I$(top_srcdir)/logger\
									-I$(top_srcdir)/util\
									-I$(top_srcdir)/comm\
									-I$(top_srcdir)/clicry

redcli_LDFLAGS = `libgcrypt-config --libs`\
								 -lpthread -ldl -lm -lncurses -lreadline

redcli_LDADD = $(top_srcdir)/clicry/libclicry.a\
							 $(top_srcdir)/comm/libcomm.a\
							 $(top_srcdir)/util/libutil.a\
							 $(top_srcdir)/logger/liblogger.a\
							 $(top_srcdir)/eno/libeno.a

