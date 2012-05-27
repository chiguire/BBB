#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__


#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "GameObjects.h"
//#include "ParticleTest.h"
#include <vector>

using namespace cocos2d;
using namespace CocosDenshion;
using namespace std;

class GameScene : public cocos2d::CCLayer
{
public:
	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();
	void createOuterSensors();

	// there's no 'id' in cpp, so we recommand to return the exactly class pointer
	static cocos2d::CCScene* scene();
	
	// a selector callback
	virtual void menuCloseCallback(CCObject* pSender);
	virtual void menuRestartCallback(CCObject* pSender);

	virtual void registerWithTouchDispatcher();
	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);

	virtual void update(ccTime dt);

	// implement the "static node()" method manually
	LAYER_NODE_FUNC(GameScene);

	void addWarpZone();
	void addWarpZone(int actualX, int actualY, int newX, int newY);
	void spriteMoveFinished(CCNode* sender);
	void gameTimer(ccTime dt);
	bool isWarpZone(b2Body *b);
	void restart();

	b2World *world;
	b2ContactListener *contactListener;

	CCLabelTTF* hudLabel;
	CCSprite* background;
	CCSprite* hudBackground;

	b2Body *outerSensors;

	Ball* player;
	EndPoint* endpoint;

	CCMutableArray<WarpZone *> *warpZones;

	ForceLines *forceLines;

	bool doesRestart;
};


class BBBContactListener : public b2ContactListener
{
public:
  BBBContactListener(GameScene *gs);
  void BeginContact(b2Contact* contact);
  void EndContact(b2Contact* contact);
  void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
  void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);

  GameScene *gs;
};

#endif // __HELLOWORLD_SCENE_H__
