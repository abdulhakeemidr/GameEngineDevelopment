/** @file Week14-6-PrintingAnimationsDemo
 *  @brief print all animations of a model into the console
 *  @attention we are using SliceHorizontal animation state as our second animation
 *  @note Arrows keys to move Sinbad
 *  @note WASD to move the camera, P to stop, space bar to see in th wireframe
 *  @author Hooman Salamat
 *  @bug No known bugs.
 */

#include "Ogre.h"
#include "OgreApplicationContext.h"
#include "OgreInput.h"
#include "OgreRTShaderSystem.h"
#include <iostream>
using namespace std;
using namespace Ogre;
using namespace OgreBites;

Ogre::Vector3 translate(0, 0, 0);
float rotX = 0.0f;
float rotY = 0.0f;

bool walked = false;
Ogre::Vector3 NinjaTranslate(0, 0, 0);
//we want to move at 50 units per second 
float WalkingSpeed = 50.0f;
float NinjaRotation = 0.0f;

class ExampleFrameListener : public Ogre::FrameListener
{
private:
    Ogre::SceneNode* _sceneNode;
    Ogre::SceneNode* _camNode;
    float _movementspeed;
    float _mousespeed;
    Ogre::Entity* _ent;
    Ogre::AnimationState* _aniState;
    Ogre::AnimationState* _aniStateTop;

public:

    ExampleFrameListener(Ogre::SceneNode* sceneNode, Ogre::Entity* ent, Ogre::SceneNode* camNode)
    {
        _sceneNode = sceneNode;
        _camNode = camNode;
        _movementspeed = 2.0f;
        _mousespeed = 0.002f;
       
        _ent = ent;
        _aniState = _ent->getAnimationState("Attack1");
        _aniState->setEnabled(true);
        _aniState->setLoop(false);

        //Attack1,Attack2, Attack3, Backflip, Block, Climb, Crouch, Death1, Death2, HighJump, Idle1, Idle2, Idle
        //Jump, JumpNoHeight, Kick, SideKick, Spin, Stealth, Walk

        _aniStateTop = _ent->getAnimationState("Attack2");
        _aniStateTop->setEnabled(true);
        _aniStateTop->setLoop(false);
    }

    bool frameStarted(const Ogre::FrameEvent& evt)
    {

        float rotXNew = rotX * evt.timeSinceLastFrame * -1;
        float rotYNew = rotY * evt.timeSinceLastFrame * -1;


        //_camNode->yaw(Ogre::Radian(rotXNew * _mousespeed));
        //_camNode->pitch(Ogre::Radian(rotYNew * _mousespeed));
        //_camNode->moveRelative(translate * evt.timeSinceLastFrame * _movementspeed);
        _camNode->translate(translate * evt.timeSinceLastFrame * _movementspeed);

        if (walked)
        {
            _aniState->setEnabled(true);
            _aniStateTop->setEnabled(true);
            if (_aniState->hasEnded())
            {
                _aniState->setTimePosition(0.0f);
            }
            if (_aniStateTop->hasEnded())
            {
                _aniStateTop->setTimePosition(0.0f);
            }
        }
        else
        {
            _aniState->setTimePosition(0.0f);
            _aniState->setEnabled(false);
            _aniStateTop->setTimePosition(0.0f);
            _aniStateTop->setEnabled(false);
        }


        //The last thing we need to do is to apply translation and rotation to our model's scene node :
        _sceneNode->translate(NinjaTranslate * evt.timeSinceLastFrame * WalkingSpeed);
        _sceneNode->resetOrientation();
        _sceneNode->yaw(Ogre::Radian(NinjaRotation));

        _aniState->addTime(evt.timeSinceLastFrame);
        _aniStateTop->addTime(evt.timeSinceLastFrame);
                
        return true;
    }
};

class Game
    : public ApplicationContext
    , public InputListener
{
private:
    SceneNode* mNinjaNode;
    SceneManager* mScnMgr;
    Root* mRoot;
    Ogre::PolygonMode mPolyMode;
    Camera* mCam;
    SceneNode* mCamNode;
    Entity* mNinjaEnt;
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
    : ApplicationContext("Week14-9-NinjaAnimationsDemo")
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

    createScene();
    createCamera();
    createFrameListener();
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
    Plane plane(Vector3::UNIT_Y, 1);
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



    mNinjaEnt = mScnMgr->createEntity("ninja.mesh");
    mNinjaEnt->setCastShadows(true);
    mNinjaNode = mScnMgr->createSceneNode("NinjaNode");
    mNinjaNode->attachObject(mNinjaEnt);
    mScnMgr->getRootSceneNode()->addChild(mNinjaNode);
    mNinjaNode->setScale(0.4f, 0.4f, 0.4f);
    mNinjaNode->yaw(Degree(180), Ogre::Node::TS_WORLD);
   
    mNinjaNode->setPosition(0, 0, 0);

    Ogre::Entity* sword1 = mScnMgr->createEntity("Sword1", "Sword.mesh");
    Ogre::Entity* sword2 = mScnMgr->createEntity("Sword2", "Sword.mesh");
    //Now attach the sword to the model using a bone name:
    //It works if I use one of them at a time!
    //mNinjaEnt->attachObjectToBone("Joint8", sword2);

    //Step1: get all the animations that the model has as a set :
    Ogre::AnimationStateSet* set = mNinjaEnt->getAllAnimationStates();
    Ogre::AnimationStateIterator iter = set->getAnimationStateIterator();
    cout << "animation states list: " << endl;
    while (iter.hasMoreElements())
    {
        std::cout << iter.getNext()->getAnimationName() <<std::endl;
    }

    if (mNinjaEnt->hasSkeleton()) {
        Ogre::SkeletonInstance* m_skeleton = mNinjaEnt->getSkeleton();
        //Ogre::Skeleton::BoneIterator it = m_skeleton->getBoneIterator(); --> Deprecated
        Ogre::Skeleton::BoneList bones = m_skeleton->getBones();
        for (auto bone : bones)
        {
            cout << bone->getName() << endl;
        }
    }
   
}

void Game::createCamera()
{
    //! [camera]
    mCamNode = mScnMgr->getRootSceneNode()->createChildSceneNode();

    // create the camera
    mCam = mScnMgr->createCamera("myCam");
    mCam->setNearClipDistance(5); // specific to this sample
    mCam->setAutoAspectRatio(true);
    mCamNode->attachObject(mCam);
    mCamNode->setPosition(200, 300, 400);
    mCamNode->lookAt(Ogre::Vector3(0, 0, 0), Node::TS_WORLD);

    // and tell it to render into the main window
    getRenderWindow()->addViewport(mCam);
}


void Game::createFrameListener()
{
    Ogre::FrameListener* FrameListener = new ExampleFrameListener(mNinjaNode, mNinjaEnt, mCamNode);
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

    translate = Ogre::Vector3(0, 0, 0);
    NinjaTranslate = Ogre::Vector3(0, 0, 0);
    walked = false;

    switch (evt.keysym.sym)
    {
    case SDLK_ESCAPE:
        getRoot()->queueEndRendering();
        break;
    case SDLK_UP:
        NinjaTranslate += Ogre::Vector3(0, 0, -1);
        NinjaRotation = 0.0f;
        walked = true;
        break;
    case SDLK_DOWN:
        NinjaTranslate += Ogre::Vector3(0, 0, +1);
        NinjaRotation = 3.14f;
        walked = true;
        break;
    case SDLK_LEFT:
        NinjaTranslate += Ogre::Vector3(-1, 0, 0);
        NinjaRotation = 1.57f;
        walked = true;
        break;
    case SDLK_RIGHT:
        NinjaTranslate += Ogre::Vector3(+1, 0, 0);
        NinjaRotation = -1.57f;
        walked = true;
        break;
    case 'w':
        translate += Ogre::Vector3(0, 0, -1);
        break;
    case 's':
        translate += Ogre::Vector3(0, 0, 1);
        break;
    case 'a':
        translate += Ogre::Vector3(-1, 0, 0);
        break;
    case 'd':
        translate += Ogre::Vector3(1, 0, 0);
        break;
    case 'p':
        translate = Ogre::Vector3(0, 0, 0);
        break; 
    case ' ':
        if (mPolyMode == Ogre::PolygonMode::PM_SOLID)
            mPolyMode = Ogre::PolygonMode::PM_WIREFRAME;
        else
            mPolyMode = Ogre::PolygonMode::PM_SOLID;
         mCam->setPolygonMode(mPolyMode);
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