
SYD is a C++ toolkit to help perform image processing tasks on a database of patient in the field of Target Radionuclide Therapy. It is developed to manage set of SPECT/CT images and perform tasks such as computing activity or integrated activity in various ROIs.

Principles
==========


Main classes
============

The following classes are mapped to a SQL Table in the sqlite database.

In a syd::ClinicDatabase : manage the 3 following tables:
* syd::Patient : contains patient's info such as name (initials). 'synfrizz_id' is the patient number in the synfrizz study.
* syd::Serie : contains information about a set of dicom images. Associated with a Patient.
* syd::RoiType : simple list of names of ROIs (such as 'liver', 'LeftKidney').


In a syd::StudyDatabase : manage the 3 following tables:
* syd::Timepoint : when a spect/ct image is acquired, this object store information (link to spect and ct images, time etc).
* syd::RoiMaskImage
* syd::RawImage


<!--- syd::Activity -->


TODO
=====

Step1 : initial database creation
---------------------------------

Create a DB of dicom images organised by patients. Store some searchable text information read from the dicom header. Store information not read from the dicom such as the injected activity or time of injection.

sydCreate ClinicalDatabase db *FIXME*
sydInsertDicom --> warning patient 'name'
sydDump

*FIXME* how to modify the data (injection?)

Step2:
------

Select spect, find associated dicom, convert to raw image (mhd).
- sydDump for selection
- sydInsertTimepoint for insertion/conversion

Preprocess image (patient boundaries extraction, bg, crop).


Register, warp, compute 'average', images.

Associate ROIs




Dependencies
============

* gengetopt
For command line argument processing.
- http://www.gnu.org/software/gengetopt/gengetopt.html
- GNU gengetopt 2.22.5
- make and install

* sqlite3
For database management.
- http://www.sqlite.org
- version 3.7.13

* odb
For database management through OO code.
- http://www.codesynthesis.com/products/odb/
- odb 2.3.0
- install lib, odb compiler and  libodb-sqlite

* ceres-solver + glog + eigen3
For advanced optimisation algorithms.
- eigen : http://eigen.tuxfamily.org version 3.2.2
- glog : https://code.google.com/p/google-glog
  - on mac OSX : glog from svn
  - ceres-solver :http://ceres-solver.org version 1.9.0
    (from git)

* Already include : easylogging
- http://easylogging.org
- copy from git version 2014-08-06
- 4a74f67ced8929b7617ac7fb6b547dd94b130d48

* dcmtk
For DICOM management.
- http://dicom.offis.de/dcmtk.php.en
- From git.
- With option `DCMTK_WITH_TIFF=OFF` to avoid linking warning with ljpeg, ltiff.

* itk
For image processing.
- http://www.itk.org
- version 4.5.2

* Optional : sqlitestudio
* Optional : http://www.dcm4che.org *FIXME*

DICOM
=====

Discover series from folder.

Command : InsertDicom into ClinicalTrialDB
==========================================
Search recursively for dicom series in a folder. Sort by Patient/TimePoints/Series.




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

Timepoint table
==============
`serie_id`          -> serie
`patient_id`        -> patient (not strictly needed)
`number`            -> order in the list of timepoint for the patient
`time_from_injection_in_hours` -> computed from dicom date and injection_date

Registration table
==================
`timepoint_id`
`ref_number`
`moving_number`



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
