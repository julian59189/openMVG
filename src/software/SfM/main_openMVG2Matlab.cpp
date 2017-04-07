#include "openMVG/sfm/sfm.hpp"
#include "openMVG/image/image.hpp"
#include "openMVG/image/image_converter.hpp"


#include "third_party/cmdLine/cmdLine.h"
#include "third_party/progress/progress.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <iterator>
#include <iomanip>
#include <map>

using namespace openMVG;
using namespace openMVG::cameras;
using namespace openMVG::geometry;
using namespace openMVG::image;
using namespace openMVG::sfm;


bool exportToMatlab(
  const SfM_Data & sfm_data,
  const std::string & outDirectory
  )
{
  // WARNING: Observation::id_feat is used to put the ID of the 3D landmark.
  std::map<IndexT, std::vector<Observation> > observationsPerView;
  
  {
    const std::string landmarksFilename = stlplus::filespec_to_path(outDirectory, "scene.landmarks");
    std::ofstream landmarksFile(landmarksFilename);
    landmarksFile << "# landmarkId X Y Z\n";
    for(const auto& s: sfm_data.structure)
    {
      const IndexT landmarkId = s.first;
      const Landmark& landmark = s.second;
      landmarksFile << landmarkId << " " << landmark.X[0] << " " << landmark.X[1] << " " << landmark.X[2] << "\n";
      for(const auto& obs: landmark.obs)
      {
        const IndexT obsView = obs.first; // The ID of the view that provides this 2D observation.
        observationsPerView[obsView].push_back(Observation(obs.second.x, landmarkId));
      }
    }
    landmarksFile.close();
  }
  
  // Export observations per view
  for(const auto & obsPerView : observationsPerView)
  {
    const std::vector<Observation>& viewObservations = obsPerView.second;
    const IndexT viewId = obsPerView.first;
    
    const std::string viewFeatFilename = stlplus::filespec_to_path(outDirectory, std::to_string(viewId) + ".reconstructedFeatures");
    std::ofstream viewFeatFile(viewFeatFilename);
    viewFeatFile << "# landmarkId x y\n";
    for(const Observation& obs: viewObservations)
    {
      viewFeatFile << obs.id_feat << " " << obs.x[0] << " " << obs.x[1] << "\n";
    }
    viewFeatFile.close();
  }
  
  // Expose camera poses
  {
    const std::string cameraPosesFilename = stlplus::filespec_to_path(outDirectory, "cameras.poses");
    std::ofstream cameraPosesFile(cameraPosesFilename);
    cameraPosesFile << "# viewId R11 R12 R13 R21 R22 R23 R31 R32 R33 C1 C2 C3\n";
    for(const auto& v: sfm_data.views)
    {
      const View& view = *v.second.get();
      if(!sfm_data.IsPoseAndIntrinsicDefined(&view))
        continue;
      const Pose3& pose = sfm_data.poses.at(view.id_pose);
      cameraPosesFile << view.id_view
              << " " << pose.rotation()(0, 0)
              << " " << pose.rotation()(0, 1)
              << " " << pose.rotation()(0, 2)
              << " " << pose.rotation()(1, 0)
              << " " << pose.rotation()(1, 1)
              << " " << pose.rotation()(1, 2)
              << " " << pose.rotation()(2, 0)
              << " " << pose.rotation()(2, 1)
              << " " << pose.rotation()(2, 2)
              << " " << pose.center()(0)
              << " " << pose.center()(1)
              << " " << pose.center()(2)
              << "\n";
    }
    cameraPosesFile.close();
  }
  
  // Expose camera intrinsics
  // Note: we export it per view, It is really redundant but easy to parse.
  {
    const std::string cameraIntrinsicsFilename = stlplus::filespec_to_path(outDirectory, "cameras.intrinsics");
    std::ofstream cameraIntrinsicsFile(cameraIntrinsicsFilename);
    cameraIntrinsicsFile << "# viewId f u0 v0 k1 k2 k3\n";
    for(const auto& v: sfm_data.views)
    {
      const View& view = *v.second.get();
      if(!sfm_data.IsPoseAndIntrinsicDefined(&view))
        continue;
      const IntrinsicBase& intrinsics = *sfm_data.intrinsics.at(view.id_intrinsic).get();
      cameraIntrinsicsFile << view.id_view;
      for(double p: intrinsics.getParams())
        cameraIntrinsicsFile << " " << p;
      cameraIntrinsicsFile << "\n";
    }
    cameraIntrinsicsFile.close();
  }
  
  return true;
}

int main(int argc, char *argv[])
{
  CmdLine cmd;
  std::string sSfM_Data_Filename;
  std::string sOutDir = "";

  cmd.add( make_option('i', sSfM_Data_Filename, "sfmdata") );
  cmd.add( make_option('o', sOutDir, "outdir") );

  try {
      if (argc == 1) throw std::string("Invalid command line parameter.");
      cmd.process(argc, argv);
  } catch(const std::string& s) {
      std::cerr << "Usage: " << argv[0] << '\n'
      << "[-i|--sfmdata] filename, the SfM_Data file to convert\n"
      << "[-o|--outdir] path\n"
      << std::endl;

      std::cerr << s << std::endl;
      return EXIT_FAILURE;
  }

  sOutDir = stlplus::folder_to_path(sOutDir);

  // Create output dir
  if (!stlplus::folder_exists(sOutDir))
    stlplus::folder_create( sOutDir );

  // Read the input SfM scene
  SfM_Data sfm_data;
  if (!Load(sfm_data, sSfM_Data_Filename, ESfM_Data(ALL)))
  {
    std::cerr << std::endl
      << "The input SfM_Data file \""<< sSfM_Data_Filename << "\" cannot be read." << std::endl;
    return EXIT_FAILURE;
  }

  if (!exportToMatlab(sfm_data, sOutDir))
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
