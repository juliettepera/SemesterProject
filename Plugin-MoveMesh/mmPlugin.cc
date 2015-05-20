#include "mmPlugin.hh"

void mmPlugin::initializePlugin()
{
    m_vh0.clear();
    m_fphandles.clear();
    m_idFixed.clear();
    m_posFixed.clear();
    m_hitPoint = OpenMesh::Vec3d(0,0,0);
    m_list_vertex.reset();
    m_IdObject = -1;
    m_FixPoint = 4;
    m_discretize = 0;
    m_sizeX = 5;
    m_sizeY = 5;
    m_vertices = m_sizeX*m_sizeY;
    m_edges = (m_sizeX-1)*m_sizeY + (m_sizeY-1)*m_sizeX;
    m_faces = (m_sizeX - 1)*(m_sizeY - 1);
    m_masse = 1.0;
    m_length = 1.0;
    m_EdgesCons.clear();
    m_VectorEdge.clear();
    m_LaplaceCons.clear();
    m_MV.resize(3,m_vertices);
    m_MV.setZero();
    m_pickMode = 0;
    m_dragMode = 0;
    m_dragedVertex = 0;
    m_windIntensity = 0;
    m_windDirection = ShapeOp::Vector3(0,0,0);

    //m_Draged.clear();
    //m_oldPos = QPoint(0,0,0);
    //m_newPos = QPoint(0,0,0);
    //m_PickedMesh->clear();

    QIcon* toolIcon = new QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"icon.png");


    //**********************************************************************************************
    QWidget* toolBox = new QWidget();

    sizeXSpin = new QSpinBox(toolBox);
    sizeXSpin->setMinimum(3);
    sizeXSpin->setMaximum(100);
    sizeXSpin->setValue(5);

    sizeYSpin = new QSpinBox(toolBox);
    sizeYSpin->setMinimum(3);
    sizeYSpin->setMaximum(100);
    sizeYSpin->setValue(5);

    discretizeSpin = new QSpinBox(toolBox);
    discretizeSpin->setMinimum(0);
    discretizeSpin->setMaximum(4);
    discretizeSpin->setValue(0);

    loadButton = new QPushButton("&Load",toolBox);
    loadButton->setDisabled(true);

    fixPointSpin = new QSpinBox(toolBox);
    fixPointSpin->setMinimum(1);
    fixPointSpin->setMaximum(20);
    fixPointSpin->setValue(4);
    fixPointSpin->setDisabled(true);

    pickButton = new QPushButton("&Pick",toolBox);
    pickButton->setDisabled(true);

    solveButton = new QPushButton("&Solve",toolBox);
    solveButton->setDisabled(true);

    dragButton = new QPushButton("&Drag",toolBox);
    dragButton->setDisabled(true);

    windIntensitySlider = new QSlider(toolBox);
    windIntensitySlider->setOrientation(Qt::Horizontal);
    windIntensitySlider->setMinimum(0);
    windIntensitySlider->setValue(0);
    windIntensitySlider->setMaximum(10);
    windIntensitySlider->setDisabled(true);

    windXSlider = new QSlider(toolBox);
    windXSlider->setOrientation(Qt::Vertical);
    windXSlider->setMinimum(-10);
    windXSlider->setValue(0);
    windXSlider->setMaximum(10);
    windXSlider->setDisabled(true);

    windYSlider = new QSlider(toolBox);
    windYSlider->setOrientation(Qt::Vertical);
    windYSlider->setMinimum(-10);
    windYSlider->setValue(0);
    windYSlider->setMaximum(10);
    windYSlider->setDisabled(true);

    windZSlider = new QSlider(toolBox);
    windZSlider->setOrientation(Qt::Vertical);
    windZSlider->setMinimum(-10);
    windZSlider->setValue(0);
    windZSlider->setMaximum(10);
    windZSlider->setDisabled(true);

    QGridLayout* layout = new QGridLayout(toolBox);

    QLabel* labelX = new QLabel("Width");
    QLabel* labelY = new QLabel("Height");
    QLabel* labelD = new QLabel("Discretization #");
    QLabel* labelF = new QLabel("Fix point #");
    QLabel* labelWI = new QLabel("Wind Intensity");
    QLabel* labelWX = new QLabel("X");
    QLabel* labelWY = new QLabel("Y");
    QLabel* labelWZ = new QLabel("Z");


    layout->addWidget( labelX,        1,1);
    layout->addWidget( sizeXSpin,     2,1);
    layout->addWidget( labelY,        1,2);
    layout->addWidget( sizeYSpin,     2,2);
    layout->addWidget( labelD,        1,3);
    layout->addWidget( discretizeSpin,2,3);
    layout->addWidget( loadButton,    2,4);

    layout->addWidget( labelF,        3,1);
    layout->addWidget( fixPointSpin,  4,1);
    layout->addWidget( pickButton,    4,2);
    layout->addWidget( dragButton,    4,3);

    layout->addWidget( solveButton,   5,1,1,2);

    layout->addWidget( labelWX,6,1);
    layout->addWidget( windXSlider,7,1,3,1);
    layout->addWidget( labelWY, 6,2);
    layout->addWidget( windYSlider,7,2,3,1);
    layout->addWidget( labelWZ,6,3);
    layout->addWidget( windZSlider,7,3,3,1);
    layout->addWidget( labelWI,6,4,1,2);
    layout->addWidget( windIntensitySlider,7,4,1,2);

    layout->addItem(new QSpacerItem(10,10,QSizePolicy::Expanding,QSizePolicy::Expanding),2,0,1,2);

    //**********************************************************************************************
    connect( sizeXSpin , SIGNAL(valueChanged(int)) , this, SLOT(changeXYValue()));
    connect( sizeYSpin , SIGNAL(valueChanged(int)) , this, SLOT(changeXYValue()));
    connect( discretizeSpin , SIGNAL(valueChanged(int)) , this, SLOT(changeXYValue()));
    connect( loadButton, SIGNAL(clicked()), this, SLOT(addQuadrimesh()));
    connect( fixPointSpin , SIGNAL(valueChanged(int)) , this, SLOT(changeFixPointValue()));
    connect( pickButton, SIGNAL(clicked()), this, SLOT(pickVertex()));
    connect( dragButton, SIGNAL(clicked()), this, SLOT(dragVertex()));
    connect( solveButton, SIGNAL(clicked()), this, SLOT(solveOptimazation()));
    connect( windXSlider, SIGNAL(sliderReleased()), this, SLOT(changeWind()));
    connect( windYSlider, SIGNAL(sliderReleased()), this, SLOT(changeWind()));
    connect( windZSlider, SIGNAL(sliderReleased()), this, SLOT(changeWind()));
    connect( windIntensitySlider, SIGNAL(sliderReleased()), this, SLOT(changeWind()));

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
    std::cout << "slotAllCleared" << std::endl;

    /*
    m_EdgesCons.clear();
    m_LaplaceCons.clear();
    m_VectorEdge.clear();
    m_MV.resize(3,m_vertices);
    m_MV.setZero();
    m_list_vertex.reset();

    //m_Draged.clear();
    //m_oldPos = QPoint(0,0,0);
    //m_newPos = QPoint(0,0,0);
    //m_PickedMesh->clear();
    */

    m_vh0.clear();
    m_fphandles.clear();
    m_idFixed.clear();
    m_posFixed.clear();
    m_hitPoint = OpenMesh::Vec3d(0,0,0);

    m_IdObject = -1;
    m_vertices = m_sizeX*m_sizeY;
    m_edges = (m_sizeX-1)*m_sizeY + (m_sizeY-1)*m_sizeX;
    m_faces = (m_sizeX - 1)*(m_sizeY - 1);
    m_masse = 1.0;
    m_length = 1.0;

    m_FixPoint = 4;
    m_discretize = 0;
    m_sizeX = 5;
    m_sizeY = 5;

    m_pickMode = 0;
    m_dragMode = 0;
    m_dragedVertex = 0;
    m_windIntensity = 0;
    m_windDirection = ShapeOp::Vector3(0,1,0);

    sizeXSpin->setValue(5);
    sizeYSpin->setValue(5);
    fixPointSpin->setValue(4);
    discretizeSpin->setValue(0);
    windIntensitySlider->setValue(0);
    windXSlider->setValue(0);
    windYSlider->setValue(0);
    windZSlider->setValue(0);
    loadButton->setEnabled(true);
    sizeXSpin->setEnabled(true);
    sizeYSpin->setEnabled(true);
    discretizeSpin->setEnabled(true);

    fixPointSpin->setDisabled(true);
    pickButton->setDisabled(true);
    solveButton->setDisabled(true);
    dragButton->setDisabled(true);
    windIntensitySlider->setDisabled(true);
    windXSlider->setDisabled(true);
    windYSlider->setDisabled(true);
    windZSlider->setDisabled(true);
}

//**********************************************************************************************
// Create a new quad mesh composed of X vertices, then save it and load it to the viewer
// Code inspired from the PrimitivesGenerator plugin
//**********************************************************************************************
void mmPlugin::changeXYValue()
{
    m_sizeX = sizeXSpin->value();
    m_sizeY = sizeYSpin->value();
    m_discretize = discretizeSpin->value();

    m_vertices = m_sizeX*m_sizeY;
    m_edges = (m_sizeX-1)*m_sizeY + (m_sizeY-1)*m_sizeX;
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

        PolyMesh::EdgeIter e_it = m_PickedMesh->edges_begin();
        m_length = m_PickedMesh->calc_edge_length(*e_it);

        m_PickedMesh->update_normals();

        m_IdObject = newObject;
        save(m_IdObject,"/Users/Juju/Documents/project/files/Quadmesh.ply");

        discretizeLenght();

        PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::WIREFRAME);
        emit updatedObject(m_IdObject,UPDATE_ALL);

        PluginFunctions::viewAll();

        fixPointSpin->setEnabled(true);
        pickButton->setEnabled(true);

        loadButton->setDisabled(true);
        sizeXSpin->setDisabled(true);
        sizeYSpin->setDisabled(true);
        discretizeSpin->setDisabled(true);

        return newObject;
    }

    return -1;
}

void mmPlugin::discretizeLenght()
{
    if( m_discretize != 0 )
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

            int k = 0;

            for( v_it = m_PickedMesh->vertices_begin() ; v_it != v_end ; v_it++ )
            {
                PolyMesh::VertexHandle vh;
                PolyMesh::Point p;
                vh = *v_it;
                p = m_PickedMesh->point(vh);
                newMesh->add_vertex(p);
                k++;
            }

            PolyMesh::EdgeIter e_it;
            PolyMesh::EdgeIter e_end = m_PickedMesh->edges_end();

            PolyMesh::EdgeHandle he;
            PolyMesh::HalfedgeHandle he_01, he_10;

            PolyMesh::VertexHandle vh0, vh1, vhi, vprev;
            PolyMesh::Point p0, p1, pi;

            // laplacian constraint
            std::vector<int> index;

            for( e_it = m_PickedMesh->edges_begin() ; e_it != e_end ; e_it++ )
            {
                he = *e_it;

                he_01 = m_PickedMesh->halfedge_handle(he,0);
                he_10 = m_PickedMesh->halfedge_handle(he,1);

                vh0 = m_PickedMesh->from_vertex_handle(he_01);
                p0 = m_PickedMesh->point(vh0);
                m_PickedMesh->property(m_list_vertex,he_01).push_back(vh0);

                index.push_back(vh0.idx());

                vh1 = m_PickedMesh->to_vertex_handle(he_01);
                p1 = m_PickedMesh->point(vh1);

                for( int i = 0 ; i < m_discretize ; i++ )
                {
                    double a = double(i+1)/double(m_discretize+1);
                    pi = (1-a)*p0 + a*p1;
                    vhi = newMesh->add_vertex(pi);
                    m_PickedMesh->property(m_list_vertex,he_01).push_back(vhi);

                    if( index.size() < 3 )
                    {
                        index.push_back(vhi.idx());
                    }
                    else
                    {
                        //std::cout << index.at(0) << " " << index.at(1) << " " << index.at(2) << std::endl;
                        m_LaplaceCons.push_back(index);

                        index.clear();
                        index.push_back(vprev.idx());
                        index.push_back(vhi.idx());
                    }

                    vprev = vhi;

                }
                m_PickedMesh->property(m_list_vertex,he_01).push_back(vh1);

                index.push_back(vh1.idx());
                m_LaplaceCons.push_back(index);
                //std::cout << index.at(0) << " " << index.at(1) << " " << index.at(2) << std::endl;

                index.clear();

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

                        if( halfe == 0 )
                        {
                            m_fphandles.push_back(vi);
                        }
                        else if( halfe == 1 || halfe == 2 )
                        {
                            if( i != 0 )
                            {
                                m_fphandles.push_back(vi);
                            }
                        }
                        else if( halfe == 3 )
                        {
                            if( i != 0 && i != m_PickedMesh->property(m_list_vertex,*fh_it).size()-1 )
                            {
                                m_fphandles.push_back(vi);
                            }
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

            m_masse = double(m_sizeX*m_sizeY)/double(m_vertices);

            e_it = m_PickedMesh->edges_begin();
            m_length = m_PickedMesh->calc_edge_length(*e_it);

            m_IdObject = newObject;

            save(m_IdObject,"/Users/Juju/Documents/project/files/QuadmeshDiscretized.ply");
        }
    }
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

    if( m_idFixed.size() < m_FixPoint )
    {
        PluginFunctions::actionMode(Viewer::PickingMode);
        PluginFunctions::pickMode("MyPickMode");
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
            m_pickMode = 1;
        }
    }
    else if( PluginFunctions::pickMode() == "DragVertex" && PluginFunctions::actionMode() == Viewer::PickingMode)
    {

        if( _event->type() == QEvent::MouseButtonPress && m_dragMode == 0 )
        {
            unsigned int node_idx, target_idx;
            OpenMesh::Vec3d hitPoint;

            if ( PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_ANYTHING,_event->pos(), node_idx, target_idx, &hitPoint) )
            {
                m_hitPoint = hitPoint;
                findSelectVertex_draged();
                m_dragMode = 1;
            }
        }

        if( _event->type() == QEvent::MouseButtonRelease && m_dragMode == 1 )
        {
            unsigned int node_idx, target_idx;
            OpenMesh::Vec3d hitPoint;

            if ( ! PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_ANYTHING,_event->pos(), node_idx, target_idx, &hitPoint) )
            {
                //if mouse is not over an object, get the unprojected coordinates and the last z-value
                QPoint position = _event->pos();
                ACG::Vec3d viewCoords = ACG::Vec3d(position.x(), PluginFunctions::viewerProperties().glState().context_height() - position.y(), 0.5);
                hitPoint = PluginFunctions::viewerProperties().glState().unproject(viewCoords);
            }

            m_PickedMesh->set_point(*m_Draged,hitPoint);
            for( int i = 0; i<m_idFixed.size() ; i++)
            {
                if( m_dragedVertex == m_idFixed[i] )
                {
                    m_posFixed.at(i) = hitPoint;
                }
            }

            m_dragMode = 2;
            m_dragedVertex = -1;

            emit updatedObject(m_IdObject, UPDATE_GEOMETRY);
            solveOptimazation();
        }
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
            solveOptimazationInit();

            PluginFunctions::actionMode(Viewer::ExamineMode);
        }
        m_pickMode = 0;
    }

    if( m_dragMode == 2 )
    {
        PluginFunctions::actionMode(Viewer::ExamineMode);
        m_dragMode = 0;
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
                m_hitPoint = OpenMesh::Vec3d(-1,-1,-1);
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
                }
            }
        }
    }
}

//**********************************************************************************************
// Solve the Optimization
//**********************************************************************************************
void mmPlugin::solveOptimazationInit()
{
    getPoints();
    solveShape();
    setNewPositions();

    solveButton->setEnabled(true);
    dragButton->setEnabled(true);
    windIntensitySlider->setEnabled(true);
    windXSlider->setEnabled(true);
    windYSlider->setEnabled(true);
    windZSlider->setEnabled(true);
}

void mmPlugin::solveOptimazation()
{
    m_MV.resize(3,m_vertices);
    m_MV.setZero();

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

    solveShape();
    setNewPositions();
}

void mmPlugin::getPoints()
{
    m_MV.resize(3,m_vertices);
    m_MV.setZero();

    m_EdgesCons.clear();
    m_VectorEdge.clear();

    // GET ALL THE VERTICES
    PolyMesh::VertexIter v_it;
    PolyMesh::VertexIter v_end = m_PickedMesh->vertices_end();
    OpenMesh::Vec3d p;

    PolyMesh::HalfedgeHandle he_h;

    int nbV = 0;
    for( v_it = m_PickedMesh->vertices_begin(); v_it != v_end; v_it++ )
    {
        p = m_PickedMesh->point(*v_it);

        m_MV(0,nbV) = p[0];
        m_MV(1,nbV) = p[1];
        m_MV(2,nbV) = p[2];
        nbV++;

        m_VectorEdge.push_back(he_h);
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

        std::vector<int> index;
        index.push_back( vh0.idx() );
        index.push_back( vh1.idx() );
        m_EdgesCons.push_back(index);

        m_VectorEdge.at(vh0.idx()) = he_01;
        nbedges++;
    }

    // GET THE LAPLACIAN VERTICES
    if( m_discretize == 0 )
    {
        int k = 0;
        std::vector<int> index;

        for( int j = 0 ; j < m_sizeY ; j++ )
        {
            for( int i = 1 ; i < m_sizeX-1 ; i++ )
            {
                index.clear();
                k = i+j*m_sizeX;
                index.push_back(m_vh0[k-1].idx());
                index.push_back(m_vh0[k].idx());
                index.push_back(m_vh0[k+1].idx());
                //std::cout << index.at(0) << " " << index.at(1) << " " << index.at(2) << std::endl;
                m_LaplaceCons.push_back(index);
            }
        }

        for( int i = 0 ; i < m_sizeX ; i++ )
        {
            for( int j = 1 ; j < m_sizeY-1 ; j++ )
            {
                index.clear();
                k = i+j*m_sizeX;
                index.push_back(m_vh0[k-m_sizeX].idx());
                index.push_back(m_vh0[k].idx());
                index.push_back(m_vh0[k+m_sizeX].idx());
                //std::cout << index.at(0) << " " << index.at(1) << " " << index.at(2) << std::endl;
                m_LaplaceCons.push_back(index);
            }
        }
    }
    else
    {
        PolyMesh::VertexIter v_it;
        PolyMesh::VertexIter v_end = m_PickedMesh->vertices_end();
        PolyMesh::VertexVertexIter vv_it;


        int valence = 0;
        for( v_it = m_PickedMesh->vertices_begin() ; v_it != v_end ; v_it++ )
        {
            valence = m_PickedMesh->valence(*v_it);

            if( valence == 3 )
            {
                std::vector<int> index;
                index.clear();
                index.push_back((*v_it).idx());

                for ( vv_it = m_PickedMesh->vv_iter(*v_it); vv_it.is_valid(); ++vv_it)
                {
                    if( m_PickedMesh->is_boundary(*vv_it) )
                    {
                        index.push_back((*vv_it).idx());
                    }
                }

                std::vector<int> orderedIndex;
                orderedIndex.push_back(index.at(1));
                orderedIndex.push_back(index.at(0));
                orderedIndex.push_back(index.at(2));
                m_LaplaceCons.push_back(orderedIndex);
                //std::cout << orderedIndex.at(0) << " " << orderedIndex.at(1) << " " << orderedIndex.at(2) << std::endl;

            }
            else if( valence == 4 )
            {
                std::vector<int> index1;
                std::vector<int> index2;

                index1.clear();
                index2.clear();

                index1.push_back((*v_it).idx());
                index2.push_back((*v_it).idx());

                int i = 0;

                for ( vv_it = m_PickedMesh->vv_iter(*v_it); vv_it.is_valid(); ++vv_it)
                {
                    if( i == 0 || i==2 )
                    {
                        index1.push_back((*vv_it).idx());
                    }
                    else if( i == 1 || i==3 )
                    {
                        index2.push_back((*vv_it).idx());
                    }
                    i++;
                }

                std::vector<int> orderedIndex;
                orderedIndex.push_back(index1.at(1));
                orderedIndex.push_back(index1.at(0));
                orderedIndex.push_back(index1.at(2));
                m_LaplaceCons.push_back(orderedIndex);
                //std::cout << orderedIndex.at(0) << " " << orderedIndex.at(1) << " " << orderedIndex.at(2) << std::endl;


                orderedIndex.clear();
                orderedIndex.push_back(index2.at(1));
                orderedIndex.push_back(index2.at(0));
                orderedIndex.push_back(index2.at(2));
                m_LaplaceCons.push_back(orderedIndex);
                //std::cout << orderedIndex.at(0) << " " << orderedIndex.at(1) << " " << orderedIndex.at(2) << std::endl;


            }
        }

    }
    /*std::cout << "Laplacian :" << std::endl;
    for(int i = 0; i<m_LaplaceCons.size() ; i++)
    {
        std::cout << m_LaplaceCons.at(i).at(0) << " " << m_LaplaceCons.at(i).at(1) << " " << m_LaplaceCons.at(i).at(2) << std::endl;
    }*/
}

void mmPlugin::solveShape()
{
    ShapeOp::Solver s;
    s.setPoints(m_MV);
    ShapeOp::Scalar edgelength_weight = 1000;
    ShapeOp::Scalar closeness_weight = 200;
    ShapeOp::Scalar laplacian_weight = 200;

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
    for( int i = 0 ; i < m_EdgesCons.size() ; i++)
    {
        std::vector<int> id_vector;
        id_vector.push_back(m_EdgesCons.at(i).at(0));
        id_vector.push_back(m_EdgesCons.at(i).at(1));
        auto edge_constraint = std::make_shared<ShapeOp::EdgeStrainConstraint>(id_vector,edgelength_weight,s.getPoints());
        edge_constraint->setEdgeLength(m_length);
        s.addConstraint(edge_constraint);
    }

    //add gravity force
    {
        auto gravity_force = std::make_shared<ShapeOp::GravityForce>(ShapeOp::Vector3(0,0,-0.05));
        s.addForces(gravity_force);
    }

    //add wind force
    /*for( int i = 0 ; i < m_MV.cols() ; i++)
    {
        auto wind_force = std::make_shared<ShapeOp::VertexForce>(m_windDirection,i);
        s.addForces(wind_force);
    }*/

    for( int i = 0 ; i < m_VectorEdge.size() ; i++)
    {
        double NWind = sqrt( m_windDirection[0]*m_windDirection[0] + m_windDirection[1]*m_windDirection[1] + m_windDirection[2]*m_windDirection[2]);

        double sinT = 1;

        if( NWind != 0 )
        {
            PolyMesh::Normal EdgeVector = m_PickedMesh->calc_edge_vector(m_VectorEdge.at(i));

            ShapeOp::Vector3 cross;
            cross[0] = EdgeVector[1]*m_windDirection[2] - EdgeVector[2]*m_windDirection[1];
            cross[1] = EdgeVector[2]*m_windDirection[0] - EdgeVector[0]*m_windDirection[2];
            cross[2] = EdgeVector[0]*m_windDirection[1] - EdgeVector[1]*m_windDirection[0];

            double Ncross = sqrt( cross[0]*cross[0] + cross[1]*cross[1] + cross[1]*cross[1]);
            double NEdgeVector = sqrt( EdgeVector[0]*EdgeVector[0] + EdgeVector[1]*EdgeVector[1] + EdgeVector[2]*EdgeVector[2]);

            sinT = Ncross/( NEdgeVector*NWind );

            std::cout << "Ncross: " << Ncross << " Nedge: " << NEdgeVector << " NWind: " << NWind << " sinT: " << sinT << std::endl;
        }
        auto wind_force = std::make_shared<ShapeOp::VertexForce>(sinT*m_windDirection,i);
        s.addForces(wind_force);
    }



    // add laplacian constraint
    if( m_discretize == 0 )
    {
        for( int i = 0; i < m_LaplaceCons.size() ; i++)
        {
            std::vector<int> id_vector;
            id_vector.push_back(m_LaplaceCons.at(i).at(0));
            id_vector.push_back(m_LaplaceCons.at(i).at(1));
            id_vector.push_back(m_LaplaceCons.at(i).at(2));

            auto laplacian_constraint = std::make_shared<ShapeOp::UniformLaplacianConstraint>(id_vector,laplacian_weight,s.getPoints(),true);
            s.addConstraint(laplacian_constraint);
        }
    }
    else if( m_discretize != 0 )
    {
        for( int i = 0; i < m_LaplaceCons.size() ; i++)
        {
            std::vector<int> id_vector;
            id_vector.push_back(m_LaplaceCons.at(i).at(1));
            id_vector.push_back(m_LaplaceCons.at(i).at(0));
            id_vector.push_back(m_LaplaceCons.at(i).at(2));

            auto laplacian_constraint = std::make_shared<ShapeOp::UniformLaplacianConstraint>(id_vector,laplacian_weight,s.getPoints(),true);
            s.addConstraint(laplacian_constraint);
        }
    }

    s.initialize(false,m_masse);
    s.solve(50);
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


    save(m_IdObject,"/Users/Juju/Documents/project/files/NewShape.obj");
    PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::WIREFRAME);
    emit updatedObject(m_IdObject,UPDATE_ALL);
    PluginFunctions::viewAll();
}

//**********************************************************************************************
// Drag the fixed vertex onto new positions
//**********************************************************************************************
void mmPlugin::dragVertex()
{
    PluginFunctions::actionMode(Viewer::PickingMode);
    PluginFunctions::pickMode("DragVertex");
}

//**********************************************************************************************
// Compute the wind
//**********************************************************************************************
void mmPlugin::changeWind()
{
    m_windIntensity = windIntensitySlider->value()/10.0;

    double x = windXSlider->value();
    double y = windYSlider->value();
    double z = windZSlider->value();

    m_windDirection = ShapeOp::Vector3(x,y,z);

    m_windDirection = m_windIntensity*m_windDirection;

    std::cout << "wind: " << m_windDirection << std::endl;
}




//**********************************************************************************************
Q_EXPORT_PLUGIN2( movemeshplugin , mmPlugin );
