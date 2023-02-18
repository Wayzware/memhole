#!/bin/sh
module="memhole"
device="memhole"

# invoke rmmod with all arguments we got
/sbin/rmmod $module $* || exit 1

# Remove stale nodes

rm -f /dev/${device} /dev/${device}[0-3] 
rm -f /dev/${device}priv
rm -f /dev/${device} /dev/${device}[0-3]
rm -f /dev/${device}single
rm -f /dev/${device}uid
rm -f /dev/${device}wuid
