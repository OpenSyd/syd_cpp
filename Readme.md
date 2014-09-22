
SYD
===

Toolkit. The goal is to manage a database of patient data, including images, for performing TRT (Targeted Radionuclide Therapy) related tasks.

DB management
=============

todo

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
