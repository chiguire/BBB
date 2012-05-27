#include "MenuScene.h"
#include "GameScene.h"

USING_NS_CC;

CCScene* Menu::scene()
{

	CCScene *scene = CCScene::node();
	
	Menu *layer = Menu::node();

	scene->addChild(layer);

	return scene;
}


bool Menu::init()
{


	if ( !CCLayer::init() )
	{
		return false;
	}


	CCMenuItemImage *pCloseItem = CCMenuItemImage::itemFromNormalImage(
										"CloseNormal.png",
										"CloseSelected.png",
										this,
										menu_selector(Menu::menuCloseCallback) );
	pCloseItem->setPosition( ccp(CCDirector::sharedDirector()->getWinSize().width - 20, 20) );



	CCMenuItemImage *pGameItem = CCMenuItemImage::itemFromNormalImage(
										"menu/StartButton-off.png",
										"menu/StartButton-on.png",
										this,
										menu_selector(Menu::menuStartCallback) );

	float x=CCDirector::sharedDirector()->getWinSize().width/ 2 - pGameItem->getContentSize().width/2;
	float y=CCDirector::sharedDirector()->getWinSize().height/ 2 - pGameItem->getContentSize().height/2;
	pGameItem->setPosition( ccp(x, y) );


	CCMenu* pMenu = CCMenu::menuWithItems(pCloseItem, pGameItem,NULL);
	pMenu->setPosition( CCPointZero );
	this->addChild(pMenu, 1);


	CCSize size = CCDirector::sharedDirector()->getWinSize();


	CCSprite* pSprite = CCSprite::spriteWithFile("menu/GameMenu.png");


	pSprite->setPosition( ccp(size.width/2, size.height/2) );


	this->addChild(pSprite, 0);
	
	return true;
}

void Menu::menuCloseCallback(CCObject* pSender)
{
	CCDirector::sharedDirector()->end();
}
void Menu::menuStartCallback(CCObject* pSender)
{


	CCDirector *pDirector = CCDirector::sharedDirector();


	CCScene *pScene = GameScene::scene();

	// run
	pDirector->replaceScene(pScene);
}
