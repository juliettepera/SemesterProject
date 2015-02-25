#include "mmPlugin.hh"

void mmPlugin::initializePlugin()
{
   //**********************************************************************************************
   m_idNodePicked = -1;
   m_XPicked = 0.0;
   m_YPicked = 0.0;
   m_ZPicked = 0.0;

   //**********************************************************************************************
   QWidget* toolBox = new QWidget();
   
   QPushButton* pickButton = new QPushButton("&Pick",toolBox);

   QLabel* fileLabel = new QLabel(tr("File : "));
   QLabel* coordinateLabel = new QLabel(tr("Coordinates picked : "));

   filePicked = new QLineEdit("-",toolBox);
   XPicked = new QLineEdit("-",toolBox);
   YPicked = new QLineEdit("-",toolBox);
   ZPicked = new QLineEdit("-",toolBox);
    
   QGridLayout* layout = new QGridLayout(toolBox);

   layout->addWidget( pickButton , 1, 1);
   layout->addWidget( fileLabel , 2 , 1);
   layout->addWidget( filePicked , 2, 2);
   layout->addWidget(coordinateLabel , 3 , 1);
   layout->addWidget( XPicked , 3, 2);
   layout->addWidget( YPicked , 3, 3);
   layout->addWidget( ZPicked , 3, 4);

   layout->addItem(new QSpacerItem(10,10,QSizePolicy::Expanding,QSizePolicy::Expanding),2,0,1,2);

   //**********************************************************************************************
   connect( pickButton, SIGNAL(clicked()), this, SLOT(pickVertex()));
   emit addToolbox( tr("MoveMesh") , toolBox );
}

void mmPlugin::pluginsInitialized()
{
    emit addPickMode("MyPickMode");
}

void mmPlugin::updateLineEdit()
{
    if(m_idNodePicked != -1 )
    {
        XPicked->setText(QString::number(m_XPicked));
        YPicked->setText(QString::number(m_YPicked));
        ZPicked->setText(QString::number(m_ZPicked));
        filePicked->setText(m_ObjectPicked->filename());
    }
    else
    {
        XPicked->setText("-");
        YPicked->setText("-");
        ZPicked->setText("-");
        filePicked->setText("No file selected");
    }
}

void mmPlugin::workWithMesh()
{
    for (PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS); o_it != PluginFunctions::objectsEnd();++o_it)
      {
        if(o_it.index()==m_ObjectPicked)
        {
            if(o_it->dataType(DATA_TRIANGLE_MESH))
            {
                std::cout << "triangle mesh" << std::endl;
                TriMesh* pickedMesh = PluginFunctions::triMesh(*o_it);
            }
            else if(o_it->dataType(DATA_POLY_MESH))
            {
                std::cout << "poly mesh" << std::endl;
                PolyMesh* pickedMesh = PluginFunctions::polyMesh(*o_it);
            }
            else
            {
                std::cout << "nope" << std::endl;
            }
        }
    }
}

void mmPlugin::pickVertex()
{
    PluginFunctions::actionMode(Viewer::PickingMode);
    PluginFunctions::pickMode("MyPickMode");
}

void mmPlugin::slotMouseEvent(QMouseEvent* _event)
{
    if ( PluginFunctions::pickMode() == "MyPickMode" && PluginFunctions::actionMode() == Viewer::PickingMode )
    {
        if (_event->type() == QEvent::MouseButtonPress)
        {
            unsigned int node_idx, target_idx;
            OpenMesh::Vec3d hitPoint;

            // Get picked object's identifier
            if ( PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_ANYTHING,_event->pos(), node_idx, target_idx, &hitPoint) )
            {               
               m_XPicked = hitPoint.data()[0];
               m_YPicked = hitPoint.data()[1];
               m_ZPicked = hitPoint.data()[2];
               m_idNodePicked = node_idx;

               // Get picked object
               if ( ! PluginFunctions::getPickedObject(m_idNodePicked, m_ObjectPicked) )
               {
                    emit log(LOGINFO, "Picking failed");
               }
            }
            else
            {
               m_idNodePicked = -1;
            }
       }
     }
     PluginFunctions::actionMode(Viewer::ExamineMode);
     updateLineEdit();
     workWithMesh();
}

Q_EXPORT_PLUGIN2( movemeshplugin , mmPlugin );
