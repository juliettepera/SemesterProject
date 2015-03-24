#ifndef SOLVEC_H
#define SOLVEC_H

// ShapeOp Libraries
#include <Constraint.h>
#include <Solver.h>
#include <Force.h>

// OpenMesh Libraries
#include <ObjectTypes/PolyMesh/PolyMesh.hh>

// Other Libraries
#include <string>
#include <iostream>
#include <math.h>

class solvec
{
    public:
        solvec();
        ShapeOp::Matrix3X getPoints(PolyMesh* mesh, std::vector<PolyMesh::VertexIter> vertexFixed);
        ShapeOp::Matrix3X solveShape(ShapeOp::Matrix3X MV, ShapeOp::MatrixXX ME, std::vector<int> fix);

    private:
        PolyMesh* m_Mesh;

};

#endif // SOLVEC_H
