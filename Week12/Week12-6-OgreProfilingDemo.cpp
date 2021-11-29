/** @file Week12-6-OgreProfilingDemo
 *  @brief Profiling Demo using ogre profiling
 *  The profiler allows you to measure the performance of your code.
 *  If you are using the Ogre SDK and you want to use the Profiler, it is advisable to switch to the source code version of Ogre,
 *  because the SDK is shipped with OGRE_PROFILING=OFF, so Profiling is disabled by default
 *  How to read the profile:
 *  On the left side are the profile names with a number in parentheses next to it. 
 *  This number is the number of times this profile was called during this frame. 
 *  If this number is 0, that means that the profile was called before, but is not being called currently. 
 *  The bars on the left represent the frame time statistics. You can see indicators above which show that a profile can take anywhere 
 *  from 0% to 100% of the frame time. 
 *  The big yellow bars show the current frame percentage that the profile is taking. 
 *  The green line shows the minimum frame time, the red line is the maximum frame time, and the blue line is the average frame time. 
 *  Big discrepancies between the average and maximum can possibly be the sign of a performance bottleneck (however it could be the profiler acting strangely). 
 *  These results will be printed to the log when the application ends or you can manually do it by calling logResults().
 *  @author Hooman Salamat
 *  @bug No known bugs.
 */

#include "Ogre.h"
#include "OgreApplicationContext.h"
#include "OgreInput.h"
#include "OgreRTShaderSystem.h"
#include "OgreTrays.h"
#include "ConsoleColor.h"
#include <iostream>
using namespace std;
using namespace Ogre;
using namespace OgreBites;

Ogre::Vector3 translate(0, 0, 0);
float rotX = 0.0f;
float rotY = 0.0f;

class ExampleFrameListener : public Ogre::FrameListener
{
private:
    Ogre::SceneNode* _sceneNode;
    Ogre::SceneNode* _camNode;
    float _movementspeed;
    float _mousespeed;
public:

    ExampleFrameListener(Ogre::SceneNode* sceneNode, Ogre::SceneNode* camNode)
    {
        _sceneNode = sceneNode;
        _camNode = camNode;
        _movementspeed = 2.0f;
        _mousespeed = 0.002f;
    }

    bool frameStarted(const Ogre::FrameEvent& evt)
    {

        //!step1: The profiler will need you to specify the main program loop.To do this, you call the following at the very beginning of the 
        //frameStarted() function in your FrameListener class :
        OgreProfileBegin("Ogre Main Loop");

        float rotXNew = rotX * evt.timeSinceLastFrame * -1;
        float rotYNew = rotY * evt.timeSinceLastFrame * -1;

        //Stop camera movement
        //_camNode->yaw(Ogre::Radian(rotXNew * _mousespeed));
        //_camNode->pitch(Ogre::Radian(rotYNew * _mousespeed));
        //_camNode->translate(translate * evt.timeSinceLastFrame * _movementspeed);

        return true;
    }

    bool frameEnded(const Ogre::FrameEvent& evt)
    {
        //!step2: Add this at the very end of the frameEnded() function:
        //!Make sure the names match exactly, otherwise the profiler will fail an assert. Next you want to profile some of your code. 
        OgreProfileEnd("Ogre Main Loop");
        return true;
    }
};

class Game
    : public ApplicationContext
    , public InputListener
{
private:
    SceneNode* SinbadNode;
    SceneManager* mScnMgr;
    Root* mRoot;
    Ogre::PolygonMode mPolyMode;
    Camera* cam;
    SceneNode* camNode;
public:
    Game();
    virtual ~Game() {}

    void setup();
    bool keyPressed(const KeyboardEvent& evt);
    void createScene();
    void createCamera();
    bool mouseMoved(const MouseMotionEvent& evt);
    void createFrameListener();
};


Game::Game()
    : ApplicationContext("Week12-6-OgreProfilingDemo")
{
}


void Game::setup()
{
    // do not forget to call the base first
    ApplicationContext::setup();
    addInputListener(this);

    // get a pointer to the already created root
    mRoot = getRoot();
    mScnMgr = mRoot->createSceneManager();


    // register our scene with the RTSS
    RTShader::ShaderGenerator* shadergen = RTShader::ShaderGenerator::getSingletonPtr();
    shadergen->addSceneManager(mScnMgr);

    mPolyMode = Ogre::PolygonMode::PM_SOLID;

    //!step3: you must add this in order to add a tray
    mScnMgr->addRenderQueueListener(mOverlaySystem);
    // Give the profiler a timer and enable it
    Ogre::Profiler::getSingleton().setTimer(Root::getSingleton().getTimer());
    Ogre::Profiler::getSingleton().setEnabled(true);
    //You can change how frequently the display is updated to suit your tastes like this
    Ogre::Profiler::getSingleton().setUpdateDisplayFrequency(100);

    //!step4: Next you want to profile some of your code. You do this by calling OgreProfile() and using braces ({}) to limit the scope.
    //!Note that OgreProfile cannot be called in the same scope as another OgreProfile().
    
    //you have to add this..there is a bug, so system doesn't the first profile
    {
        OgreProfile("Ogre Profile");
    }
    {
        OgreProfile("Scene Profile");
        createScene();
    }
    {
        OgreProfile("Camera Profile");
        createCamera();
    }
    {
        OgreProfile("Frame Profile");
        createFrameListener();
    }
}

void Game::createScene()
{
    // -- tutorial section start --

    Ogre::SceneNode* node = mScnMgr->createSceneNode("Node1");
    mScnMgr->getRootSceneNode()->addChild(node);

    //! [turnlights]
    mScnMgr->setAmbientLight(ColourValue(0.5, 0.5, 0.5));
    mScnMgr->setShadowTechnique(ShadowTechnique::SHADOWTYPE_STENCIL_ADDITIVE);
    //! [turnlights]

    //! [newlight]
    //
    Light* light1 = mScnMgr->createLight("Light1");
    light1->setType(Ogre::Light::LT_DIRECTIONAL);
    // Set Light Color
    light1->setDiffuseColour(1.0f, 1.0f, 1.0f);
    // Set Light Reflective Color
    light1->setSpecularColour(1.0f, 1.0f, 0.0f);
    // Set Light (Range, Brightness, Fade Speed, Rapid Fade Speed)
    //light1->setAttenuation(10, 0.5, 0.045, 0.0);

    //
    Entity* lightEnt = mScnMgr->createEntity("LightEntity", "sphere.mesh");
    SceneNode* lightNode = node->createChildSceneNode("LightNode");
    lightNode->attachObject(lightEnt);
    lightNode->attachObject(light1);
    lightNode->setScale(0.01f, 0.01f, 0.01f);

    //! [newlight]



    //! [lightDirection]
    lightNode->setDirection(1, -1, 0);
    //! [lightDirection]




    //The first thing we'll do is create an abstract Plane object. This is not the mesh, it is more of a blueprint.
    Plane plane(Vector3::UNIT_Y, -10);
    //Now we'll ask the MeshManager to create us a mesh using our Plane blueprint. The MeshManager is already keeping track of the resources we loaded when initializing our application. On top of this, it can create new meshes for us.
    MeshManager::getSingleton().createPlane(
        "ground", RGN_DEFAULT,
        plane,
        1500, 1500, 200, 200,
        true,
        1, 5, 5,
        Vector3::UNIT_Z);

    //Now we will create a new Entity using this mesh.
    //We want to tell our SceneManager not to cast shadows from our ground Entity. It would just be a waste. Don't get confused, this means the ground won't cast a shadow, it doesn't mean we can't cast shadows on to the ground.
    Entity* groundEntity = mScnMgr->createEntity("ground");
    mScnMgr->getRootSceneNode()->createChildSceneNode()->attachObject(groundEntity);
    groundEntity->setCastShadows(false);
    //And finally we need to give our ground a material.
    groundEntity->setMaterialName("Examples/BeachStones");


    Entity* Sinbad = mScnMgr->createEntity("Sinbad.mesh");
    Sinbad->setCastShadows(true);
    SceneNode* SinbadNode = mScnMgr->createSceneNode("SinbadNode");
    SinbadNode->attachObject(Sinbad);
    mScnMgr->getRootSceneNode()->addChild(SinbadNode);
    SinbadNode->setScale(3.0f, 3.0f, 3.0f);
    SinbadNode->setPosition(0, 4.0, 0);

    // -- tutorial section end --
}

void Game::createCamera()
{
    //! [camera]
    camNode = mScnMgr->getRootSceneNode()->createChildSceneNode();

    // create the camera
    cam = mScnMgr->createCamera("myCam");
    cam->setNearClipDistance(5); // specific to this sample
    cam->setAutoAspectRatio(true);
    camNode->attachObject(cam);
    camNode->setPosition(0, 100, 200);
    camNode->lookAt(Ogre::Vector3(0, 0, 0), Node::TS_WORLD);
    //cam->setPolygonMode(Ogre::PM_WIREFRAME);

    // and tell it to render into the main window
    getRenderWindow()->addViewport(cam);
}


void Game::createFrameListener()
{
    Ogre::FrameListener* FrameListener = new ExampleFrameListener(SinbadNode, camNode);
    mRoot->addFrameListener(FrameListener);
}


bool Game::mouseMoved(const MouseMotionEvent& evt)
{
    rotX = evt.xrel;
    rotY = evt.yrel;
    return true;
}

bool Game::keyPressed(const KeyboardEvent& evt)
{

    switch (evt.keysym.sym)
    {
    case SDLK_ESCAPE:
        getRoot()->queueEndRendering();
        break;
    case 'w':
        translate = Ogre::Vector3(0, 0, -1);
        break;
    case 's':
        translate = Ogre::Vector3(0, 0, 1);
        break;
    case 'a':
        translate = Ogre::Vector3(-1, 0, 0);
        break;
    case 'd':
        translate = Ogre::Vector3(1, 0, 0);
        break;
    case 'p':
        translate = Ogre::Vector3(0, 0, 0);
        break;
    case ' ':
        if (mPolyMode == Ogre::PolygonMode::PM_SOLID)
            mPolyMode = Ogre::PolygonMode::PM_WIREFRAME;
        else
            mPolyMode = Ogre::PolygonMode::PM_SOLID;
        cam->setPolygonMode(mPolyMode);
        break;
    default:
        break;
    }
    return true;
}


int main(int argc, char** argv)
{
    try
    {
        Game app;
        app.initApp();
        app.getRoot()->startRendering();
        app.closeApp();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error occurred during execution: " << e.what() << '\n';
        return 1;
    }

    return 0;
}

//! [fullsource]
