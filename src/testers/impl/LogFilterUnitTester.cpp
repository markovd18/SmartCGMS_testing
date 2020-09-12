#include <iostream>
#include <rtl/FilterLib.h>
#include <rtl/hresult.h>
#include <utils/string_utils.h>
#include "../LogFilterUnitTester.h"

const std::string LogFilterUnitTester::FILTER_CONFIG = "[Filter_001_{c0e942b9-3928-4b81-9b43-a347668200ba}]";
const std::string LogFilterUnitTester::LOG_FILE_GENERATION_TEST_LOG = "logFileGenerationTestLog.log";
const std::string LogFilterUnitTester::CORRECT_LOG_FILE_NAME_TEST_LOG = "correctLogFileNameTestLog.log";

LogFilterUnitTester::LogFilterUnitTester(CDynamic_Library* library, TestFilter* testFilter,const GUID* guid)
                    : GenericUnitTester(library, testFilter, guid){
	//
}

/**
	Executes all tests specific to filter tested by this UnitTester.
*/
void LogFilterUnitTester::executeSpecificTests() {
	logger.info(L"Executing specific tests...");
	executeConfigTest(L"empty log file name test",
                   FILTER_CONFIG + "\n\nLog_File = ", E_FAIL);
	executeConfigTest(L"correct log file name test",
                   FILTER_CONFIG + "\n\nLog_File = " + CORRECT_LOG_FILE_NAME_TEST_LOG, S_OK);
    std::remove(CORRECT_LOG_FILE_NAME_TEST_LOG.c_str());    //the test above should create junk file so we delete it
	executeTest(L"log file generation test", std::bind(&LogFilterUnitTester::logFileGenerationTest, this));
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
		return E_FAIL;
	}

	scgms::SPersistent_Filter_Chain_Configuration configuration;
	refcnt::Swstr_list errors;
	std::string memory = FILTER_CONFIG + "\n\nLog_File = " + LOG_FILE_GENERATION_TEST_LOG;

	HRESULT result = configuration ? S_OK : E_FAIL;
	if (result == S_OK)
	{
		configuration->Load_From_Memory(memory.c_str(), memory.size(), errors.get());
		logger.info(L"Loading configuration from memory...");
	}
	else {
		logger.error(L"Error while creating configuration!");
		shutDownTest();
		return E_FAIL;
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
			result = E_FAIL;
		}
		else {
			filterExecutor->Terminate();
			std::ifstream file(LOG_FILE_GENERATION_TEST_LOG);
			if (file.good())
			{
				logger.debug(L"Log file created succesfully!");
				result = S_OK;
			}
			else {
				logger.error(L"Failed to create log file!");
				result = E_FAIL;
			}
			file.close();
		}
	}
	else {
		logger.error(L"Failed to configure filter:!\n"
		L"(" + std::wstring(memory.begin(), memory.end()) + L")");
		logger.error(L"expected result: " + Widen_Char(std::system_category().message(S_OK).c_str()));
		logger.error(L"actual result: " + Widen_Char(std::system_category().message(result).c_str()));
		result = E_FAIL;
	}

	shutDownTest();
	std::remove(LOG_FILE_GENERATION_TEST_LOG.c_str());
	return result;
}
