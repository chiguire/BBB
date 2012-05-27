/*
 * GameObjects.cpp
 *
 *  Created on: 22/05/2012
 *      Author: Yole
 */

#include "cocos2d.h"
#include "GameObjects.h"

Ball::Ball(const char *filename, b2World *world)
{
	this->initWithFile(filename, CCRect(0, 0, 74, 70));
	//this->autorelease();

	b2CircleShape circleShape = b2CircleShape();
	circleShape.m_radius = 2.25f/2.0f;

	b2FixtureDef bodyFixDef;
	bodyFixDef.shape = &circleShape;
	bodyFixDef.density = 1.0f;
	bodyFixDef.friction = 0.0f;
	bodyFixDef.restitution = 1.0f;

	b2BodyDef bDef;
	bDef.position.x = tob2d(60);
	bDef.position.y = tob2d(540);
	bDef.allowSleep = false;
	bDef.fixedRotation = true;
	bDef.type = b2_dynamicBody;
	bDef.userData = this;

	this->body = world->CreateBody(&bDef);
	this->body->CreateFixture(&bodyFixDef);
}

void Ball::restart() {
	this->body->SetTransform(b2Vec2(tob2d(60), tob2d(540)), 0);
	this->body->SetLinearVelocity(b2Vec2(0,0));
}

EndPoint::EndPoint(const char *filename, b2World *world)
{
	this->initWithFile(filename);
	this->autorelease();

	b2CircleShape circleShape = b2CircleShape();
	circleShape.m_radius = 1.56f/2.0f;

	b2FixtureDef bodyFixDef;
	bodyFixDef.shape = &circleShape;
	bodyFixDef.density = 1.0f;
	bodyFixDef.friction = 0.0f;
	bodyFixDef.restitution = 1.0f;
	bodyFixDef.isSensor = true;

	b2BodyDef bDef;
	bDef.position.x = tob2d(1024-60);
	bDef.position.y = tob2d(600-60);
	bDef.allowSleep = true;
	bDef.fixedRotation = true;
	bDef.type = b2_kinematicBody;
	bDef.userData = this;

	this->body = world->CreateBody(&bDef);
	this->body->CreateFixture(&bodyFixDef);
}

WarpZone::WarpZone(const char *filename, b2World *world)
{
	this->initWithFile(filename);
	this->autorelease();

	b2PolygonShape polyShape;
	polyShape.SetAsBox(3.125f, 3.125f);

	b2FixtureDef bodyFixDef;
	bodyFixDef.shape = &polyShape;
	bodyFixDef.density = 1.0f;
	bodyFixDef.friction = 0.0f;
	bodyFixDef.restitution = 1.0f;
	bodyFixDef.isSensor = true;

	b2BodyDef bDef;
	bDef.position.x = tob2d(500);
	bDef.position.y = tob2d(300);
	bDef.allowSleep = true;
	bDef.fixedRotation = true;
	bDef.type = b2_staticBody;
	bDef.userData = this;

	this->body = world->CreateBody(&bDef);
	this->body->CreateFixture(&bodyFixDef);

	this->touched = false;
}

ForceLines::ForceLines(b2World *world) {
	this->lines = vector< vector<CCPoint *> *>();
	this->world = world;
	this->bodies = vector<b2Body *>();

	this->bDef = new b2BodyDef();
	this->bDef->type = b2_staticBody;

	this->fDef = new b2FixtureDef();
	this->fDef->friction = 0.1f;
	this->fDef->density = 1;
	this->fDef->restitution = 0.0f;
}

void ForceLines::createLine() {
	this->currentLine = new vector<CCPoint *>();
	this->lines.push_back(this->currentLine);
	this->currentBody = this->world->CreateBody(this->bDef);
}

void ForceLines::pushPoint(CCPoint *p) {
	this->currentLine->push_back(p);

	if (this->currentLine->size() > 1) {
		b2EdgeShape *shape = new b2EdgeShape();
		shape->Set(b2Vec2(tob2d((this->currentLine->at(this->currentLine->size()-2))->x),
				          tob2d((this->currentLine->at(this->currentLine->size()-2))->y)),
				   b2Vec2(tob2d((this->currentLine->at(this->currentLine->size()-1))->x),
				          tob2d((this->currentLine->at(this->currentLine->size()-1))->y)));
		this->fDef->shape = shape;
		this->currentBody->CreateFixture(this->fDef);
	}
}

void ForceLines::draw()
{
	CCLayer::draw();

	glLineWidth( 1.0f );
	glColor4f(1.0, 0.0, 0.0, 1.0);

	vector<vector<CCPoint *> *>::iterator it;
	for (it = this->lines.begin(); it != this->lines.end(); it++) {
		vector<CCPoint *> *line = *it;
		unsigned int i = 0;

		if (line->size() <= 1) {
			continue;
		}

		if (it+1 == this->lines.end()) {
			glColor4f(1.0, 1.0, 0.0, 1.0);
		}

		for (i = 0; i != line->size()-1; i++) {
			ccDrawLine( *((*line)[i]), *((*line)[i+1]) );
		}
	}
}
