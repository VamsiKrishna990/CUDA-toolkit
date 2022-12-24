CC=nvcc
APPN= ldshr
USE_GPU = true
RSRC=$(APPN).x $(APPN)_svc_proc.c $(APPN).c
CFLAGS= -DDEBUG
RPCGEN = rpcgen
CLIENT_AND_SERVER: $(APPN) $(APPN)_svc
all: $(CLIENT_AND_SERVER)
$(APPN)_xdr.c $(APPN)_svc.c $(APPN)_clnt.c $(APPN).h:: $(APPN).x
	$(RPCGEN) $(APPN).x
$(APPN): $(APPN).h $(APPN).o $(APPN)_clnt.o $(APPN)_xdr.o
	$(CC) $(CFLAGS) -o $(APPN) $(APPN).o $(APPN)_clnt.o $(APPN)_xdr.o \
	$(LFLAGS) 
$(APPN).o: $(APPN).c $(APPN).h
	$(CC) -c -o $@ $(CFLAGS) $(APPN).c 
ifeq ($(USE_GPU), true)
getmax.o: getmax.cu
	$(CC) -c -o $@ $(CFLAGS) getmax.cu
else
getmax.o: getmax.c
	$(CC) -c -o $@ $(CFLAGS) getmax.c
endif
$(APPN)_svc_proc.o: $(APPN)_svc_proc.c $(APPN).h
	$(CC) -c -o $@ $(CFLAGS) $(APPN)_svc_proc.c
$(APPN)_svc: $(APPN).h $(APPN)_svc_proc.o $(APPN)_svc.o getmax.o $(APPN)_xdr.o
	$(CC) $(CFLAGS) -o $(APPN)_svc $(APPN)_svc_proc.o getmax.o $(APPN)_svc.o $(APPN)_xdr.o $(LFLAGS) -lm
clean:
	$(RM) $(APPN) $(APPN)_svc $(APPN)_svc.c $(APPN)_xdr.c $(APPN)_clnt.c $(APPN).h *.o *.*%
$(APPN).ar: $(RSRC)
	bundle $(RSRC) > $(APPN).ar
