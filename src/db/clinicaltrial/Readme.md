

All tables have an `id` field (not shown here).


Patient table
==============
`name` -> or initials
`dicom_patient_name`
`dicom_patient_id`
`folder`
`weight_in_kg`
`synfrizz_id` -> 0 if not synfrizz (for example, calibration fantom)
`was_treated` boolean
`injection_date` -> date in string format = 2013-02-12 10:16 (for Indium not Yttrium)
`injected_quantity_in_MBq` (for Indium not Yttrium)

No "Study" table: a single 'study' by patient.


TimePoint table
===========
`patient_id`
`acquisition_date` -> date+hour in string format = 2013-02-12 10:16
`time_from_injection_in_hours`
`time_number`

Serie table
===========
`time_point_id`
`ct_image_id`
`spect_image_id`
`description` ==> ct or spect ?

For a given timepoint, several series could exist: several FOV, several reconstruction parameters. or warped version

Image Table -> core ?
===========
`mhd_filename`
`dicom_folder`
md5 *FIXME* ?


DB Creation/Evolution
=====================

- how to create a DB of a given type ?
```
    odb -d sqlite --generate-query --generate-schema person.hxx
    sqlite3 --user=odb_test --database=odb_test < person.sql
```
But *DELETE* the content of the db

- how to modify the schema an existing DB of a given type ?

Not
  implemented here. Need MANUAL change in both db (sql) and .hxx table file.


 ~/softs/sqlitestudio-2.1.5.bin -c /home/dsarrut/src/images/synfrizz/synfrizz.db


Registration/warp DB = a Serie table *FIXME* --> à mettre dans autre db
====================================

With a single Serie by timepoint.
