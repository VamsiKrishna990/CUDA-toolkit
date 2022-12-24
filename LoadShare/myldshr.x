struct MyEP {
	int NV;
	int MV;
	int SV;
};s

struct MyList {
	double value;
	MyList *next;
};

program PRINTER {
	version PRINTER_V1 {
		double GET_LOAD() = 1;
		double GET_MAX_GPU(exp_param) = 2;
		MyList UPD_MyList(MyList) = 3;
	} = 1;
} = 0x20004970;
