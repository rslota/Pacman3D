#include "make_ref.h"
#include "InfoVisitor.h"
#include "board_def.h"
#include "Board.h"
#include "NPC.h"
#include "GhostFactory.h"
#include "FPSManipulator.h"

#include <iostream>
#include <thread>

#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osgViewer/Viewer>
#include <osg/MatrixTransform>
#include <osg/Fog>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/OrbitManipulator>
#include <osg/Texture2D>

bool loadShaderSource(osg::Shader* obj, const std::string& fileName )
{
    std::string fqFileName = osgDB::findDataFile(fileName);
    if( fqFileName.length() == 0 )
    {
        std::cout << "File \"" << fileName << "\" not found." << std::endl;
        return false;
    }
    bool success = obj->loadShaderSourceFromFile( fqFileName.c_str());
    if ( !success  )
    {
        std::cout << "Couldn't load file: " << fileName << std::endl;
        return false;
    }
    else
    {
        return true;
    }
}


int main(int argc, char** argv)
{
    osg::ArgumentParser arguments( &argc, argv );

    std::string dbPath;
    arguments.read("--db_path", dbPath);

    std::srand ( unsigned ( std::time(0) ) );

    auto board = Board(boardDefinition, boardSizeX, boardSizeY, dbPath);
    auto ghostFactory = GhostFactory();

    auto main_obj = make_ref<osg::Group>();
    main_obj->addChild(board.draw().get());

    auto ghostModel = osgDB::readNodeFile(dbPath + "/cow.osg");
    auto ghostCount = 16;
    while(ghostCount--)
    {
        main_obj->addChild(ghostFactory.drawGhost(board, ghostModel).get());
    }

    // init rotate
    auto init_rotate = make_ref<osg::MatrixTransform>();
    init_rotate->setMatrix( osg::Matrix::rotate(osg::PI * 2, osg::Vec3(1.0f, 0.0f, 0.0f)) );

    // chain rotates
    init_rotate->addChild(main_obj);

    // Root group
    auto root = make_ref<osg::Group>();
    root->addChild(init_rotate);

    // Setup fog
    osg::ref_ptr<osg::Fog> fog = new osg::Fog;
    fog->setMode( osg::Fog::EXP );
    fog->setStart( 0.0f );
    fog->setEnd(board.getFieldSizeX() * 20);
    fog->setDensity(0.03);
    fog->setColor( osg::Vec4(0.5, 0.5, 0.5, 1.0) );

    //root->getOrCreateStateSet()->setAttributeAndModes(fog.get());

    // Print node graph
    InfoVisitor info;
    root->accept(info);

    // Start viewer
    osgViewer::Viewer viewer;

    double height = std::min(board.getFieldSizeX(), board.getFieldSizeY()) / 1.5;

    auto fpsManipulator = make_ref<FPSManipulator>();
    fpsManipulator->setHomePosition(
        osg::Vec3d(board.getFieldCenterX(1), board.getFieldCenterY(10), height),
        osg::Vec3d(0.0f, 0.0f, height),
        osg::Vec3d(0.0f, 0.0f, 1.0f)
    );


    auto keySwitch = make_ref<osgGA::KeySwitchMatrixManipulator>();
    keySwitch->addNumberedMatrixManipulator(make_ref<osgGA::OrbitManipulator>());
    keySwitch->addNumberedMatrixManipulator(fpsManipulator);
    viewer.setCameraManipulator(keySwitch);

    viewer.home();


    osg::Geode* WallGeode = new osg::Geode();
    osg::Geometry* WallGeometry = new osg::Geometry();

    WallGeode->addDrawable(WallGeometry);
    //specify vertices
    osg::Vec3dArray* WallVertices = new osg::Vec3dArray;

    uint32_t lod = 100;
    double totalSizeX = 16.0;
    double totalSizeY = 16.0;
    double totalSizeZ = 16.0;

    double sizeX = totalSizeX / lod;
    double sizeY = totalSizeY / lod;
    double sizeZ = totalSizeZ / lod;

    for(int i = 0; i < lod; ++i) {
        for(int j = 0; j < lod; ++j) {
            WallVertices->push_back(osg::Vec3d(0, j * sizeY, i * sizeZ));
            WallVertices->push_back(osg::Vec3d(0, j * sizeY, i * sizeZ + sizeZ));
            WallVertices->push_back(osg::Vec3d(0, j * sizeY + sizeY, i * sizeZ + sizeZ));
            WallVertices->push_back(osg::Vec3d(0, j * sizeY + sizeY, i * sizeZ));
        }
    }

    for(int i = 0; i < lod; ++i) {
        for(int j = 0; j < lod; ++j) {
            WallVertices->push_back(osg::Vec3d(totalSizeX, j * sizeY, i * sizeZ));
            WallVertices->push_back(osg::Vec3d(totalSizeX, j * sizeY, i * sizeZ + sizeZ));
            WallVertices->push_back(osg::Vec3d(totalSizeX, j * sizeY + sizeY, i * sizeZ + sizeZ));
            WallVertices->push_back(osg::Vec3d(totalSizeX, j * sizeY + sizeY, i * sizeZ));
        }
    }

    for(int i = 0; i < lod; ++i) {
        for(int j = 0; j < lod; ++j) {
            WallVertices->push_back(osg::Vec3d(i * sizeX, 0, j * sizeZ));
            WallVertices->push_back(osg::Vec3d(i * sizeX + sizeX, 0, j * sizeZ));
            WallVertices->push_back(osg::Vec3d(i * sizeX + sizeX, 0, j * sizeZ + sizeZ));
            WallVertices->push_back(osg::Vec3d(i * sizeX, 0, j * sizeZ + sizeZ));
        }
    }

    for(int i = 0; i < lod; ++i) {
        for(int j = 0; j < lod; ++j) {
            WallVertices->push_back(osg::Vec3d(i * sizeX, totalSizeZ, j * sizeZ));
            WallVertices->push_back(osg::Vec3d(i * sizeX + sizeX, totalSizeZ, j * sizeZ));
            WallVertices->push_back(osg::Vec3d(i * sizeX + sizeX, totalSizeZ, j * sizeZ + sizeZ));
            WallVertices->push_back(osg::Vec3d(i * sizeX, totalSizeZ, j * sizeZ + sizeZ));
        }
    }

//    WallVertices->push_back( osg::Vec3d(0,0,0)); // front left
//    WallVertices->push_back( osg::Vec3d(16,0,0) ); // front right
//    WallVertices->push_back( osg::Vec3d(16,16,0) ); // back right
//    WallVertices->push_back( osg::Vec3d(0,16,0) ); // back left
//    WallVertices->push_back( osg::Vec3d(0,0,16)); // front left
//    WallVertices->push_back( osg::Vec3d(16,0,16) ); // front right
//    WallVertices->push_back( osg::Vec3d(16,16,16) ); // back right
//    WallVertices->push_back( osg::Vec3d(0,16,16) ); // back left
    WallGeometry->setVertexArray( WallVertices );

    //specify the kind of geometry we want to draw here
    osg::DrawElementsUInt* WallBase = new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
    for(int i = 0; i < lod * lod * 4 * 4; ++i)
        WallBase->push_back(i);
    //specify the order we want to draw the base of our geometry
//    WallBase->push_back(0);
//    WallBase->push_back(1);
//    WallBase->push_back(2);
//    WallBase->push_back(3);
//    // 0 3        4 7
//    WallBase->push_back(4);                 // 1 2        5 6
//    WallBase->push_back(5);
//    WallBase->push_back(6);
//    WallBase->push_back(7);

//    WallBase->push_back(0);
//    WallBase->push_back(1);
//    WallBase->push_back(5);
//    WallBase->push_back(4);
//
//    WallBase->push_back(2);
//    WallBase->push_back(3);
//    WallBase->push_back(7);
//    WallBase->push_back(6);
////
////
//    WallBase->push_back(0);
//    WallBase->push_back(3);
//    WallBase->push_back(7);
//    WallBase->push_back(4);
//
//
//    WallBase->push_back(1);
//    WallBase->push_back(2);
//    WallBase->push_back(6);
//    WallBase->push_back(5);
    WallGeometry->addPrimitiveSet(WallBase);


    osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array;

    texcoords->push_back( osg::Vec2(0.0f, 1.0f) );
    texcoords->push_back( osg::Vec2(1.0f, 1.0f) );
    texcoords->push_back( osg::Vec2(1.0f, 0.0f) );
    texcoords->push_back( osg::Vec2(0.0f, 0.0f) );


    texcoords->push_back( osg::Vec2(1.0f, 1.0f) );
    texcoords->push_back( osg::Vec2(0.0f, 1.0f) );
    texcoords->push_back( osg::Vec2(0.0f, 0.0f) );
    texcoords->push_back( osg::Vec2(1.0f, 0.0f) );
    WallGeometry->setTexCoordArray( 0, texcoords.get() );
//
//
    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
    osg::ref_ptr<osg::Image> image = osgDB::readImageFile( "/Users/RoXeon/Projects/Pacman3D/db/wall3_small.bmp" );
    texture->setImage( image.get() );

//    WallGeode->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture.get() );

    auto translate = make_ref<osg::MatrixTransform>(osg::Matrix::translate(50, 40, 0));
    translate->addChild(WallGeode);

    viewer.setSceneData( root );

    osgViewer::Viewer::Windows windows;
    viewer.getWindows(windows);
    viewer.getCamera()->setClearColor(osg::Vec4{0, 0, 0, 0});

    viewer.getCamera()->getView()->setLightingMode(osg::View::HEADLIGHT);
    auto defaultLight = viewer.getCamera()->getView()->getLight();
    defaultLight->setDiffuse(osg::Vec4(1, 1, 1, 1));
    defaultLight->setAmbient(osg::Vec4(1, 1, 1, 1));
    defaultLight->setSpecular(osg::Vec4(1, 1, 1, 1));

    auto lightSource = make_ref<osg::LightSource>();
    lightSource->setReferenceFrame(osg::LightSource::ABSOLUTE_RF);
    auto light = lightSource->getLight();
    light->setPosition(osg::Vec4{0, 0, 0, 1});
    light->setDirection(osg::Vec3{0, 1, 0});

    light->setLightNum(2);
    light->setSpotExponent(2);
    light->setSpotCutoff(osg::DegreesToRadians(25.));
    light->setDiffuse(osg::Vec4(1, 1, 1, 1));
    light->setAmbient(osg::Vec4(0.2, 0.2, 0.2, 1));
    light->setSpecular(osg::Vec4(1, 1, 1, 1));
    light->setDirection(osg::Vec3(0, -1, 0));
    light->setAmbient(osg::Vec4(0, 0, 0, 1));
    light->setPosition(osg::Vec4(0, 1, 1, 1));

    root->addChild(lightSource);

    auto program = make_ref<osg::Program>();
    auto fragmentObject = make_ref<osg::Shader>(osg::Shader::FRAGMENT);
    loadShaderSource(fragmentObject, dbPath + "/shader.frag");
    auto vertexObject = make_ref<osg::Shader>(osg::Shader::VERTEX);
    loadShaderSource(vertexObject, dbPath + "/shader.vert");
    //program->addShader(vertexObject);
    //program->addShader(fragmentObject);
    root->getOrCreateStateSet()->setAttributeAndModes(program, osg::StateAttribute::ON);

    for(osgViewer::GraphicsWindow *window: windows)
    {
        window->useCursor(false);
    }

    return viewer.run();
}
