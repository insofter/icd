/* Service1Soap12Proxy.cpp
   Generated by gSOAP 2.8.8 from schem.h

Copyright(C) 2000-2012, Robert van Engelen, Genivia Inc. All Rights Reserved.
The generated code is released under one of the following licenses:
1) GPL or 2) Genivia's license for commercial use.
This program is released under the GPL with the additional exemption that
compiling, linking, and/or using OpenSSL is allowed.
*/

#include "Service1Soap12Proxy.h"

Service1Soap12Proxy::Service1Soap12Proxy()
{	Service1Soap12Proxy_init(SOAP_IO_DEFAULT, SOAP_IO_DEFAULT);
}

Service1Soap12Proxy::Service1Soap12Proxy(const struct soap &_soap) : soap(_soap)
{ }

Service1Soap12Proxy::Service1Soap12Proxy(const char *url)
{	Service1Soap12Proxy_init(SOAP_IO_DEFAULT, SOAP_IO_DEFAULT);
	soap_endpoint = url;
}

Service1Soap12Proxy::Service1Soap12Proxy(soap_mode iomode)
{	Service1Soap12Proxy_init(iomode, iomode);
}

Service1Soap12Proxy::Service1Soap12Proxy(const char *url, soap_mode iomode)
{	Service1Soap12Proxy_init(iomode, iomode);
	soap_endpoint = url;
}

Service1Soap12Proxy::Service1Soap12Proxy(soap_mode imode, soap_mode omode)
{	Service1Soap12Proxy_init(imode, omode);
}

Service1Soap12Proxy::~Service1Soap12Proxy()
{ }

void Service1Soap12Proxy::Service1Soap12Proxy_init(soap_mode imode, soap_mode omode)
{	soap_imode(this, imode);
	soap_omode(this, omode);
	soap_endpoint = NULL;
	static const struct Namespace namespaces[] =
{
	{"SOAP-ENV", "http://www.w3.org/2003/05/soap-envelope", "http://www.w3.org/2003/05/soap-envelope", NULL},
	{"SOAP-ENC", "http://www.w3.org/2003/05/soap-encoding", "http://www.w3.org/2003/05/soap-encoding", NULL},
	{"xsi", "http://www.w3.org/2001/XMLSchema-instance", "http://www.w3.org/*/XMLSchema-instance", NULL},
	{"xsd", "http://www.w3.org/2001/XMLSchema", "http://www.w3.org/*/XMLSchema", NULL},
	{"icd2", "http://tempuri.org/icdtcpwebservice/icdtcpws/Service1Soap", NULL, NULL},
	{"icd1", "http://tempuri.org/icdtcpwebservice/icdtcpws", NULL, NULL},
	{"icd3", "http://tempuri.org/icdtcpwebservice/icdtcpws/Service1Soap12", NULL, NULL},
	{NULL, NULL, NULL, NULL}
};
	soap_set_namespaces(this, namespaces);
}

void Service1Soap12Proxy::destroy()
{	soap_destroy(this);
	soap_end(this);
}

void Service1Soap12Proxy::reset()
{	destroy();
	soap_done(this);
	soap_init(this);
	Service1Soap12Proxy_init(SOAP_IO_DEFAULT, SOAP_IO_DEFAULT);
}

void Service1Soap12Proxy::soap_noheader()
{	this->header = NULL;
}

const SOAP_ENV__Header *Service1Soap12Proxy::soap_header()
{	return this->header;
}

const SOAP_ENV__Fault *Service1Soap12Proxy::soap_fault()
{	return this->fault;
}

const char *Service1Soap12Proxy::soap_fault_string()
{	return *soap_faultstring(this);
}

const char *Service1Soap12Proxy::soap_fault_detail()
{	return *soap_faultdetail(this);
}

int Service1Soap12Proxy::soap_close_socket()
{	return soap_closesock(this);
}

int Service1Soap12Proxy::soap_force_close_socket()
{	return soap_force_closesock(this);
}

void Service1Soap12Proxy::soap_print_fault(FILE *fd)
{	::soap_print_fault(this, fd);
}

#ifndef WITH_LEAN
#ifndef WITH_COMPAT
void Service1Soap12Proxy::soap_stream_fault(std::ostream& os)
{	::soap_stream_fault(this, os);
}
#endif

char *Service1Soap12Proxy::soap_sprint_fault(char *buf, size_t len)
{	return ::soap_sprint_fault(this, buf, len);
}
#endif

int Service1Soap12Proxy::HelloWorld(const char *endpoint, const char *soap_action, _icd1__HelloWorld *icd1__HelloWorld, _icd1__HelloWorldResponse *icd1__HelloWorldResponse)
{	struct soap *soap = this;
	struct __icd3__HelloWorld soap_tmp___icd3__HelloWorld;
	if (endpoint)
		soap_endpoint = endpoint;
	if (!soap_endpoint)
		soap_endpoint = "http://192.168.123.4/icdtcp3webservice/Service1.asmx";
	if (!soap_action)
		soap_action = "http://tempuri.org/icdtcpwebservice/icdtcpws/HelloWorld";
	soap->encodingStyle = NULL;
	soap_tmp___icd3__HelloWorld.icd1__HelloWorld = icd1__HelloWorld;
	soap_begin(soap);
	soap_serializeheader(soap);
	soap_serialize___icd3__HelloWorld(soap, &soap_tmp___icd3__HelloWorld);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___icd3__HelloWorld(soap, &soap_tmp___icd3__HelloWorld, "-icd3:HelloWorld", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___icd3__HelloWorld(soap, &soap_tmp___icd3__HelloWorld, "-icd3:HelloWorld", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	if (!icd1__HelloWorldResponse)
		return soap_closesock(soap);
	icd1__HelloWorldResponse->soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	icd1__HelloWorldResponse->soap_get(soap, "icd1:HelloWorldResponse", "");
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int Service1Soap12Proxy::GetTime(const char *endpoint, const char *soap_action, _icd1__GetTime *icd1__GetTime, _icd1__GetTimeResponse *icd1__GetTimeResponse)
{	struct soap *soap = this;
	struct __icd3__GetTime soap_tmp___icd3__GetTime;
	if (endpoint)
		soap_endpoint = endpoint;
	if (!soap_endpoint)
		soap_endpoint = "http://192.168.123.4/icdtcp3webservice/Service1.asmx";
	if (!soap_action)
		soap_action = "http://tempuri.org/icdtcpwebservice/icdtcpws/GetTime";
	soap->encodingStyle = NULL;
	soap_tmp___icd3__GetTime.icd1__GetTime = icd1__GetTime;
	soap_begin(soap);
	soap_serializeheader(soap);
	soap_serialize___icd3__GetTime(soap, &soap_tmp___icd3__GetTime);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___icd3__GetTime(soap, &soap_tmp___icd3__GetTime, "-icd3:GetTime", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___icd3__GetTime(soap, &soap_tmp___icd3__GetTime, "-icd3:GetTime", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	if (!icd1__GetTimeResponse)
		return soap_closesock(soap);
	icd1__GetTimeResponse->soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	icd1__GetTimeResponse->soap_get(soap, "icd1:GetTimeResponse", "");
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int Service1Soap12Proxy::GetTime2(const char *endpoint, const char *soap_action, _icd1__GetTime2 *icd1__GetTime2, _icd1__GetTime2Response *icd1__GetTime2Response)
{	struct soap *soap = this;
	struct __icd3__GetTime2 soap_tmp___icd3__GetTime2;
	if (endpoint)
		soap_endpoint = endpoint;
	if (!soap_endpoint)
		soap_endpoint = "http://192.168.123.4/icdtcp3webservice/Service1.asmx";
	if (!soap_action)
		soap_action = "http://tempuri.org/icdtcpwebservice/icdtcpws/GetTime2";
	soap->encodingStyle = NULL;
	soap_tmp___icd3__GetTime2.icd1__GetTime2 = icd1__GetTime2;
	soap_begin(soap);
	soap_serializeheader(soap);
	soap_serialize___icd3__GetTime2(soap, &soap_tmp___icd3__GetTime2);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___icd3__GetTime2(soap, &soap_tmp___icd3__GetTime2, "-icd3:GetTime2", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___icd3__GetTime2(soap, &soap_tmp___icd3__GetTime2, "-icd3:GetTime2", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	if (!icd1__GetTime2Response)
		return soap_closesock(soap);
	icd1__GetTime2Response->soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	icd1__GetTime2Response->soap_get(soap, "icd1:GetTime2Response", "");
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int Service1Soap12Proxy::TestSession(const char *endpoint, const char *soap_action, _icd1__TestSession *icd1__TestSession, _icd1__TestSessionResponse *icd1__TestSessionResponse)
{	struct soap *soap = this;
	struct __icd3__TestSession soap_tmp___icd3__TestSession;
	if (endpoint)
		soap_endpoint = endpoint;
	if (!soap_endpoint)
		soap_endpoint = "http://192.168.123.4/icdtcp3webservice/Service1.asmx";
	if (!soap_action)
		soap_action = "http://tempuri.org/icdtcpwebservice/icdtcpws/TestSession";
	soap->encodingStyle = NULL;
	soap_tmp___icd3__TestSession.icd1__TestSession = icd1__TestSession;
	soap_begin(soap);
	soap_serializeheader(soap);
	soap_serialize___icd3__TestSession(soap, &soap_tmp___icd3__TestSession);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___icd3__TestSession(soap, &soap_tmp___icd3__TestSession, "-icd3:TestSession", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___icd3__TestSession(soap, &soap_tmp___icd3__TestSession, "-icd3:TestSession", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	if (!icd1__TestSessionResponse)
		return soap_closesock(soap);
	icd1__TestSessionResponse->soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	icd1__TestSessionResponse->soap_get(soap, "icd1:TestSessionResponse", "");
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int Service1Soap12Proxy::OutDevice(const char *endpoint, const char *soap_action, _icd1__OutDevice *icd1__OutDevice, _icd1__OutDeviceResponse *icd1__OutDeviceResponse)
{	struct soap *soap = this;
	struct __icd3__OutDevice soap_tmp___icd3__OutDevice;
	if (endpoint)
		soap_endpoint = endpoint;
	if (!soap_endpoint)
		soap_endpoint = "http://192.168.123.4/icdtcp3webservice/Service1.asmx";
	if (!soap_action)
		soap_action = "http://tempuri.org/icdtcpwebservice/icdtcpws/OutDevice";
	soap->encodingStyle = NULL;
	soap_tmp___icd3__OutDevice.icd1__OutDevice = icd1__OutDevice;
	soap_begin(soap);
	soap_serializeheader(soap);
	soap_serialize___icd3__OutDevice(soap, &soap_tmp___icd3__OutDevice);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___icd3__OutDevice(soap, &soap_tmp___icd3__OutDevice, "-icd3:OutDevice", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___icd3__OutDevice(soap, &soap_tmp___icd3__OutDevice, "-icd3:OutDevice", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	if (!icd1__OutDeviceResponse)
		return soap_closesock(soap);
	icd1__OutDeviceResponse->soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	icd1__OutDeviceResponse->soap_get(soap, "icd1:OutDeviceResponse", "");
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int Service1Soap12Proxy::LoginDevice(const char *endpoint, const char *soap_action, _icd1__LoginDevice *icd1__LoginDevice, _icd1__LoginDeviceResponse *icd1__LoginDeviceResponse)
{	struct soap *soap = this;
	struct __icd3__LoginDevice soap_tmp___icd3__LoginDevice;
	if (endpoint)
		soap_endpoint = endpoint;
	if (!soap_endpoint)
		soap_endpoint = "http://192.168.123.4/icdtcp3webservice/Service1.asmx";
	if (!soap_action)
		soap_action = "http://tempuri.org/icdtcpwebservice/icdtcpws/LoginDevice";
	soap->encodingStyle = NULL;
	soap_tmp___icd3__LoginDevice.icd1__LoginDevice = icd1__LoginDevice;
	soap_begin(soap);
	soap_serializeheader(soap);
	soap_serialize___icd3__LoginDevice(soap, &soap_tmp___icd3__LoginDevice);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___icd3__LoginDevice(soap, &soap_tmp___icd3__LoginDevice, "-icd3:LoginDevice", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___icd3__LoginDevice(soap, &soap_tmp___icd3__LoginDevice, "-icd3:LoginDevice", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	if (!icd1__LoginDeviceResponse)
		return soap_closesock(soap);
	icd1__LoginDeviceResponse->soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	icd1__LoginDeviceResponse->soap_get(soap, "icd1:LoginDeviceResponse", "");
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int Service1Soap12Proxy::GetUpdateDataBin(const char *endpoint, const char *soap_action, _icd1__GetUpdateDataBin *icd1__GetUpdateDataBin, _icd1__GetUpdateDataBinResponse *icd1__GetUpdateDataBinResponse)
{	struct soap *soap = this;
	struct __icd3__GetUpdateDataBin soap_tmp___icd3__GetUpdateDataBin;
	if (endpoint)
		soap_endpoint = endpoint;
	if (!soap_endpoint)
		soap_endpoint = "http://192.168.123.4/icdtcp3webservice/Service1.asmx";
	if (!soap_action)
		soap_action = "http://tempuri.org/icdtcpwebservice/icdtcpws/GetUpdateDataBin";
	soap->encodingStyle = NULL;
	soap_tmp___icd3__GetUpdateDataBin.icd1__GetUpdateDataBin = icd1__GetUpdateDataBin;
	soap_begin(soap);
	soap_serializeheader(soap);
	soap_serialize___icd3__GetUpdateDataBin(soap, &soap_tmp___icd3__GetUpdateDataBin);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___icd3__GetUpdateDataBin(soap, &soap_tmp___icd3__GetUpdateDataBin, "-icd3:GetUpdateDataBin", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___icd3__GetUpdateDataBin(soap, &soap_tmp___icd3__GetUpdateDataBin, "-icd3:GetUpdateDataBin", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	if (!icd1__GetUpdateDataBinResponse)
		return soap_closesock(soap);
	icd1__GetUpdateDataBinResponse->soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	icd1__GetUpdateDataBinResponse->soap_get(soap, "icd1:GetUpdateDataBinResponse", "");
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int Service1Soap12Proxy::HasUpdateBin(const char *endpoint, const char *soap_action, _icd1__HasUpdateBin *icd1__HasUpdateBin, _icd1__HasUpdateBinResponse *icd1__HasUpdateBinResponse)
{	struct soap *soap = this;
	struct __icd3__HasUpdateBin soap_tmp___icd3__HasUpdateBin;
	if (endpoint)
		soap_endpoint = endpoint;
	if (!soap_endpoint)
		soap_endpoint = "http://192.168.123.4/icdtcp3webservice/Service1.asmx";
	if (!soap_action)
		soap_action = "http://tempuri.org/icdtcpwebservice/icdtcpws/HasUpdateBin";
	soap->encodingStyle = NULL;
	soap_tmp___icd3__HasUpdateBin.icd1__HasUpdateBin = icd1__HasUpdateBin;
	soap_begin(soap);
	soap_serializeheader(soap);
	soap_serialize___icd3__HasUpdateBin(soap, &soap_tmp___icd3__HasUpdateBin);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___icd3__HasUpdateBin(soap, &soap_tmp___icd3__HasUpdateBin, "-icd3:HasUpdateBin", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___icd3__HasUpdateBin(soap, &soap_tmp___icd3__HasUpdateBin, "-icd3:HasUpdateBin", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	if (!icd1__HasUpdateBinResponse)
		return soap_closesock(soap);
	icd1__HasUpdateBinResponse->soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	icd1__HasUpdateBinResponse->soap_get(soap, "icd1:HasUpdateBinResponse", "");
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int Service1Soap12Proxy::SendData(const char *endpoint, const char *soap_action, _icd1__SendData *icd1__SendData, _icd1__SendDataResponse *icd1__SendDataResponse)
{	struct soap *soap = this;
	struct __icd3__SendData soap_tmp___icd3__SendData;
	if (endpoint)
		soap_endpoint = endpoint;
	if (!soap_endpoint)
		soap_endpoint = "http://192.168.123.4/icdtcp3webservice/Service1.asmx";
	if (!soap_action)
		soap_action = "http://tempuri.org/icdtcpwebservice/icdtcpws/SendData";
	soap->encodingStyle = NULL;
	soap_tmp___icd3__SendData.icd1__SendData = icd1__SendData;
	soap_begin(soap);
	soap_serializeheader(soap);
	soap_serialize___icd3__SendData(soap, &soap_tmp___icd3__SendData);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___icd3__SendData(soap, &soap_tmp___icd3__SendData, "-icd3:SendData", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___icd3__SendData(soap, &soap_tmp___icd3__SendData, "-icd3:SendData", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	if (!icd1__SendDataResponse)
		return soap_closesock(soap);
	icd1__SendDataResponse->soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	icd1__SendDataResponse->soap_get(soap, "icd1:SendDataResponse", "");
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}
/* End of client proxy code */
