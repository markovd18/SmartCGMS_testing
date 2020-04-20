﻿// main.cpp : Defines the entry point for the application.
//

#include <iostream>
#include "../../smartcgms/src/common/rtl/Dynamic_Library.h"
#include "../../smartcgms/src/common/iface/FilterIface.h"
#include "../../smartcgms/src/common/rtl/guid.h"
#include "../../smartcgms/src/common/rtl/hresult.h"
#include "TestFilter.h"
#include "UnitTester.h"

#ifdef _WIN32
const wchar_t* LIB_DIR = L"../../smartcgms/windows_64/filters/";
#elif __APPLE__
const wchar_t* LIB_DIR = L"../../smartcgms/macos_64/filters/";
#else
const wchar_t* LIB_DIR = L"../../smartcgms/debian_64/filters/";
#endif

const wchar_t* GUID_FORMAT = L"XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX";

/**
    Prints few types how to use the application and which parameters to specify.
*/
void print_help() {
    std::wcerr << "Execute with two parameters <test_type> <tested_subject>\n"
        "<test_type> ... '-u' = filter unit tests / '-r' = scenario regression tests\n"
        "<tested_subject> ... <filter_guid> - GUID of filter to test with unit tests / "
        "<config_path> - path to filter chain config file\n"
        "a) -u <filter_guid>\n"
        "b) -r <config_path>\n";
}

/**
    Parses guid in string from command-line into hexadecimal numbers and creates GUID structure.
*/
GUID parse_guid(std::string guid_string) {
    GUID guid{};

    if (!guid_string.empty()) {
        std::string delimeter = "-";
        std::string token;
        std::string sub_token;
        size_t position = 0;
        int counter = 0;

        try {
            while ((position = guid_string.find(delimeter)) != std::string::npos) {
                token = guid_string.substr(0, position);

                switch (counter) {
                case 0:
                    guid.Data1 = std::stoul(token, nullptr, 16);
                    break;
                case 1:
                    guid.Data2 = std::stoi(token, nullptr, 16);
                    break;
                case 2:
                    guid.Data3 = std::stoi(token, nullptr, 16);
                    break;
                case 3:
                    for (int i = 0; i < 2; i++) {
                        sub_token = token.substr(0, 2 * sizeof(char));
                        guid.Data4[i] = std::stoi(sub_token, nullptr, 16);
                        token.erase(0, 2 * sizeof(char));
                    }
                    break;
                default:
                    std::wcerr << L"Invalid format of GUID inserted!\n"
                        << "Expected format: " << GUID_FORMAT << "\n";
                    exit(2);
                }

                guid_string.erase(0, position + delimeter.length());
                counter++;
            }

            for (int i = 2; i < 8; i++) {
                token = guid_string.substr(0, 2 * sizeof(char));
                guid.Data4[i] = std::stoi(token, nullptr, 16);
                guid_string.erase(0, 2 * sizeof(char));
            }
        }
        catch (std::runtime_error) {
            std::wcerr << L"Invalid format of GUID inserted!\n"
                << L"Expected format: " << GUID_FORMAT << "\n";
            exit(2);
        }
    }
    return guid;
}

/**
    Loads library of given filter and starts unit tests.
*/
HRESULT execute_unit_testing(std::string guid_string) {
    CDynamic_Library::Set_Library_Base(LIB_DIR);
    CDynamic_Library library;
    GUID guid = parse_guid(guid_string);

    TestFilter testFilter = TestFilter();
    UnitTester unitTester = UnitTester(&library, &testFilter, &guid);

    return S_OK;
}

/**
    Loads filter configuration and executes regression tests.
*/
int execute_regression_testing(const char* config_path) {
    return 0;
}

/**
    Entry point of application.
*/
int MainCalling main(int argc, char* argv[])
{
    if (argc < 2) {
        std::wcerr << L"Wrong parameter count!\n";
        print_help();
        return 1;
    }

    if (argv[1][0] == '-') {
        std::string parameter = "";
        switch (argv[1][1]) {
        case 'u':   // unit testing
            if (argv[2] != NULL) {
                parameter = argv[2];
            }
            return execute_unit_testing(parameter);
        case 'r':   // regression testing
            return execute_regression_testing(argv[2]);
        default:
            std::wcerr << L"Unknown type of testing requested!\n";
            print_help();
            return 2;
        }
    }
    else {
        std::wcerr << L"Unsupported command: " << argv[1] << "\n";
        print_help();
        return 2;
    }

    return 0;
}
