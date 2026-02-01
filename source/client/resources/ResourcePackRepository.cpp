#include "ResourcePackRepository.hpp"

const std::string ResourcePackRepository::RESOURCE_PACKS_PATH = "resource_packs";
const std::string ResourcePackRepository::BEHAVIOR_PACK_PATH = "behavior_packs";
const std::string ResourcePackRepository::RESOURCE_PACK_DOWNLOAD_CACHE_PATH = "resource_pack_download_cache";
const std::string ResourcePackRepository::RESOURCE_PACK_ICON_PATH = "pack.png"; // "pack_icon.png" on PE
std::vector<std::string> ResourcePackRepository::EXPECTED_EXTENSIONS;