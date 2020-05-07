#pragma once

#include "GuidFileMapper.h"
#include "constants.h"

GuidFileMapper::GuidFileMapper() {

	guidFileMap.insert(std::pair<GUID, const wchar_t*>(LOG_GUID, LOG_LIBRARY));
	
}

/**
	Returns instance of GuidFileMapper.
*/
GuidFileMapper& GuidFileMapper::GetInstance() {
	static GuidFileMapper instance;
	return instance;
}

/**
	Returns file name of dynamic library asociated with given GUID.
*/
const wchar_t* GuidFileMapper::getFileName(const GUID& guid) {
	return guidFileMap[guid];
}

const std::map<GUID, const wchar_t*> GuidFileMapper::getMap()
{
	return guidFileMap;
}