////////////////////////////////////////////////////
 This file is made from Stub.h to make reading easier
 ////////////////////////////////////////////////////
/* Stub.h
   Generated by gSOAP 2.8.8 from schem.h
*/
class SOAP_CMAC _icd1__HelloWorld
{
};

class SOAP_CMAC _icd1__HelloWorldResponse
{
	std::string *HelloWorldResult;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* optional element of type xsd:string */
};

class SOAP_CMAC _icd1__GetTime
{
};

class SOAP_CMAC _icd1__GetTimeResponse
{
	std::string *GetTimeResult;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* optional element of type xsd:string */
};

class SOAP_CMAC _icd1__GetTime2
{
	int l;	/* required element of type xsd:int */
};

class SOAP_CMAC _icd1__GetTime2Response
{
	std::string *GetTime2Result;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* optional element of type xsd:string */
};

class SOAP_CMAC _icd1__TestSession
{
};

class SOAP_CMAC _icd1__TestSessionResponse
{
	int TestSessionResult;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type xsd:int */
};

class SOAP_CMAC _icd1__OutDevice
{
	int ErrorNo;	/* required element of type xsd:int */
};

class SOAP_CMAC _icd1__OutDeviceResponse
{
	std::string *OutDeviceResult;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* optional element of type xsd:string */
};

class SOAP_CMAC _icd1__LoginDevice
{
	int idd;	/* required element of type xsd:int */
	std::string *name;	/* optional element of type xsd:string */
	std::string *DevInfo;	/* optional element of type xsd:string */
};

class SOAP_CMAC _icd1__LoginDeviceResponse
{
	std::string *LoginDeviceResult;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* optional element of type xsd:string */
};

class SOAP_CMAC _icd1__GetUpdateDataBin
{
	int nPage;	/* required element of type xsd:int */
	int nSize;	/* required element of type xsd:int */
};

class SOAP_CMAC _icd1__GetUpdateDataBinResponse
{
	std::string *GetUpdateDataBinResult;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* optional element of type xsd:string */
};

class SOAP_CMAC _icd1__HasUpdateBin
{
	std::string *DevVer;	/* optional element of type xsd:string */
};

class SOAP_CMAC _icd1__HasUpdateBinResponse
{
	std::string *HasUpdateBinResult;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* optional element of type xsd:string */
};

class SOAP_CMAC _icd1__SendData
{
	std::string *data;	/* optional element of type xsd:string */
};

class SOAP_CMAC _icd1__SendDataResponse
{
	std::string *SendDataResult;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* optional element of type xsd:string */
};

/* Operation wrapper: */
struct __icd2__HelloWorld
{
	_icd1__HelloWorld *icd1__HelloWorld;	/* optional element of type icd1:HelloWorld */
};

/* Operation wrapper: */
struct __icd2__GetTime
{
	_icd1__GetTime *icd1__GetTime;	/* optional element of type icd1:GetTime */
};

/* Operation wrapper: */
struct __icd2__GetTime2
{
	_icd1__GetTime2 *icd1__GetTime2;	/* optional element of type icd1:GetTime2 */
};

/* Operation wrapper: */
struct __icd2__TestSession
{
	_icd1__TestSession *icd1__TestSession;	/* optional element of type icd1:TestSession */
};

/* Operation wrapper: */
struct __icd2__OutDevice
{
	_icd1__OutDevice *icd1__OutDevice;	/* optional element of type icd1:OutDevice */
};

/* Operation wrapper: */
struct __icd2__LoginDevice
{
	_icd1__LoginDevice *icd1__LoginDevice;	/* optional element of type icd1:LoginDevice */
};

/* Operation wrapper: */
struct __icd2__GetUpdateDataBin
{
	_icd1__GetUpdateDataBin *icd1__GetUpdateDataBin;	/* optional element of type icd1:GetUpdateDataBin */
};

/* Operation wrapper: */
struct __icd2__HasUpdateBin
{
	_icd1__HasUpdateBin *icd1__HasUpdateBin;	/* optional element of type icd1:HasUpdateBin */
};

/* Operation wrapper: */
struct __icd2__SendData
{
	_icd1__SendData *icd1__SendData;	/* optional element of type icd1:SendData */
};

/* Operation wrapper: */
struct __icd3__HelloWorld
{
	_icd1__HelloWorld *icd1__HelloWorld;	/* optional element of type icd1:HelloWorld */
};

/* Operation wrapper: */
struct __icd3__GetTime
{
	_icd1__GetTime *icd1__GetTime;	/* optional element of type icd1:GetTime */
};

/* Operation wrapper: */
struct __icd3__GetTime2
{
	_icd1__GetTime2 *icd1__GetTime2;	/* optional element of type icd1:GetTime2 */
};

/* Operation wrapper: */
struct __icd3__TestSession
{
	_icd1__TestSession *icd1__TestSession;	/* optional element of type icd1:TestSession */
};

/* Operation wrapper: */
struct __icd3__OutDevice
{
	_icd1__OutDevice *icd1__OutDevice;	/* optional element of type icd1:OutDevice */
};

/* Operation wrapper: */
struct __icd3__LoginDevice
{
	_icd1__LoginDevice *icd1__LoginDevice;	/* optional element of type icd1:LoginDevice */
};

/* Operation wrapper: */
struct __icd3__GetUpdateDataBin
{
	_icd1__GetUpdateDataBin *icd1__GetUpdateDataBin;	/* optional element of type icd1:GetUpdateDataBin */
};

/* Operation wrapper: */
struct __icd3__HasUpdateBin
{
	_icd1__HasUpdateBin *icd1__HasUpdateBin;	/* optional element of type icd1:HasUpdateBin */
};

/* Operation wrapper: */
struct __icd3__SendData
{
	_icd1__SendData *icd1__SendData;	/* optional element of type icd1:SendData */
};

/* SOAP Header: */
struct SOAP_ENV__Header
{
private:
	char dummy;	/* dummy member to enable compilation */
};

/* SOAP Fault Code: */
struct SOAP_ENV__Code
{
	char *SOAP_ENV__Value;	/* optional element of type xsd:QName */
	struct SOAP_ENV__Code *SOAP_ENV__Subcode;	/* optional element of type SOAP-ENV:Code */
};

/* SOAP-ENV:Detail */
struct SOAP_ENV__Detail
{
	char *__any;
	int __type;	/* any type of element <fault> (defined below) */
	void *fault;	/* transient */
};

/* SOAP-ENV:Reason */
struct SOAP_ENV__Reason
{
	char *SOAP_ENV__Text;	/* optional element of type xsd:string */
};

/* SOAP Fault: */
struct SOAP_ENV__Fault
{
	char *faultcode;	/* optional element of type xsd:QName */
	char *faultstring;	/* optional element of type xsd:string */
	char *faultactor;	/* optional element of type xsd:string */
	struct SOAP_ENV__Detail *detail;	/* optional element of type SOAP-ENV:Detail */
	struct SOAP_ENV__Code *SOAP_ENV__Code;	/* optional element of type SOAP-ENV:Code */
	struct SOAP_ENV__Reason *SOAP_ENV__Reason;	/* optional element of type SOAP-ENV:Reason */
	char *SOAP_ENV__Node;	/* optional element of type xsd:string */
	char *SOAP_ENV__Role;	/* optional element of type xsd:string */
	struct SOAP_ENV__Detail *SOAP_ENV__Detail;	/* optional element of type SOAP-ENV:Detail */
};

typedef char *_QName;
typedef char *_XML;
/* End of Stub.h */