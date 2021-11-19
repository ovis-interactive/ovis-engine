#include <ovis/utils/log.hpp>
#include <ovis/core/asset_library.hpp>

namespace ovis {

DirectoryAssetLibrary::DirectoryAssetLibrary(std::string_view directory)
    : directory_(std::filesystem::absolute(directory)) {
  Rescan();
}

bool DirectoryAssetLibrary::Contains(std::string_view asset_id) const {
  return assets_.count(std::string(asset_id)) > 0;
}

std::vector<std::string> DirectoryAssetLibrary::GetAssets() const {
  std::vector<std::string> assets;
  assets.reserve(assets_.size());
  for (const auto& asset : assets_) {
    assets.push_back(asset.first);
  }
  return assets;
}

std::string DirectoryAssetLibrary::GetAssetType(std::string_view asset_id) const {
  const auto asset_data_iterator = assets_.find(std::string(asset_id));
  if (asset_data_iterator != assets_.end()) {
    return asset_data_iterator->second.type;
  } else {
    return "";
  }
}

std::vector<std::string> DirectoryAssetLibrary::GetAssetFileTypes(std::string_view asset_id) const {
  const auto asset_data_iterator = assets_.find(std::string(asset_id));
  if (asset_data_iterator != assets_.end()) {
    return asset_data_iterator->second.filenames;
  } else {
    return {};
  }
}

std::optional<std::string> DirectoryAssetLibrary::LoadAssetTextFile(std::string_view asset_id,
                                                                    std::string_view filename) const {
  const std::optional<std::string> complete_filename = GetAssetFilename(asset_id, filename);
  if (!complete_filename) {
    LogE("Cannot load asset file '{}' for asset '{}': asset does not exist", filename, asset_id);
    return {};
  } else {
    LogV("Loading text asset file: {}", *complete_filename);
    return LoadTextFile(*complete_filename);
  }
}

std::optional<Blob> DirectoryAssetLibrary::LoadAssetBinaryFile(std::string_view asset_id,
                                                               std::string_view filename) const {
  const std::optional<std::string> complete_filename = GetAssetFilename(asset_id, filename);
  if (!complete_filename) {
    LogE("Cannot load asset file '{}' for asset '{}': asset does not exist", filename, asset_id);
    return {};
  } else {
    LogV("Loading binary asset file: {}", *complete_filename);
    return LoadBinaryFile(*complete_filename);
  }
}

std::vector<std::string> DirectoryAssetLibrary::GetAssetsWithType(std::string_view type) const {
  const auto asset_range = assets_with_type_.equal_range(std::string(type));
  std::vector<std::string> assets;
  assets.reserve(assets_with_type_.count(std::string(type)));
  for (auto asset_iterator = asset_range.first; asset_iterator != asset_range.second; ++asset_iterator) {
    assets.push_back(asset_iterator->second);
  }
  return assets;
}

bool DirectoryAssetLibrary::CreateAsset(
    std::string_view asset_id, std::string_view type,
    const std::vector<std::pair<std::string, std::variant<std::string, Blob>>>& files) {
  if (Contains(asset_id)) {
    return false;
  }

  for (const auto& file : files) {
    const std::string complete_filename = directory_ / std::filesystem::path(fmt::format("{}.{}.{}", asset_id, type, file.first));
    if (std::holds_alternative<std::string>(file.second)) {
      if (!WriteTextFile(complete_filename, std::get<std::string>(file.second))) {
        // TODO: delete
        return false;
      }
    } else {
      if (!WriteBinaryFile(complete_filename, std::get<Blob>(file.second))) {
        // TODO: delete
        return false;
      }
    }
  }

  Rescan();
  return true;
}

bool DirectoryAssetLibrary::SaveAssetFile(std::string_view asset_id, std::string_view filename,
                                          std::variant<std::string, Blob> content) {
  const std::optional<std::string> complete_filename = GetAssetFilename(asset_id, filename);

  if (!complete_filename) {
    LogE("Cannot write asset file for unknown asset: '{}'", asset_id);
    return false;
  } else {
    if (std::holds_alternative<std::string>(content)) {
      return WriteTextFile(*complete_filename, std::get<std::string>(content));
    } else {
      return WriteBinaryFile(*complete_filename, std::get<Blob>(content));
    }
  }
}

bool DirectoryAssetLibrary::DeleteAsset(std::string_view asset_id) {
  const std::vector<std::string> filenames = GetAssetFileTypes(asset_id);

  for (std::string_view filename : filenames) {
    const std::optional<std::string> asset_complete_filename = GetAssetFilename(asset_id, filename);
    if (asset_complete_filename) {
      std::filesystem::remove(*asset_complete_filename);
    }
  }

  Rescan();
  return true;
}

void DirectoryAssetLibrary::Rescan() {
  assets_.clear();
  assets_with_type_.clear();

  if (!std::filesystem::exists(directory_)) {
    return;
  }

  for (const auto& directory_entry : std::filesystem::recursive_directory_iterator(directory_)) {
    if (directory_entry.is_regular_file()) {
      const auto file_path = directory_entry.path();
      const std::string complete_filename = file_path.filename();

      const auto first_dot_position = complete_filename.find('.');
      const auto second_dot_position = complete_filename.find('.', first_dot_position + 1);
      if (first_dot_position != complete_filename.npos && second_dot_position != complete_filename.npos) {
        const std::string asset_id = complete_filename.substr(0, first_dot_position);
        const std::string asset_type =
            complete_filename.substr(first_dot_position + 1, second_dot_position - first_dot_position - 1);
        const std::string filename = complete_filename.substr(second_dot_position + 1);

        auto& asset_data = assets_[asset_id];
        if (asset_data.type != "" && asset_data.type != asset_type) {
          LogW("Conflicting information on type of asset '{}': '{}' vs '{}'", asset_id, asset_data.type, asset_type);
        } else {
          if (asset_data.type == "") {
            assets_with_type_.insert(std::make_pair(asset_type, asset_id));
            asset_data.type = asset_type;
          }
          asset_data.filenames.push_back(filename);
          LogV("Discovered file type '{}' for asset '{}' of type '{}'", filename, asset_id, asset_type);
        }
      } else {
        LogW(
            "Ignoring file '{}' during asset discovery. Fomat must be '[asset_id].[asset_type].[extension]' where "
            "neither asset_id nor asset_type may be empty or contain a '.'",
            complete_filename);
      }
    }
  }
}

std::optional<std::string> DirectoryAssetLibrary::GetAssetFilename(std::string_view asset_id,
                                                                   std::string_view filename) const {
  auto asset_data = assets_.find(std::string(asset_id));
  if (asset_data == assets_.end()) {
    return {};
  } else {
    return directory_ / asset_data->second.directory /
           std::filesystem::path(fmt::format("{}.{}.{}", asset_id, asset_data->second.type, filename));
  }
}

namespace {
static std::unique_ptr<AssetLibrary> engine_asset_library;
static std::unique_ptr<AssetLibrary> application_asset_library;
}  // namespace

namespace detail {
void SetEngineAssetLibrary(std::unique_ptr<AssetLibrary> asset_library) {
  engine_asset_library = std::move(asset_library);
}
void SetApplicationAssetLibrary(std::unique_ptr<AssetLibrary> asset_library) {
  application_asset_library = std::move(asset_library);
}
}  // namespace detail

void SetEngineAssetsDirectory(std::string_view directory) {
  CreateEngineAssetLibrary<DirectoryAssetLibrary>(directory);
}

void SetApplicationAssetsDirectory(std::string_view directory) {
  CreateApplicationAssetLibrary<DirectoryAssetLibrary>(directory);
}

AssetLibrary* GetEngineAssetLibrary() {
  return engine_asset_library.get();
}

AssetLibrary* GetApplicationAssetLibrary() {
  return application_asset_library.get();
}

}  // namespace ovis