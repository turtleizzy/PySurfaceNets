#include "SurfaceNets.h"
#include <iostream>

MMSurfaceNet* runSurfaceNets(unsigned short* data, int arraySize[3], float voxelSize[3])
{
    MMSurfaceNet* m_surfaceNet = new MMSurfaceNet(data, arraySize, voxelSize);
    MMSurfaceNet::RelaxAttrs m_relaxAttrs;
    m_relaxAttrs.maxDistFromCellCenter = 0.5f;
    m_relaxAttrs.numRelaxIterations = 20;
    m_relaxAttrs.maxDistFromCellCenter = 1.0f;
    
    m_surfaceNet->relax(m_relaxAttrs);

    return m_surfaceNet;
}

std::tuple<std::vector<int>, std::vector<py::array_t<float>>,  std::vector<py::array_t<int>>>
runSurfaceNetsNpWrapper(const py::array_t<unsigned short> inputArray, const py::array_t<float> voxelSize)
{
    if (inputArray.ndim() != 3)
        throw std::runtime_error("inputArray must be 3-dimensional.");
    if (voxelSize.ndim() != 1 || voxelSize.shape(0) != 3)
        throw std::runtime_error("voxelSize.shape should be [3].");
    int _arraySize[3] = {inputArray.shape(0), inputArray.shape(1), inputArray.shape(2)};
    auto voxelSizePtr = voxelSize.data();
    float _voxelSize[3] = {voxelSizePtr[0], voxelSizePtr[1], voxelSizePtr[2]};
	unsigned short* data;
    int bufferSize = _arraySize[0] * _arraySize[1] * _arraySize[2];
    std::vector<int> labels;
    std::vector<py::array_t<float>> verts;
    std::vector<py::array_t<int>> faces;

    data = new unsigned short[bufferSize];
    auto inputArrayAccessor = inputArray.unchecked<3>();
    unsigned short* pData = data;
    for (int k = 0; k < _arraySize[2]; k++)
        for (int j = 0; j < _arraySize[1]; j++)
            for (int i = 0; i < _arraySize[0]; i++) 
                *pData++ = (unsigned short) inputArrayAccessor(i, j, k);
    
    auto resSN = runSurfaceNets(data, _arraySize, _voxelSize);

    auto resObj = new MMGeometryOBJ(resSN);

    labels = resObj->labels();
    std::vector<int> resLabels;
    for (auto labelIter=labels.begin(); labelIter != labels.end(); labelIter ++) {
        if (*labelIter == 0) continue;
        float* curVertsData;
        int* curFacesData;
        auto curObjData = resObj->objData(*labelIter);

        curVertsData = new float[curObjData.vertexPositions.size() * 3];
        curFacesData = new int[curObjData.triangles.size() * 3];
        py::capsule freeCurVertsData(curVertsData, [](void *f) {
            float *foo = reinterpret_cast<float *>(f);
            std::cerr << "freeing memory @ " << f << "\n";
            delete[] foo;
        });
        py::capsule freeCurFacesData(curFacesData, [](void *f) {
            int *foo = reinterpret_cast<int *>(f);
            std::cerr << "freeing memory @ " << f << "\n";
            delete[] foo;
        });
        
        std::vector<size_t> curVertsDataShape = { curObjData.vertexPositions.size(), 3 };
        std::vector<size_t> curFacesDataShape = { curObjData.triangles.size(), 3 };

        auto curVerts = py::array_t<float>(
            curVertsDataShape,
            curVertsData,
            freeCurVertsData
        );

        auto curFaces = py::array_t<int>(
            curFacesDataShape, 
            curFacesData,
            freeCurFacesData
        );
        auto curVertsDataAccessor = curVerts.mutable_unchecked<2>();
        for (int i=0; i<curObjData.vertexPositions.size(); i++)
            for (int j=0;j<3;j++)
                curVertsDataAccessor(i, j) = curObjData.vertexPositions[i][j];
        auto curFacesDataAccessor = curFaces.mutable_unchecked<2>();
        for (int i=0; i<curObjData.triangles.size(); i++)
            for (int j=0;j<3;j++)
                curFacesDataAccessor(i, j) = curObjData.triangles[i][j];
        verts.push_back(curVerts);
        faces.push_back(curFaces);
        resLabels.push_back(*labelIter);
    }
    delete[] data;
    delete resSN;
    delete resObj;
    return { resLabels, verts, faces };
}

PYBIND11_MODULE(PySurfaceNets, m) {
    m.doc() = "Multi-label surface nets";
    m.def("SurfaceNets", &runSurfaceNetsNpWrapper, "Run multi-label surface net.");
}

