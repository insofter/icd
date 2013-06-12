/* Stub.h
   Generated by gSOAP 2.8.8 from schem.h

Copyright(C) 2000-2012, Robert van Engelen, Genivia Inc. All Rights Reserved.
The generated code is released under one of the following licenses:
1) GPL or 2) Genivia's license for commercial use.
This program is released under the GPL with the additional exemption that
compiling, linking, and/or using OpenSSL is allowed.
*/

#ifndef Stub_H
#define Stub_H
#include <vector>
#define SOAP_NAMESPACE_OF_icd1	"http://insofter.pl/webservices"
#include "stdsoap2.h"
#if GSOAP_VERSION != 20808
# error "GSOAP VERSION MISMATCH IN GENERATED CODE: PLEASE REINSTALL PACKAGE"
#endif


/******************************************************************************\
 *                                                                            *
 * Enumerations                                                               *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Types with Custom Serializers                                              *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Classes and Structs                                                        *
 *                                                                            *
\******************************************************************************/


#if 0 /* volatile type: do not declare here, declared elsewhere */

#endif

#if 0 /* volatile type: do not declare here, declared elsewhere */

#endif

#ifndef SOAP_TYPE_icd1__UpdatePack
#define SOAP_TYPE_icd1__UpdatePack (8)
/* icd1:UpdatePack */
class SOAP_CMAC icd1__UpdatePack
{
public:
	std::string *Link;	/* optional element of type xsd:string */
	std::string *Md5;	/* optional element of type xsd:string */
	LONG64 FileSize;	/* required element of type xsd:long */
	std::string *UpdateForce;	/* optional element of type xsd:string */
	std::string *NewVersion;	/* optional element of type xsd:string */
	std::string *Message;	/* optional element of type xsd:string */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 8; } /* = unique id SOAP_TYPE_icd1__UpdatePack */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         icd1__UpdatePack() { icd1__UpdatePack::soap_default(NULL); }
	virtual ~icd1__UpdatePack() { }
};
#endif

#ifndef SOAP_TYPE_icd1__GetMacIddPack
#define SOAP_TYPE_icd1__GetMacIddPack (9)
/* icd1:GetMacIddPack */
class SOAP_CMAC icd1__GetMacIddPack
{
public:
	int Idd;	/* required element of type xsd:int */
	std::string *Mac;	/* optional element of type xsd:string */
	std::string *Message;	/* optional element of type xsd:string */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 9; } /* = unique id SOAP_TYPE_icd1__GetMacIddPack */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         icd1__GetMacIddPack() { icd1__GetMacIddPack::soap_default(NULL); }
	virtual ~icd1__GetMacIddPack() { }
};
#endif

#ifndef SOAP_TYPE__icd1__GetTime
#define SOAP_TYPE__icd1__GetTime (10)
/* icd1:GetTime */
class SOAP_CMAC _icd1__GetTime
{
public:
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 10; } /* = unique id SOAP_TYPE__icd1__GetTime */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         _icd1__GetTime() { _icd1__GetTime::soap_default(NULL); }
	virtual ~_icd1__GetTime() { }
};
#endif

#ifndef SOAP_TYPE__icd1__GetTimeResponse
#define SOAP_TYPE__icd1__GetTimeResponse (11)
/* icd1:GetTimeResponse */
class SOAP_CMAC _icd1__GetTimeResponse
{
public:
	std::string *GetTimeResult;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* optional element of type xsd:string */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 11; } /* = unique id SOAP_TYPE__icd1__GetTimeResponse */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         _icd1__GetTimeResponse() { _icd1__GetTimeResponse::soap_default(NULL); }
	virtual ~_icd1__GetTimeResponse() { }
};
#endif

#ifndef SOAP_TYPE__icd1__LoginDevice
#define SOAP_TYPE__icd1__LoginDevice (12)
/* icd1:LoginDevice */
class SOAP_CMAC _icd1__LoginDevice
{
public:
	int idd;	/* required element of type xsd:int */
	std::string *mac;	/* optional element of type xsd:string */
	std::string *deviceIds;	/* optional element of type xsd:string */
	std::string *devInfo;	/* optional element of type xsd:string */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 12; } /* = unique id SOAP_TYPE__icd1__LoginDevice */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         _icd1__LoginDevice() { _icd1__LoginDevice::soap_default(NULL); }
	virtual ~_icd1__LoginDevice() { }
};
#endif

#ifndef SOAP_TYPE__icd1__LoginDeviceResponse
#define SOAP_TYPE__icd1__LoginDeviceResponse (13)
/* icd1:LoginDeviceResponse */
class SOAP_CMAC _icd1__LoginDeviceResponse
{
public:
	int LoginDeviceResult;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type xsd:int */
	std::string *message;	/* optional element of type xsd:string */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 13; } /* = unique id SOAP_TYPE__icd1__LoginDeviceResponse */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         _icd1__LoginDeviceResponse() { _icd1__LoginDeviceResponse::soap_default(NULL); }
	virtual ~_icd1__LoginDeviceResponse() { }
};
#endif

#ifndef SOAP_TYPE__icd1__SendData
#define SOAP_TYPE__icd1__SendData (14)
/* icd1:SendData */
class SOAP_CMAC _icd1__SendData
{
public:
	std::string *data;	/* optional element of type xsd:string */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 14; } /* = unique id SOAP_TYPE__icd1__SendData */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         _icd1__SendData() { _icd1__SendData::soap_default(NULL); }
	virtual ~_icd1__SendData() { }
};
#endif

#ifndef SOAP_TYPE__icd1__SendDataResponse
#define SOAP_TYPE__icd1__SendDataResponse (15)
/* icd1:SendDataResponse */
class SOAP_CMAC _icd1__SendDataResponse
{
public:
	int SendDataResult;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type xsd:int */
	std::string *message;	/* optional element of type xsd:string */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 15; } /* = unique id SOAP_TYPE__icd1__SendDataResponse */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         _icd1__SendDataResponse() { _icd1__SendDataResponse::soap_default(NULL); }
	virtual ~_icd1__SendDataResponse() { }
};
#endif

#ifndef SOAP_TYPE__icd1__SendData3
#define SOAP_TYPE__icd1__SendData3 (16)
/* icd1:SendData3 */
class SOAP_CMAC _icd1__SendData3
{
public:
	std::string *data;	/* optional element of type xsd:string */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 16; } /* = unique id SOAP_TYPE__icd1__SendData3 */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         _icd1__SendData3() { _icd1__SendData3::soap_default(NULL); }
	virtual ~_icd1__SendData3() { }
};
#endif

#ifndef SOAP_TYPE__icd1__SendData3Response
#define SOAP_TYPE__icd1__SendData3Response (17)
/* icd1:SendData3Response */
class SOAP_CMAC _icd1__SendData3Response
{
public:
	int SendData3Result;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type xsd:int */
	std::string *message;	/* optional element of type xsd:string */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 17; } /* = unique id SOAP_TYPE__icd1__SendData3Response */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         _icd1__SendData3Response() { _icd1__SendData3Response::soap_default(NULL); }
	virtual ~_icd1__SendData3Response() { }
};
#endif

#ifndef SOAP_TYPE__icd1__LogoutDevice
#define SOAP_TYPE__icd1__LogoutDevice (18)
/* icd1:LogoutDevice */
class SOAP_CMAC _icd1__LogoutDevice
{
public:
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 18; } /* = unique id SOAP_TYPE__icd1__LogoutDevice */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         _icd1__LogoutDevice() { _icd1__LogoutDevice::soap_default(NULL); }
	virtual ~_icd1__LogoutDevice() { }
};
#endif

#ifndef SOAP_TYPE__icd1__LogoutDeviceResponse
#define SOAP_TYPE__icd1__LogoutDeviceResponse (19)
/* icd1:LogoutDeviceResponse */
class SOAP_CMAC _icd1__LogoutDeviceResponse
{
public:
	int LogoutDeviceResult;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type xsd:int */
	std::string *message;	/* optional element of type xsd:string */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 19; } /* = unique id SOAP_TYPE__icd1__LogoutDeviceResponse */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         _icd1__LogoutDeviceResponse() { _icd1__LogoutDeviceResponse::soap_default(NULL); }
	virtual ~_icd1__LogoutDeviceResponse() { }
};
#endif

#ifndef SOAP_TYPE__icd1__GetServiceInfo
#define SOAP_TYPE__icd1__GetServiceInfo (20)
/* icd1:GetServiceInfo */
class SOAP_CMAC _icd1__GetServiceInfo
{
public:
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 20; } /* = unique id SOAP_TYPE__icd1__GetServiceInfo */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         _icd1__GetServiceInfo() { _icd1__GetServiceInfo::soap_default(NULL); }
	virtual ~_icd1__GetServiceInfo() { }
};
#endif

#ifndef SOAP_TYPE__icd1__GetServiceInfoResponse
#define SOAP_TYPE__icd1__GetServiceInfoResponse (21)
/* icd1:GetServiceInfoResponse */
class SOAP_CMAC _icd1__GetServiceInfoResponse
{
public:
	std::string *GetServiceInfoResult;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* optional element of type xsd:string */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 21; } /* = unique id SOAP_TYPE__icd1__GetServiceInfoResponse */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         _icd1__GetServiceInfoResponse() { _icd1__GetServiceInfoResponse::soap_default(NULL); }
	virtual ~_icd1__GetServiceInfoResponse() { }
};
#endif

#ifndef SOAP_TYPE__icd1__GetDeviceUpdateInfo
#define SOAP_TYPE__icd1__GetDeviceUpdateInfo (22)
/* icd1:GetDeviceUpdateInfo */
class SOAP_CMAC _icd1__GetDeviceUpdateInfo
{
public:
	std::string *softVersion;	/* optional element of type xsd:string */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 22; } /* = unique id SOAP_TYPE__icd1__GetDeviceUpdateInfo */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         _icd1__GetDeviceUpdateInfo() { _icd1__GetDeviceUpdateInfo::soap_default(NULL); }
	virtual ~_icd1__GetDeviceUpdateInfo() { }
};
#endif

#ifndef SOAP_TYPE__icd1__GetDeviceUpdateInfoResponse
#define SOAP_TYPE__icd1__GetDeviceUpdateInfoResponse (23)
/* icd1:GetDeviceUpdateInfoResponse */
class SOAP_CMAC _icd1__GetDeviceUpdateInfoResponse
{
public:
	int GetDeviceUpdateInfoResult;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type xsd:int */
	icd1__UpdatePack *response;	/* optional element of type icd1:UpdatePack */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 23; } /* = unique id SOAP_TYPE__icd1__GetDeviceUpdateInfoResponse */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         _icd1__GetDeviceUpdateInfoResponse() { _icd1__GetDeviceUpdateInfoResponse::soap_default(NULL); }
	virtual ~_icd1__GetDeviceUpdateInfoResponse() { }
};
#endif

#ifndef SOAP_TYPE__icd1__GetMacIdd
#define SOAP_TYPE__icd1__GetMacIdd (24)
/* icd1:GetMacIdd */
class SOAP_CMAC _icd1__GetMacIdd
{
public:
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 24; } /* = unique id SOAP_TYPE__icd1__GetMacIdd */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         _icd1__GetMacIdd() { _icd1__GetMacIdd::soap_default(NULL); }
	virtual ~_icd1__GetMacIdd() { }
};
#endif

#ifndef SOAP_TYPE__icd1__GetMacIddResponse
#define SOAP_TYPE__icd1__GetMacIddResponse (25)
/* icd1:GetMacIddResponse */
class SOAP_CMAC _icd1__GetMacIddResponse
{
public:
	int GetMacIddResult;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type xsd:int */
	icd1__GetMacIddPack *response;	/* optional element of type icd1:GetMacIddPack */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 25; } /* = unique id SOAP_TYPE__icd1__GetMacIddResponse */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         _icd1__GetMacIddResponse() { _icd1__GetMacIddResponse::soap_default(NULL); }
	virtual ~_icd1__GetMacIddResponse() { }
};
#endif

#ifndef SOAP_TYPE___icd2__GetTime
#define SOAP_TYPE___icd2__GetTime (35)
/* Operation wrapper: */
struct __icd2__GetTime
{
public:
	_icd1__GetTime *icd1__GetTime;	/* optional element of type icd1:GetTime */
};
#endif

#ifndef SOAP_TYPE___icd2__LoginDevice
#define SOAP_TYPE___icd2__LoginDevice (39)
/* Operation wrapper: */
struct __icd2__LoginDevice
{
public:
	_icd1__LoginDevice *icd1__LoginDevice;	/* optional element of type icd1:LoginDevice */
};
#endif

#ifndef SOAP_TYPE___icd2__SendData
#define SOAP_TYPE___icd2__SendData (43)
/* Operation wrapper: */
struct __icd2__SendData
{
public:
	_icd1__SendData *icd1__SendData;	/* optional element of type icd1:SendData */
};
#endif

#ifndef SOAP_TYPE___icd2__SendData3
#define SOAP_TYPE___icd2__SendData3 (47)
/* Operation wrapper: */
struct __icd2__SendData3
{
public:
	_icd1__SendData3 *icd1__SendData3;	/* optional element of type icd1:SendData3 */
};
#endif

#ifndef SOAP_TYPE___icd2__LogoutDevice
#define SOAP_TYPE___icd2__LogoutDevice (51)
/* Operation wrapper: */
struct __icd2__LogoutDevice
{
public:
	_icd1__LogoutDevice *icd1__LogoutDevice;	/* optional element of type icd1:LogoutDevice */
};
#endif

#ifndef SOAP_TYPE___icd2__GetServiceInfo
#define SOAP_TYPE___icd2__GetServiceInfo (55)
/* Operation wrapper: */
struct __icd2__GetServiceInfo
{
public:
	_icd1__GetServiceInfo *icd1__GetServiceInfo;	/* optional element of type icd1:GetServiceInfo */
};
#endif

#ifndef SOAP_TYPE___icd2__GetDeviceUpdateInfo
#define SOAP_TYPE___icd2__GetDeviceUpdateInfo (59)
/* Operation wrapper: */
struct __icd2__GetDeviceUpdateInfo
{
public:
	_icd1__GetDeviceUpdateInfo *icd1__GetDeviceUpdateInfo;	/* optional element of type icd1:GetDeviceUpdateInfo */
};
#endif

#ifndef SOAP_TYPE___icd2__GetMacIdd
#define SOAP_TYPE___icd2__GetMacIdd (63)
/* Operation wrapper: */
struct __icd2__GetMacIdd
{
public:
	_icd1__GetMacIdd *icd1__GetMacIdd;	/* optional element of type icd1:GetMacIdd */
};
#endif

#ifndef SOAP_TYPE___icd3__GetTime
#define SOAP_TYPE___icd3__GetTime (65)
/* Operation wrapper: */
struct __icd3__GetTime
{
public:
	_icd1__GetTime *icd1__GetTime;	/* optional element of type icd1:GetTime */
};
#endif

#ifndef SOAP_TYPE___icd3__LoginDevice
#define SOAP_TYPE___icd3__LoginDevice (67)
/* Operation wrapper: */
struct __icd3__LoginDevice
{
public:
	_icd1__LoginDevice *icd1__LoginDevice;	/* optional element of type icd1:LoginDevice */
};
#endif

#ifndef SOAP_TYPE___icd3__SendData
#define SOAP_TYPE___icd3__SendData (69)
/* Operation wrapper: */
struct __icd3__SendData
{
public:
	_icd1__SendData *icd1__SendData;	/* optional element of type icd1:SendData */
};
#endif

#ifndef SOAP_TYPE___icd3__SendData3
#define SOAP_TYPE___icd3__SendData3 (71)
/* Operation wrapper: */
struct __icd3__SendData3
{
public:
	_icd1__SendData3 *icd1__SendData3;	/* optional element of type icd1:SendData3 */
};
#endif

#ifndef SOAP_TYPE___icd3__LogoutDevice
#define SOAP_TYPE___icd3__LogoutDevice (73)
/* Operation wrapper: */
struct __icd3__LogoutDevice
{
public:
	_icd1__LogoutDevice *icd1__LogoutDevice;	/* optional element of type icd1:LogoutDevice */
};
#endif

#ifndef SOAP_TYPE___icd3__GetServiceInfo
#define SOAP_TYPE___icd3__GetServiceInfo (75)
/* Operation wrapper: */
struct __icd3__GetServiceInfo
{
public:
	_icd1__GetServiceInfo *icd1__GetServiceInfo;	/* optional element of type icd1:GetServiceInfo */
};
#endif

#ifndef SOAP_TYPE___icd3__GetDeviceUpdateInfo
#define SOAP_TYPE___icd3__GetDeviceUpdateInfo (77)
/* Operation wrapper: */
struct __icd3__GetDeviceUpdateInfo
{
public:
	_icd1__GetDeviceUpdateInfo *icd1__GetDeviceUpdateInfo;	/* optional element of type icd1:GetDeviceUpdateInfo */
};
#endif

#ifndef SOAP_TYPE___icd3__GetMacIdd
#define SOAP_TYPE___icd3__GetMacIdd (79)
/* Operation wrapper: */
struct __icd3__GetMacIdd
{
public:
	_icd1__GetMacIdd *icd1__GetMacIdd;	/* optional element of type icd1:GetMacIdd */
};
#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_SOAP_ENV__Header
#define SOAP_TYPE_SOAP_ENV__Header (80)
/* SOAP Header: */
struct SOAP_ENV__Header
{
#ifdef WITH_NOEMPTYSTRUCT
private:
	char dummy;	/* dummy member to enable compilation */
#endif
};
#endif

#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_SOAP_ENV__Code
#define SOAP_TYPE_SOAP_ENV__Code (81)
/* SOAP Fault Code: */
struct SOAP_ENV__Code
{
public:
	char *SOAP_ENV__Value;	/* optional element of type xsd:QName */
	struct SOAP_ENV__Code *SOAP_ENV__Subcode;	/* optional element of type SOAP-ENV:Code */
};
#endif

#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_SOAP_ENV__Detail
#define SOAP_TYPE_SOAP_ENV__Detail (83)
/* SOAP-ENV:Detail */
struct SOAP_ENV__Detail
{
public:
	char *__any;
	int __type;	/* any type of element <fault> (defined below) */
	void *fault;	/* transient */
};
#endif

#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_SOAP_ENV__Reason
#define SOAP_TYPE_SOAP_ENV__Reason (86)
/* SOAP-ENV:Reason */
struct SOAP_ENV__Reason
{
public:
	char *SOAP_ENV__Text;	/* optional element of type xsd:string */
};
#endif

#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_SOAP_ENV__Fault
#define SOAP_TYPE_SOAP_ENV__Fault (87)
/* SOAP Fault: */
struct SOAP_ENV__Fault
{
public:
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
#endif

#endif

/******************************************************************************\
 *                                                                            *
 * Typedefs                                                                   *
 *                                                                            *
\******************************************************************************/

#ifndef SOAP_TYPE__QName
#define SOAP_TYPE__QName (5)
typedef char *_QName;
#endif

#ifndef SOAP_TYPE__XML
#define SOAP_TYPE__XML (6)
typedef char *_XML;
#endif


/******************************************************************************\
 *                                                                            *
 * Externals                                                                  *
 *                                                                            *
\******************************************************************************/


#endif

/* End of Stub.h */