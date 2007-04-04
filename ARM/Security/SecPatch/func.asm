/*
		RTMODEL "ARMv4M", "USED"
        RTMODEL "ARMv4T", "USED"
        RTMODEL "ARMv5T", "USED"
        RTMODEL "StackAlign4", "USED"
        RTMODEL "__cpu_mode", "__pcs__interwork"
        RTMODEL "__data_model", "absolute"
        RTMODEL "__dlib_file_descriptor", "0"
        RTMODEL "__endian", "little"
        RTMODEL "__rt_version", "6"
*/;===================================
	RSEG CODE:CODE:NOROOT(2)
	
	EXTERN patch
	CODE16
		RSEG N3PT:CODE:ROOT(1)
	
	BLX patch

	END
	