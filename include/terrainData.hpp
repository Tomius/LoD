#ifndef LOD_TERRAINDATA_HPP_
#define LOD_TERRAINDATA_HPP_

#include <string>
#include <fstream>
#include <vector>

class RawTerrainData {
  void InitFromRawTerrain(const std::string& filename);
  void InitFromAsc(const std::string& filename);
  void InitFromImage(const std::string& filename);
public:
  size_t w, h;
  std::vector<unsigned char> heightData;
  RawTerrainData(const std::string& filename);
  void Save(const std::string& filename);
};

#endif // LOD_TERRAINDATA_HPP_
