
All tables have an `id` field (not shown here).


Patient table
==============
`name`            -> patient initials. Must be unique
`synfrizz_id`     -> 0 if not synfrizz (for example, calibration fantom)
`path`            -> folder with the data (relative to DB folder)
`weight_in_kg`    -> weight
`was_treated`     -> boolean
`injection_date`  -> date in string format = 2013-02-12 10:16 (for Indium not Yttrium)
`injected_quantity_in_MBq` (for Indium not Yttrium)

Serie table
===========
`patient_id`        -> patient
`dicom_uid`         -> sop (for NM) or series (for CT) UID from the dicom
`dicom_description` -> string with SeriesDescription and ImageID
`path`              -> (sub)folder with the data, relative to patient folder
`acquisition_date`  -> image acquisition date
`modality`          -> CT or NM

DB Creation/Evolution
=====================

- how to create a DB of a given type ?
```
    odb -d sqlite --generate-query --generate-schema person.hxx
    sqlite3 --user=odb_test --database=odb_test < person.sql
```
But *DELETE* the content of the db

- how to modify the schema an existing DB of a given type ?
Not implemented here. Need MANUAL change in both db (sql) and .hxx table file.

- use sqlitestudio
 ~/softs/sqlitestudio-2.1.5.bin -c /home/dsarrut/src/images/synfrizz/synfrizz.db
