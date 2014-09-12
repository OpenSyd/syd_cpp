
SYD
===

Toolkit. The goal is to manage a database of patient data, including images, to perform bash processes.

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

* dcmtk
For DICOM management.
- http://dicom.offis.de/dcmtk.php.en
- From git.
- With option `DCMTK_WITH_TIFF=OFF` to avoid linking warning with ljpeg, ltiff.

* Optional : sqlitestudio
* Optional : http://www.dcm4che.org *FIXME*

DICOM
=====

Discover series from folder.

Command : InsertDicom into ClinicalTrialDB
==========================================
Search recursively for dicom series in a folder. Sort by Patient/TimePoints/Series.
