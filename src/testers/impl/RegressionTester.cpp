//
// Created by Martin on 22.4.2020.
//
#include <iface/DeviceIface.h>
#include <rtl/FilterLib.h>
#include <rtl/referencedImpl.h>
#include <rtl/hresult.h>
#include <utils/string_utils.h>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <algorithm>
#include <rtl/scgmsLib.h>
#include "../RegressionTester.h"
#include "../../utils/constants.h"
#include "../../utils/LogUtils.h"

tester::RegressionTester::RegressionTester(std::wstring config_filepath) : config_filepath(std::move(config_filepath)),
                                                                   resultLog(Narrow_WChar(cnst::LOG_FILE)){
    loadConfig();
}

void MainCalling tester::RegressionTester::loadConfig() {

    if (config_filepath.empty()) {
        Logger::getInstance().error(L"Cannot load configuration from empty path!");
        throw std::invalid_argument("Cannot load configuration from empty path!");
    }

    scgms::IPersistent_Filter_Chain_Configuration *iconfig;
    auto configFactory = scgms::factory::resolve_symbol<scgms::TCreate_Persistent_Filter_Chain_Configuration>("create_persistent_filter_chain_configuration");
    configFactory(&iconfig);
    refcnt::Swstr_list errors;


    HRESULT rc = iconfig ? S_OK : E_FAIL;
    if (rc == E_FAIL) {
        throw std::runtime_error("Error creating configuration instance");
    }

    iconfig->Load_From_File(this->config_filepath.c_str(), errors.get());
    errors.for_each([](auto str) { std::wcerr << str << std::endl; });

    if (!Succeeded(rc)) {
        std::wcerr << L"Cannot load the configuration file " << this->config_filepath << std::endl << std::endl;
        throw std::ios_base::failure("Cannot load the configuration file");
    }

    if (rc == S_FALSE) {
        std::wcerr << L"Warning: some filters were not loaded!" << std::endl;
        Logger::getInstance().warn(L"Warning: some filters were not loaded!");
        log::printAndEmptyErrors(errors);
    }

    scgms::IFilter_Executor *executor;
    auto executorFactory = scgms::factory::resolve_symbol<scgms::TExecute_Filter_Configuration>("execute_filter_configuration");
    executorFactory(iconfig, nullptr, nullptr, nullptr, &executor, errors.get());
    scgms::SFilter_Executor gFilter_Executor { iconfig, nullptr, nullptr, errors };

    log::printAndEmptyErrors(errors);

    if (!executor) {
        std::wcerr << L"Could not execute the filters!" << std::endl;
        throw std::runtime_error("Could not execute the filters!");
    }


    // wait for filters to finish, or user to close the app
    executor->Terminate(TRUE);
}

HRESULT tester::RegressionTester::compareLogs(const std::string& referenceLog) {

    if (config_filepath.empty()) {
        std::wcerr << L"Can't compare logs without configuration!\n";
        Logger::getInstance().error(L"Can't compare logs without configuration!");
        return E_FAIL;
    }

    std::vector<std::vector<std::string>> resultLogLinesVector = log::readLogFile(this->resultLog);
    std::sort(resultLogLinesVector.begin() + 1, resultLogLinesVector.end(),
        [](const std::vector<std::string>& first, const std::vector<std::string>& second) {
        return std::stoi(first.at(0)) < std::stoi(second.at(0));
        });

    std::vector<std::vector<std::string>> referenceLogLinesVector = log::readLogFile(referenceLog);
    std::sort(referenceLogLinesVector.begin() + 1, referenceLogLinesVector.end(),
        [](const std::vector<std::string>& first, const std::vector<std::string>& second) {
        return std::stoi(first.at(0)) < std::stoi(second.at(0));
        });

    std::vector<std::vector<std::string>> missingLines;
    std::vector<std::string> mismatchLine;
    std::vector<std::string> expectedLine;
    bool logsAreEqual = true;
    bool match = false;
    bool firstMismatch = true;
    int lastComparedLine = 1;
    if (resultLogLinesVector[0].size() != referenceLogLinesVector[0].size()) {
        // different number of parametes in line is not correct
        std::wcerr << L"There is different number of parameters in first line!\n";
        Logger::getInstance().error(L"There is different number of parameters in first line!");
        return E_FAIL;
    }

    for (size_t i = 1; i < referenceLogLinesVector.size(); i++) {
        for (size_t j = lastComparedLine; j < resultLogLinesVector.size(); j++) {
            if (log::compareLines(resultLogLinesVector.at(j), referenceLogLinesVector.at(i))) {
                resultLogLinesVector.erase(resultLogLinesVector.begin() + j);
                lastComparedLine = j;
                match = true;
                break;
            }
        }

        if (!match) {
            missingLines.push_back(referenceLogLinesVector.at(i));
            if (firstMismatch) {
                expectedLine = missingLines.at(0);
                mismatchLine = resultLogLinesVector.at(lastComparedLine);
                firstMismatch = false;
            }
            logsAreEqual = false;

        }
        match = false;
    }

    if (logsAreEqual) {
        std::wcout << "Test result is OK!\n";
        Logger::getInstance().info(L"Test result is OK!");
        if (resultLogLinesVector.size() > 1) {
            Logger::getInstance().info(L"There were reduntant lines found:");
            std::wcout << L"There were redundant lines found!\n";
            resultLogLinesVector.erase(resultLogLinesVector.begin());
            log::infoLogLines(resultLogLinesVector);
        }

        return S_OK;
    } else {
        Logger::getInstance().error(L"Test failed!");
        Logger::getInstance().error(L"First mismatch:");
        Logger::getInstance().error(L"Expected line:");
        log::errorLogLine(expectedLine);
        Logger::getInstance().error(L"Actual line:");
        log::errorLogLine(mismatchLine);
        Logger::getInstance().error(L"Lines that were not found:");
        log::infoLogLines(missingLines);

        std::wcout << L"There were lines missing in log file!\n";
        std::wcout << L"Test failed!\n";
        return E_FAIL;
    }
}

