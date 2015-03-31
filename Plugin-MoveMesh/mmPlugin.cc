#include "mmPlugin.hh"

void mmPlugin::initializePlugin()
{
   //**********************************************************************************************
   m_IdObject = -1;
   m_FixPoint = 4;
   m_discretize = 3;
   m_sizeX = 5;
   m_sizeY = 5;
   m_vertices = m_sizeX*m_sizeY;
   m_edges = 2*(m_sizeX-1)*(m_sizeY-1) + m_sizeX + m_sizeY - 2;
   m_idFixed.clear();
   m_posFixed.clear();
   m_hitPoint = OpenMesh::Vec3d(0,0,0);

   //**********************************************************************************************
   QWidget* toolBox = new QWidget();

   sizeXSpin = new QSpinBox(toolBox);
   sizeXSpin->setMinimum(5);
   sizeXSpin->setMaximum(100);
   sizeXSpin->setValue(5);

   sizeYSpin = new QSpinBox(toolBox);
   sizeYSpin->setMinimum(5);
   sizeYSpin->setMaximum(100);
   sizeYSpin->setValue(5);

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

   layout->addWidget( sizeXSpin, 1, 1);
   layout->addWidget( sizeYSpin, 1, 2);
   layout->addWidget( loadButton, 1, 3);
   layout->addWidget( fixPointSpin, 2, 1);
   layout->addWidget( pickButton, 2, 2);
   layout->addWidget( discretizeSpin, 3, 1);
   layout->addWidget( discretButton , 3, 2);
   layout->addWidget( solveButton , 4, 1);

   layout->addItem(new QSpacerItem(10,10,QSizePolicy::Expanding,QSizePolicy::Expanding),2,0,1,2);

   //**********************************************************************************************
   connect( sizeXSpin , SIGNAL(valueChanged(int)) , this, SLOT(changeXYValue()));
   connect( sizeYSpin , SIGNAL(valueChanged(int)) , this, SLOT(changeXYValue()));
   connect( loadButton, SIGNAL(clicked()), this, SLOT(addQuadrimesh()));
   connect( pickButton, SIGNAL(clicked()), this, SLOT(pickVertex()));
   connect( fixPointSpin , SIGNAL(valueChanged(int)) , this, SLOT(changeFixPointValue()));
   connect( discretButton, SIGNAL(clicked()), this, SLOT(discretizeLenght()));
   connect( discretizeSpin , SIGNAL(valueChanged(int)) , this, SLOT(changeDiscretizeValue()));
   connect( solveButton, SIGNAL(clicked()), this, SLOT(solveOptimazation()));
   //connect( , SIGNAL(clicked()), this, SLOT(allCleared()));

   emit addToolbox( tr("MoveMesh") , toolBox );
}

//**********************************************************************************************
void mmPlugin::pluginsInitialized()
{
    emit log(LOGWARN,"Move Mesh Initialized");
    emit addPickMode("MyPickMode");
    loadButton->setEnabled(true);
}

void mmPlugin::allCleared()
{
    loadButton->setEnabled(true);
    sizeXSpin->setEnabled(true);
    sizeYSpin->setEnabled(true);
    pickButton->setEnabled(true);
    fixPointSpin->setEnabled(true);
    discretizeSpin->setEnabled(true);
    discretButton->setEnabled(true);
    solveButton->setEnabled(true);
}

//**********************************************************************************************
// Create a new quad mesh composed of X vertices, then save it and load it to the viewer
// Code inspired from the PrimitivesGenerator plugin
//**********************************************************************************************
void mmPlugin::changeXYValue()
{
    m_sizeX = sizeXSpin->value();
    m_sizeY = sizeYSpin->value();
    m_vertices = m_sizeX*m_sizeY;
    m_edges = 2*(m_sizeX-1)*(m_sizeY-1) + m_sizeX + m_sizeY - 2;
}

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
  int newObject = createNewObject();

  PolyMeshObject* object;
  if ( !PluginFunctions::getObject(newObject,object) )
  {
    emit log(LOGERR,"Unable to create new Object");
    return -1;
  }
  else
  {
    object->setName( "Quadrimesh " + QString::number(newObject) );

    m_PickedMesh =  object->mesh();

    m_PickedMesh->clear();

    // add the new vertices
    int k = 0;
    m_vh0.resize(m_sizeX*m_sizeY);
    for( int j = 0 ; j < m_sizeY ; j++ )
    {
        for( int i = 0 ; i < m_sizeX ; i++ )
        {
            m_vh0[k] = m_PickedMesh->add_vertex(PolyMesh::Point(i,j, 0));
            k++;
        }
    }

    k = 0;
    m_vh1.resize(m_sizeX*m_sizeY);
    for( int j = 0 ; j < m_sizeY ; j++ )
    {
        for( int i = 0 ; i < m_sizeX ; i++ )
        {
            m_vh1[k] = m_PickedMesh->add_vertex(PolyMesh::Point(i,j, 0));
            k++;
        }
    }

    k = 0;
    m_vh2.resize(m_sizeX*m_sizeY);
    for( int j = 0 ; j < m_sizeY ; j++ )
    {
        for( int i = 0 ; i < m_sizeX ; i++ )
        {
            m_vh2[k] = m_PickedMesh->add_vertex(PolyMesh::Point(i,j, 0));
            k++;
        }
    }

    // Add the faces
    for( int j = 0 ; j < m_sizeY ; j++ )
    {
        for( int i = 0 ; i < m_sizeX-1 ; i++ )
        {
            m_fphandles.clear();
            m_fphandles.push_back(m_vh0[j*m_sizeX+i]);
            m_fphandles.push_back(m_vh1[j*m_sizeX+i]);
            m_fphandles.push_back(m_vh0[j*m_sizeX+(i+1)]);
            m_PickedMesh->add_face(m_fphandles);
        }
     }

     for( int i = 0 ; i < m_sizeX ; i++ )
     {
        for( int j = 0 ; j < m_sizeY-1 ; j++ )
        {
            m_fphandles.clear();
            m_fphandles.push_back(m_vh0[j*m_sizeX+i]);
            m_fphandles.push_back(m_vh2[j*m_sizeX+i]);
            m_fphandles.push_back(m_vh0[(j+1)*m_sizeX+i]);
            m_PickedMesh->add_face(m_fphandles);
        }
     }

    m_PickedMesh->update_normals();

    m_IdObject = newObject;
    save(m_IdObject,"/Users/Juju/Documents/project/files/Quadmesh.ply");

    PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::WIREFRAME);
    emit updatedObject(newObject,UPDATE_ALL);

    PluginFunctions::viewAll();

    fixPointSpin->setEnabled(true);
    pickButton->setEnabled(true);
    loadButton->setDisabled(true);
    sizeXSpin->setDisabled(true);
    sizeYSpin->setDisabled(true);

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

     if(m_idFixed.size() < m_FixPoint )
     {
        PluginFunctions::actionMode(Viewer::PickingMode);
        PluginFunctions::pickMode("MyPickMode");
     }
     else
     {
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
               std::cout << "not on the mesh " << std::endl;
            }
       }
     }

     if( m_idFixed.size() < m_FixPoint )
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
        PluginFunctions::actionMode(Viewer::ExamineMode);
     }

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

            for( int i = 0 ; i < m_idFixed.size() ; i++ )
            {
                if( (*v_it).idx() == m_idFixed[i] )
                {
                    check = -1;
                }
            }

            if( check == 0 )
            {
                PolyMesh::Point p = m_PickedMesh->point(*v_it);

                for( int i = 0 ; i < m_posFixed.size() ; i++ )
                {
                    if( p == m_posFixed[i] )
                    {
                        check = -1;
                    }
                }

                if( check == 0 )
                {
                   m_idFixed.push_back((*v_it).idx());
                   Vector v(p[0],p[1],p[2]);
                   m_posFixed.push_back(v);
                   std::cout << "fix point# " << m_idFixed.size() << " : " << v << std::endl;
                   RPC::callFunctionValue<int>("primitivesgenerator","addSphere",v,0.3);
                }
            }
          }
     }

     save(m_IdObject,"/Users/Juju/Documents/project/files/OutputSolver.ply");
     emit updatedObject(m_IdObject,UPDATE_ALL);
     PluginFunctions::viewAll();
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

   /* PolyMesh::VertexHandle vhf;
    PolyMesh::VertexHandle vht;

    PolyMesh::EdgeHandle eh;
    PolyMesh::HalfedgeHandle heh;

    PolyMesh::Point Pf;
    PolyMesh::Point Pt;

    PolyMesh::Point P;

    int k = 0;

    for( int i = 0 ; i< m_edges ; i++ )
    {
        std::cout << "check all edges" << std::endl;

        // get the two vertices of the edge
        vhf = m_PickedMesh->vertex_handle(m_ME(i,0));
        vht = m_PickedMesh->vertex_handle(m_ME(i,1));

        std::cout << "vertex: " << vhf << " / " << vht << std::endl;

        PolyMesh::VertexOHalfedgeIter voh_it = m_PickedMesh->voh_iter(vhf);

        for( ; voh_it; ++voh_it)
        {
            heh = *voh_it;

            if( vht == m_PickedMesh->to_vertex_handle(heh) )
            {
                eh = m_PickedMesh->edge_handle(heh);
            }
        }

        std::cout << "edge: " << eh << std::endl;

        // get their points
        Pf = m_PickedMesh->point(vhf);
        Pt = m_PickedMesh->point(vht);

        // depending on the discretizing number
        for( int i = 0 ; i < m_FixPoint ; i++ )
        {
            // calcule the new points in-between
            P[0] = (i+1)*((Pf[0]+Pt[0])/(m_FixPoint+1));
            P[1] = (i+1)*((Pf[1]+Pt[1])/(m_FixPoint+1));
            P[2] = (i+1)*((Pf[2]+Pt[2])/(m_FixPoint+1));

            // set the new vertices at those points (where to put the vertex handles)
            m_vh3[k] = m_PickedMesh->add_vertex(P);
            m_vh4[k] = m_PickedMesh->add_vertex(P);
            k++;
        }

        // delete the existing edges ( or face? )
        //m_PickedMesh->delete_edge(eh,false);

        // add the new faces
        for( int i = 0 ; i < m_FixPoint ; i++ )
        {
            m_fphandles.clear();
            m_fphandles.push_back(vhf);
            m_fphandles.push_back();
            m_fphandles.push_back();
            m_PickedMesh->add_face();
        }


    }
    //m_PickedMesh->garbage_collection();
    */

}

//**********************************************************************************************
// Solve the Optimization
// Not working yet
//**********************************************************************************************
void mmPlugin::solveOptimazation()
{
    getPoints();
    solveShape();
    setNewPositions();
}

void mmPlugin::getPoints()
{
    m_MV.resize(3,m_vertices);
    m_ME.resize(m_edges,2);

    // GET ALL THE VERTICES
    PolyMesh::VertexIter v_it;
    PolyMesh::VertexIter v_end = m_PickedMesh->vertices_end();
    OpenMesh::Vec3d p;

    int nbV = 0;
    for( v_it = m_PickedMesh->vertices_begin(); v_it != v_end; v_it++ )
    {
         if( nbV < m_sizeX*m_sizeY )
         {
            p = m_PickedMesh->point(*v_it);

            m_MV(0,nbV) = p[0];
            m_MV(1,nbV) = p[1];
            m_MV(2,nbV) = p[2];

            nbV++;
        }
    }

    // GET ALL THE EDGES
    int nbedges = 0;
    for( int j = 0 ; j < m_sizeY ; j++ )
    {
        for( int i = 0 ; i < m_sizeX ; i++)
        {
            if( i < m_sizeX-1)
            {
                m_ME(nbedges,0) = j*m_sizeX+i;
                m_ME(nbedges,1) = j*m_sizeX+(i+1);
                nbedges++;
            }

            if( j < m_sizeY-1)
            {
                m_ME(nbedges,0) = j*m_sizeX+i;
                m_ME(nbedges,1) = (j+1)*m_sizeX+i;
                nbedges++;
            }
        }
    }
}

void mmPlugin::solveShape()
{
      ShapeOp::Solver s;
      s.setPoints(m_MV);
      ShapeOp::Scalar edgelength_weight = 1.0;
      ShapeOp::Scalar closeness_weight = 10;

      //add a closeness constraint to the fixed vertex.
      for(int i = 0 ; i < m_idFixed.size()  ; i++)
      {
        std::vector<int> id_vector;
        id_vector.push_back(m_idFixed[i]);
        auto closs_contraint = std::make_shared<ShapeOp::ClosenessConstraint>(id_vector, closeness_weight, s.getPoints());

        Vector pos1 = m_posFixed[i];
        ShapeOp::Vector3 pos2;
        pos2(0) = pos1[0];
        pos2(1) = pos1[1];
        pos2(2) = pos1[2];

        closs_contraint->setPosition(pos2);

        s.addConstraint(closs_contraint);
      }

      //add edge contraint for all edges
      for( int i = 0 ; i < m_ME.rows() ; i++)
      {
        std::vector<int> id_vector;
        id_vector.push_back(m_ME(i,0));
        id_vector.push_back(m_ME(i,1));
        auto edge_constraint = std::make_shared<ShapeOp::EdgeStrainConstraint>(id_vector,edgelength_weight,s.getPoints());
        edge_constraint->setEdgeLength(1.5);
        s.addConstraint(edge_constraint);
      }

      //add gravity constraint
      {
        auto gravity_force = std::make_shared<ShapeOp::GravityForce>(ShapeOp::Vector3(0,0,-2));
        s.addForces(gravity_force);
      }

      s.initialize(false);
      s.solve(500);
      m_MV = s.getPoints();
}

void mmPlugin::setNewPositions()
{     
    PolyMesh::VertexIter v_it;
    PolyMesh::VertexIter v_end = m_PickedMesh->vertices_end();
    OpenMesh::Vec3d NewPoint;

    int nbV = 0;
    for( v_it = m_PickedMesh->vertices_begin(); v_it != v_end; v_it++ )
    {
        if( nbV < m_sizeX*m_sizeY )
        {
            NewPoint[0] = m_MV(0,nbV);
            NewPoint[1] = m_MV(1,nbV);
            NewPoint[2] = m_MV(2,nbV);
            m_PickedMesh->set_point(*v_it,NewPoint);
        }
        nbV++;
    }

    nbV = 0;
    for( v_it = m_PickedMesh->vertices_begin(); v_it != v_end; v_it++ )
    {
        if( nbV >= m_sizeX*m_sizeY && nbV < 2*(m_sizeX*m_sizeY) )
        {
            NewPoint[0] = m_MV(0,nbV-m_sizeX*m_sizeY);
            NewPoint[1] = m_MV(1,nbV-m_sizeX*m_sizeY);
            NewPoint[2] = m_MV(2,nbV-m_sizeX*m_sizeY);
            m_PickedMesh->set_point(*v_it,NewPoint);
        }
        nbV++;
    }

    nbV = 0;
    for( v_it = m_PickedMesh->vertices_begin(); v_it != v_end; v_it++ )
    {
        if( nbV >= 2*(m_sizeX*m_sizeY) )
        {
            NewPoint[0] = m_MV(0,nbV-2*m_sizeX*m_sizeY);
            NewPoint[1] = m_MV(1,nbV-2*m_sizeX*m_sizeY);
            NewPoint[2] = m_MV(2,nbV-2*m_sizeX*m_sizeY);
            m_PickedMesh->set_point(*v_it,NewPoint);
        }
        nbV++;
    }

     emit updatedObject(m_IdObject,UPDATE_ALL);
}

//**********************************************************************************************
Q_EXPORT_PLUGIN2( movemeshplugin , mmPlugin );
