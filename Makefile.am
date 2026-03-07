#-----------------------------------------------------------------------------
# Desc: Main Makefile
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

SUBDIRS = eno logger util comm dio buffer bcalc schema lock cache row btree\
					token tran comp sql dbms vm srvcry clicry srv cli redb_install_db

install-data-local:
	test -d "/usr/lib/systemd/system" && \
		cp "service/redb.service" "/usr/lib/systemd/system"
	getent passwd redb &> /dev/null || \
		useradd --home-dir /var/lib/redb --shell /bin/false --system\
		--user-group redb
	test -d "/usr/lib/redb" || $(MKDIR_P) "/usr/lib/redb"
	test -d "/usr/lib/redb" && \
		cp "service/redb-systemd-helper" "/usr/lib/redb" && \
		chmod 755 /usr/lib/redb/redb-systemd-helper

uninstall-local:
	test -f "/usr/lib/systemd/system/redb.service" && \
		rm -f "/usr/lib/systemd/system/redb.service"
	test -d "/usr/lib/redb" && rm -fR "/usr/lib/redb"
#	getent passwd redb &> /dev/null && userdel --force redb
#	getent group redb &> /dev/null && groupdel redb
