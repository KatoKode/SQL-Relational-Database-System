#-----------------------------------------------------------------------------
# Desc: RedB server program Makefile
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

bin_PROGRAMS = redd
redd_SOURCES = main.cpp

redd_CPPFLAGS = `pkg-config --cflags glib-2.0`\
								`pkg-config --cflags json-glib-1.0`\
								`libgcrypt-config --cflags`\
								-I$(top_srcdir)/eno\
								-I$(top_srcdir)/logger\
								-I$(top_srcdir)/util\
								-I$(top_srcdir)/comm\
								-I$(top_srcdir)/dio\
								-I$(top_srcdir)/buffer\
								-I$(top_srcdir)/bcalc\
								-I$(top_srcdir)/schema\
								-I$(top_srcdir)/lock\
								-I$(top_srcdir)/cache\
								-I$(top_srcdir)/row\
								-I$(top_srcdir)/btree\
								-I$(top_srcdir)/inc\
								-I$(top_srcdir)/token\
								-I$(top_srcdir)/tran\
								-I$(top_srcdir)/comp\
								-I$(top_srcdir)/sql\
								-I$(top_srcdir)/dbms\
								-I$(top_srcdir)/vm\
								-I$(top_srcdir)/srvcry

redd_LDFLAGS = `pkg-config --libs glib-2.0`\
						 `pkg-config --libs json-glib-1.0`\
						 `libgcrypt-config --libs`\
						 -lpthread -ldl -lm

redd_LDADD = $(top_srcdir)/srvcry/libsrvcry.a\
						 $(top_srcdir)/vm/libvm.a\
						 $(top_srcdir)/dbms/libdbms.a\
						 $(top_srcdir)/sql/libsql.a\
						 $(top_srcdir)/comp/libcomp.a\
						 $(top_srcdir)/tran/libtran.a\
						 $(top_srcdir)/token/libtoken.a\
						 $(top_srcdir)/btree/libbtree.a\
						 $(top_srcdir)/row/librow.a\
						 $(top_srcdir)/cache/libcache.a\
						 $(top_srcdir)/lock/liblock.a\
						 $(top_srcdir)/schema/libschema.a\
						 $(top_srcdir)/bcalc/libbcalc.a\
						 $(top_srcdir)/buffer/libbuffer.a\
						 $(top_srcdir)/dio/libdio.a\
						 $(top_srcdir)/comm/libcomm.a\
						 $(top_srcdir)/util/libutil.a\
						 $(top_srcdir)/logger/liblogger.a\
						 $(top_srcdir)/eno/libeno.a

install:
	install redd /usr/sbin
