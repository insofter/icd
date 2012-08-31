#configurable variables begin
WSDL_PATH=http://192.168.123.4/icdtcp3webservice/Service1.asmx?WSDL
XXSOAPPROXY_H=Service1SoapProxy.h
#configurable variables end

STUB_H=Stub.h
HEADER=schem.h
SCHEM=schem.wsdl
XMLNAMESPACE=icd


all: wget wsdl soapcpp stdsoap human_stub human_soapproxy list


help:
	@echo ''
	@echo '    This  Makefile is used to create gSOAP headers and sources. It'
	@echo '    connects  to server, and downloads  WSDL  schema file. Path is'
	@echo '    configured by WSDL_PATH variable.'
	@echo ''
	@echo '    Next  it  generates "human readable" headers  from  STUB_H and'
	@echo '    XXSOAPPROXY_H where XX means your service name. Human readable'
	@echo '    headers names start with "_" so we can find them easily.'
	@echo '    _XXSoapProxy.h contains list of server  functions with params.'
	@echo '    _Stub.h contains parameter classes needed by server functions.'
	@echo ''
	@echo '    Usage:'
	@echo '       make            for create all files'
	@echo '       make clean      for delete all files'
	@echo ''
	@echo '    Cleaned, copied directory can be used fo  another project. You'
	@echo "    shouldn't run this make during compilation because it needs to"
	@echo '    connect to configured  server. This makefile was made for make'
	@echo '    life easier. Cookies  are enabled by "#define WITH_COOKIES" in'
	@echo '    stdsoap2.h according to documentation.'
	@echo ''
	@echo '    Headers: You have to include two files to your project:'
	@echo '       #include "gsoap/XXSoapProxy.h"'
	@echo '       #include "gsoapXXSoap.nsmap"'
	@echo ''
	@echo '    Compilation:  You have to compile and link following  files to'
	@echo '    your project:'
	@echo '       gsoap/C.cpp'
	@echo '       gsoap/Service1SoapProxy.cpp'
	@echo '       gsoap/stdsoap2.cpp'
	@echo '         (gSOAP is statically  compiled  into  your  project, see'
	@echo '          documentation and "-lgsoap++" option for shared library)'
	@echo ''
	@echo '    Binaries  wsdl2h and soapcpp2 are compiled for i386 GNU/Linux.'
	@echo '    For other versions visit http://gsoap2.sourceforge.net/ .'
	@echo ''


wget: rm_schem
	wget ${WSDL_PATH} -O ${SCHEM}

rm_schem:
	-rm ${SCHEM} &> /dev/null


wsdl: 
	./wsdl2h -o ${HEADER} -n${XMLNAMESPACE} ${SCHEM}

soapcpp:
	./soapcpp2 -i -C -Iimport -p "" -x ${HEADER}

stdsoap: stdsoap_h
	cp import/stdsoap2.c* ./

stdsoap_h:
	echo "#define WITH_COOKIES" > stdsoap2.h && cat import/stdsoap2.h >> stdsoap2.h

#only useful information from Stub.h == class variables
human_stub: human_stub_head
	cat ${STUB_H} | grep -v virtual | grep -v "#" | grep -v "/\* icd1" \
		| grep -v "()" | grep -v "public" | grep -v "struct soap" \
		| grep -v "\*\*\*\*" | grep -v " \* " \
		| grep -v "Copyright" | grep -v "generated code" \
		| grep -v "Genivia's license" | grep -v "released under" \
		| grep -v "compiling, linking" \
		| sed '/^$$/d; s/};/};\n/g' \
		>> _${STUB_H} 

human_stub_head:
	echo -e "////////////////////////////////////////////////////\n" \
          "This file is made from Stub.h to make reading easier\n" \
		      "////////////////////////////////////////////////////" \
					> _${STUB_H}

#only useful information from XXSoapProxy.h == methods and parameters
human_soapproxy: human_soapproxy_head
	cat ${XXSOAPPROXY_H} | grep "virtual	int" | grep -v "endpoint" \
		| grep -v "_close_socket" \
		| sed -e 's/{/\n{/g; s/}/}\n/g' \
		| grep -v "return" \
		| sed -e 's/(/(\n    /g; s/)/\n)/g; s/;/;\n\n/g; s/,/,\n   /g' \
	  >> _${XXSOAPPROXY_H}
	
human_soapproxy_head:	
	echo -e "///////////////////////////////////////////////////////////\n" \
          "This file is made from XXSoapProxy.h to make reading easier\n" \
		      "///////////////////////////////////////////////////////////" \
					> _${XXSOAPPROXY_H}

list:
	ls -lah


clean:
	-rm *.h *.c *.cpp *.nsmap ${SCHEM} &> /dev/null



