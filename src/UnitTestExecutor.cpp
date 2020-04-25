#include <iostream>
#include "UnitTestExecutor.h"
#include "../../smartcgms/src/common/rtl/Dynamic_Library.h"
#include "../../smartcgms/src/common/rtl/guid.h"
#include "../../smartcgms/src/common/rtl/hresult.h"
#include "constants.h"

UnitTestExecutor::UnitTestExecutor() {
	CDynamic_Library::Set_Library_Base(LIB_DIR);
}

/**
	Executes all tests on a filter represented by given GUID.
*/
void UnitTestExecutor::executeFilterTests(GUID& guid) {
	if (Is_Invalid_GUID(guid))
	{
		std::wcerr << L"Invalid GUID passed!\n";
		exit(E_INVALIDARG);
	}
	//TODO GenericFilterExecutor.executeAllTests() podle zadan�ho guid
	CDynamic_Library library;
	
}

/**
	Executes all tests across all filters.
*/
void UnitTestExecutor::executeAllTests() {
	// proj�t v�echny filtry a spustit v�echny testy, asi podle mapy
}