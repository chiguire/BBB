/*
 * GameObjects.h
 *
 *  Created on: 22/05/2012
 *      Author: Yole
 */

#ifndef GAMEOBJECTS_H_
#define GAMEOBJECTS_H_

#define PXL2MTS 32.0f
#define tob2d(__X__) __X__ / PXL2MTS
#define fromb2d(__X__) __X__ * PXL2MTS

#include "cocos2d.h"
#include "Box2D/Box2D.h"

using namespace cocos2d;

class Ball : public cocos2d::CCSprite
{
public:
	Ball(const char *filename, b2World *world);
	void restart();

	b2Body *body;
};

class EndPoint : public cocos2d::CCSprite
{
public:
	EndPoint(const char *filename, b2World *world);
	b2Body *body;
};

class WarpZone : public cocos2d::CCSprite
{
public:
	WarpZone(const char *filename, b2World *world);
	b2Body *body;
	b2Vec2 *toPoint;
	bool touched;
};

class ForceLines : public cocos2d::CCLayer
{
public:
	ForceLines(b2World *world);
	virtual void draw();

	void createLine();
	void pushPoint(CCPoint *p);

	b2World *world;
	vector< vector<CCPoint *> *> lines;
	vector<CCPoint*> *currentLine;

	b2BodyDef *bDef;
	b2FixtureDef *fDef;
	vector<b2Body *> bodies;
	b2Body *currentBody;
};
#endif /* GAMEOBJECTS_H_ */
