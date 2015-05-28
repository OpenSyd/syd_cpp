

db=$1

echo "Migrate db : " $db

sqlite3 ${db} .dump  > ${db}.sql
sydCreateDatabase StandardDatabase ${db} test -f
sqlite3 -init ${db}.sql ${db} .exit
