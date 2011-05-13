// $Id$

#include "HDF5Writer.h"
#include "SIMbase.h"
#include "IntegrandBase.h"
#include <sstream>
#include <numeric>
#include <sys/stat.h>

#ifdef HAS_HDF5
#include <hdf5.h>
#endif

#ifdef PARALLEL_PETSC
#include <mpi.h>
#endif


HDF5Writer::HDF5Writer (const std::string& name) : m_hdf5(name+".hdf5")
{
#ifdef HAS_HDF5
  struct stat temp;
  // file already exists - open and find the next group
  if (stat(m_hdf5.c_str(),&temp) == 0)
    m_flag = H5F_ACC_RDWR;
  else
    m_flag = H5F_ACC_TRUNC;
#endif
#ifdef PARALLEL_PETSC
  MPI_Comm_rank(MPI_COMM_WORLD,&m_rank);
  MPI_Comm_size(MPI_COMM_WORLD,&m_size);
#else
  m_rank = 0;
#endif
}

HDF5Writer::~HDF5Writer()
{
}

int HDF5Writer::getLastTimeLevel()
{
#ifdef HAS_HDF5
  if (m_flag == H5F_ACC_TRUNC)
    return -1;

  hid_t acc_tpl = H5P_DEFAULT;
#ifdef PARALLEL_PETSC
  MPI_Info info = MPI_INFO_NULL;
  acc_tpl = H5Pcreate (H5P_FILE_ACCESS);
  H5Pset_fapl_mpio(acc_tpl, MPI_COMM_WORLD, info);
#endif

  m_file = H5Fopen(m_hdf5.c_str(),m_flag,acc_tpl);
  int result=0;
  while (1) {
    std::stringstream str;
    str << '/' << result;
    if (!checkGroupExistence(m_file,str.str().c_str()))
      break;
    result++;
  }
  H5Fclose(m_file);

  return result-1;
#else
  return -1;
#endif
}

void HDF5Writer::openFile(int level)
{
#ifdef HAS_HDF5
  hid_t acc_tpl = H5P_DEFAULT;
#ifdef PARALLEL_PETSC
  MPI_Info info = MPI_INFO_NULL;
  acc_tpl = H5Pcreate (H5P_FILE_ACCESS);
  H5Pset_fapl_mpio(acc_tpl, MPI_COMM_WORLD, info);
#endif
  if (m_flag == H5F_ACC_TRUNC)
    m_file = H5Fcreate(m_hdf5.c_str(),m_flag,H5P_DEFAULT,acc_tpl);
  else
    m_file = H5Fopen(m_hdf5.c_str(),m_flag,acc_tpl);

  std::stringstream str;
  str << '/' << level;
  if (!checkGroupExistence(m_file,str.str().c_str())) {
    hid_t group = H5Gcreate2(m_file,str.str().c_str(),0,H5P_DEFAULT,H5P_DEFAULT);
    H5Gclose(group);
  }
#ifdef PARALLEL_PETSC
  H5Pclose(acc_tpl);
#endif
#endif
}

void HDF5Writer::closeFile(int level)
{
#ifdef HAS_HDF5
  H5Fflush(m_file,H5F_SCOPE_GLOBAL);
  H5Fclose(m_file);
  m_flag = H5F_ACC_RDWR;
#endif
}

void HDF5Writer::readArray(int group, const std::string& name,
                           int& len, double*& data)
{
#ifdef HAS_HDF5
  hid_t set = H5Dopen2(group,name.c_str(),H5P_DEFAULT);
  hsize_t siz = H5Dget_storage_size(set);
  siz /= 8;
  len = siz;
  data = new double[siz];
  H5Dread(set,H5T_NATIVE_DOUBLE,H5S_ALL,H5S_ALL,H5P_DEFAULT,data);
  H5Dclose(set);
#else
  std::cout << "HDF5Writer: compiled without HDF5 support, no data read" << std::endl;
#endif
}

void HDF5Writer::writeArray(int group, const std::string& name,
                            int len, void* data)
{
#ifdef HAS_HDF5
#ifdef PARALLEL_PETSC
  int lens[m_size];
  std::fill(lens,lens+m_size,len);
  MPI_Alltoall(lens,1,MPI_INT,lens,1,MPI_INT,MPI_COMM_WORLD);
  int total_len=std::accumulate(lens,lens+m_size,0);
  hsize_t start = (hsize_t)std::accumulate(lens,lens+m_rank,0);
#else
  int total_len = len;
  hsize_t start = 0;
#endif
  hsize_t stride = 1;
  hsize_t siz = total_len;
  hid_t space = H5Screate_simple(1,&siz,NULL);
  hid_t set = H5Dcreate2(group,name.c_str(),
                         H5T_NATIVE_DOUBLE,space,H5P_DEFAULT,H5P_DEFAULT,H5P_DEFAULT);
  hid_t file_space = H5Dget_space(set);
  if (len ) {
    siz = len;
    H5Sselect_hyperslab(file_space,H5S_SELECT_SET,&start,&stride,&siz,NULL);
    hid_t mem_space = H5Screate_simple(1,&siz,NULL);
    H5Dwrite(set,H5T_NATIVE_DOUBLE,mem_space,file_space,H5P_DEFAULT,data);
    H5Sclose(mem_space);
  }
  H5Sclose(file_space);
  H5Dclose(set);
#else
  std::cout << "HDF5Writer: compiled without HDF5 support, no data written" << std::endl;
#endif
}

void HDF5Writer::readVector(int level, const DataEntry& entry)
{
//  readArray(level,entry.first,entry.second.size,entry.second.data);
}

void HDF5Writer::writeVector(int level, const DataEntry& entry)
{
  writeArray(level,entry.first,entry.second.size,entry.second.data);
}

void HDF5Writer::readSIM(int level, const DataEntry& entry)
{
#ifdef HAS_HDF5
  SIMbase* sim = static_cast<SIMbase*>(entry.second.data);
  Vector* sol = static_cast<Vector*>(entry.second.data2);
  if (!sol) return;

  for (int i = 0; i < sim->getNoPatches(); ++i) {
    std::stringstream str;
    str << level;
    str << '/';
    str << i+1;
    hid_t group2 = H5Gopen2(m_file,str.str().c_str(),H5P_DEFAULT);
    int loc = sim->getLocalPatchIndex(i+1);
    if (loc > 0) {
      double* tmp = NULL; int siz = 0;
      readArray(group2,entry.first,siz,tmp);
      sim->injectPatchSolution(*sol,Vector(tmp,siz),loc-1,entry.second.size);
      delete[] tmp;
    }
    H5Gclose(group2);
  }
#endif
}

void HDF5Writer::writeSIM(int level, const DataEntry& entry)
{
#ifdef HAS_HDF5
  SIMbase* sim = static_cast<SIMbase*>(entry.second.data);
  Vector* sol = static_cast<Vector*>(entry.second.data2);
  if (!sol) return;

  const Integrand* prob = sim->getProblem();
  for (int i = 0; i < sim->getNoPatches(); ++i) {
    std::stringstream str;
    str << level;
    str << '/';
    str << i+1;
    hid_t group2;
    if (checkGroupExistence(m_file,str.str().c_str()))
      group2 = H5Gopen2(m_file,str.str().c_str(),H5P_DEFAULT);
    else
      group2 = H5Gcreate2(m_file,str.str().c_str(),0,H5P_DEFAULT,H5P_DEFAULT);
    int loc = sim->getLocalPatchIndex(i+1);
    if (loc > 0) // we own the patch
      sim->extractPatchSolution(*sol,loc-1);
    if (entry.second.size == -1) {
      Matrix field;
      if (loc > 0)
	sim->evalSecondarySolution(field,loc-1);
      for (size_t j = 0; j < prob->getNoFields(2); ++j)
        writeArray(group2,prob->getField2Name(j),field.cols(),
		   field.getRow(j+1).ptr());
    }
    Vector& psol = const_cast<Integrand*>(prob)->getSolution();
    writeArray(group2, entry.first, loc > 0 ? psol.size() : 0, psol.ptr());
    H5Gclose(group2);
  }
#endif
}

bool HDF5Writer::checkGroupExistence(int parent, const char* path)
{
#ifdef HAS_HDF5
  // turn off errors to avoid cout spew
  herr_t (*old_func)(void*);
  void* old_client_data;
  H5Eget_auto(&old_func,&old_client_data);
  H5Eset_auto(NULL,NULL);
  bool result =H5Gget_objinfo((hid_t)parent,path,0,NULL) == 0;
  H5Eset_auto(old_func,old_client_data);
  return result;
#endif
  return false;
}