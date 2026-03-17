# SRDB (red-db) – Lightweight Client-Server SQL Relational Database

[![License: GPL-2.0](https://img.shields.io/badge/License-GPL%202.0-blue.svg)](https://opensource.org/licenses/GPL-2.0)
[![Stars](https://img.shields.io/github/stars/KatoKode/SQL-Relational-Database-System?style=social)](https://github.com/KatoKode/SQL-Relational-Database-System/stargazers)

Original Link: sourceforge.net/projects/red-db/

SRDB (aka red-db) is a **lightweight, persistent client-server RDBMS** written entirely in **C++14**. Designed for simplicity, reliability, and educational value, it implements a full SQL engine without relying on heavy external database libraries.

<div align="center">
  <img src="https://github.com/user-attachments/assets/3364fa85-1d9b-45b3-8558-a77334c6ec09"
       height="640"
       width="48%" 
       alt="Neon green terminal screenshot 1">
  <img src="https://github.com/user-attachments/assets/e938418e-9753-4e2c-9457-acb7b1eb4b07"
       height="640"
       width="48%" 
       alt="Neon green terminal screenshot 2">
</div>

*(Interactive terminal client showing SELECT with WHERE and ORDER BY, computed expressions, and formatted output)*

## Features

- **Full SQL support** (DDL + DML):
  - CREATE / ALTER / DROP DATABASE, TABLE, USER, INDEX, FOREIGN KEY
  - INSERT, UPDATE, DELETE, SELECT with JOIN (INNER/LEFT/RIGHT), WHERE, ORDER BY, expressions
  - LOAD DATA INFILE / SELECT ... INTO OUTFILE
  - SOURCE for script execution
- **Storage Engine**: Disk-based **B+Tree** with sibling-linked leaves, block caching, background flush thread
- **Query Execution**: Hand-written recursive-descent parser >> bytecode compiler >> virtual machine
- **Networking & Security**:
  - Multi-threaded TCP server (per-client FSM threads)
  - Encrypted credentials & statements (libgcrypt)
  - Authentication, GRANT/REVOKE privileges, host-based access
- **Client**: ncurses + readline terminal interface with command history
- **Schema**: Persistent JSON format (json-glib)
- **Reliability**: Valgrind-clean, shared/unique locking, foreign/unique key enforcement

## Security & Deployment

SRDB was designed with security and operational reliability in mind:

- **Installer** (`redb_install_db`): Fully automated setup that creates the credential and privilege tables, generates cryptographic material, bootstraps the root user, and drops privileges to a dedicated `redb` system user.
- **Reader/Writer Locking**: RAII `shared_lock` and `unique_lock` classes built on `pthread_rwlock_t` for safe concurrent access to tables and the B+Tree.
- **End-to-End Encryption**: Asymmetric RSA key exchange (libgcrypt) for client authentication and statement encryption + SHA-256 password hashing.
- **Systemd Integration**: Production-ready service file + helper script that creates secure directories (`/var/lib/redb`, `/tmp/redb`), sets correct ownership/permissions, and starts the daemon as non-root.
- **Data Integrity**: Strict per-column validation (type, size, NOT NULL, auto-increment, foreign/primary key checks) inside the `row` class.

These features make SRDB suitable as a lightweight, self-contained database for tools, testing, or learning systems programming.

## Tech Stack

- C++14+ • ncurses • readline • glib-2.0 • json-glib • libgcrypt
- Systemd service integration (Linux only)
- Direct I/O, manual block management, custom locking

## Quick Start (Linux)

1. **Prerequisites**
   ```bash
   sudo apt update
   sudo apt install build-essential libncurses5-dev libreadline-dev libglib2.0-dev libjson-glib-dev libgcrypt20-dev

Library Requirements
====================

ncurses
readline
glib-2.0
json-glib-1.0
libgcrypt-1.6.1 or later

Host and Service Settings
=========================
On the client, the hostname 'redbhost' must be set in the /etc/hosts file.

Example:
```bash
192.168.0.10    redbhost
```
On the client and the server, the service 'redb' must be set in the
/etc/services file.

Example:
```bash
redb             40004/tcp    # redb Server
```
Firewall Settings
=================

If necessary, be sure to open the 'redb' port in your firewall.
```bash
Example: (as root)

iptables -I INPUT 1 -p tcp -dport redb -j ACCEPT
```
Compiler Requirements
=====================

C++14 or higher

Recommended Configure Command
=============================
```bash
./configure CXX=g++-7 CXXFLAGS="-O2 -std=c++14"
```
Initial Server Startup
======================
```bash
systemctl enable redb.service

systemctl start redb.service
```
At this point server (redd) is running and will start each time OS boots.

Stopping Server
===============
```bash
systemctl stop redb.service
```
Starting Server
===============
```bash
systemctl start redb.service
```
Initial Login and Security
==========================

Initially user root has no password.
```bash
Initial user: root

Initial password: <none>
```
Log into server as root as follows:
```bash
  redcli -u root
```
If the server is running the client will provide the following prompt:
```bash
  redb>
```
At this point use the ALTER USER statement to set a password for root.

Client Syntax
=============

[]  denotes optional parameter
```bash
redcli -u <username> [-p]
```
  -u    username option (required)
  
  -p    password option (prompts user for password)

History File
============

The client (redcli) creates a history file '.redcli_history' in the users home
directory. Deleting or moving this file will result in no history when client
is run.

Script Files
============

Location of script files, INFILEs and OUTFILEs is /tmp/redb. Only the filename
need be provided to SQL statements like SOURCE, LOAD DATA INFILE, and SELECT
statements that use INTO OUTFILE. See examples below.

Examples:

LOAD DATA INFILE 'filename' ...;

SELECT ... INTO OUTFILE 'filename';

SOURCE 'filename';

NOTE: Set owner and group of scripts copied to /tmp/redb to redb:root. Set
      flags to 0x600 where user has read/write, group has none, and other has
      none.
      
      Ex: chmod 600 /tmp/redb/scr.sql
      Ex: chown redb:root /tmp/redb/scr.sql

Supported SQL Syntax
====================

  ------------------
  ALTER TABLE Syntax
  ------------------

  ALTER TABLE NAME alter_opt alter_spec ;

  alter_opt: ADD | DROP

  alter_spec: foreign_key | index

  foreign_key: FOREIGN KEY ( column_list ) REFERENCES table_reference
    ( column_list )

  index: UNIQUE INDEX ( column_list ) | INDEX ( column_list )

  -----------------
  ALTER USER Syntax
  -----------------

  ALTER USER 'username'@'hostname' IDENTIFIED BY 'auth-string' opt_replace ;

  opt_replace: REPLACE 'auth-string'

  ----------------------
  CREATE DATABASE Syntax
  ----------------------

  CREATE DATABASE NAME ;

  -------------------
  CREATE TABLE Syntax
  -------------------

  CREATE TABLE NAME ( column_definition_list ) ;

  column_definition_list: column_definition
    | column_definition_list , column_definition

  column_definition: NAME data_type | PRIMARY KEY ( NAME )

  data_type: CHAR ( INT )
    | ENUM ( enum_list )
    | INT
    | FLOAT ( INT , INT )
    | VARCHAR ( INT )

  enum_list: 'string'
    | enum_list , 'string'

  -------------
  DELETE Syntax
  -------------

  DELETE FROM NAME WHERE expr ;

  ---------------
  DESCRIBE Syntax
  ---------------

  DESCRIBE NAME ;

  --------------------
  DROP DATABASE Syntax
  --------------------

  DROP DATABASE NAME ;

  -----------------
  DROP TABLE Syntax
  -----------------

  DROP TABLE NAME ;

  ----------------
  DROP USER Syntax
  ----------------

  DROP USER 'username'@'hostname' ;

  ------------
  GRANT Syntax
  ------------

  GRANT priv_list ON object_type TO 'username'@'hostname'
    IDENTIFIED BY 'auth-string' ;

  priv_list: priv_type
    | priv_list , priv_type

  priv_type: ALL
    | ALTER
    | CREATE
    | CREATE USER
    | DELETE
    | DROP
    | FILE
    | GRANT
    | INDEX
    | INSERT
    | RELOAD
    | SELECT
    | SHOW DATABASE
    | SHUTDOWN
    | UPDATE

  object_type: *
    | * . *
    | NAME . *
    | NAME . NAME
    | NAME

  -------------
  INSERT Syntax
  -------------

  INSERT INTO NAME opt_column_list VALUES insert_value_list ;

  opt_column_list:
    | ( column_list )

  column_list: NAME
    | column_list , NAME

  insert_value_list: ( value_list )

  value_list: expr
    | value_list , expr

  ----------------
  LOAD DATA Syntax
  ----------------

  LOAD DATA INFILE 'filename' INTO TABLE NAME ;

  -------------
  REVOKE Syntax
  -------------

  REVOKE priv_list ON object_type FROM 'username'@'hostname'
    IDENTIFIED BY 'auth-string' ;

  -------------
  SELECT Syntax
  -------------

  SELECT select_expr_list FROM table_reference_list opt_where
    opt_order_by opt_into_outfile ;

  select_expr_list: select_expr
    | select_expr_list , select_expr

  select_expr: expr

  table_reference_list: table_reference
    | table_reference_list , table_reference

  table_reference: table_factor
    | joined_table
  
  table_factor: NAME opt_as_alias
    | NAME . NAME opt_as_alias

  opt_as_alias: NAME
    | AS NAME

  joined_table: table_reference join_opt table_factor join_spec

  join_opt: [ INNER | { LEFT | RIGHT } [ OUTER ] ] JOIN

  join_spec: ON expr | USING ( column_list )

  column_list: NAME
    | column_list , NAME

  opt_where: WHERE expr

  opt_order_by: ORDER BY column_list
    | ORDER BY ( column_list )

  opt_into_outfile: INTO OUTFILE 'filename'

  ---------------------
  SHOW DATABASES Syntax
  ---------------------

  SHOW DATABASES ;

  ------------------
  SHOW TABLES Syntax
  ------------------

  SHOW TABLES ;

  -------------
  SOURCE Syntax
  -------------

  SOURCE 'filename' ;

  -------------
  UPDATE Syntax
  -------------

  UPDATE NAME SET assignment_list WHERE expr ;

  assignment_list: assignment
    | assignment_list , assignment

  assignment: NAME = expr

  ----------
  USE Syntax
  ----------

  USE NAME ;

  -----------------
  EXPRESSION Syntax
  -----------------

  expr: FLOAT
    | INT
    | NAME
    | NAME . NAME
    | NAME . *
    | VARCHAR

  expr: expr * expr
    | expr + expr
    | expr - expr
    | expr / expr

  expr: expr < expr
    | expr > expr
    | expr == expr
    | expr <= expr
    | expr >= expr
