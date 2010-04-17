#! /bin/sh

BACKUP_SCRIPT=/home/manu/calculinator/ccltor_backup.sh
BACKUP_LOG_FILE=/home/manu/calculinator/logs/backup.log

su - manu -c "sh $BACKUP_SCRIPT >> $BACKUP_LOG_FILE 2>&1"
