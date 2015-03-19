#include "mmPlugin.hh"

void mmPlugin::initializePlugin()
{
   //**********************************************************************************************
   m_IdObject = -1;
   m_FixPoint = 4;
   m_discretize = 3;
   m_vFixed.clear();
   m_hitPoint = OpenMesh::Vec3d(0,0,0);

   //**********************************************************************************************
   QWidget* toolBox = new QWidget();

   loadButton = new QPushButton("&Load",toolBox);
   loadButton->setDisabled(true);

   pickButton = new QPushButton("&Pick",toolBox);
   pickButton->setDisabled(true);

   discretButton = new QPushButton("&Discretize",toolBox);
   discretButton->setDisabled(true);

   solveButton = new QPushButton("&Solve",toolBox);
   solveButton->setDisabled(true);

   fixPointSpin = new QSpinBox(toolBox);
   fixPointSpin->setMinimum(1);
   fixPointSpin->setMaximum(10);
   fixPointSpin->setValue(4);
   fixPointSpin->setDisabled(true);

   discretizeSpin = new QSpinBox(toolBox);
   discretizeSpin->setMinimum(1);
   discretizeSpin->setMaximum(5);
   discretizeSpin->setValue(3);
   discretizeSpin->setDisabled(true);
    
   QGridLayout* layout = new QGridLayout(toolBox);

   layout->addWidget( loadButton, 1, 1);
   layout->addWidget( fixPointSpin, 2, 1);
   layout->addWidget( pickButton, 2, 2);
   layout->addWidget( discretizeSpin, 3, 1);
   layout->addWidget( discretButton , 3, 2);
   layout->addWidget( solveButton , 4, 1);

   layout->addItem(new QSpacerItem(10,10,QSizePolicy::Expanding,QSizePolicy::Expanding),2,0,1,2);

   //**********************************************************************************************
   connect( loadButton, SIGNAL(clicked()), this, SLOT(addQuadrimesh()));
   connect( pickButton, SIGNAL(clicked()), this, SLOT(pickVertex()));
   connect( fixPointSpin , SIGNAL(valueChanged(int)) , this, SLOT(changeFixPointValue()));
   connect( discretButton, SIGNAL(clicked()), this, SLOT(discretizeLenght()));
   connect( discretizeSpin , SIGNAL(valueChanged(int)) , this, SLOT(changeDiscretizeValue()));
   connect( solveButton, SIGNAL(clicked()), this, SLOT(solveOptimazation()));

   emit addToolbox( tr("MoveMesh") , toolBox );
}

//**********************************************************************************************
void mmPlugin::pluginsInitialized()
{
    emit log(LOGWARN,"Move Mesh Initialized");
    emit addPickMode("MyPickMode");
    loadButton->setEnabled(true);
}

//**********************************************************************************************
// Create a new quad mesh composed of X vertices, then save it and load it to the viewer
// Code inspired from the PrimitivesGenerator plugin
//**********************************************************************************************
int mmPlugin::createNewObject()
{
  int objectId = -1;

  emit addEmptyObject( DATA_POLY_MESH, objectId );

  PolyMeshObject* object;
  if ( !PluginFunctions::getObject(objectId,object) )
  {
    emit log(LOGERR,"Unable to create new Object1");
    return -1;
  }

  return objectId;
}

int mmPlugin::addQuadrimesh()
{
    std::cout << "Hello" << std::endl;

  int newObject = createNewObject();

  PolyMeshObject* object;
  if ( !PluginFunctions::getObject(newObject,object) )
  {
    emit log(LOGERR,"Unable to create new Object2");
    return -1;
  }
  else
  {
    object->setName( "Quadrimesh " + QString::number(newObject) );

    m_PickedMesh =  object->mesh();

    m_PickedMesh->clear();

    // Add 31 vertices
    m_vphandles.resize(31);
    m_vphandles[0] = m_PickedMesh->add_vertex(PolyMesh::Point(3,-3, 0));
    m_vphandles[1] = m_PickedMesh->add_vertex(PolyMesh::Point(3,-1, 0));
    m_vphandles[2] = m_PickedMesh->add_vertex(PolyMesh::Point(3, 1, 0));
    m_vphandles[3] = m_PickedMesh->add_vertex(PolyMesh::Point(3, 3, 0));

    m_vphandles[4] = m_PickedMesh->add_vertex(PolyMesh::Point(2,-4, 0));
    m_vphandles[5] = m_PickedMesh->add_vertex(PolyMesh::Point(2,-2, 0));
    m_vphandles[6] = m_PickedMesh->add_vertex(PolyMesh::Point(2, 0, 0));
    m_vphandles[7] = m_PickedMesh->add_vertex(PolyMesh::Point(2, 2, 0));
    m_vphandles[8] = m_PickedMesh->add_vertex(PolyMesh::Point(2, 4, 0));

    m_vphandles[9] = m_PickedMesh->add_vertex(PolyMesh::Point(1,-3, 0));
    m_vphandles[10] = m_PickedMesh->add_vertex(PolyMesh::Point(1,-1, 0));
    m_vphandles[11] = m_PickedMesh->add_vertex(PolyMesh::Point(1, 1, 0));
    m_vphandles[12] = m_PickedMesh->add_vertex(PolyMesh::Point(1, 3, 0));

    m_vphandles[13] = m_PickedMesh->add_vertex(PolyMesh::Point(0,-4, 0));
    m_vphandles[14] = m_PickedMesh->add_vertex(PolyMesh::Point(0,-2, 0));
    m_vphandles[15] = m_PickedMesh->add_vertex(PolyMesh::Point(0, 0, 0));
    m_vphandles[16] = m_PickedMesh->add_vertex(PolyMesh::Point(0, 2, 0));
    m_vphandles[17] = m_PickedMesh->add_vertex(PolyMesh::Point(0, 4, 0));

    m_vphandles[18] = m_PickedMesh->add_vertex(PolyMesh::Point(-1,-3, 0));
    m_vphandles[19] = m_PickedMesh->add_vertex(PolyMesh::Point(-1,-1, 0));
    m_vphandles[20] = m_PickedMesh->add_vertex(PolyMesh::Point(-1, 1, 0));
    m_vphandles[21] = m_PickedMesh->add_vertex(PolyMesh::Point(-1, 3, 0));

    m_vphandles[22] = m_PickedMesh->add_vertex(PolyMesh::Point(-2,-4, 0));
    m_vphandles[23] = m_PickedMesh->add_vertex(PolyMesh::Point(-2,-2, 0));
    m_vphandles[24] = m_PickedMesh->add_vertex(PolyMesh::Point(-2, 0, 0));
    m_vphandles[25] = m_PickedMesh->add_vertex(PolyMesh::Point(-2, 2, 0));
    m_vphandles[26] = m_PickedMesh->add_vertex(PolyMesh::Point(-2, 4, 0));

    m_vphandles[27] = m_PickedMesh->add_vertex(PolyMesh::Point(-3,-3, 0));
    m_vphandles[28] = m_PickedMesh->add_vertex(PolyMesh::Point(-3,-1, 0));
    m_vphandles[29] = m_PickedMesh->add_vertex(PolyMesh::Point(-3, 1, 0));
    m_vphandles[30] = m_PickedMesh->add_vertex(PolyMesh::Point(-3, 3, 0));

    // color the points
    PolyMesh::VertexIter v_it;
    PolyMesh::VertexIter v_end = m_PickedMesh->vertices_end();
    for (v_it = m_PickedMesh->vertices_begin(); v_it != v_end; ++v_it)
    {
        m_PickedMesh->set_color(*v_it, PolyMesh::Color(0,0,0,0) );
    }

    // Add faces
    m_fphandles.push_back(m_vphandles[0]);
    m_fphandles.push_back(m_vphandles[4]);
    m_fphandles.push_back(m_vphandles[9]);
    m_fphandles.push_back(m_vphandles[5]);
    m_PickedMesh->add_face(m_fphandles);

    m_fphandles.clear();
    m_fphandles.push_back(m_vphandles[1]);
    m_fphandles.push_back(m_vphandles[5]);
    m_fphandles.push_back(m_vphandles[10]);
    m_fphandles.push_back(m_vphandles[6]);
    m_PickedMesh->add_face(m_fphandles);

    m_fphandles.clear();
    m_fphandles.push_back(m_vphandles[2]);
    m_fphandles.push_back(m_vphandles[6]);
    m_fphandles.push_back(m_vphandles[11]);
    m_fphandles.push_back(m_vphandles[7]);
    m_PickedMesh->add_face(m_fphandles);

    m_fphandles.clear();
    m_fphandles.push_back(m_vphandles[3]);
    m_fphandles.push_back(m_vphandles[7]);
    m_fphandles.push_back(m_vphandles[12]);
    m_fphandles.push_back(m_vphandles[8]);
    m_PickedMesh->add_face(m_fphandles);

    m_fphandles.clear();
    m_fphandles.push_back(m_vphandles[5]);
    m_fphandles.push_back(m_vphandles[9]);
    m_fphandles.push_back(m_vphandles[14]);
    m_fphandles.push_back(m_vphandles[10]);
    m_PickedMesh->add_face(m_fphandles);

    m_fphandles.clear();
    m_fphandles.push_back(m_vphandles[6]);
    m_fphandles.push_back(m_vphandles[10]);
    m_fphandles.push_back(m_vphandles[15]);
    m_fphandles.push_back(m_vphandles[11]);
    m_PickedMesh->add_face(m_fphandles);

    m_fphandles.clear();
    m_fphandles.push_back(m_vphandles[7]);
    m_fphandles.push_back(m_vphandles[11]);
    m_fphandles.push_back(m_vphandles[16]);
    m_fphandles.push_back(m_vphandles[12]);
    m_PickedMesh->add_face(m_fphandles);

    m_fphandles.clear();
    m_fphandles.push_back(m_vphandles[9]);
    m_fphandles.push_back(m_vphandles[13]);
    m_fphandles.push_back(m_vphandles[18]);
    m_fphandles.push_back(m_vphandles[14]);
    m_PickedMesh->add_face(m_fphandles);

    m_fphandles.clear();
    m_fphandles.push_back(m_vphandles[10]);
    m_fphandles.push_back(m_vphandles[14]);
    m_fphandles.push_back(m_vphandles[19]);
    m_fphandles.push_back(m_vphandles[15]);
    m_PickedMesh->add_face(m_fphandles);

    m_fphandles.clear();
    m_fphandles.push_back(m_vphandles[11]);
    m_fphandles.push_back(m_vphandles[15]);
    m_fphandles.push_back(m_vphandles[20]);
    m_fphandles.push_back(m_vphandles[16]);
    m_PickedMesh->add_face(m_fphandles);

    m_fphandles.clear();
    m_fphandles.push_back(m_vphandles[12]);
    m_fphandles.push_back(m_vphandles[16]);
    m_fphandles.push_back(m_vphandles[21]);
    m_fphandles.push_back(m_vphandles[17]);
    m_PickedMesh->add_face(m_fphandles);

    m_fphandles.clear();
    m_fphandles.push_back(m_vphandles[14]);
    m_fphandles.push_back(m_vphandles[18]);
    m_fphandles.push_back(m_vphandles[23]);
    m_fphandles.push_back(m_vphandles[19]);
    m_PickedMesh->add_face(m_fphandles);

    m_fphandles.clear();
    m_fphandles.push_back(m_vphandles[15]);
    m_fphandles.push_back(m_vphandles[19]);
    m_fphandles.push_back(m_vphandles[24]);
    m_fphandles.push_back(m_vphandles[20]);
    m_PickedMesh->add_face(m_fphandles);

    m_fphandles.clear();
    m_fphandles.push_back(m_vphandles[16]);
    m_fphandles.push_back(m_vphandles[20]);
    m_fphandles.push_back(m_vphandles[25]);
    m_fphandles.push_back(m_vphandles[21]);
    m_PickedMesh->add_face(m_fphandles);

    m_fphandles.clear();
    m_fphandles.push_back(m_vphandles[18]);
    m_fphandles.push_back(m_vphandles[22]);
    m_fphandles.push_back(m_vphandles[27]);
    m_fphandles.push_back(m_vphandles[23]);
    m_PickedMesh->add_face(m_fphandles);

    m_fphandles.clear();
    m_fphandles.push_back(m_vphandles[19]);
    m_fphandles.push_back(m_vphandles[23]);
    m_fphandles.push_back(m_vphandles[28]);
    m_fphandles.push_back(m_vphandles[24]);
    m_PickedMesh->add_face(m_fphandles);

    m_fphandles.clear();
    m_fphandles.push_back(m_vphandles[20]);
    m_fphandles.push_back(m_vphandles[24]);
    m_fphandles.push_back(m_vphandles[29]);
    m_fphandles.push_back(m_vphandles[25]);
    m_PickedMesh->add_face(m_fphandles);

    m_fphandles.clear();
    m_fphandles.push_back(m_vphandles[21]);
    m_fphandles.push_back(m_vphandles[25]);
    m_fphandles.push_back(m_vphandles[30]);
    m_fphandles.push_back(m_vphandles[26]);
    m_PickedMesh->add_face(m_fphandles);

    m_PickedMesh->update_normals();

    m_IdObject = newObject;
    save(m_IdObject,"/Users/Juju/Documents/project/files/Quadmesh.ply");

    emit updatedObject(newObject,UPDATE_ALL);

    PluginFunctions::viewAll();

    fixPointSpin->setEnabled(true);
    pickButton->setEnabled(true);
    loadButton->setDisabled(true);

    return newObject;
  }

  return -1;
}

//**********************************************************************************************
// Select the number of fix points we want, then allow th selection with the pick button
// We add a sphere on each fix vertex, using the PrimitivesGenerator plugin
//**********************************************************************************************
void mmPlugin::changeFixPointValue()
{
    m_FixPoint = fixPointSpin->value();
}

void mmPlugin::pickVertex()
{
     fixPointSpin->setDisabled(true);

     if( m_vFixed.size() < m_FixPoint )
     {
        PluginFunctions::actionMode(Viewer::PickingMode);
        PluginFunctions::pickMode("MyPickMode");
     }
     else
     {
        std::cout << "no more fix points possible" << std::endl;
        pickButton->setDisabled(true);
        discretButton->setEnabled(true);
        solveButton->setEnabled(true);
     }
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
               findSelectVertex();
            }
            else
            {
               std::cout << "not an object " << std::endl;
            }
       }
     }
     PluginFunctions::actionMode(Viewer::ExamineMode);

}

void mmPlugin::findSelectVertex()
{
     PolyMesh::VertexIter v_it;
     PolyMesh::VertexIter v_end = m_PickedMesh->vertices_end();

     OpenMesh::Vec3d ActualPoint;

     for (v_it = m_PickedMesh->vertices_begin(); v_it != v_end; ++v_it)
     {

          ActualPoint = m_PickedMesh->point( *v_it );
          double X = fabs(double(ActualPoint[0])-double(m_hitPoint[0]));
          double Y = fabs(double(ActualPoint[1])-double(m_hitPoint[1]));
          double Z = fabs(double(ActualPoint[2])-double(m_hitPoint[2]));

          if( (X<=0.25) && (Y<=0.25) && (Z<=0.25) )
          {
            int check = 0;
            for( int i = 0 ; i < m_vFixed.size() ; i++ )
            {
                if( v_it == m_vFixed[i] )
                {
                    check = -1;
                }
            }

            if( check == 0 )
            {
              m_vFixed.push_back(v_it);
              PolyMesh::Point p = m_PickedMesh->point(v_it);
              Vector v(p[0],p[1],p[2]);
              int val = RPC::callFunctionValue<int>("primitivesgenerator","addSphere",v,0.3);
            }
            else
            {
                std::cout << "Allready fixed" << std::endl;
            }
          }
     }
     emit updatedObject(m_IdObject,UPDATE_ALL);
}

//**********************************************************************************************
// Discretize the length of each edge with a certain number of vertex
// Not working yet
//**********************************************************************************************
void mmPlugin::changeDiscretizeValue()
{
    m_discretize = discretizeSpin->value();
}

void mmPlugin::discretizeLenght()
{
       discretizeSpin->setDisabled(true);

       std::cout << "vertex " << m_PickedMesh->n_vertices() << " edges " << m_PickedMesh->n_edges() << std::endl;

/*       PolyMesh::HalfedgeIter he_it;
       PolyMesh::HalfedgeIter he_end = m_PickedMesh->halfedges_end();

       PolyMesh::VertexHandle v1, v2;
       PolyMesh::Point p1, p2;

       PolyMesh::EdgeHandle e;

       PolyMesh::Point precP, nextP;
       PolyMesh::VertexHandle precV, nextV;
       double delta = 0;

    // delete faces -> garbage collection

       for( he_it = m_PickedMesh->halfedges_begin() ; he_it != he_end ; he_it++ )
       {
            v1 = m_PickedMesh->to_vertex_handle(he_it);
            p1 = m_PickedMesh->point(v1);

            v2 = m_PickedMesh->from_vertex_handle(he_it);
            p2 = m_PickedMesh->point(v2);

            e = m_PickedMesh->edge_handle(he_it);

            delta = fabs(p1[0]-p2[0])/(m_discretize+1);

            m_PickedMesh->delete_edge(e,false);

            precV = v1;
            for(int i = 1 ; i <= m_discretize ; i++ )
            {
                if( p1[0] < p2[0] )
                {
                    precV = v1;
                    precP[0] = p1[0];

                    if( p1[1] < p2[1] )
                    {
                        precP[1] = p1[1];
                    }
                    else
                    {
                        precP[1] = p2[1];
                    }
                }
                else
                {
                    precV = v2;
                    precP[0] = p2[0];

                    if( p1[1] < p2[1] )
                    {
                        precP[1] = p1[1];
                    }
                    else
                    {
                        precP[1] = p2[1];
                    }
                }

                nextP[0] = precP[0] + i*delta;
                nextP[1] = precP[1] + i*delta;

                nextV = m_PickedMesh->new_vertex(nextP);
                m_PickedMesh->new_edge(precV,nextV);
                precV = nextV;
            }
            m_PickedMesh->new_edge(nextV,v2);
            he_it++;
       }

       std::cout << "vertex " << m_PickedMesh->n_vertices() << " edges " << m_PickedMesh->n_edges() << std::endl;

       save(m_IdObject,"/Users/Juju/Documents/project/files/QuadmeshDiscretized.ply");

       emit updatedObject(m_IdObject,UPDATE_ALL);

       PluginFunctions::viewAll();

       */

}

//**********************************************************************************************
// Solve the Optimization
// Not working yet
//**********************************************************************************************
int mmPlugin::solveOptimazation()
{
    m_mySolver.hello();
}

//**********************************************************************************************
Q_EXPORT_PLUGIN2( movemeshplugin , mmPlugin );
