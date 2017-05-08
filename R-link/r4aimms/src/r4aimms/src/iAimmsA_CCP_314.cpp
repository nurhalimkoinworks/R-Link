// An ASCII interface (assuming CurrentCodePage) to a 3.14 (or later) libaimms3
#define _CRT_SECURE_NO_WARNINGS
#include "iAimmsFactory.h"
#include <aimmsifc/iAimms.h>
#include <Common/DynamicLib.h>
#include <exception>
#ifdef _MSC_VER
#include <windows.h>
#endif
#include "ImplHelpers.h"

#include "StringConversion.h"
#include "Converters.h"

// NO CONVERSIONS NECESSARY
// input C string
#define inCS( in )		in
// input AIMMS string
#define inAS( in )		in
// input AIMMS value
#define inV( in )		in
// output AIMMS string
#define outAS( out )	out
// output AIMMS string
#define outV( out )		out
// output AIMMS Value array
#define outVA( n, out )	out
// input ValueArray
#define inVA( n, in )	in


// We want to access the 'A' postfix methods directly 
#define DECORATED_DLL_NAME_T DECORATED_DLL_NAME_A

namespace aimmsifc {


class iAimmsA_CCP_314 : public iAimmsA, public iFactoryManage
{
	Common::DynamicLib m_Library;
	DECLARE_FUNCTION_POINTERS;

	// the types as they are defined in the underlying AimmsAPI
	// We call the 'A' versions of all string related functions so we should use 'char'
	typedef AimmsValueType<char> DllValueType;
	typedef AimmsStringType<char> DllStringType;
	typedef char DllCharType;

	aimmsifc::ccp2uni_t m_ConvertToAimms;
	aimmsifc::uni2ccp_t m_ConvertFromAimms;

public:
	iAimmsA_CCP_314(const char *szFileName)
		: m_Library(szFileName)
		, INIT_FUNCTION_POINTERS
	{}
	virtual ~iAimmsA_CCP_314() {}

	DECLARE_ALL_NONCHAR_METHODS

	DECLARE_ALL_VALUE_N_METHODS

	DECLARE_ALL_STRING_METHODS

	virtual int APILastError( int *code, CharType *message, int messageSize )
	{ 
		GET_FCN(FCN_APILastError, DECORATED_DLL_NAME_T(AimmsAPILastError,8), int, (int*, DllCharType*) ) 
		if (messageSize==-1) {
			return fcn(code,message);
		} else {
			CharType buf[4096];
			int rval = fcn(code,buf);
			strncpy(message,buf,messageSize-1);
			return rval;
		}
	}

	virtual bool AimmsIsUnicode() 
	{
		return true;
	}

	virtual wchar_t *ConvertToAimms(wchar_t *outBuf, size_t outBufSize, const CharType *inBuf) 
	{
		if (!outBufSize || !outBuf || !inBuf) return 0;
		std::wstring tmp;
		m_ConvertToAimms.convert(inBuf,tmp);
		wcsncpy(outBuf,tmp.c_str(),outBufSize-1);
		return outBuf;
	}
	virtual char *ConvertToAimms(char *outBuf, size_t outBufSize, const CharType *inBuf)
	{
		throw std::runtime_error("ConvertToAimms type mismatch with underlying Aimms");
	}
	virtual CharType *ConvertFromAimms(CharType *outBuf, size_t outBufSize, const wchar_t *inBuf) 
	{
		if (!outBufSize || !outBuf || !inBuf) return 0;
		std::string tmp;
		m_ConvertFromAimms.convert(inBuf,tmp);
		strncpy(outBuf,tmp.c_str(),outBufSize-1);
		return outBuf;
	}
	virtual CharType *ConvertFromAimms(CharType *outBuf, size_t outBufSize, const char *inBuf) 
	{
		throw std::runtime_error("ConvertToAimms type mismatch with underlying Aimms");
	}

private:
	// Helper function for ErrorMessage / ErrorCode / ErrorCategory
	const CharType *CallFunctionAndReturnString1Arg( const DllCharType* (WINAPI *fcn)(int), int n )
	{
        return fcn(n);
	}
	const CharType *CallFunctionAndReturnString2Arg( const DllCharType* (WINAPI *fcn)(int,int), int n1, int n2 )
	{
		return fcn(n1,n2);
	}

};

iAimmsA *CreateiAimmsA_CCP_314(const char *dllname)
{
	return new iAimmsA_CCP_314(dllname);
}

} // end namespace