#include "mmPlugin.hh"

void mmPlugin::initializePlugin()
{
   //**********************************************************************************************
   m_idNodePicked = -1;
   m_hitPoint = OpenMesh::Vec3d(0,0,0);
   m_ObjectPicked = -1 ;
   m_vFixed.clear();

   //**********************************************************************************************
   QWidget* toolBox = new QWidget();

   QPushButton* loadButton = new QPushButton("&Load",toolBox);
   QPushButton* pickButton = new QPushButton("&Pick",toolBox);
   QPushButton* fixButton = new QPushButton("&Fixed",toolBox);
   QPushButton* discretButton = QPushButton("&Discretize",toolBox);
    
   QGridLayout* layout = new QGridLayout(toolBox);

   layout->addWidget( loadButton , 1, 1);
   layout->addWidget( pickButton , 2, 1);
   layout->addWidget( fixButton , 3, 1);
   layout->addWidget( discretButton , 4, 1);

   layout->addItem(new QSpacerItem(10,10,QSizePolicy::Expanding,QSizePolicy::Expanding),2,0,1,2);

   //**********************************************************************************************
   connect( loadButton, SIGNAL(clicked()), this, SLOT(loadMesh()));
   connect( pickButton, SIGNAL(clicked()), this, SLOT(pickVertex()));
   connect( fixButton, SIGNAL(clicked()), this, SLOT(showFixedPoints()));
   connect( discretButton, SIGNAL(clicked()), this, SLOT(discretizeLenght()));
   emit addToolbox( tr("MoveMesh") , toolBox );
}

//**********************************************************************************************
void mmPlugin::pluginsInitialized()
{
    emit addPickMode("MyPickMode");
}

//**********************************************************************************************
int mmPlugin::createExampleMesh()
{
  MyMesh mesh;

  // generate vertices
  MyMesh::VertexHandle vhandle[31];
  vhandle[0] = mesh.add_vertex(MyMesh::Point(3,-3, 0));
  vhandle[1] = mesh.add_vertex(MyMesh::Point(3,-1, 0));
  vhandle[2] = mesh.add_vertex(MyMesh::Point(3, 1, 0));
  vhandle[3] = mesh.add_vertex(MyMesh::Point(3, 3, 0));

  vhandle[4] = mesh.add_vertex(MyMesh::Point(2,-4, 0));
  vhandle[5] = mesh.add_vertex(MyMesh::Point(2,-2, 0));
  vhandle[6] = mesh.add_vertex(MyMesh::Point(2, 0, 0));
  vhandle[7] = mesh.add_vertex(MyMesh::Point(2, 2, 0));
  vhandle[8] = mesh.add_vertex(MyMesh::Point(2, 4, 0));

  vhandle[9] = mesh.add_vertex(MyMesh::Point(1,-3, 0));
  vhandle[10] = mesh.add_vertex(MyMesh::Point(1,-1, 0));
  vhandle[11] = mesh.add_vertex(MyMesh::Point(1, 1, 0));
  vhandle[12] = mesh.add_vertex(MyMesh::Point(1, 3, 0));

  vhandle[13] = mesh.add_vertex(MyMesh::Point(0,-4, 0));
  vhandle[14] = mesh.add_vertex(MyMesh::Point(0,-2, 0));
  vhandle[15] = mesh.add_vertex(MyMesh::Point(0, 0, 0));
  vhandle[16] = mesh.add_vertex(MyMesh::Point(0, 2, 0));
  vhandle[17] = mesh.add_vertex(MyMesh::Point(0, 4, 0));

  vhandle[18] = mesh.add_vertex(MyMesh::Point(-1,-3, 0));
  vhandle[19] = mesh.add_vertex(MyMesh::Point(-1,-1, 0));
  vhandle[20] = mesh.add_vertex(MyMesh::Point(-1, 1, 0));
  vhandle[21] = mesh.add_vertex(MyMesh::Point(-1, 3, 0));

  vhandle[22] = mesh.add_vertex(MyMesh::Point(-2,-4, 0));
  vhandle[23] = mesh.add_vertex(MyMesh::Point(-2,-2, 0));
  vhandle[24] = mesh.add_vertex(MyMesh::Point(-2, 0, 0));
  vhandle[25] = mesh.add_vertex(MyMesh::Point(-2, 2, 0));
  vhandle[26] = mesh.add_vertex(MyMesh::Point(-2, 4, 0));

  vhandle[27] = mesh.add_vertex(MyMesh::Point(-3,-3, 0));
  vhandle[28] = mesh.add_vertex(MyMesh::Point(-3,-1, 0));
  vhandle[29] = mesh.add_vertex(MyMesh::Point(-3, 1, 0));
  vhandle[30] = mesh.add_vertex(MyMesh::Point(-3, 3, 0));

  std::vector<MyMesh::VertexHandle>  face_vhandles;
  face_vhandles.clear();
  face_vhandles.push_back(vhandle[0]);
  face_vhandles.push_back(vhandle[4]);
  face_vhandles.push_back(vhandle[9]);
  face_vhandles.push_back(vhandle[5]);
  mesh.add_face(face_vhandles);

  face_vhandles.clear();
  face_vhandles.push_back(vhandle[1]);
  face_vhandles.push_back(vhandle[5]);
  face_vhandles.push_back(vhandle[10]);
  face_vhandles.push_back(vhandle[6]);
  mesh.add_face(face_vhandles);

  face_vhandles.clear();
  face_vhandles.push_back(vhandle[2]);
  face_vhandles.push_back(vhandle[6]);
  face_vhandles.push_back(vhandle[11]);
  face_vhandles.push_back(vhandle[7]);
  mesh.add_face(face_vhandles);

  face_vhandles.clear();
  face_vhandles.push_back(vhandle[3]);
  face_vhandles.push_back(vhandle[7]);
  face_vhandles.push_back(vhandle[12]);
  face_vhandles.push_back(vhandle[8]);
  mesh.add_face(face_vhandles);

  face_vhandles.clear();
  face_vhandles.push_back(vhandle[5]);
  face_vhandles.push_back(vhandle[9]);
  face_vhandles.push_back(vhandle[14]);
  face_vhandles.push_back(vhandle[10]);
  mesh.add_face(face_vhandles);

  face_vhandles.clear();
  face_vhandles.push_back(vhandle[6]);
  face_vhandles.push_back(vhandle[10]);
  face_vhandles.push_back(vhandle[15]);
  face_vhandles.push_back(vhandle[11]);
  mesh.add_face(face_vhandles);

  face_vhandles.clear();
  face_vhandles.push_back(vhandle[7]);
  face_vhandles.push_back(vhandle[11]);
  face_vhandles.push_back(vhandle[16]);
  face_vhandles.push_back(vhandle[12]);
  mesh.add_face(face_vhandles);

  face_vhandles.clear();
  face_vhandles.push_back(vhandle[9]);
  face_vhandles.push_back(vhandle[13]);
  face_vhandles.push_back(vhandle[18]);
  face_vhandles.push_back(vhandle[14]);
  mesh.add_face(face_vhandles);

  face_vhandles.clear();
  face_vhandles.push_back(vhandle[10]);
  face_vhandles.push_back(vhandle[14]);
  face_vhandles.push_back(vhandle[19]);
  face_vhandles.push_back(vhandle[15]);
  mesh.add_face(face_vhandles);

  face_vhandles.clear();
  face_vhandles.push_back(vhandle[11]);
  face_vhandles.push_back(vhandle[15]);
  face_vhandles.push_back(vhandle[20]);
  face_vhandles.push_back(vhandle[16]);
  mesh.add_face(face_vhandles);

  face_vhandles.clear();
  face_vhandles.push_back(vhandle[12]);
  face_vhandles.push_back(vhandle[16]);
  face_vhandles.push_back(vhandle[21]);
  face_vhandles.push_back(vhandle[17]);
  mesh.add_face(face_vhandles);

  face_vhandles.clear();
  face_vhandles.push_back(vhandle[14]);
  face_vhandles.push_back(vhandle[18]);
  face_vhandles.push_back(vhandle[23]);
  face_vhandles.push_back(vhandle[19]);
  mesh.add_face(face_vhandles);

  face_vhandles.clear();
  face_vhandles.push_back(vhandle[15]);
  face_vhandles.push_back(vhandle[19]);
  face_vhandles.push_back(vhandle[24]);
  face_vhandles.push_back(vhandle[20]);
  mesh.add_face(face_vhandles);

  face_vhandles.clear();
  face_vhandles.push_back(vhandle[16]);
  face_vhandles.push_back(vhandle[20]);
  face_vhandles.push_back(vhandle[25]);
  face_vhandles.push_back(vhandle[21]);
  mesh.add_face(face_vhandles);

  face_vhandles.clear();
  face_vhandles.push_back(vhandle[18]);
  face_vhandles.push_back(vhandle[22]);
  face_vhandles.push_back(vhandle[27]);
  face_vhandles.push_back(vhandle[23]);
  mesh.add_face(face_vhandles);

  face_vhandles.clear();
  face_vhandles.push_back(vhandle[19]);
  face_vhandles.push_back(vhandle[23]);
  face_vhandles.push_back(vhandle[28]);
  face_vhandles.push_back(vhandle[24]);
  mesh.add_face(face_vhandles);

  face_vhandles.clear();
  face_vhandles.push_back(vhandle[20]);
  face_vhandles.push_back(vhandle[24]);
  face_vhandles.push_back(vhandle[29]);
  face_vhandles.push_back(vhandle[25]);
  mesh.add_face(face_vhandles);

  face_vhandles.clear();
  face_vhandles.push_back(vhandle[21]);
  face_vhandles.push_back(vhandle[25]);
  face_vhandles.push_back(vhandle[30]);
  face_vhandles.push_back(vhandle[26]);
  mesh.add_face(face_vhandles);

  // write mesh to output.obj
  try
  {
    if ( !OpenMesh::IO::write_mesh(mesh, "/Users/Juju/Documents/project/QuadriMesh.off") )
    {
      std::cerr << "Cannot write mesh to file 'QuadriMesh.off'" << std::endl;
      return 1;
    }
  }
  catch( std::exception& x )
  {
    std::cerr << x.what() << std::endl;
    return 1;
  }
  return 0;
}

void mmPlugin::loadMesh()
{
    if( createExampleMesh() != 0 )
    {
        emit log(LOGINFO, "Creating failed");
    }

    //load("/Users/Juju/Documents/project/QuadriMesh.off",DATA_POLY_MESH,m_idLoadedMesh);
}

//**********************************************************************************************
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
               m_hitPoint = hitPoint;
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
     findSelectVertex();
}

void mmPlugin::findSelectVertex()
{
      for (PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS); o_it != PluginFunctions::objectsEnd();++o_it)
      {
        if(o_it.index()==m_ObjectPicked)
        {
            if(o_it->dataType(DATA_POLY_MESH))
            {
                PolyMesh* pickedMesh = PluginFunctions::polyMesh(*o_it);

                PolyMesh::VertexIter v_it;
                PolyMesh::VertexIter v_end = pickedMesh->vertices_end();

                OpenMesh::Vec3d ActualPoint;

                for (v_it = pickedMesh->vertices_begin(); v_it != v_end; ++v_it)
                {

                    ActualPoint = pickedMesh->point( *v_it );
                    double X = fabs(double(ActualPoint[0])-double(m_hitPoint[0]));
                    double Y = fabs(double(ActualPoint[1])-double(m_hitPoint[1]));

                    if( (X<=0.25) && (Y<=0.25) )
                    {
                        m_vFixed.push_back(v_it);
                        std::cout << "fixing point #" << v_it << std::endl;
                    }
                }
            }
            else
            {
                 emit log(LOGINFO, "Not a quadri mesh");
            }
        }
    }
}

//**********************************************************************************************
void mmPlugin::showFixedPoints()
{
      for (PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS); o_it != PluginFunctions::objectsEnd();++o_it)
      {
        if(o_it.index()==m_ObjectPicked)
        {
            if(o_it->dataType(DATA_POLY_MESH))
            {
                PolyMesh* pickedMesh = PluginFunctions::polyMesh(*o_it);

                PolyMesh::VertexIter v_it;
                PolyMesh::VertexIter v_end = pickedMesh->vertices_end();

                for (v_it = pickedMesh->vertices_begin(); v_it != v_end; ++v_it)
                {
                    for(int i=0 ; i < m_vFixed.size() ; i++)
                    {
                        if( v_it == m_vFixed.at(i) )
                        {
                            std::cout << "coloring point #" << v_it << std::endl;
                            pickedMesh->set_color(*v_it, PolyMesh::Color(255,0,0,0.3) );
                        }
                    }
                }
             }
         }
       }
}

//**********************************************************************************************
void mmPlugin::discretizeLenght()
{

}

Q_EXPORT_PLUGIN2( movemeshplugin , mmPlugin );
