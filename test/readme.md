

    sydCreateDatabase StandardDatabase test.db test-data -f

    sydInsert test.db Radionuclide Indium111 67.313
    sydInsert test.db Radionuclide Yttrium90 64.053

    sydInsert test.db Tag study1 "Initial study"
    sydInsert test.db Tag before_reg "Images before registration"

    sydInsert test.db Patient zh 1 100 0121BC1N07
	sydInsert test.db Patient vl 2 92
	sydInsert test.db Patient gg 3 103
	sydInsert test.db Patient rc 4 50
	sydInsert test.db Patient lg 5 62
	sydInsert test.db Patient ll 6 61
	sydInsert test.db Patient jm 8 64
	sydInsert test.db Patient bs 9 75
	sydInsert test.db Patient be 10 73.5
	sydInsert test.db Patient cf 11 96
	sydInsert test.db Patient ma 12 60
	sydInsert test.db Patient bm 13 65
	sydInsert test.db Patient dm 14 71
	sydInsert test.db Patient bf 15 60
	sydInsert test.db Patient wn 16 105
	sydInsert test.db Patient mn 17 66
	sydInsert test.db Patient ac 18 62
	sydInsert test.db Patient pd 19 75
	sydInsert test.db Patient cf2 20 56.5

	sydInsert test.db Injection jm Indium111 "2013-02-12 10:16" 206.43
	sydInsert test.db Injection zh Indium111 "2012-01-10 13:00" 193.02
	sydInsert test.db Injection vl Indium111 "2012-01-17 11:50" 190.48
	sydInsert test.db Injection gg Indium111 "2012-02-28 10:57" 217.4
	sydInsert test.db Injection rc Indium111 "2012-05-15 10:36" 216.24
	sydInsert test.db Injection lg Indium111 "2012-10-09 10:30" 210
	sydInsert test.db Injection ll Indium111 "2012-10-16 10:15" 215.95
	sydInsert test.db Injection bs Indium111 "2013-03-12 10:45" 183
	sydInsert test.db Injection be Indium111 "2013-03-19 10:30" 173.76
	sydInsert test.db Injection cf Indium111 "2013-04-23 10:07" 193
	sydInsert test.db Injection ma Indium111 "2013-05-14 12:30" 183.99
	sydInsert test.db Injection bm Indium111 "2013-07-09 13:10" 194
	sydInsert test.db Injection dm Indium111 "2013-11-05 11:18" 183.94
	sydInsert test.db Injection bf Indium111 "2013-11-26 10:48" 188.74
	sydInsert test.db Injection wn Indium111 "2013-12-17 10:33" 00
	sydInsert test.db Injection mn Indium111 "2014-01-14 10:23" 184.5
	sydInsert test.db Injection ac Indium111 "2014-07-15 11:58" 193
	sydInsert test.db Injection pd Indium111 "2014-08-26 10:12" 192
	sydInsert test.db Injection cf2 Indium111 "2014-11-18 11:18" 180

	sydInsert test.db Injection vl Yttrium90 "2012-02-15 11:10" 384
	sydInsert test.db Injection gg Yttrium90 "2012-03-28 10:15" 1161
	sydInsert test.db Injection gg Yttrium90 "2012-06-27 10:15" 1113
	sydInsert test.db Injection jm Yttrium90 "2013-02-27 10:40" 1113
	sydInsert test.db Injection be Yttrium90 "2013-04-10 10:22" 384
	sydInsert test.db Injection cf Yttrium90 "2013-05-16 10:57" 985
	sydInsert test.db Injection dm Yttrium90 "2013-11-20 10:40" 360
	sydInsert test.db Injection bf Yttrium90 "2013-12-11 10:24" 1161

    sydInsertDicom test.db jm Indium111 dicom/

    sydInsertTimepoint test.db study1 1 2
