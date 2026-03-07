redb_wait() {
  for i in {1..60}; do
    if [[ ! -s /var/lib/redb/etc/cred.d ]]; then
      sleep 1
    fi
  done
}

redb_start() {
  redb_wait
  echo "done"
}

redb_start
