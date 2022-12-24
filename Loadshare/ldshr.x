struct exp_param {
	int N;
	int M;
	int S;
};

struct list {
	double value;
	list *next;
};

program PRINTER {
	version PRINTER_V1 {
		double GET_LOAD() = 1;
		double GET_MAX_GPU(exp_param) = 2;
		list UPD_LIST(list) = 3;
	} = 1;
} = 0x20004970;
