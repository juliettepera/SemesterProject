#include "mmPlugin.hh"

void mmPlugin::initializePlugin()
{
    //**********************************************************************************************
    m_IdObject = -1;
    m_FixPoint = 4;
    m_discretize = 0;
    m_sizeX = 5;
    m_sizeY = 5;
    m_vertices = m_sizeX*m_sizeY;
    m_edges = (m_sizeX-1)*m_sizeY + (m_sizeY-1)*m_sizeX;
    m_faces = (m_sizeX - 1)*(m_sizeY - 1);
    m_idFixed.clear();
    m_posFixed.clear();
    m_hitPoint = OpenMesh::Vec3d(0,0,0);
    m_vh0.clear();
    m_fphandles.clear();

    m_pickMode = 0;
    m_dragMode = 0;
    m_dragedVertex = 0;

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
    discretizeSpin->setMaximum(1);
    discretizeSpin->setValue(0);

    loadButton = new QPushButton("&Load",toolBox);
    loadButton->setDisabled(true);

    pickButton = new QPushButton("&Pick",toolBox);
    pickButton->setDisabled(true);

    solveButton = new QPushButton("&Solve",toolBox);
    solveButton->setDisabled(true);

    dragButton = new QPushButton("&Drag",toolBox);
    dragButton->setDisabled(true);

    fixPointSpin = new QSpinBox(toolBox);
    fixPointSpin->setMinimum(1);
    fixPointSpin->setMaximum(20);
    fixPointSpin->setValue(4);
    fixPointSpin->setDisabled(true);

    QGridLayout* layout = new QGridLayout(toolBox);

    layout->addWidget( sizeXSpin, 1, 1);
    layout->addWidget( sizeYSpin, 1, 2);
    layout->addWidget( discretizeSpin, 1, 3);

    layout->addWidget( loadButton, 2, 1);

    layout->addWidget( fixPointSpin, 3, 1);
    layout->addWidget( pickButton, 3, 2);

    layout->addWidget( solveButton , 4, 1);
    layout->addWidget( dragButton , 4, 2);

    layout->addItem(new QSpacerItem(10,10,QSizePolicy::Expanding,QSizePolicy::Expanding),2,0,1,2);

    //**********************************************************************************************
    connect( sizeXSpin , SIGNAL(valueChanged(int)) , this, SLOT(changeXYValue()));
    connect( sizeYSpin , SIGNAL(valueChanged(int)) , this, SLOT(changeXYValue()));
    connect( discretizeSpin , SIGNAL(valueChanged(int)) , this, SLOT(changeXYValue()));

    connect( loadButton, SIGNAL(clicked()), this, SLOT(addQuadrimesh()));

    connect( pickButton, SIGNAL(clicked()), this, SLOT(pickVertex()));
    connect( fixPointSpin , SIGNAL(valueChanged(int)) , this, SLOT(changeFixPointValue()));

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
    m_IdObject = -1;
    m_FixPoint = 4;
    m_discretize = 0;
    m_sizeX = 5;
    m_sizeY = 5;
    m_vertices = m_sizeX*m_sizeY;
    m_edges = (m_sizeX-1)*m_sizeY + (m_sizeY-1)*m_sizeX;
    m_faces = (m_sizeX - 1)*(m_sizeY - 1);
    m_idFixed.clear();
    m_posFixed.clear();
    m_hitPoint = OpenMesh::Vec3d(0,0,0);
    m_vh0.clear();
    m_fphandles.clear();

    m_pickMode = 0;
    m_dragMode = 0;
    m_dragedVertex = 0;

    sizeXSpin->setValue(5);
    sizeYSpin->setValue(5);
    fixPointSpin->setValue(4);
    discretizeSpin->setValue(0);

    loadButton->setEnabled(true);
    sizeXSpin->setEnabled(true);
    sizeYSpin->setEnabled(true);
    discretizeSpin->setEnabled(true);


    fixPointSpin->setDisabled(true);
    pickButton->setDisabled(true);
    solveButton->setDisabled(true);
    dragButton->setDisabled(true);    
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

            PolyMesh::VertexHandle vh0, vh1, vhi, vprev;
            PolyMesh::Point p0, p1, pi;

            // laplacian constraint
            int nbLaplacian = ((m_sizeX+(m_sizeX-1)*m_discretize)-2)*m_sizeY + ((m_sizeY+(m_sizeY-1)*m_discretize)-2)*m_sizeX;
            m_ML.resize(nbLaplacian,3);
            m_ML.setConstant(-1);
            int nbL = 0;
            int nbI = 0;

            for( e_it = m_PickedMesh->edges_begin() ; e_it != e_end ; e_it++ )
            {
                he = *e_it;

                he_01 = m_PickedMesh->halfedge_handle(he,0);
                he_10 = m_PickedMesh->halfedge_handle(he,1);

                vh0 = m_PickedMesh->from_vertex_handle(he_01);
                m_PickedMesh->property(m_list_vertex,he_01).push_back(vh0);

                nbI = 0;
                m_ML(nbL,nbI) = vh0.idx();
                std::cout << "m_ML(" << nbL << "," << nbI << ") = " << m_ML(nbL,nbI) << std::endl;
                nbI++;

                vh1 = m_PickedMesh->to_vertex_handle(he_01);

                p0 = m_PickedMesh->point(vh0);
                p1 = m_PickedMesh->point(vh1);

                for( int i = 0 ; i < m_discretize ; i++ )
                {
                    double a = double(i+1)/double(m_discretize+1);
                    pi = a*p0 + (1-a)*p1;
                    vhi = newMesh->add_vertex(pi);
                    m_PickedMesh->property(m_list_vertex,he_01).push_back(vhi);

                    if( nbI <3 )
                    {
                        m_ML(nbL,nbI) = vhi.idx();
                        std::cout << "m_ML(" << nbL << "," << nbI << ") = " << m_ML(nbL,nbI) << std::endl;
                        nbI++;
                    }
                    else
                    {
                        nbL++;
                        nbI = 0;
                        m_ML(nbL,nbI) = vprev.idx();
                        std::cout << "m_ML(" << nbL << "," << nbI << ") = " << m_ML(nbL,nbI) << std::endl;
                        nbI++;

                        m_ML(nbL,nbI) = vhi.idx();
                        std::cout << "m_ML(" << nbL << "," << nbI << ") = " << m_ML(nbL,nbI) << std::endl;
                        nbI++;
                    }

                    vprev = vhi;
                }
                m_PickedMesh->property(m_list_vertex,he_01).push_back(vh1);

                m_ML(nbL,nbI) = vh1.idx();
                std::cout << "m_ML(" << nbL << "," << nbI << ") = " << m_ML(nbL,nbI) << std::endl;
                nbL++;

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
                    m_posFixed[i] = hitPoint;
                }
            }

            m_dragMode = 2;
            m_dragedVertex = -1;

            emit updatedObject(m_IdObject, UPDATE_GEOMETRY);
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

            solveButton->setEnabled(true);
            dragButton->setEnabled(true);

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
    m_dragedVertex = -1;
}

//**********************************************************************************************
// Solve the Optimization
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
    m_ML.setZero();

    m_ME.resize(m_edges,2);
    m_ML.setZero();

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

    // GET THE LAPLACIAN VERTICES

    if( m_discretize == 0 )
    {
        int nbLaplacian = (m_sizeX-2)*m_sizeY + (m_sizeY-2)*m_sizeX;
        m_ML.resize(nbLaplacian,3);
        m_ML.setConstant(-1);

        int nbL = 0;
        int nbI = 0;
        int k = 0;

        for( int j = 0 ; j < m_sizeY ; j++ )
        {
            for( int i = 1 ; i < m_sizeX-1 ; i++ )
            {
                k = i+j*m_sizeX;
                nbI = 0;
                m_ML(nbL,nbI) = m_vh0[k-1].idx();
                std::cout << "m_ML(" << nbL << "," << nbI << ") = " << m_ML(nbL,nbI) << std::endl;
                nbI++;
                m_ML(nbL,nbI) = m_vh0[k].idx();
                std::cout << "m_ML(" << nbL << "," << nbI << ") = " << m_ML(nbL,nbI) << std::endl;
                nbI++;
                m_ML(nbL,nbI) = m_vh0[k+1].idx();
                std::cout << "m_ML(" << nbL << "," << nbI << ") = " << m_ML(nbL,nbI) << std::endl;
                nbL++;
            }
        }

        for( int i = 0 ; i < m_sizeX ; i++ )
        {
            for( int j = 1 ; j < m_sizeY-1 ; j++ )
            {
                k = i+j*m_sizeX;
                nbI = 0;
                m_ML(nbL,nbI) = m_vh0[k-m_sizeX].idx();
                std::cout << "m_ML(" << nbL << "," << nbI << ") = " << m_ML(nbL,nbI) << std::endl;
                nbI++;
                m_ML(nbL,nbI) = m_vh0[k].idx();
                std::cout << "m_ML(" << nbL << "," << nbI << ") = " << m_ML(nbL,nbI) << std::endl;
                nbI++;
                m_ML(nbL,nbI) = m_vh0[k+m_sizeX].idx();
                std::cout << "m_ML(" << nbL << "," << nbI << ") = " << m_ML(nbL,nbI) << std::endl;
                nbL++;
            }
        }
    }
}

void mmPlugin::solveShape()
{
    ShapeOp::Solver s;
    s.setPoints(m_MV);
    ShapeOp::Scalar edgelength_weight = 200;
    ShapeOp::Scalar closeness_weight = 200;
    ShapeOp::Scalar laplacian_weight = 200;

    //add a closeness constraint to the fixed vertex.
    std::cout << "set closseness contraint " << std::endl;
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
    std::cout << "set edges constraint " << std::endl;
    for( int i = 0 ; i < m_ME.rows() ; i++)
    {
        std::vector<int> id_vector;
        id_vector.push_back(m_ME(i,0));
        id_vector.push_back(m_ME(i,1));
        auto edge_constraint = std::make_shared<ShapeOp::EdgeStrainConstraint>(id_vector,edgelength_weight,s.getPoints());
        s.addConstraint(edge_constraint);
    }

    //add gravity constraint
    std::cout << "set gravity constraint " << std::endl;
    {
        auto gravity_force = std::make_shared<ShapeOp::GravityForce>(ShapeOp::Vector3(0,0,-0.05));
        s.addForces(gravity_force);
    }


    // add laplacian constraint
    if( m_discretize == 0 )
    {
        for( int i = 0; i < m_ML.rows() ; i++)
        {
            std::vector<int> id_vector;
            if( m_ML(i,0) != -1 && m_ML(i,1) != -1 && m_ML(i,2) != -1 )
            {
                id_vector.push_back(m_ML(i,0));
                id_vector.push_back(m_ML(i,1));
                id_vector.push_back(m_ML(i,2));
            }

            auto laplacian_constraint = std::make_shared<ShapeOp::UniformLaplacianConstraint>(id_vector,laplacian_weight,s.getPoints(),true);
            s.addConstraint(laplacian_constraint);
        }
    }
    else
    {
        for( int i = 0; i < m_ML.rows() ; i++)
        {
            std::vector<int> id_vector;
            if( m_ML(i,0) != -1 && m_ML(i,1) != -1 && m_ML(i,2) != -1 )
            {
                id_vector.push_back(m_ML(i,1));
                id_vector.push_back(m_ML(i,0));
                id_vector.push_back(m_ML(i,2));
            }

            auto laplacian_constraint = std::make_shared<ShapeOp::UniformLaplacianConstraint>(id_vector,laplacian_weight,s.getPoints(),true);
            s.addConstraint(laplacian_constraint);
        }
    }

    s.initialize(false);
    s.solve(1000);
    m_MV = s.getPoints();

}

void mmPlugin::setNewPositions()
{     
    PolyMesh::VertexIter v_it;
    PolyMesh::VertexIter v_end = m_PickedMesh->vertices_end();
    OpenMesh::Vec3d NewPoint;

    int nbV = 0;
    std::cout << "set new vertex " << std::endl;
    for( v_it = m_PickedMesh->vertices_begin(); v_it != v_end; v_it++ )
    {
        NewPoint[0] = m_MV(0,nbV);
        NewPoint[1] = m_MV(1,nbV);
        NewPoint[2] = m_MV(2,nbV);
        m_PickedMesh->set_point(*v_it,NewPoint);
        nbV++;
    }


    std::cout << "save file" << std::endl;
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
Q_EXPORT_PLUGIN2( movemeshplugin , mmPlugin );
