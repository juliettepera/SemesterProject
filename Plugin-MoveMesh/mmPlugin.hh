#ifndef MMPLUGIN_HH_INCLUDED
#define MMPLUGIN_HH_INCLUDED

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/ToolboxInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/MouseInterface.hh>
#include <OpenFlipper/BasePlugin/PickingInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>
#include "OpenFlipper/BasePlugin/PluginFunctions.hh"

#include <ObjectTypes/PolyMesh/PolyMesh.hh>

#include "solvec.h"
#include "cmath"

//typedef OpenMesh::PolyMesh_ArrayKernelT<>  MyMesh;

class mmPlugin : public QObject, BaseInterface, ToolboxInterface,
                        LoggingInterface, LoadSaveInterface, MouseInterface, PickingInterface
{
  Q_OBJECT
  Q_INTERFACES(BaseInterface)
  Q_INTERFACES(ToolboxInterface)
  Q_INTERFACES(LoggingInterface)
  Q_INTERFACES(MouseInterface)
  Q_INTERFACES(PickingInterface)
  Q_INTERFACES(LoadSaveInterface)

  private:

       PolyMesh* m_PickedMesh;

       std::vector<PolyMesh::VertexHandle> m_vphandles;
       std::vector<PolyMesh::VertexHandle> m_fphandles;
       std::vector<PolyMesh::VertexIter> m_vFixed;

       OpenMesh::Vec3d m_hitPoint;

       int m_IdObject;

       int m_FixPoint;
       int m_discretize;

       solvec m_mySolver;

       QPushButton* loadButton;
       QSpinBox* fixPointSpin;
       QPushButton* pickButton;
       QSpinBox* discretizeSpin;
       QPushButton* discretButton;
       QPushButton* solveButton;

  public:
        // BaseInterface
        QString name() { return (QString("Move Mesh Plugin by Juliette")); };
        QString description( ) { return (QString("Move vertex of a mesh and update all the other")); };

       // MoveMesh
       int createNewObject();
       void findSelectVertex();
       void showFixedPoints();

   public slots:
        int addQuadrimesh() ;
        void pickVertex();
        void changeFixPointValue();
        void discretizeLenght();
        void changeDiscretizeValue();
        int solveOptimazation();

   private slots:   
        // BaseInterface
        void initializePlugin();
        void pluginsInitialized();

        // MouseInterface
        void slotMouseEvent(QMouseEvent* _event);

   signals:
        //BaseInterface
        //void updateView();
        void updatedObject(int _id, const UpdateType& _type);

        //LoggingInterface
        void log(Logtype _type, QString _message);
        void log(QString _message);

        //PickingInterface
        void addPickMode( const std::string& _mode);

        // ToolboxInterface
        void addToolbox( QString _name  , QWidget* _widget);

        // LoadSaveInterface
        void addEmptyObject( DataType _type, int& _id);
        void save(int _id, QString _filename);

};

#endif //mmPlugin




