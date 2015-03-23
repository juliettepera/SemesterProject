#ifndef SOLVEC_H
#define SOLVEC_H

// ShapeOp Libraries
#include <Constraint.h>
#include <Solver.h>

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
        void hello();
        int getPoints(PolyMesh* mesh, std::vector<PolyMesh::VertexIter> vertexFixed);

    private:
};

#endif // SOLVEC_H
