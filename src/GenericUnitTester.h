#pragma once

#ifndef _GENERIC_UNIT_TESTER_H_
#define _GENERIC_UNIT_TESTER_H_

#include <mutex>
#include <functional>
#include "../../smartcgms/src/common/rtl/Dynamic_Library.h"
#include "TestFilter.h"
#include "Logger.h"

/**
    Contains generic tests and methods, which can be applied on any filter.
*/
class GenericUnitTester {
private:
    CDynamic_Library* filterLibrary;
    CDynamic_Library* scgmsLibrary;
    TestFilter* testFilter;
    const GUID* tested_guid;
    scgms::IFilter* testedFilter;

    std::mutex testMutex;
    std::condition_variable testCv;
    HRESULT lastTestResult;

    void loadFilter();
    void loadScgmsLibrary();
    HRESULT runTestInThread(std::function<HRESULT(void)> test);
    void runTest(std::function<HRESULT(void)> test);
    void executeTest(std::wstring testName, std::function<HRESULT(void)> test);
    void printResult(HRESULT result);

public:
    Logger& logger = Logger::GetInstance();
    GenericUnitTester(CDynamic_Library* library, TestFilter* testFilter,const GUID* guid);
    ~GenericUnitTester();
    HRESULT infoEventTest();
    bool isFilterLoaded();
    void executeAllTests();
    void executeGenericTests();
    // Executes all tests for a specific filter. Needs to be implemented by derived class.
    virtual void executeSpecificTests() = 0;
    //...

};


#endif // !_GENERIC_UNIT_TESTER_H_

