#-----------------------------------------------------------------------------
# Desc: Server Public Key Encryption/Decryption Library Makefile
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

noinst_LIBRARIES = libsrvcry.a
libsrvcry_a_SOURCES = srvcry.h srvcry.cpp

libsrvcry_a_CPPFLAGS =\
										`libgcrypt-config --cflags`\
										-I$(top_srcdir)/eno -I$(top_srcdir)/logger\
										-I$(top_srcdir)/util
