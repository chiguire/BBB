#include "GameScene.h"

#include <stdio.h>
USING_NS_CC;

CCScene* GameScene::scene()
{
	// 'scene' is an autorelease object
	CCScene *scene = CCScene::node();
	
	// 'layer' is an autorelease object
	GameScene *layer = GameScene::node();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool GameScene::init()
{
	//////////////////////////////
	// 1. super init first
	if ( !CCLayer::init() )
	{
		return false;
	}

	this->setIsTouchEnabled( true );

	/////////////////////////////
	// 2. add a menu item with "X" image, which is clicked to quit the program
	//    you may modify it.

	// add a "close" icon to exit the progress. it's an autorelease object
	CCMenuItemImage *pCloseItem = CCMenuItemImage::itemFromNormalImage(
										"CloseNormal.png",
										"CloseSelected.png",
										this,
										menu_selector(GameScene::menuCloseCallback) );
	pCloseItem->setPosition( ccp(CCDirector::sharedDirector()->getWinSize().width - 20, 20) );

	// add a "close" icon to exit the progress. it's an autorelease object
	CCMenuItemImage *pRestartItem = CCMenuItemImage::itemFromNormalImage(
										"CloseNormal.png",
										"CloseSelected.png",
										this,
										menu_selector(GameScene::menuRestartCallback) );
	pRestartItem->setPosition( ccp(CCDirector::sharedDirector()->getWinSize().width - 70, 20) );

	// create menu, it's an autorelease object
	CCMenu* pMenu = CCMenu::menuWithItems(pCloseItem, pRestartItem, NULL);
	pMenu->setPosition( CCPointZero );
	this->addChild(pMenu, 1);

	/////////////////////////////
	// 3. add your codes below...

	b2Vec2 grav;
	grav.y = -8.0f;

	this->world = new b2World(grav);

	this->contactListener = new BBBContactListener(this);
	this->world->SetContactListener(this->contactListener);

	// ask director the window size
	CCSize size = CCDirector::sharedDirector()->getWinSize();

	this->background = CCSprite::spriteWithFile("game/stars.png");
	this->background->setPosition( ccp(size.width/2, size.height/2) );
	this->addChild(this->background, 0);

	this->createOuterSensors();

	this->player = new Ball("game/hero.png", this->world);
	this->player->setPosition( ccp(60, 60) );
	this->player->setTag(1);
	this->addChild(this->player, 0);
	
	this->endpoint = new EndPoint("game/Star.png", this->world);
	this->endpoint->setPosition( ccp(size.width-60, 60) );
	this->endpoint->setTag(2);
	this->addChild(this->endpoint, 0);

	this->hudBackground = CCSprite::spriteWithFile("game/hud.png");
	this->hudBackground->setPosition( ccp(120, 35) );
	this->addChild(this->hudBackground, 0);

	this->hudLabel = CCLabelTTF::labelWithString("Score: 0", "Arial", 42);
	this->hudLabel->setPosition( ccp(170, 50) );
	this->addChild(this->hudLabel, 1);

	this->warpZones = new CCMutableArray<WarpZone *>();

	// set default volume
	SimpleAudioEngine::sharedEngine()->setEffectsVolume(0.5);
	SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume(0.5);

	//SimpleAudioEngine::sharedEngine()->playBackgroundMusic(CCFileUtils::fullPathFromRelativePath("musicbgm.mp3"), true);

	this->schedule( schedule_selector(GameScene::update) );
	this->schedule( schedule_selector(GameScene::gameTimer), 1.0 );

	//this->addWarpZone(100, 30, 200, 540);

	this->forceLines = new ForceLines(this->world);
	this->addChild(forceLines);

	this->doesRestart = false;
	return true;
}

void GameScene::createOuterSensors()
{
	b2PolygonShape shape;
	b2FixtureDef fixDef;
	b2BodyDef bDef;

	fixDef.shape = &shape;
	fixDef.isSensor = true;

	bDef.position.x = tob2d(512);
	bDef.position.y = tob2d(300);
	bDef.type = b2_staticBody;

	this->outerSensors = this->world->CreateBody(&bDef);

	shape.SetAsBox(tob2d(1224), tob2d(50), b2Vec2(0, 400), 0.0f);
	this->outerSensors->CreateFixture(&fixDef);

	shape.SetAsBox(tob2d(1224), tob2d(50), b2Vec2(0, -400), 0.0f);
	this->outerSensors->CreateFixture(&fixDef);

	shape.SetAsBox(tob2d(50), tob2d(800), b2Vec2(-612, 0), 0.0f);
	this->outerSensors->CreateFixture(&fixDef);

	shape.SetAsBox(tob2d(50), tob2d(800), b2Vec2(612, 0), 0.0f);
	this->outerSensors->CreateFixture(&fixDef);
}
void GameScene::registerWithTouchDispatcher()
{
    CCTouchDispatcher::sharedDispatcher()->addTargetedDelegate(this, 0, false);
}

bool GameScene::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	this->forceLines->createLine();
	return true;
}

void GameScene::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
	CCPoint location = pTouch->locationInView( pTouch->view() );
	CCPoint convertedLocation = CCDirector::sharedDirector()->convertToGL(location);
	CCPoint *p = new CCPoint(convertedLocation.x, convertedLocation.y);

	this->forceLines->pushPoint(p);
}

void GameScene::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
	CCPoint location = pTouch->locationInView( pTouch->view() );
	CCPoint convertedLocation = CCDirector::sharedDirector()->convertToGL(location);
	CCPoint *p = new CCPoint(convertedLocation.x, convertedLocation.y);

	CCFiniteTimeAction* actionMove =
	  CCMoveTo::actionWithDuration( (ccTime)2.0f, convertedLocation );
	this->player->runAction( CCSequence::actions(actionMove,
	  NULL, NULL) );

	this->forceLines->pushPoint(p);
}

void GameScene::update(ccTime dt)
{
	this->doesRestart = false;

	this->world->Step(1.0f/60.0f, 10, 10);

	if (this->doesRestart) {
		this->restart();
	}

	for (b2Body *b = this->world->GetBodyList(); b; b = b->GetNext()) {
		if (b->GetUserData() != NULL) {
			CCSprite *myActor = (CCSprite *)b->GetUserData();
			myActor->setPosition( CCPointMake( fromb2d(b->GetPosition().x), fromb2d(b->GetPosition().y)));
			myActor->setRotation( -1 * CC_RADIANS_TO_DEGREES(b->GetAngle()));
		}
	}
}

void GameScene::menuCloseCallback(CCObject* pSender)
{
	CCDirector::sharedDirector()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}

void GameScene::menuRestartCallback(CCObject* pSender)
{
	this->restart();
}

void GameScene::addWarpZone()
{
      WarpZone *target = new WarpZone("game/warpzone.png", this->world);

      // Determine where to spawn the target along the Y axis
      CCSize winSize = CCDirector::sharedDirector()->getWinSize();
      int minY = target->getContentSize().height/2;
      int maxY = winSize.height
                            -  target->getContentSize().height/2;
      int rangeY = maxY - minY;
      // srand( TimGetTicks() );
      int actualY = ( rand() % rangeY ) + minY;
      int newY = ( rand() % rangeY ) + minY;

      int minX = target->getContentSize().width/2;
	  int maxX = winSize.width
	  					  -  target->getContentSize().width/2;
	  int rangeX = maxX - minX;
	  // srand( TimGetTicks() );
	  int actualX = ( rand() % rangeX ) + minX;
	  int newX = ( rand() % rangeX ) + minX;

      // Create the target slightly off-screen along the right edge,
      // and along a random position along the Y axis as calculated
      target->setPosition( ccp(actualX, actualY) );
      target->body->SetTransform(b2Vec2(tob2d(actualX), tob2d(actualY)), target->body->GetAngle());
      target->setTag(2);
      this->addChild(target);

      this->warpZones->addObject(target);

      // Determine speed of the target
      int minDuration = (int)2.0;
      int maxDuration = (int)4.0;
      int rangeDuration = maxDuration - minDuration;
      // srand( TimGetTicks() );
      int actualDuration = ( rand() % rangeDuration )
                                          + minDuration;

      // Create the actions
      CCFiniteTimeAction* actionMove =
          CCMoveTo::actionWithDuration( (ccTime)actualDuration,
          ccp(actualX, actualY) );
      CCFiniteTimeAction* actionMoveDone =
          CCCallFuncN::actionWithTarget( this,
          callfuncN_selector(GameScene::spriteMoveFinished));
      target->runAction( CCSequence::actions(actionMove,
          actionMoveDone, NULL) );

      target->toPoint = new b2Vec2(tob2d(newX), tob2d(newY));
}

void GameScene::addWarpZone(int actualX, int actualY, int newX, int newY)
{
      WarpZone *target = new WarpZone("game/warpzone.png", this->world);
      target->body->SetTransform(b2Vec2(tob2d(actualX), tob2d(actualY)), target->body->GetAngle());
      target->toPoint = new b2Vec2(tob2d(newX), tob2d(newY));

      // Determine where to spawn the target along the Y axis
      CCSize winSize = CCDirector::sharedDirector()->getWinSize();

      // Create the target slightly off-screen along the right edge,
      // and along a random position along the Y axis as calculated
      target->setPosition( ccp(actualX, actualY) );
      target->setTag(2);
      this->addChild(target);

      this->warpZones->addObject(target);

      // Determine speed of the target
      int minDuration = (int)2.0;
      int maxDuration = (int)4.0;
      int rangeDuration = maxDuration - minDuration;
      // srand( TimGetTicks() );
      int actualDuration = ( rand() % rangeDuration )
                                          + minDuration;

      // Create the actions
      CCFiniteTimeAction* actionMove =
          CCMoveTo::actionWithDuration( (ccTime)actualDuration,
          ccp(actualX, actualY) );
      CCFiniteTimeAction* actionMoveDone =
          CCCallFuncN::actionWithTarget( this,
          callfuncN_selector(GameScene::spriteMoveFinished));
      target->runAction( CCSequence::actions(actionMove,
          actionMoveDone, NULL) );
}

void GameScene::spriteMoveFinished(CCNode* sender)
{
  CCSprite *sprite = (CCSprite *)sender;

  if (sprite->getTag() == 2) {
	  WarpZone *g = (WarpZone *)sprite;
	  this->world->DestroyBody(g->body);
	  this->warpZones->removeObject(g);
  }

  this->removeChild(sprite, true);
}

void GameScene::gameTimer(ccTime dt)
{
    this->addWarpZone();
}

bool GameScene::isWarpZone(b2Body *b) {
	CCMutableArray<WarpZone *>::CCMutableArrayIterator it;

	for (it=this->warpZones->begin(); it != this->warpZones->end(); it++) {
		WarpZone *wz = *it;
		if (wz->body == b) return true;
	}

	return false;
}

void GameScene::restart() {
	this->player->restart();
}

/*** BBBCONTACTLISTENER ***/
/*** BBBCONTACTLISTENER ***/
/*** BBBCONTACTLISTENER ***/

BBBContactListener::BBBContactListener(GameScene *gs) {
	this->gs = gs;
}

void BBBContactListener::BeginContact(b2Contact* contact) {
	b2Body *a = contact->GetFixtureA()->GetBody();
	b2Body *b = contact->GetFixtureB()->GetBody();

	b2Body *player;
	b2Body *outerSensors;
	b2Body *warpZone;
	b2Body *endpoint;

	if (a->GetUserData() == this->gs->player) {
		player = a;
	} else if (b->GetUserData() == this->gs->player) {
		player = b;
	}

	fprintf(stderr, "Intentando ver outerSensors. \n");
	if (a == this->gs->outerSensors) {
		outerSensors = a;
		fprintf(stderr, "A lo es! \n");
	} else if (b == this->gs->outerSensors) {
		outerSensors = b;
		fprintf(stderr, "B lo es! \n");
	}

	if (this->gs->isWarpZone(a)) {
		warpZone = a;
	} else if (this->gs->isWarpZone(b)) {
		warpZone = b;
	}

	if (a == this->gs->endpoint->body) {
		endpoint = a;
	} else if (b == this->gs->endpoint->body) {
		endpoint = b;
	}

	if (player && outerSensors) {
		//this->gs->doesRestart = true;
	}

	if (player && warpZone) {

	}
}

void BBBContactListener::EndContact(b2Contact* contact) {

}

void BBBContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold) {

}

void BBBContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {

}
