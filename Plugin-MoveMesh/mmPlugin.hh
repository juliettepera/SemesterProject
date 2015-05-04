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

// ShapeOp Libraries
#include <Constraint.h>
#include <Solver.h>
#include <Force.h>

#include <cmath>

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

       PolyMesh* m_PickedMesh;

       std::vector<PolyMesh::VertexHandle> m_vh0;
       std::vector<PolyMesh::VertexHandle> m_vh1;
       std::vector<PolyMesh::VertexHandle> m_fphandles;

       std::vector<int> m_idFixed;
       std::vector<Vector> m_posFixed;

       OpenMesh::Vec3d m_hitPoint;

       ShapeOp::MatrixXX m_ME;
       ShapeOp::Matrix3X m_MV;
       ShapeOp::MatrixXX m_ML;

       OpenMesh::HPropHandleT<std::vector<PolyMesh::VertexHandle>> m_list_vertex;
       int m_IdObject;

       int m_FixPoint;
       int m_discretize;
       int m_sizeX;
       int m_sizeY;
       int m_vertices;
       int m_edges;
       int m_faces;

       int m_nbL;

       bool m_pickMode;
       int m_dragMode;

       int m_dragedVertex;
       PolyMesh::VertexIter m_Draged;

       QPoint m_oldPos;
       QPoint m_newPos;

       QSpinBox* sizeXSpin;
       QSpinBox* sizeYSpin;
       QSpinBox* discretizeSpin;

       QPushButton* loadButton;

       QSpinBox* fixPointSpin;
       QPushButton* pickButton;

       QPushButton* solveButton;
       QPushButton* dragButton;

  public:
        // BaseInterface
        QString name() { return (QString("Move Mesh Plugin by Juliette")); };
        QString description( ) { return (QString("Move vertex of a mesh and update all the other")); };

        // MoveMesh
        int createNewObject();
        void discretizeLenght();

        void findSelectVertex_fixed();
        void findSelectVertex_draged();

       //Solve
       void getPoints();
       void solveShape();
       void setNewPositions();


   public slots:
        int addQuadrimesh();
        void changeXYValue();

        void pickVertex();
        void changeFixPointValue();

        void solveOptimazation();
        void dragVertex();

   private slots:   
        // BaseInterface
        void initializePlugin();
        void pluginsInitialized();
        void slotAllCleared();


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




