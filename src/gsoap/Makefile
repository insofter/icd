
WSDL_PATH=http://192.168.123.4/icdtcp3webservice/Service1.asmx?WSDL
#WSDL_PATH=http://79.110.195.114/icdtcp3webservice/Service1.asmx?WSDL
#WSDL_PATH=http://79.110.195.114/icdtcp3webservice/Service1.asmx?WSDL
#WSDL_PATH=http://www.insofter.pl/icdtcp3webservice/Service1.asmx?WSDL

#WSDL_PATH=http://192.168.123.194:8732/Design_Time_Addresses/wcfServicelibrary1/Service1/mex

XXSOAPPROXY_H=Service1SoapProxy.h
STUB_H=Stub.h
HEADER=kom.h


all: wget wsdl soapcpp stdsoap human_stub human_soapproxy list

wget: rm_schem
	wget ${WSDL_PATH} -O schem.wsdl

rm_schem:
	-rm schem.wsdl &> /dev/null


wsdl: 
	./wsdl2h -o ${HEADER} -nicd schem.wsdl

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
	-rm *.h *.c *.cpp *.nsmap schem.wsdl &> /dev/null
