
// Copyright (c) 2015 Pierre MOULON.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENMVG_SFM_REGIONS_PROVIDER_HPP
#define OPENMVG_SFM_REGIONS_PROVIDER_HPP

#include <openMVG/types.hpp>
#include <openMVG/sfm/sfm_data.hpp>
#include <openMVG/features/regions.hpp>
#include <openMVG/features/image_describer.hpp>
#include <openMVG/config.hpp>

#include "third_party/progress/progress.hpp"

#include <memory>

namespace openMVG {
namespace sfm {

/// Abstract Regions provider
/// Allow to load and return the regions related to a view
struct Regions_Provider
{
  /// Regions per ViewId of the considered SfM_Data container
  Hash_Map<IndexT, std::unique_ptr<features::Regions> > regions_per_view;

  void clearDescriptors()
  {
    for(auto& it: regions_per_view)
    {
      it.second->clearDescriptors();
    }
  }

  // Load Regions related to a provided SfM_Data View container
  virtual bool load(
    const SfM_Data & sfm_data,
    const std::string & feat_directory,
    std::unique_ptr<features::Regions>& region_type,
    const std::set<IndexT>& filter = std::set<IndexT>())
  {
    C_Progress_display my_progress_bar( sfm_data.GetViews().size(),
      std::cout, "\n- Regions Loading -\n");
    // Read for each view the corresponding regions and store them
    bool bContinue = true;

    #pragma omp parallel num_threads(3)
    for (Views::const_iterator iter = sfm_data.GetViews().begin();
      iter != sfm_data.GetViews().end() && bContinue; ++iter)
    {
      #pragma omp single nowait
      {
        const std::string sImageName = stlplus::create_filespec(sfm_data.s_root_path, std::to_string(iter->second.get()->id_view));
        const std::string basename = stlplus::basename_part(sImageName);
        
        if(filter.empty() || filter.find(iter->second.get()->id_view) != filter.end())
        {
          const std::string featFile = stlplus::create_filespec(feat_directory, basename, ".feat");
          const std::string descFile = stlplus::create_filespec(feat_directory, basename, ".desc");

          std::unique_ptr<features::Regions> regions_ptr(region_type->EmptyClone());
          if (!regions_ptr->Load(featFile, descFile))
          {
            OPENMVG_LOG_WARNING("Invalid regions files for the view: " << sImageName);
            #pragma omp critical
            bContinue = false;
          }
          #pragma omp critical
          {
            regions_per_view[iter->second.get()->id_view] = std::move(regions_ptr);
            ++my_progress_bar;
          }
        }
        
      }
    }
    return bContinue;
  }

}; // Regions_Provider

} // namespace sfm
} // namespace openMVG

#endif // OPENMVG_SFM_REGIONS_PROVIDER_HPP
