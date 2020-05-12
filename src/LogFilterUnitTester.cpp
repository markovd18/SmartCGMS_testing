#include <iostream>
#include <fstream>
#include "LogFilterUnitTester.h"
#include "../../smartcgms/src/common/rtl/FilterLib.h"

LogFilterUnitTester::LogFilterUnitTester(CDynamic_Library* library, TestFilter* testFilter,const GUID* guid) : GenericUnitTester(library, testFilter, guid){
	//
}

/**
	Executes all tests specific to filter tested by this UnitTester.
*/
void LogFilterUnitTester::executeSpecificTests() {
	logger.info(L"Executing specific tests...");
	executeTest(L"empty log file name test", std::bind(&LogFilterUnitTester::emptyLogFileNameTest, this));
	executeTest(L"correct log file name test", std::bind(&LogFilterUnitTester::correctLogFileNameTest, this));
	executeTest(L"log file generation test", std::bind(&LogFilterUnitTester::logFileGenerationTest, this));
}

/**
	Tests if filter can be configured with empty file name.
*/
HRESULT LogFilterUnitTester::emptyLogFileNameTest()
{
	if (!isFilterLoaded())
	{
		std::wcerr << L"No filter loaded! Can't execute test.\n";
		logger.error(L"No filter loaded! Can't execute test.");
		return E_FAIL;
	}

	scgms::SPersistent_Filter_Chain_Configuration configuration;
	refcnt::Swstr_list errors;
	std::string memory = "[Filter_001_{c0e942b9-3928-4b81-9b43-a347668200ba}]\n\nLog_File = ";

	HRESULT result = configuration ? S_OK : E_FAIL;
	if (result == S_OK)
	{
		configuration->Load_From_Memory(memory.c_str(), memory.size(), errors.get());
		logger.info(L"Loading configuration from memory...");
	}
	else {
		logger.error(L"Error while creating configuration!");
	}

	scgms::IFilter_Configuration_Link** begin, ** end;
	configuration->get(&begin, &end);

	logger.info(L"Configuring filter...");
	result = testedFilter->Configure(begin[0], errors.get());
	
	if (!SUCCEEDED(result)) {	// test shouldn't succeed, we don't want to configure output log without name
		return S_OK;
	}
	else {
		logger.error(L"Filter was configured succesfully, but shouldn't be!");
		return E_FAIL;
	}
}

/**
	Tests correct configuration of the filter.
*/
HRESULT LogFilterUnitTester::correctLogFileNameTest()
{
	if (!isFilterLoaded())
	{
		std::wcerr << L"No filter loaded! Can't execute test.\n";
		logger.error(L"No filter loaded! Can't excecute test.");
		return E_FAIL;
	}

	scgms::SPersistent_Filter_Chain_Configuration configuration;
	refcnt::Swstr_list errors;
	std::string memory = "[Filter_001_{c0e942b9-3928-4b81-9b43-a347668200ba}]\n\nLog_File = log.log";

	HRESULT result = configuration ? S_OK : E_FAIL;
	if (result == S_OK)
	{
		configuration->Load_From_Memory(memory.c_str(), memory.size(), errors.get());
		logger.info(L"Loading configuration from memory...");
	}
	else {
		logger.error(L"Error while creating configuration!");
	}

	scgms::IFilter_Configuration_Link** begin, ** end;
	configuration->get(&begin, &end);

	result = testedFilter->Configure(begin[0], errors.get());
	logger.info(L"Configuring filter...");

	if (SUCCEEDED(result)) {
		return S_OK;
	}
	else {
		logger.error(L"Failed to configure filter!");
		return E_FAIL;
	}
}

/**
	Tests if log file is created.
*/
HRESULT LogFilterUnitTester::logFileGenerationTest()
{
	if (!isFilterLoaded())
	{
		std::wcerr << L"No filter loaded! Can't execute test.\n";
		logger.error(L"No filter loaded! Can't execute test!");
		exit(E_FAIL);
	}

	scgms::SPersistent_Filter_Chain_Configuration configuration;
	refcnt::Swstr_list errors;
	std::string memory = "[Filter_001_{c0e942b9-3928-4b81-9b43-a347668200ba}]\n\nLog_File = log.log";

	HRESULT result = configuration ? S_OK : E_FAIL;
	if (result == S_OK)
	{
		configuration->Load_From_Memory(memory.c_str(), memory.size(), errors.get());
		logger.info(L"Loading configuration from memory...");
	}
	else {
		logger.error(L"Error while creating configuration!");
	}

	scgms::IFilter_Configuration_Link** begin, ** end;
	configuration->get(&begin, &end);

	result = testedFilter->Configure(begin[0], errors.get());
	logger.info(L"Configuring filter...");

	if (SUCCEEDED(result)) {
		scgms::SFilter_Executor filterExecutor = { configuration.get(), nullptr, nullptr, errors };
		if (!filterExecutor)
		{
			std::wcerr << L"Could not execute configuration! ";
			logger.error(L"Could not execute configuration! (" + std::wstring(memory.begin(), memory.end()) + L")");
			return E_FAIL;
		}
		else {
			filterExecutor->Terminate();
			std::ifstream file("../bin/log.log");
			if (file.good())
			{
				logger.debug(L"Log file created succesfully!");
				return S_OK;
			}
			else {
				logger.error(L"Failed to create log file!");
				return E_FAIL;
			}
		}
	}
	else {
		return E_FAIL;
	}
}
