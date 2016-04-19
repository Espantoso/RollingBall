#include "phy.h"

phy::phy(QWidget *parent) : QWidget(parent) {
    scene = new QGraphicsScene(0, 0, winWidth, winHeight, this);
    view = new QGraphicsView(scene, this);
    view->setRenderHint(QPainter::Antialiasing);
    resize(winWidth, winHeight);
    setFixedSize(winWidth, winHeight);
    /////////////////////////////////////
    b2Vec2 gravity(0.0f, 10.0f);
    world = new b2World(gravity);
    world->SetAllowSleeping(true);
    world->SetWarmStarting(true);
    //createStaticPolygon(0,400,winWidth-1,winHeight-400);
    //for (int i=0; i<10; i++) {
        //createPolygon(rInt(30,winWidth-30),rInt(10,50),rInt(10,20),rInt(16,36));
    //}
    createStaticLine(0, 0, 0, 200);
    createStaticLine(0,200,winWidth-30,400);
    createStaticLine(winWidth-30, 400, winWidth-30, 0);
    for (int i=2; i<16; i++) createCircle(i*30, 10, 10);
    /////////////////////////////////////
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updatePhysics()));
    timer->start(1000 / timeStep);
}

phy::~phy() {
}

void phy::createPolygon(int x, int y, int width, int height) {
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.allowSleep = true;
    bodyDef.angularDamping = 1.0f;
    bodyDef.position.Set(x / pixPerUnit, y / pixPerUnit);
    b2Body *body = world->CreateBody(&bodyDef);
    b2PolygonShape bodyShape;
    //bodyShape.m_vertexCount = 4;
    //bodyShape.m_vertices[0] = b2Vec2(0, 0);
    //bodyShape.m_vertices[1] = b2Vec2(width / pixPerUnit, 0);
//    bodyShape.m_vertices[2] = b2Vec2(width / pixPerUnit, height / pixPerUnit);
//    bodyShape.m_vertices[3] = b2Vec2(0, height / pixPerUnit);
//    bodyShape.Set(bodyShape.m_vertices, 4);
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &bodyShape; // ������� �������������� b2PolygonShape � b2Shape
    fixtureDef.density = 1.0f; // ���������
    fixtureDef.friction = 0.3f; // ����������� ������
    fixtureDef.restitution=0.6f; // ���������� (!!!)
    body->CreateFixture(&fixtureDef);
    //body->SetAngularVelocity(0.1f * rInt(10,30)); // �����-�� ������� ������ �� ����
    QGraphicsRectItem *polygon = new QGraphicsRectItem;
    polygon->setPos(x, y);
    polygon->setRect(0, 0, width, height);
    scene->addItem(polygon);
    body->SetUserData(polygon);
    bodyList<<body;
}

void phy::createStaticPolygon(int x, int y, int width, int height) {
    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position.Set(x / pixPerUnit, y / pixPerUnit);
    b2Body *body = world->CreateBody(&bodyDef);
    b2PolygonShape bodyShape;
//    bodyShape.m_vertexCount = 4;
//    bodyShape.m_vertices[0] = b2Vec2(0, 0);
//    bodyShape.m_vertices[1] = b2Vec2(width / pixPerUnit, 0);
//    bodyShape.m_vertices[2] = b2Vec2(width / pixPerUnit, height / pixPerUnit);
//    bodyShape.m_vertices[3] = b2Vec2(0, height / pixPerUnit);
//    bodyShape.Set(bodyShape.m_vertices, 4);
    body->CreateFixture(&bodyShape, 0.0f);
    //
    QGraphicsRectItem *polygon = new QGraphicsRectItem;
    polygon->setPos(x, y);
    polygon->setRect(0, 0, width, height);
    scene->addItem(polygon);
    body->SetUserData(polygon);
}

void phy::createStaticLine(int x1, int y1, int x2, int y2) {
    b2BodyDef bodyDef;
    bodyDef.position.Set(x1 / pixPerUnit, y1 / pixPerUnit);
    b2Body *body = world->CreateBody(&bodyDef);
    b2EdgeShape shape;
    shape.Set(b2Vec2(0, 0), b2Vec2((x2-x1) / pixPerUnit, (y2-y1) / pixPerUnit));
    body->CreateFixture(&shape, 0.0f);
    //
    QGraphicsLineItem *line = new QGraphicsLineItem;
    line->setPos(x1, y1);
    line->setLine(0, 0, x2-x1, y2-y1);
    scene->addItem(line);
    body->SetUserData(line);
}

void phy::createLine(int x1, int y1, int x2, int y2) {
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(x1 / pixPerUnit, y1 / pixPerUnit);
    b2Body *body = world->CreateBody(&bodyDef);
    b2EdgeShape shape;
    shape.Set(b2Vec2(0, 0), b2Vec2((x2-x1) / pixPerUnit, (y2-y1) / pixPerUnit));
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape; // ������� �������������� b2EdgeShape � b2Shape
    fixtureDef.density = 1.0f; // ���������
    fixtureDef.friction = 0.3f; // ����������� ������
    body->CreateFixture(&fixtureDef);
    //
    QGraphicsLineItem *line = new QGraphicsLineItem;
    line->setPos(x1, y1);
    line->setLine(0, 0, x2-x1, y2-y1);
    scene->addItem(line);
    body->SetUserData(line);
    bodyList<<body;
}

void phy::createCircle(int x, int y, int r) {
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.angularDamping = 1.0f;
    bodyDef.position.Set(x / pixPerUnit, y / pixPerUnit);
    b2Body *body = world->CreateBody(&bodyDef);
    b2CircleShape shape;
    shape.m_radius = r / pixPerUnit;
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    fixtureDef.restitution = 0.6f; // ����������
    body->CreateFixture(&fixtureDef);
    //
    QGraphicsEllipseItem *ellipse = new QGraphicsEllipseItem;
    ellipse->setPos(x, y);
    ellipse->setRect(0, 0, r*2, r*2);
    scene->addItem(ellipse);
    body->SetUserData(ellipse);
    bodyList<<body;
}

void phy::updatePhysics() {
    if (!bodyList.isEmpty()) {
        QGraphicsItem *item;
        QGraphicsRectItem *rect;
        QGraphicsLineItem *line;
        QGraphicsEllipseItem *ellipse;
        b2Body *body;
        world->Step(1.0 / timeStep, velocityIterations, positionIterations);
        bodyListIt = bodyList.begin();
        while (bodyListIt!=bodyList.end()) {
            body = *bodyListIt++;
            item = reinterpret_cast<QGraphicsItem*>(body->GetUserData());
            switch (item->type()) {
            case 3: rect = reinterpret_cast<QGraphicsRectItem*>(item);
                rect->setPos(body->GetPosition().x * pixPerUnit, body->GetPosition().y * pixPerUnit);
                rect->setRotation(body->GetAngle());
                break;
            case 4: ellipse = reinterpret_cast<QGraphicsEllipseItem*>(item);
                ellipse->setPos(body->GetPosition().x * pixPerUnit, body->GetPosition().y * pixPerUnit);
                ellipse->setRotation(body->GetAngle());
                break;
            case 5: line = reinterpret_cast<QGraphicsLineItem*>(item);
                line->setPos(body->GetPosition().x * pixPerUnit, body->GetPosition().y * pixPerUnit);
                line->setRotation(body->GetAngle());
                break;
            }
        }
    }
}

int phy::rInt(int a, int b) {
    return qrand()%(b-a+1)+a;
}

