// @HEADER
// ************************************************************************
//
//               Digital Image Correlation Engine (DICe)
//                 Copyright 2015 Sandia Corporation.
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact: Dan Turner (dzturne@sandia.gov)
//
// ************************************************************************
// @HEADER

#include <DICe_Initializer.h>

#include <Teuchos_RCP.hpp>

#include <iostream>
#include <fstream>
#include <math.h>
#include <cassert>

namespace DICe {

inline bool operator<(const def_triad& lhs, const def_triad& rhs){
  if(lhs.u_==rhs.u_){
    if(lhs.v_==rhs.v_){
      return lhs.t_ < lhs.t_;
    }
    else
      return lhs.v_<rhs.v_;
  }
  else
    return lhs.u_ < rhs.u_;
}
inline bool operator==(const def_triad& lhs, const def_triad& rhs){
  return lhs.u_ == rhs.u_ && lhs.v_ == rhs.v_ && lhs.t_ == rhs.t_;
}

Path_Initializer::Path_Initializer(Teuchos::RCP<Image> def_image,
  Teuchos::RCP<Subset> subset,
  const char * file_name,
  const size_t num_neighbors):
  Initializer(def_image,subset),
  num_triads_(0),
  num_neighbors_(num_neighbors)
{
  DEBUG_MSG("Constructor for Path_Initializer with file: "  << file_name);
  assert(num_neighbors_>0);
  // read in the solution file:
  std::string line;
  std::fstream path_file(file_name,std::ios_base::in);
  TEUCHOS_TEST_FOR_EXCEPTION(!path_file.is_open(),std::runtime_error,"Error, unable to load path file.");
  int_t num_lines = 0;
  // get the number of lines in the file:
  while (std::getline(path_file, line)){
    ++num_lines;
  }
  DEBUG_MSG("number of locations: " << num_lines);
  path_file.clear();
  path_file.seekg(0,std::ios::beg);

  // there are 3 columns of data
  // id u v theta
  // TODO better checking of the input format
  // TODO enable other resolutions for the input files
  // currently the resolution is 0.5 pixels for u and v and 0.01 radians for theta
  scalar_t u_tmp=0.0,v_tmp=0.0,t_tmp=0.0;
  for(int_t line=0;line<num_lines;++line){
    path_file >> u_tmp >> v_tmp >> t_tmp;
    u_tmp = floor((u_tmp*2)+0.5)/2;
    v_tmp = floor((v_tmp*2)+0.5)/2;
    t_tmp = floor(t_tmp*100+0.5)/100;
    def_triad uvt(u_tmp,v_tmp,t_tmp);
    triads_.insert(uvt);
  }
  path_file.close();
  num_triads_ = triads_.size();
  assert(num_triads_>0);

  DEBUG_MSG("creating the point cloud");
  point_cloud_ = Teuchos::rcp(new Point_Cloud<scalar_t>());
  point_cloud_->pts.resize(num_triads_);
  std::set<def_triad>::iterator it = triads_.begin();
  int_t id = 0;
  for(;it!=triads_.end();++it){
    point_cloud_->pts[id].x = (*it).u_;
    point_cloud_->pts[id].y = (*it).v_;
    point_cloud_->pts[id].z = (*it).t_;
    id++;
  }
  DEBUG_MSG("building the kd-tree");
  kd_tree_ = Teuchos::rcp(new my_kd_tree_t(3 /*dim*/, *point_cloud_.get(), nanoflann::KDTreeSingleIndexAdaptorParams(10 /* max leaf */) ) );
  kd_tree_->buildIndex();

  // now set up the neighbor list for each triad:
  neighbors_ = std::vector<size_t>(num_triads_*num_neighbors_,0);
  scalar_t query_pt[3];
  def_triad t(0,0,0);
  std::vector<size_t> ret_index(num_neighbors_);
  std::vector<scalar_t> out_dist_sqr(num_neighbors_);
  it = triads_.begin();
  id = 0;
  for(;it!=triads_.end();++it){
    //t = *it;
    query_pt[0] = (*it).u_;
    query_pt[1] = (*it).v_;
    query_pt[2] = (*it).t_;
    kd_tree_->knnSearch(&query_pt[0], num_neighbors_, &ret_index[0], &out_dist_sqr[0]);
    for(size_t i=0;i<num_neighbors_;++i){
      neighbors_[id*num_neighbors_ + i] = ret_index[i];
    }
    id++;
  }
}

void
Path_Initializer::closest_triad(const scalar_t &u,
  const scalar_t &v,
  const scalar_t &t,
  size_t id,
  scalar_t & distance_sqr)const{

  scalar_t query_pt[3];
  std::vector<size_t> ret_index(num_neighbors_,0.0);
  std::vector<scalar_t> out_dist_sqr(num_neighbors_,0.0);
  query_pt[0] = u;
  query_pt[1] = v;
  query_pt[2] = t;
  kd_tree_->knnSearch(&query_pt[0], 1, &ret_index[0], &out_dist_sqr[0]);
  id = ret_index[0];
  distance_sqr = out_dist_sqr[0];
}

scalar_t
Path_Initializer::initial_guess(Teuchos::RCP<std::vector<scalar_t> > deformation,
  const scalar_t & u,
  const scalar_t & v,
  const scalar_t & t){

  // find the closes triad in the set:
  int_t id = -1;
  scalar_t dist = 0.0;
  closest_triad(u,v,t,id,dist);

  // iterate over the closest 6 triads to this one to see which one is best:
  // start with the given guess
  (*deformation)[DISPLACEMENT_X] = u;
  (*deformation)[DISPLACEMENT_Y] = v;
  (*deformation)[ROTATION_Z] = t;
  // TODO what to do with the rest of the deformation entries (zero them)?
  subset_->initialize(def_image_,DEF_INTENSITIES,deformation);
  // assumes that check for blocking subsets has already been performed
  subset_->turn_off_obstructed_pixels(deformation);
  // assumes that the reference subset has already been initialized
  scalar_t gamma = subset_->gamma();
  scalar_t best_u = u;
  scalar_t best_v = v;
  scalar_t best_t = t;
  scalar_t best_gamma = gamma;

  for(size_t neigh = 0;neigh<num_neighbors_;++neigh){
    const size_t neigh_id = neighbor(id,neigh);
    (*deformation)[DISPLACEMENT_X] = point_cloud_->pts[neigh_id].x;
    (*deformation)[DISPLACEMENT_Y] = point_cloud_->pts[neigh_id].y;
    (*deformation)[ROTATION_Z] = point_cloud_->pts[neigh_id].z;
    std::cout << "checking neigh: " << neigh_id << " " << (*deformation)[DISPLACEMENT_X] << " " << (*deformation)[DISPLACEMENT_X] << " " << (*deformation)[ROTATION_Z] << std::endl;
    // TODO what to do with the rest of the deformation entries (zero them)?
    subset_->initialize(def_image_,DEF_INTENSITIES,deformation);
    // assumes that check for blocking subsets has already been performed
    subset_->turn_off_obstructed_pixels(deformation);
    // assumes that the reference subset has already been initialized
    gamma = subset_->gamma();
    std::cout << "gamma value " << gamma << std::endl;
    if(gamma < best_gamma){
      std::cout << " winner" << std::endl;
      best_gamma = gamma;
      best_u = (*deformation)[DISPLACEMENT_X];
      best_v = (*deformation)[DISPLACEMENT_Y];
      best_t = (*deformation)[ROTATION_Z];
    }
  }
  (*deformation)[DISPLACEMENT_X] = best_u;
  (*deformation)[DISPLACEMENT_Y] = best_v;
  (*deformation)[ROTATION_Z] = best_t;
  return best_gamma;
}

scalar_t
Path_Initializer::initial_guess(Teuchos::RCP<std::vector<scalar_t> > deformation){
  scalar_t gamma = 0.0;
  scalar_t best_u = 0.0;
  scalar_t best_v = 0.0;
  scalar_t best_t = 0.0;
  scalar_t best_gamma = 100.0;

  // iterate the entire set of triads:
  for(size_t id = 0;id<num_triads_;++id){
    (*deformation)[DISPLACEMENT_X] = point_cloud_->pts[id].x;
    (*deformation)[DISPLACEMENT_Y] = point_cloud_->pts[id].y;
    (*deformation)[ROTATION_Z] = point_cloud_->pts[id].z;
    std::cout << "checking triad: " << id << " " << (*deformation)[DISPLACEMENT_X] << " " << (*deformation)[DISPLACEMENT_X] << " " << (*deformation)[ROTATION_Z] << std::endl;
    // TODO what to do with the rest of the deformation entries (zero them)?
    subset_->initialize(def_image_,DEF_INTENSITIES,deformation);
    // assumes that check for blocking subsets has already been performed
    subset_->turn_off_obstructed_pixels(deformation);
    // assumes that the reference subset has already been initialized
    gamma = subset_->gamma();
    std::cout << "gamma value " << gamma << std::endl;
    if(gamma < best_gamma){
      std::cout << " winner" << std::endl;
      best_gamma = gamma;
      best_u = (*deformation)[DISPLACEMENT_X];
      best_v = (*deformation)[DISPLACEMENT_Y];
      best_t = (*deformation)[ROTATION_Z];
    }
  }
  (*deformation)[DISPLACEMENT_X] = best_u;
  (*deformation)[DISPLACEMENT_Y] = best_v;
  (*deformation)[ROTATION_Z] = best_t;
  return best_gamma;
}

}// End DICe Namespace