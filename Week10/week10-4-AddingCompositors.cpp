/** @file Week10-4-AddingCompositors
 *  @brief Adding Compositors
 *  Previously, we have created three compositors, which we are now going to add to our
 *  application with the capability to turn each one off and on using keyboard input.
 *  We also are going to need to save which compositor is turned on; add three
 *  Booleans for this task: bool _comp1, _comp2, _comp3;
 *  We are going to use keyboard input to switch the compositors on and off. To be able
 *  to differentiate between key presses, we need to know the previous state of the key:
 *  bool _down1, _down2, _down3;
 *  Use OpenGL 3+
 *  @author Hooman Salamat
 *  @bug No known bugs.
 */

#include "Ogre.h"
#include "OgreApplicationContext.h"
#include "OgreInput.h"
#include "OgreRTShaderSystem.h"
#include <OgreCompositorManager.h>
#include <iostream>

using namespace Ogre;
using namespace OgreBites;

Ogre::Vector3 translate(0, 0, 0);
float rotX = 0.0f;
float rotY = 0.0f;
float _movementspeed;

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
        _movementspeed = 200.0f;
        _mousespeed = 0.02f;
    }

    bool frameStarted(const Ogre::FrameEvent& evt)
    {
        std::cout << "Frame started" << std::endl;

        _camNode->yaw(Ogre::Radian(rotX * _mousespeed * evt.timeSinceLastFrame));
        _camNode->pitch(Ogre::Radian(rotY * _mousespeed * evt.timeSinceLastFrame));
        rotX = 0.0f;
        rotY = 0.0f;

        _sceneNode->translate(translate * evt.timeSinceLastFrame);
        translate = Ogre::Vector3(0, 0, 0);

        return true;
    }

    bool frameEnded(const Ogre::FrameEvent& evt)
    {
        
        std::cout << "Frame ended" << std::endl;
        return true;
    }

    bool frameRenderingQueued(const Ogre::FrameEvent& evt)
    {
        std::cout << "Frame queued" << std::endl;
        return true;
    }
};

class Game
    : public ApplicationContext
    , public InputListener
{
private:
    SceneManager* mScnMgr;
    Root* mRoot;
    Camera* mCamera;
    Ogre::Viewport* mViewport;
    SceneNode* mCamNode;
    bool _keepRunning;
    bool comp1, comp2, comp3;
    bool down1, down2, down3;
public:
    Game();
    virtual ~Game() {}

    void setup();
    void createScene();
    void createCamera();
    bool keyPressed(const KeyboardEvent& evt);
    bool mouseMoved(const MouseMotionEvent& e);
    void createFrameListener();
    void renderOneFrame();
    bool keepRunning();
    Ogre::SceneNode* SinbadNode;
};


Game::Game()
    : ApplicationContext("Week10-4")
{
    _keepRunning = true;
    _movementspeed = 50.0f;
    comp1 = false;
    comp2 = false;
    comp3 = false;

    down1 = false;
    down2 = false;
    down3 = false;
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
    createCamera();
    createScene();
    createFrameListener();

    Ogre::CompositorManager::getSingleton().addCompositor(mViewport, "Compositor2");
    Ogre::CompositorManager::getSingleton().addCompositor(mViewport, "Compositor3");
    Ogre::CompositorManager::getSingleton().addCompositor(mViewport, "Compositor7");
}

void Game::createScene()
{

    // -- tutorial section start --
    //! [turnlights]
    mScnMgr->setAmbientLight(ColourValue(0.5, 0.5, 0.5));


    //! [newlight]
    Light* light1 = mScnMgr->createLight("Light1");
    light1->setType(Ogre::Light::LT_DIRECTIONAL);
    // Set Light Color
    light1->setDiffuseColour(1.0f, 1.0f, 1.0f);
    // Set Light Reflective Color
    light1->setSpecularColour(1.0f, 0.0f, 0.0f);
    // Set Light (Range, Brightness, Fade Speed, Rapid Fade Speed)
    light1->setAttenuation(10, 0.5, 0.045, 0.0);

    //
    Entity* lightEnt = mScnMgr->createEntity("LightEntity", "sphere.mesh");
    SceneNode* lightNode = mScnMgr->createSceneNode("LightNode");
    //lightNode->attachObject(lightEnt);
    lightNode->attachObject(light1);
    lightNode->setScale(0.01f, 0.01f, 0.01f);


    mScnMgr->getRootSceneNode()->addChild(lightNode);
    //! [newlight]



    //! [lightpos]
    lightNode->setPosition(0, 4, 10);
    //! [lightpos]


    SinbadNode = mScnMgr->getRootSceneNode()->createChildSceneNode("Node1");
    Ogre::Entity* ent = mScnMgr->createEntity("Entity1", "Sinbad.mesh");
    //ent->setMaterial(Ogre::MaterialManager::getSingleton().getByName("MyMaterial18"));
    SinbadNode->attachObject(ent);
}

void Game::createCamera()
{

    //! [camera]
    mCamNode = mScnMgr->getRootSceneNode()->createChildSceneNode();

    // create the camera
    mCamera = mScnMgr->createCamera("myCam");
    mCamera->setNearClipDistance(5); // specific to this sample
    mCamera->setAutoAspectRatio(true);
    mCamNode->attachObject(mCamera);
    mCamNode->setPosition(0, 0, 25);
    mCamNode->lookAt(Ogre::Vector3(0, 0, 0), Node::TS_LOCAL);

    // and tell it to render into the main window
    mViewport = getRenderWindow()->addViewport(mCamera);
    mViewport->setBackgroundColour(Ogre::ColourValue(0.0, 0.0, 0.0));
    mCamera->setAspectRatio(Ogre::Real(mViewport->getActualWidth()) / Ogre::Real(mViewport->getActualHeight()));
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
        _keepRunning = false;
        break;
    case 'w':
        translate = Ogre::Vector3(0, 10, 0);
        break;
    case 's':
        translate = Ogre::Vector3(0, -10, 0);
        break;
    case 'a':
        translate = Ogre::Vector3(-10, 0, 0);
        break;
    case 'd':
        translate = Ogre::Vector3(10, 0, 0);
        break;
    case '1':
        down1 = true;
        down2 = false;
        down3 = false;
        comp1 = !comp1;
        Ogre::CompositorManager::getSingleton().setCompositorEnabled(mViewport, "Compositor2", comp1);
        break;
    case '2':
        down2 = true;
        down1 = false;
        down3 = false;
        comp2 = !comp2;
        Ogre::CompositorManager::getSingleton().setCompositorEnabled(mViewport, "Compositor3", comp2);
        break;
    case '3':
        down3 = true;
        down1 = false;
        down2 = false;
        comp3 = !comp3;
        Ogre::CompositorManager::getSingleton().setCompositorEnabled(mViewport, "Compositor7", comp3);
        break;
    default:
        break;
    }
    return true;
}

void Game::createFrameListener()
{
    Ogre::FrameListener* FrameListener = new ExampleFrameListener(SinbadNode, mCamNode);
    mRoot->addFrameListener(FrameListener);
}

void Game::renderOneFrame()
{
    //Ogre::WindowEventUtilities::messagePump();
    mRoot->renderOneFrame();
}

bool Game::keepRunning()
{
    return _keepRunning;
}


int main(int argc, char** argv)
{
    try
    {
        Game app;
        app.initApp();
        //app.getRoot()->startRendering();
        while (app.keepRunning())
        {
            app.renderOneFrame();
        }
        app.closeApp();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error occurred during execution: " << e.what() << '\n';
        return 1;
    }

    return 0;
}










