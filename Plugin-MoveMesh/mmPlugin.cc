#include "mmPlugin.hh"

void mmPlugin::initializePlugin()
{
    //**********************************************************************************************
    m_IdObject = -1;
    m_FixPoint = 4;
    m_discretize = 1;
    m_sizeX = 5;
    m_sizeY = 5;
    m_vertices = m_sizeX*m_sizeY;
    m_edges = 2*(m_sizeX-1)*(m_sizeY-1) + m_sizeX + m_sizeY - 2;
    m_faces = (m_sizeX - 1)*(m_sizeY - 1);
    m_idFixed.clear();
    m_posFixed.clear();
    m_hitPoint = OpenMesh::Vec3d(0,0,0);

    m_pickMode = 0;
    m_dragMode = 0;
    m_dragedVertex = 0;


    // Register keys
    emit registerKey( Qt::Key_O, Qt::NoModifier, "Move vertex foward");
    emit registerKey( Qt::Key_M, Qt::NoModifier, "Move vertex backward");
    emit registerKey( Qt::Key_J, Qt::NoModifier, "Move vertex left");
    emit registerKey( Qt::Key_L, Qt::NoModifier, "Move vertex right");
    emit registerKey( Qt::Key_K, Qt::NoModifier, "Move vertex down");
    emit registerKey( Qt::Key_I, Qt::NoModifier, "Move vertex up");
    emit registerKey(Qt::Key_Return, Qt::NoModifier, "stop enabling drag");


    QIcon* toolIcon = new QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"icon.png");


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

    dragButton = new QPushButton("&Drag",toolBox);
    dragButton->setEnabled(true);

    fixPointSpin = new QSpinBox(toolBox);
    fixPointSpin->setMinimum(1);
    fixPointSpin->setMaximum(20);
    fixPointSpin->setValue(4);
    fixPointSpin->setDisabled(true);

    discretizeSpin = new QSpinBox(toolBox);
    discretizeSpin->setMinimum(1);
    discretizeSpin->setMaximum(5);
    discretizeSpin->setValue(1);
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
    layout->addWidget( dragButton , 5, 1);

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
    connect( dragButton, SIGNAL(clicked()), this, SLOT(dragVertex()));


    emit addToolbox( tr("MoveMesh") , toolBox , toolIcon );
}

//**********************************************************************************************
void mmPlugin::pluginsInitialized()
{
    emit log(LOGWARN,"Move Mesh Initialized");
    emit addPickMode("MyPickMode");
    emit addPickMode("DragVertex");
    emit setPickModeMouseTracking("DragVertex",true);
    loadButton->setEnabled(true);
}

void mmPlugin::slotAllCleared()
{
    std::cout << "slot all cleared" << std::endl;
    //initializePlugin();
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
    m_faces = (m_sizeX - 1)*(m_sizeY - 1);
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

        // Add the faces
        for( int j = 0 ; j < m_sizeY-1 ; j++ )
        {
            for( int i = 0 ; i < m_sizeX-1 ; i++ )
            {
                m_fphandles.clear();
                m_fphandles.push_back(m_vh0[j*m_sizeX+i]);
                m_fphandles.push_back(m_vh0[j*m_sizeX+(i+1)]);
                m_fphandles.push_back(m_vh0[(j+1)*m_sizeX+(i+1)]);
                m_fphandles.push_back(m_vh0[(j+1)*m_sizeX+i]);
                m_PickedMesh->add_face(m_fphandles);
            }
        }

        m_PickedMesh->update_normals();

        m_IdObject = newObject;
        save(m_IdObject,"/Users/Juju/Documents/project/files/Quadmesh.ply");

        PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::WIREFRAME);
        emit updatedObject(m_IdObject,UPDATE_ALL);

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
                findSelectVertex_fixed();
            }
            else
            {
                std::cout << "not on the mesh " << std::endl;
            }

            m_pickMode = 1;
        }
    }
    else if( PluginFunctions::pickMode() == "DragVertex" && PluginFunctions::actionMode() == Viewer::PickingMode)
    {
        std::cout << "drag mode" << std::endl;

        if( _event->type() == QEvent::MouseButtonPress )
        {
            unsigned int node_idx, target_idx;
            OpenMesh::Vec3d hitPoint;

            if ( PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_ANYTHING,_event->pos(), node_idx, target_idx, &hitPoint) )
            {
                m_hitPoint = hitPoint;
                findSelectVertex_draged();
            }
            else
            {
                std::cout << "not on the mesh " << std::endl;
            }
            m_dragMode = 1;
        }
    }
    else
    {
        return;
    }

    if( m_pickMode == 1 )
    {
        if( m_idFixed.size() < m_FixPoint )
        {
            PluginFunctions::actionMode(Viewer::PickingMode);
            PluginFunctions::pickMode("MyPickMode");
        }
        else
        {
            pickButton->setDisabled(true);
            discretButton->setEnabled(true);
            discretizeSpin->setEnabled(true);
            solveButton->setEnabled(true);
            PluginFunctions::actionMode(Viewer::ExamineMode);
        }
        m_pickMode = 0;
    }
    else if( m_dragMode == 1 )
    {
        PluginFunctions::actionMode(Viewer::ExamineMode);
        m_dragMode = 0;
    }
    else
    {
        return;
    }
}

void mmPlugin::findSelectVertex_fixed()
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
                m_idFixed.push_back((*v_it).idx());
                Vector v(p[0],p[1],p[2]);
                m_posFixed.push_back(v);
                RPC::callFunctionValue<int>("primitivesgenerator","addSphere",v,0.3);
            }
        }
    }

    emit updatedObject(m_IdObject,UPDATE_ALL);
    PluginFunctions::viewAll();
}

void mmPlugin::findSelectVertex_draged()
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

        if( (X<=1) && (Y<=1) && (Z<=1) )
        {
            for( int i = 0 ; i < m_idFixed.size() ; i++)
            {
                if( (*v_it).idx() == m_idFixed[i] )
                {
                    m_Draged = v_it;
                    m_dragedVertex = (*v_it).idx();
                    std::cout << " you can drag "<< std::endl;
                    return;
                }
            }
            std::cout << "not dragable" << std::endl;
        }
    }

    m_dragedVertex = -1;
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

    PolyMesh* newMesh;

    m_PickedMesh->add_property(m_list_vertex);

    int newObject = createNewObject();

    PolyMeshObject* object;
    if ( !PluginFunctions::getObject(newObject,object) )
    {
        emit log(LOGERR,"Unable to create new Object");
    }
    else
    {
        object->setName( "QuadrimeshDiscretized " + QString::number(newObject) );

        newMesh = object->mesh();
        newMesh->clear();

        PolyMesh::VertexIter v_it;
        PolyMesh::VertexIter v_end = m_PickedMesh->vertices_end();

        for( v_it = m_PickedMesh->vertices_begin() ; v_it != v_end ; v_it++ )
        {
            PolyMesh::VertexHandle vh;
            PolyMesh::Point p;
            vh = *v_it;
            p = m_PickedMesh->point(vh);
            newMesh->add_vertex(p);
        }

        PolyMesh::EdgeIter e_it;
        PolyMesh::EdgeIter e_end = m_PickedMesh->edges_end();

        PolyMesh::EdgeHandle he;
        PolyMesh::HalfedgeHandle he_01, he_10;

        PolyMesh::VertexHandle vh0, vh1, vhi;
        PolyMesh::Point p0, p1, pi;

        for( e_it = m_PickedMesh->edges_begin() ; e_it != e_end ; e_it++ )
        {
            he = *e_it;

            he_01 = m_PickedMesh->halfedge_handle(he,0);
            he_10 = m_PickedMesh->halfedge_handle(he,1);

            vh0 = m_PickedMesh->from_vertex_handle(he_01);
            m_PickedMesh->property(m_list_vertex,he_01).push_back(vh0);

            vh1 = m_PickedMesh->to_vertex_handle(he_01);

            p0 = m_PickedMesh->point(vh0);
            p1 = m_PickedMesh->point(vh1);

            for( int i = 0 ; i < m_discretize ; i++ )
            {
                double a = double(i+1)/double(m_discretize+1);
                pi = a*p0 + (1-a)*p1;
                vhi = newMesh->add_vertex(pi);
                m_PickedMesh->property(m_list_vertex,he_01).push_back(vhi);
            }
            m_PickedMesh->property(m_list_vertex,he_01).push_back(vh1);

            for( int i = m_PickedMesh->property(m_list_vertex,he_01).size()-1 ; i >= 0 ; i-- )
            {
                m_PickedMesh->property(m_list_vertex,he_10).push_back(m_PickedMesh->property(m_list_vertex,he_01).at(i));
            }
        }

        PolyMesh::FaceIter f_it;
        PolyMesh::FaceIter f_end = m_PickedMesh->faces_end();

        PolyMesh::FaceHalfedgeIter fh_it;

        PolyMesh::VertexHandle vi;

        for( f_it = m_PickedMesh->faces_begin() ; f_it != f_end ; f_it++ )
        {
            m_fphandles.clear();
            int halfe = 0;
            for( fh_it = m_PickedMesh->fh_iter(*f_it) ; fh_it.is_valid() ; ++fh_it)
            {
                for( int i = 0 ; i < m_PickedMesh->property(m_list_vertex,*fh_it).size() ; i++ )
                {
                    vi = m_PickedMesh->property(m_list_vertex,*fh_it).at(i);
                    if( i > 0 && i <= m_discretize)
                    {
                        m_fphandles.push_back(vi);
                    }
                    else if( i == 0 && halfe == 0 )
                    {
                        m_fphandles.push_back(vi);
                    }
                    else if( i == m_discretize+1 && halfe < 3 )
                    {
                        m_fphandles.push_back(vi);
                    }
                }
                halfe++;
            }

            newMesh->add_face(m_fphandles);
        }

        newMesh->update_normals();

        m_PickedMesh->clear();
        m_PickedMesh = newMesh;

        m_vertices = m_PickedMesh->n_vertices();
        m_edges = m_PickedMesh->n_edges();
        m_faces = m_PickedMesh->n_faces();

        m_IdObject = newObject;

        save(m_IdObject,"/Users/Juju/Documents/project/files/QuadmeshDiscretized.ply");

        PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::WIREFRAME);
        emit updatedObject(m_IdObject,UPDATE_ALL);

        PluginFunctions::viewAll();
    }
}

//**********************************************************************************************
// Solve the Optimization
//**********************************************************************************************
void mmPlugin::solveOptimazation()
{
    discretButton->setDisabled(true);

    getPoints();
    solveShape();
    setNewPositions();

    dragButton->setEnabled( true );
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
        p = m_PickedMesh->point(*v_it);

        m_MV(0,nbV) = p[0];
        m_MV(1,nbV) = p[1];
        m_MV(2,nbV) = p[2];

        nbV++;
    }

    // GET ALL THE EDGES
    PolyMesh::EdgeIter e_it;
    PolyMesh::EdgeIter e_end = m_PickedMesh->edges_end();

    PolyMesh::EdgeHandle he;
    PolyMesh::HalfedgeHandle he_01;

    PolyMesh::VertexHandle vh0, vh1;

    int nbedges = 0;
    for( e_it = m_PickedMesh->edges_begin() ; e_it != e_end ; e_it++ )
    {
        he = *e_it;
        he_01 = m_PickedMesh->halfedge_handle(he,0);
        vh0 = m_PickedMesh->from_vertex_handle(he_01);
        vh1 = m_PickedMesh->to_vertex_handle(he_01);


        m_ME(nbedges,0) = vh0.idx();
        m_ME(nbedges,1) = vh1.idx();
        nbedges++;
    }
}

void mmPlugin::solveShape()
{
    ShapeOp::Solver s;
    s.setPoints(m_MV);
    ShapeOp::Scalar edgelength_weight = 200;
    ShapeOp::Scalar closeness_weight = 200;
    //ShapeOp::Scalar laplacian_weight = 200;

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
        edge_constraint->setEdgeLength(1);
        s.addConstraint(edge_constraint);
    }

    //add gravity constraint
    {
        auto gravity_force = std::make_shared<ShapeOp::GravityForce>(ShapeOp::Vector3(0,0,-0.05));
        s.addForces(gravity_force);
    }

    // add laplacian constraint
    /*for( int i = 0; i < m_MV.rows() ; i++)
    {
        std::vector<int> id_vector;
        id_vector.push_back();
        auto laplacian_constraint = std::make_shared<ShapeOp::UniformLaplacianConstraint>(id_vector,laplacian_weight,s.getPoints());
    }*/


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
        NewPoint[0] = m_MV(0,nbV);
        NewPoint[1] = m_MV(1,nbV);
        NewPoint[2] = m_MV(2,nbV);
        m_PickedMesh->set_point(*v_it,NewPoint);
        nbV++;
    }

    emit updatedObject(m_IdObject,UPDATE_ALL);
}

//**********************************************************************************************
// Drag the fixed vertex onto new positions
//**********************************************************************************************
void mmPlugin::dragVertex()
{
    PluginFunctions::actionMode(Viewer::PickingMode);
    PluginFunctions::pickMode("DragVertex");
}

void mmPlugin::slotKeyEvent( QKeyEvent* _event )
{
    PolyMesh::Point p = m_PickedMesh->point(*m_Draged);

    // Switch pressed keys
    if( m_dragedVertex != -1 )
    {
        switch (_event->key())
        {
        case Qt::Key_O :
            std::cout << "move foward" <<std::endl;
            p[0] = p[0]-1;
            break;
        case Qt::Key_M :
            std::cout << "move backward" <<std::endl;
            p[0] = p[0]+1;
            break;
        case Qt::Key_J:
            std::cout << "move left" <<std::endl;
            p[1] = p[1]+1;
            break;
        case Qt::Key_L :
            std::cout << "move right" <<std::endl;
            p[1] = p[1]-1;
            break;
        case Qt::Key_K :
            std::cout << "move down" <<std::endl;
            p[2] = p[2]-1;
            break;
        case Qt::Key_I :
            std::cout << "move up" <<std::endl;
            p[2] = p[2]+1;
            break;
         case Qt::Key_Return :
            std::cout << "stop move" <<std::endl;
            m_dragedVertex = 0;
        default:
            break;
        }
    }

    m_PickedMesh->set_point(*m_Draged,p);
    for( int i = 0; i<m_idFixed.size() ; i++)
    {
        if(m_dragedVertex == m_idFixed[i])
        {
            m_posFixed[i] = p;
        }
    }

    emit updatedObject(m_IdObject, UPDATE_GEOMETRY);
    emit updateView();
}

//**********************************************************************************************
Q_EXPORT_PLUGIN2( movemeshplugin , mmPlugin );
