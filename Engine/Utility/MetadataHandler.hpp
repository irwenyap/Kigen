/*********************************************************************
 * \file        MetadataHandler.hpp
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

#ifndef METADATA_HANDLER_HPP
#define METADATA_HANDLER_HPP

#include <string>
#include <unordered_map>

 /**
  * \class MetadataHandler
  * \brief Static utility class for managing asset metadata files.
  *
  * The `MetadataHandler` class provides methods for generating and
  * managing metadata files associated with assets. Metadata files
  * store unique identifiers (UUIDs) for assets, allowing them to
  * be referenced and tracked across different systems.
  *
  * All methods in this class are static, and the class cannot be instantiated.
  */
class MetadataHandler {
public:
    /**
     * \brief Generates a `.meta` file for the specified asset if it does not already exist.
     *
     * This method checks if a `.meta` file exists for the given asset path.
     * If not, it generates a unique UUID and saves it to a `.meta` file.
     *
     * \param assetPath The path to the asset file.
     */
    static void GenerateMetaFile(const std::string& assetPath);

    /**
     * \brief Generates a custom UUID for the specified asset.
     *
     * This method generates a unique identifier using a combination
     * of the current timestamp, a random number, and the hash of the asset path.
     *
     * \param assetPath The path to the asset file.
     * \return A unique string identifier (UUID) for the asset.
     */
    static std::string GenerateCustomUUID(const std::string& assetPath);

    /**
     * \brief Checks if a `.meta` file exists for the specified asset.
     *
     * This method determines whether a `.meta` file is present in the same
     * directory as the asset file, with a `.meta` extension appended to the asset name.
     *
     * \param assetPath The path to the asset file.
     * \return `true` if the `.meta` file exists; `false` otherwise.
     */
    static bool MetaFileExists(const std::string& assetPath);

    /**
     * \brief Retrieves the UUID from a `.meta` file.
     *
     * This method returns a UUID with the given `.meta` file.
     *
     * \param metaFilePath The path to the `.meta` file.
     */
    static std::string ParseUUIDFromMeta(const std::string& metaFilePath);
    static std::string ParseUUIDFromFilePath(const std::string& metaFilePath);
    static void InitializeUUIDMap(const std::string& rootFolder);
    static void AddToUUIDMap(const std::string& filePath);
    static std::string RetrieveFilePathFromUUID(const std::string& uuid);
private:
    /**
    * \brief Deleted default constructor to prevent instantiation.
    */
    MetadataHandler() = delete;

    /**
     * \brief Deleted destructor to prevent instantiation.
     */
    ~MetadataHandler() = delete;

    /**
     * \brief Saves the UUID to a `.meta` file.
     *
     * This method creates or overwrites a `.meta` file with the given UUID.
     *
     * \param metaFilePath The path to the `.meta` file.
     * \param uuid The UUID to write into the `.meta` file.
     */
    static void SaveMetaFile(const std::string& metaFilePath, const std::string& uuid);

    static std::unordered_map<std::string, std::string> uuidToFilePath; 	// Mapping from UUIDs to file paths
};

#endif // !METADATA_HANDLER_HPP
