
This example define a database schema named "MyDatabase" that extend the standard schema (StandardDatabase).

The only change here is an extended version of the table Patient, with an additional columne "birth_date".

Once compiled, add the location of the build library (e.g. libMyDatabase.so) to the SYD_PLUGIN environement variable, for example with :
    export SYD_PLUGIN=~/src/build-example/:${SYD_PLUGIN}

You can then use the standard tools:
    sydCreateDatabase MyDatabase mydb.db myfolder
    sydInsert mydb.db Patient toto 1 100 XXXXX "2014-05-02 12:30"
    sydInsert mydb.db Patient titi 2 80  XXXXX "2012-05-02 12:30"
    sydDump mydb.db
    sydDump mydb.db Patient

And a new tool:
    extTestTool mydb.db
