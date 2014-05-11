/**
 * @file terrainData.hpp
 * @brief Implements raw terrain data (.rtd) importer and exporter.
 */

#ifndef LOD_TERRAINDATA_H_
#define LOD_TERRAINDATA_H_

#include <string>
#include <fstream>
#include <vector>
#include <memory>

#define MAX_NUM_TERRAIN_MIPMAPS 2 // The max num of PERFORMANCE

/// A class holding terrain elevation data.
class RawTerrainData {
public:
	/// The width of the terrain data.
  size_t w;

  /// The height of the terrain data.
  size_t h;

  /// The mipmap level of the current terrain data.
  size_t level;

  /// The actual terrain data.
  std::vector<unsigned char> heightData;

private:
	/// Constucts an empty terrianData (full of memory junk).
	/** @param w - The width of the terrain data to create.
	  * @param h - The height of the terrain data to create.
	  * @param level - The mipmap level of the terrain data. */
	RawTerrainData(int w, int h, int level);

public:
	/// Loads in a terrain data, from either an .rtd, a heightmap (image), or a DEM (.asc).
	/** @param filename - The path to the file to load. */
  RawTerrainData(const std::string& filename);

  /// Saves the rtd in .rtd format.
  /** @param filename - The path to the file to save to. Should end with '.rtd' */
  void save(const std::string& filename) const;

private:
	/// Returns the next mipmap level of this rtd.
	std::unique_ptr<RawTerrainData> getNextLodLevel() const;

public:
	/// Creates a given number of mipmaps for this rtd.
	/** @param filename - The path of the file, where to save the mipmaps. Should end with '.rtd'.
	  * @param num_level - The number of mipmap levels to generate. */
  void createLoDs(const std::string& filename, int num_levels) const;

  /// Indexes the terrain data like it was 2D array.
  const unsigned char& operator()(size_t x, size_t y) const;

  /// Indexes the terrain data like it was 2D array.
  unsigned char& operator()(size_t x, size_t y);

private:
	/// Loads in a terrain data, from a '.rtd' file.
	/** @param filename - The path to the file to load. */
  void initFromRawTerrain(const std::string& filename);

  /// Loads in a terrain data, from a '.asc' (DEM) file.
	/** @param filename - The path to the file to load. */
  void initFromAsc(const std::string& filename);

  /// Loads in a terrain data, from an image file.
  /** Only the red channel will be used.
	  * @param filename - The path to the file to load. */
  void initFromImage(const std::string& filename);
};

#endif // LOD_TERRAINDATA_H_
