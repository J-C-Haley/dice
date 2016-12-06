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

#include <DICe.h>
#include <DICe_Triangulation.h>

#include <Teuchos_RCP.hpp>
#include <Teuchos_oblackholestream.hpp>
#include <Teuchos_ParameterList.hpp>

#include <iostream>

using namespace DICe;

int main(int argc, char *argv[]) {

  DICe::initialize(argc, argv);

  // only print output if args are given (for testing the output is quiet)
  int_t iprint     = argc - 1;
  int_t errorFlag  = 0;
  scalar_t errorTol = 1.0E-2;
  Teuchos::RCP<std::ostream> outStream;
  Teuchos::oblackholestream bhs; // outputs nothing
  if (iprint > 0)
    outStream = Teuchos::rcp(&std::cout, false);
  else
    outStream = Teuchos::rcp(&bhs, false);

  *outStream << "--- Begin test ---" << std::endl;

  std::vector<scalar_t> ig_1(8,0.0);
  ig_1[0] = 638.913;
  ig_1[1] = 407.295;
  ig_1[2] = 2468.53;
  ig_1[3] = 2468.25;
  ig_1[4] = -0.171198;
  ig_1[5] = 0.0638413;
  ig_1[6] = 0.0;
  ig_1[7] = 0.0;
  std::vector<scalar_t> ig_2(8,0.0);
  ig_2[0] = 628.607;
  ig_2[1] = 394.571;
  ig_2[2] = 2377.11;
  ig_2[3] = 2376.92;
  ig_2[4] = 0.0897842;
  ig_2[5] = 0.0619845;
  ig_2[6] = 0.0;
  ig_2[7] = 0.0;
  std::vector<std::vector<scalar_t> > intrinsic_gold;
  intrinsic_gold.push_back(ig_1);
  intrinsic_gold.push_back(ig_2);
  std::vector<scalar_t> T_1(4,0.0);
  T_1[0] = 0.950892;
  T_1[1] = 0.00104338;
  T_1[2] = -0.30952;
  T_1[3] = 130.755;
  std::vector<scalar_t> T_2(4,0.0);
  T_2[0] = -0.00145487;
  T_2[1] = 0.999998;
  T_2[2] = -0.00109863;
  T_2[3] = -0.610487;
  std::vector<scalar_t> T_3(4,0.0);
  T_3[0] = 0.309519;
  T_3[1] = 0.00149499;
  T_3[2] = 0.950892;
  T_3[3] = 17.1329;
  std::vector<scalar_t> T_4(4,0.0);
  T_4[0] = 0;
  T_4[1] = 0;
  T_4[2] = 0;
  T_4[3] = 1;

  std::vector<std::vector<scalar_t> > T_mat_gold;
  T_mat_gold.push_back(T_1);
  T_mat_gold.push_back(T_2);
  T_mat_gold.push_back(T_3);
  T_mat_gold.push_back(T_4);

  std::vector<scalar_t> z_1(4,0.0);
  z_1[0] = 0.987647;
  z_1[1] = 0.000580617;
  z_1[2] = -0.156696;
  z_1[3] = 65.3774;
  std::vector<scalar_t> z_2(4,0.0);
  z_2[0] = 0.000684129;
  z_2[1] = -1.0;
  z_2[2] = 0.00060666;
  z_2[3] = -0.305243;
  std::vector<scalar_t> z_3(4,0.0);
  z_3[0] = -0.156695;
  z_3[1] = -0.000706366;
  z_3[2] = -0.987647;
  z_3[3] = 8.56645;
  std::vector<scalar_t> z_4(4,0.0);
  z_4[0] = 0;
  z_4[1] = 0;
  z_4[2] = 0;
  z_4[3] = 1;

  std::vector<std::vector<scalar_t> > zero_to_world_xml_gold;
  zero_to_world_xml_gold.push_back(z_1);
  zero_to_world_xml_gold.push_back(z_2);
  zero_to_world_xml_gold.push_back(z_3);
  zero_to_world_xml_gold.push_back(z_4);

  std::vector<std::vector<scalar_t> > zero_to_world_txt_gold =
    {{1.0,0.0,0.0,0.0},
     {0.0,1.0,0.0,0.0},
     {0.0,0.0,1.0,0.0},
     {0.0,0.0,0.0,1.0}};

  *outStream << "reading calibration parameters from vic3d format" << std::endl;

  Teuchos::RCP<Triangulation> triangulation_xml = Teuchos::rcp(new Triangulation("./cal/cal_a.xml"));
  std::vector<std::vector<scalar_t> > & calibration_intrinsics_xml = *triangulation_xml->cal_intrinsics();
  std::vector<std::vector<scalar_t> > & calibration_T_mat_xml = * triangulation_xml->cal_extrinsics();
  std::vector<std::vector<scalar_t> > & zero_to_world_xml = * triangulation_xml->trans_extrinsics();

  *outStream << "testing intrinsics from vic3d format" << std::endl;

  if(calibration_intrinsics_xml.size()!=2){
    errorFlag++;
    *outStream << "Error, intrinsics array is the wrong length, should be 2 and is " << calibration_intrinsics_xml.size() << std::endl;
  }
  else{
    if(calibration_intrinsics_xml[0].size()!=8){
      errorFlag++;
      *outStream << "Error, intrinsics array is the wrong width, should be 8 and is " << calibration_intrinsics_xml[0].size() << std::endl;
    }
    else{
      for(size_t i=0;i<intrinsic_gold.size();++i){
        for(size_t j=0;j<intrinsic_gold[0].size();++j){
          if(std::abs(calibration_intrinsics_xml[i][j]-intrinsic_gold[i][j])>errorTol){
            *outStream << "Error, intrinsic value " << i << " " << j << " is not correct. Should be " << intrinsic_gold[i][j] << " is " << calibration_intrinsics_xml[i][j] << std::endl;
            errorFlag++;
          }
        }
      }
    }
  }

  *outStream << "testing T_mat from vic3d format" << std::endl;

  if(calibration_T_mat_xml.size()!=4){
    errorFlag++;
    *outStream << "Error, T_mat array is the wrong length, should be 4 and is " << calibration_T_mat_xml.size() << std::endl;
  }
  else{
    if(calibration_T_mat_xml[0].size()!=4){
      errorFlag++;
      *outStream << "Error, T_mat array is the wrong width, should be 4 and is " << calibration_T_mat_xml[0].size() << std::endl;
    }
    else{
      for(size_t i=0;i<T_mat_gold.size();++i){
        for(size_t j=0;j<T_mat_gold[0].size();++j){
          if(std::abs(calibration_T_mat_xml[i][j]-T_mat_gold[i][j])>errorTol){
            *outStream << "Error, T_mat value " << i << " " << j << " is not correct. Should be " << T_mat_gold[i][j] << " is " << calibration_T_mat_xml[i][j] << std::endl;
            errorFlag++;
          }
        }
      }
    }
  }

  *outStream << "testing camera 0 to world transform from vic3d format" << std::endl;

  if(zero_to_world_xml.size()!=4){
    errorFlag++;
    *outStream << "Error, zero_to_world array is the wrong length, should be 4 and is " << zero_to_world_xml.size() << std::endl;
  }
  else{
    if(zero_to_world_xml[0].size()!=4){
      errorFlag++;
      *outStream << "Error, zero_to_world array is the wrong width, should be 4 and is " << zero_to_world_xml[0].size() << std::endl;
    }
    else{
      for(size_t i=0;i<zero_to_world_xml_gold.size();++i){
        for(size_t j=0;j<zero_to_world_xml_gold[0].size();++j){
          if(std::abs(zero_to_world_xml[i][j]-zero_to_world_xml_gold[i][j])>errorTol){
            *outStream << "Error, zero_to_world value " << i << " " << j << " is not correct. Should be " << zero_to_world_xml_gold[i][j] << " is " << zero_to_world_xml[i][j] << std::endl;
            errorFlag++;
          }
        }
      }
    }
  }

  *outStream << "calibration parameters from vic3d format have been checked" << std::endl;

  *outStream << "reading calibration parameters from text format" << std::endl;

  Teuchos::RCP<Triangulation> triangulation_txt = Teuchos::rcp(new Triangulation("./cal/cal_a.txt"));
  std::vector<std::vector<scalar_t> > & calibration_intrinsics_txt = *triangulation_txt->cal_intrinsics();
  std::vector<std::vector<scalar_t> > & calibration_T_mat_txt = * triangulation_txt->cal_extrinsics();
  std::vector<std::vector<scalar_t> > & zero_to_world_txt = * triangulation_txt->trans_extrinsics();

  *outStream << "testing intrinsics from txt format" << std::endl;

  if(calibration_intrinsics_txt.size()!=2){
    errorFlag++;
    *outStream << "Error, intrinsics array is the wrong length, should be 2 and is " << calibration_intrinsics_txt.size() << std::endl;
  }
  else{
    if(calibration_intrinsics_txt[0].size()!=8){
      errorFlag++;
      *outStream << "Error, intrinsics array is the wrong width, should be 8 and is " << calibration_intrinsics_txt[0].size() << std::endl;
    }
    else{
      for(size_t i=0;i<intrinsic_gold.size();++i){
        for(size_t j=0;j<intrinsic_gold[0].size();++j){
          if(std::abs(calibration_intrinsics_txt[i][j]-intrinsic_gold[i][j])>errorTol){
            *outStream << "Error, intrinsic value " << i << " " << j << " is not correct. Should be " << intrinsic_gold[i][j] << " is " << calibration_intrinsics_txt[i][j] << std::endl;
            errorFlag++;
          }
        }
      }
    }
  }

  *outStream << "testing T_mat from txt format" << std::endl;

  if(calibration_T_mat_txt.size()!=4){
    errorFlag++;
    *outStream << "Error, T_mat array is the wrong length, should be 4 and is " << calibration_T_mat_txt.size() << std::endl;
  }
  else{
    if(calibration_T_mat_txt[0].size()!=4){
      errorFlag++;
      *outStream << "Error, T_mat array is the wrong width, should be 4 and is " << calibration_T_mat_txt[0].size() << std::endl;
    }
    else{
      for(size_t i=0;i<T_mat_gold.size();++i){
        for(size_t j=0;j<T_mat_gold[0].size();++j){
          if(std::abs(calibration_T_mat_txt[i][j]-T_mat_gold[i][j])>errorTol){
            *outStream << "Error, T_mat value " << i << " " << j << " is not correct. Should be " << T_mat_gold[i][j] << " is " << calibration_T_mat_txt[i][j] << std::endl;
            errorFlag++;
          }
        }
      }
    }
  }

  *outStream << "testing camera 0 to world transform from txt format" << std::endl;

  if(zero_to_world_txt.size()!=4){
    errorFlag++;
    *outStream << "Error, zero_to_world array is the wrong length, should be 4 and is " << zero_to_world_txt.size() << std::endl;
  }
  else{
    if(zero_to_world_txt[0].size()!=4){
      errorFlag++;
      *outStream << "Error, zero_to_world array is the wrong width, should be 4 and is " << zero_to_world_txt[0].size() << std::endl;
    }
    else{
      for(size_t i=0;i<zero_to_world_txt_gold.size();++i){
        for(size_t j=0;j<zero_to_world_txt_gold[0].size();++j){
          if(std::abs(zero_to_world_txt[i][j]-zero_to_world_txt_gold[i][j])>errorTol){
            *outStream << "Error, zero_to_world value " << i << " " << j << " is not correct. Should be " << zero_to_world_txt_gold[i][j] << " is " << zero_to_world_txt[i][j] << std::endl;
            errorFlag++;
          }
        }
      }
    }
  }

  *outStream << "calibration parameters from txt format have been checked" << std::endl;

  *outStream << "testing calibration txt file with custom transform" << std::endl;

  Teuchos::RCP<Triangulation> tri_custom = Teuchos::rcp(new Triangulation("./cal/cal_a_with_transform.txt"));
  std::vector<std::vector<scalar_t> > & custom_zero_to_world = * tri_custom->trans_extrinsics();
  *outStream << "testing camera 0 to world transform from txt format with custom transform" << std::endl;

  if(custom_zero_to_world.size()!=4){
    errorFlag++;
    *outStream << "Error, zero_to_world array is the wrong length, should be 4 and is " << custom_zero_to_world.size() << std::endl;
  }
  else{
    if(custom_zero_to_world[0].size()!=4){
      errorFlag++;
      *outStream << "Error, zero_to_world array is the wrong width, should be 4 and is " << custom_zero_to_world[0].size() << std::endl;
    }
    else{
      for(size_t i=0;i<zero_to_world_xml_gold.size();++i){
        for(size_t j=0;j<zero_to_world_xml_gold[0].size();++j){
          if(std::abs(custom_zero_to_world[i][j]-zero_to_world_xml_gold[i][j])>errorTol){
            *outStream << "Error, zero_to_world value " << i << " " << j << " is not correct. Should be " << zero_to_world_xml_gold[i][j] << " is " << custom_zero_to_world[i][j] << std::endl;
            errorFlag++;
          }
        }
      }
    }
  }

  *outStream << "calibration parameters from txt format with custom transform have been checked" << std::endl;

  *outStream << "testing triangulation of 3d points" << std::endl;

  Teuchos::RCP<Triangulation> tri = Teuchos::rcp(new Triangulation("./cal/cal_b.xml"));
  scalar_t xc_out=0.0,yc_out=0.0,zc_out=0.0;
  scalar_t xw_out=0.0,yw_out=0.0,zw_out=0.0;
  scalar_t x_0 = 190; scalar_t y_0 = 187;
  scalar_t x_1 = 193.8777; scalar_t y_1 = 186.0944;
  tri->triangulate(x_0,y_0,x_1,y_1,xc_out,yc_out,zc_out,xw_out,yw_out,zw_out,false);
  scalar_t global_x_gold = 46.1199;
  scalar_t global_y_gold = -25.5283;
  scalar_t global_z_gold = -6543.5;
  if(std::abs(global_x_gold - xw_out) > errorTol){
    errorFlag++;
    *outStream << "Error, triangulation x coord is wrong. Should be " << global_x_gold << " is " << xw_out << std::endl;
  }
  if(std::abs(global_y_gold - yw_out) > errorTol){
    errorFlag++;
    *outStream << "Error, triangulation y coord is wrong. Should be " << global_y_gold << " is " << yw_out << std::endl;
  }
  if(std::abs(global_z_gold - zw_out) > errorTol){
    errorFlag++;
    *outStream << "Error, triangulation z coord is wrong. Should be " << global_z_gold << " is " << zw_out << std::endl;
  }

  *outStream << "triangulation of 3d points completed and tested" << std::endl;

  *outStream << "testing projective transforms" << std::endl;

  Teuchos::RCP<Triangulation> proj_tri = Teuchos::rcp(new Triangulation());
  Teuchos::RCP<std::vector<scalar_t> > projectives = Teuchos::rcp(new std::vector<scalar_t>(8,0.0));
  (*projectives)[0] = 1.5;
  (*projectives)[1] = 0.03;
  (*projectives)[2] = -25.85;
  (*projectives)[3] = 0.3;
  (*projectives)[4] = 1.6;
  (*projectives)[5] = -18.0;
  (*projectives)[6] = 0.0005;
  (*projectives)[7] = 0.0001;
  proj_tri->set_projectives(projectives);
  const scalar_t xl0 = 75, yl0 = 380;
  scalar_t xr0 = 0.0, yr0 = 0.0;
  proj_tri->project_left_to_right_sensor_coords(xl0,yl0,xr0,yr0);

  *outStream << "xl " << xl0 << " yl " << yl0 << " xr " << xr0 << " yr " << yr0 << std::endl;

  if(std::abs(xr0 - 91.166) > errorTol || std::abs(yr0 - 569.5026) > errorTol){
    errorFlag++;
    *outStream << "Error, projective transform is incorrect" << std::endl;
  }

  *outStream << "--- End test ---" << std::endl;

  DICe::finalize();

  if (errorFlag != 0)
    std::cout << "End Result: TEST FAILED\n";
  else
    std::cout << "End Result: TEST PASSED\n";

  return 0;

}

