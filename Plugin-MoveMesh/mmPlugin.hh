#ifndef MMPLUGIN_HH_INCLUDED
#define MMPLUGIN_HH_INCLUDED

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/ToolboxInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/MouseInterface.hh>
#include <OpenFlipper/BasePlugin/KeyInterface.hh>
#include <OpenFlipper/BasePlugin/PickingInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/BasePlugin/RPCInterface.hh>
#include <ObjectTypes/Coordsys/CoordinateSystemNode.hh>

#include <ObjectTypes/PolyMesh/PolyMesh.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

// ShapeOp Libraries
#include <Constraint.h>
#include <Solver.h>
#include <Force.h>

#include <cmath>
#include <stdlib.h>
#include <Eigen/Eigen>

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif

class mmPlugin : public QObject, BaseInterface, ToolboxInterface, KeyInterface,
        LoggingInterface, LoadSaveInterface, MouseInterface, PickingInterface, RPCInterface
{
    Q_OBJECT
    Q_INTERFACES(BaseInterface)
    Q_INTERFACES(ToolboxInterface)
    Q_INTERFACES(LoggingInterface)
    Q_INTERFACES(MouseInterface)
    Q_INTERFACES(KeyInterface)
    Q_INTERFACES(PickingInterface)
    Q_INTERFACES(LoadSaveInterface)
    Q_INTERFACES(RPCInterface)

private:

//****CREATION OF FILE - DISCRETIZATION****
    // file of the created mesh
    PolyMesh* m_PickedMesh;
    // list of vertex handle in the mesh
    std::vector<PolyMesh::VertexHandle> m_vh0;
    // list of vertex handle for the face -> not sure need it at attribute
    std::vector<PolyMesh::VertexHandle> m_fphandles;
    // object id of the created mesh
    int m_IdObject;
    // size of the created mesh
    int m_sizeX;
    int m_sizeY;
    // parameters of the created mesh, updated after discretization
    int m_vertices;
    int m_edges;
    int m_faces;
    // masse of each vertex depending on the discretization
    double m_masse;
    // length of each edges depending on the discretization
    double m_length;
    // number of the in-between vertices added when discretizing
    int m_discretize;
    // ordered list of vertex of each halfedge to reconstruct the face after discretization
    OpenMesh::HPropHandleT<std::vector<PolyMesh::VertexHandle>> m_list_vertex;

//****SELECTION - FIX POINT - DRAGGED POINT****
    // list of ids of fixed vertex
    std::vector<int> m_idFixed;
    // list of position of fixed vertex
    std::vector<Vector> m_posFixed;
    // list of id of sphere for fixed vertex
    std::vector<int> m_idSphere;
    // position of the hit point when selecting a vertex
    OpenMesh::Vec3d m_hitPoint;
    // handles the old nd new position when selecting a vertex
    QPoint m_oldPos;
    QPoint m_newPos;
    // number of fixed points
    int m_FixPoint;
    // boolean for picking mode
    bool m_pickMode;
    // handles the dragging mode
    int m_dragMode;
    // index of the draged vertex
    int m_dragedVertex;
    // vertex iter of the draged vertex
    PolyMesh::VertexIter m_Draged;

//****OPTIMIZATION****
    // matrix of point to be passed to the solver
    ShapeOp::Matrix3X m_MV;
    // list of the two vertex belonging to each edges
    std::vector< std::vector<int> > m_EdgesCons;
    // list of halfedge handle of each edges to compute the vector
    std::vector<PolyMesh::HalfedgeHandle> m_VectorEdge;
    // list of the 3 vertex for each laplacian constraint
    std::vector< std::vector<int> > m_LaplaceCons;
    // value of the wind intensity
    double m_windIntensity;
    // vector of the wind direction
    Eigen::Vector3d m_windDirection;
    // file of the arrow mesh
    PolyMesh* m_Arrow;
    // object id of the arrow mesh
    int m_IdArrow;
    // transformation matrix of the arrow
    ACG::Matrix4x4d m_matrix;
    // value for the time for the animation
    int m_time;


//****INTERFACE****
    QSpinBox* sizeXSpin;
    QSpinBox* sizeYSpin;
    QSpinBox* discretizeSpin;
    QPushButton* loadButton;
    QSpinBox* fixPointSpin;
    QPushButton* pickButton;
    QPushButton* solveButton;
    QPushButton* dragButton;
    QSlider* windIntensitySlider;
    QLineEdit* windIntensityEdit;
    QDoubleSpinBox* windXBox;
    QDoubleSpinBox* windYBox;
    QDoubleSpinBox* windZBox;
    QCheckBox* arrowBox;
    QPushButton* animationButton;
    QSpinBox* timeSpin;

public:
    // BaseInterface
    QString name() { return (QString("Move Mesh Plugin by Juliette")); };
    QString description( ) { return (QString("Move vertex of a mesh and update all the other")); };

    int createNewObject();
    void discretizeLenght();
    void findSelectVertex_fixed();
    void findSelectVertex_draged();
    void getPointsInit();
    void solveShape(bool dynamic, int time, double time_step);
    void setNewPositions();
    int createArrow();

public slots:
    int addQuadrimesh();
    void changeXYValue();
    void pickVertex();
    void changeFixPointValue();
    void solveOptimazationInit();
    void solveOptimazation();
    void dragVertex();
    void changeWind();
    void animate();
    void changeTime();

private slots:
    // BaseInterface
    void initializePlugin();
    void pluginsInitialized();

    // MouseInterface
    void slotMouseEvent(QMouseEvent* _event);

signals:
    //BaseInterface
    void updatedObject(int _id, const UpdateType& _type);

    //LoggingInterface
    void log(Logtype _type, QString _message);
    void log(QString _message);

    //PickingInterface
    void addPickMode( const std::string& _mode);

    // ToolboxInterface
    void addToolbox( QString _name  , QWidget* _widget, QIcon* _icon );

    // LoadSaveInterface
    void addEmptyObject( DataType _type, int& _id);
    void save(int _id, QString _filename);

};

#endif //mmPlugin




