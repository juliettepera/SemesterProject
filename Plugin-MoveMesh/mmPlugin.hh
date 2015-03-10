#ifndef MMPLUGIN_HH_INCLUDED
#define MMPLUGIN_HH_INCLUDED

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/ToolboxInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/MouseInterface.hh>
#include <OpenFlipper/BasePlugin/PickingInterface.hh>
#include "OpenFlipper/BasePlugin/PluginFunctions.hh"
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>
#include <OpenFlipper/common/Types.hh>

#include <ObjectTypes/MeshObject/MeshObjectT.hh>
#include <ObjectTypes/PolyMesh/PolyMesh.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/AttribKernelT.hh>
#include <OpenMesh/Core/IO/MeshIO.hh>

#include "cmath"

typedef OpenMesh::PolyMesh_ArrayKernelT<>  MyMesh;

class mmPlugin : public QObject, BaseInterface, ToolboxInterface, LoggingInterface, MouseInterface, PickingInterface, LoadSaveInterface
{
  Q_OBJECT
  Q_INTERFACES(BaseInterface)
  Q_INTERFACES(ToolboxInterface)
  Q_INTERFACES(LoggingInterface)
  Q_INTERFACES(MouseInterface)
  Q_INTERFACES(PickingInterface)
  Q_INTERFACES(LoadSaveInterface)

  private:
       int m_idNodePicked;
       OpenMesh::Vec3d m_hitPoint;
       BaseObjectData* m_ObjectPicked;
       std::vector<PolyMesh::VertexIter> m_vFixed;
       PolyMesh* m_PickedMesh;
       int m_discretize;

  public:
        // BaseInterface
        QString name() { return (QString("Move Mesh Plugin by Juliette")); };
        QString description( ) { return (QString("Move vertex of a mesh and update all the other")); };

        // MoveMeshInterface
        int createExampleMesh();
        void findSelectVertex();

   public slots:
        // MoveMeshInterface
        void loadMesh();
        void pickVertex();
        void showFixedPoints();
        void discretizeLenght();

   private slots:
        // BaseInterface
        void initializePlugin();
        void pluginsInitialized();

        // MouseInterface
        void slotMouseEvent(QMouseEvent* _event);

  signals:
        //BaseInterface
        void updateView();
        void updatedObject(int _id);

        //LoggingInterface
        void log(Logtype _type, QString _message);
        void log(QString _message);

        //PickingInterface
        void addPickMode( const std::string& _mode);

        // ToolboxInterface
        void addToolbox( QString _name  , QWidget* _widget);

};

#endif //mmPlugin




