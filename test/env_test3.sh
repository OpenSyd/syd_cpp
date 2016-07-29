
## path for syd bin
export PATH=${HOME}/src/syd/build-release2/bin:$PATH

## syd plugins
export SYD_PLUGIN=~/src/syd/build-release2/lib

## syd current database
export SYD_CURRENT_DB=~/src/syd/syd-clean-API/test/test3.db

## check
echo db is $SYD_CURRENT_DB
which sydFind
