if load ${devtype} ${devnum}:1 0x40800000 qnx-ifs ; then
	if load ${devtype} ${devnum}:1 0x40100000 imx-dtb ; then
		echo "Starting QNX..." ;
		go 0x40800000 0x40100000 ;
	fi
fi
echo "Error loading QNX"
