#! /bin/sh

BACKUP_FPATTERN=/home/manu/calculinator/backups/ccltor_dump.sql.gz
BACKUP2_FPATTERN=/media/pen/ccltor_dump.sql.gz

echo "[`date`] $0 Running ..."
if [ `ps -e | grep postmaster | wc -l` -lt 1 ]; then
	echo "Error: DBMS seems to be down :-("
	exit 1
fi
mv $BACKUP_FPATTERN.4 $BACKUP_FPATTERN.5
mv $BACKUP_FPATTERN.3 $BACKUP_FPATTERN.4
mv $BACKUP_FPATTERN.2 $BACKUP_FPATTERN.3
mv $BACKUP_FPATTERN.1 $BACKUP_FPATTERN.2
mv $BACKUP_FPATTERN $BACKUP_FPATTERN.1
pg_dump calculinator | gzip > $BACKUP_FPATTERN
mv $BACKUP2_FPATTERN $BACKUP2_FPATTERN.1
cp $BACKUP_FPATTERN $BACKUP2_FPATTERN
echo "[`date`] $0 Done :-)"
