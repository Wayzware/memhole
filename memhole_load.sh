#!/bin/sh
module="memhole"
device="memhole"
mode="777"

# invoke insmod with all arguments we got
# and use a pathname, as insmod doesn't look in . by default
/sbin/insmod ./$module.ko $* || exit 1

# retrieve major number
major=$(awk "\$2==\"$module\" {print \$1}" /proc/devices)

# Remove stale nodes and replace them, then give gid and perms
rm -f /dev/memhole
mknod /dev/memhole c $major 0
chgrp staff /dev/memhole
chmod 777  /dev/memhole
