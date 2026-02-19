/*********************************************************************
 * \file        MetadataHandler.cpp
 * \brief       Handles the generation and management of metadata files
 *              (.meta) for assets in the system.
 *
 * \author      y.ziyangirwen, 2301345 (y.ziyangirwen@digipen.edu)
 * \date        November 16, 2024
 *
 * \details     This class provides utility methods to generate metadata
 *              files for assets, ensuring each asset has a unique
 *              identifier (UUID) for tracking purposes. It operates
 *              statically and does not maintain any internal state.
 *
 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/

#include "MetadataHandler.hpp"

#include <filesystem>
#include <fstream>
#include <chrono>
#include <sstream>
#include <random>
#include <iomanip>
#include <unordered_set>

#include "Logger.hpp"

std::unordered_map<std::string, std::string> MetadataHandler::uuidToFilePath;

void MetadataHandler::GenerateMetaFile(const std::string& assetPath) {
	std::string metaFilePath = assetPath + ".meta";
	if (!MetaFileExists(assetPath)) {
		std::string uuid = GenerateCustomUUID(assetPath);
		SaveMetaFile(metaFilePath, uuid);
	}
}

std::string MetadataHandler::GenerateCustomUUID(const std::string& assetPath) {
	auto now = std::chrono::system_clock::now();
	auto duration = now.time_since_epoch();
	auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

	static std::mt19937_64 rng(std::random_device{}());
	std::uniform_int_distribution<uint64_t> dist(0, UINT64_MAX);
	uint64_t randomPart = dist(rng);

	std::ostringstream oss;
	oss << std::hex << std::setfill('0');
	oss << millis << "-" << randomPart << "-" << std::hash<std::string>{}(assetPath);
	return oss.str();
}

bool MetadataHandler::MetaFileExists(const std::string& assetPath) {
	std::string metaFilePath = assetPath + ".meta";
	return std::filesystem::exists(metaFilePath);
}

std::string MetadataHandler::ParseUUIDFromMeta(const std::string& metaFilePath) {
	if (std::filesystem::exists(metaFilePath)) {
		std::ifstream metaFile(metaFilePath);
		std::string line;
		while (std::getline(metaFile, line)) {
			if (line.find("UUID:") != std::string::npos) {
				return line.substr(line.find(":") + 2);
			}
		}
	}
	return "";
}

std::string MetadataHandler::ParseUUIDFromFilePath(const std::string& filePath) {
	std::string metaFilePath = filePath + ".meta";
	if (std::filesystem::exists(metaFilePath)) {
		std::ifstream metaFile(metaFilePath);
		std::string line;
		while (std::getline(metaFile, line)) {
			if (line.find("UUID:") != std::string::npos) {
				return line.substr(line.find(":") + 2);
			}
		}
	}
	return "";
}

void MetadataHandler::InitializeUUIDMap(const std::string& assetFolder)
{
	static const std::unordered_set<std::string> supportedExtensions = {
	".png", // Texture files
	".wav", ".ogg",               // Audio files
	//".ttf", ".otf", ".fnt",                                  // Font files
	".mpg",
	".ttf"

	};

	//try {
	for (const auto& entry : std::filesystem::recursive_directory_iterator(assetFolder)) {
		if (entry.is_regular_file()) {
			auto extension = entry.path().extension().string();

			if (supportedExtensions.find(extension) != supportedExtensions.end()) {
				std::string filePath = entry.path().string();
				if (!MetaFileExists(filePath)) {
					GenerateMetaFile(filePath);
				}
					
				std::string uuid = ParseUUIDFromFilePath(filePath);
				uuidToFilePath[uuid] = filePath;

				// Debugging Info
				Logger::Instance().Log(Logger::Level::INFO, "[MetadataHandler] Mapped UUID to ", filePath);
			}
		}
	}
	//} catch (const std::filesystem::filesystem_error& e) {
	//	//std::cerr << "Filesystem error: " << e.what() << '\n';
	//} catch (const std::exception& e) {
	//	//std::cerr << "General exception: " << e.what() << '\n';
	//}

	//return metaFiles;
}

void MetadataHandler::AddToUUIDMap(const std::string& filePath) {
	if (!MetaFileExists(filePath)) {
		GenerateMetaFile(filePath);
	}

	std::string uuid = ParseUUIDFromFilePath(filePath);
	uuidToFilePath[uuid] = filePath;

	// Debugging Info
	Logger::Instance().Log(Logger::Level::INFO, "[MetadataHandler] Mapped UUID to ", filePath);
}

std::string MetadataHandler::RetrieveFilePathFromUUID(const std::string& uuid)
{
	return uuidToFilePath[uuid];
}

void MetadataHandler::SaveMetaFile(const std::string& metaFilePath, const std::string& uuid) {
	std::ofstream metaFile(metaFilePath);
	if (metaFile.is_open()) {
		metaFile << "UUID: " << uuid << "\n";
		metaFile.close();
	}
}