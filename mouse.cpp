#include "myqgraphicsview.h"
#include <QMessageBox>
void MyQGraphicsView::mousePressEvent(QMouseEvent * e)
{
    LeftButtonDown=true;
    QPointF pt = mapToScene(e->pos());
    if(isInMainMenu)
    {
        if((pt.x()>0)&&(pt.x()<300)&&(pt.y()>-150)&&(pt.y()<-100))//Начать игру
        {
            isInMainMenu=false;
            ChooseLevel(scene, this);
        }
        else if((pt.x()>0)&&(pt.x()<300)&&(pt.y()>-50)&&(pt.y()<0))//Таблица рекордов
        {
            isInMainMenu=false;
            RecordsTable(scene, this);
        }
        else if((pt.x()>0)&&(pt.x()<300)&&(pt.y()>150)&&(pt.y()<200))//Редактор уровней
        {
            isInMainMenu=false;
            LevelEditor(scene, this);
        }
        else if((pt.x()>0)&&(pt.x()<300)&&(pt.y()>50)&&(pt.y()<100))//Выход
            exit(0);
    }
    else if(isInPauseMenu)
    {
        /*if((pt.x()>0)&&((pt.x()<300))&&(pt.y()>-125)&&((pt.y()<-75)))//Вернуться к игре
        {
            isInPauseMenu=false;
            BackToGame(scene, this);
        }*/
        if((pt.x()>0)&&((pt.x()<300))&&(pt.y()>-25)&&((pt.y()<25)))//Главное меню
        {
            isInMainMenu=true;
            isInPauseMenu=false;
            MainMenu(scene, this);
        }
        else if((pt.x()>0)&&((pt.x()<300))&&(pt.y()>75)&&((pt.y()<125)))//Выход
            exit(0);
    }
    else if(isInCompletedMenu)
    {
        if((cur_level!=level_names.length())&&(pt.x()>0)&&((pt.x()<300))&&(pt.y()>50)&&((pt.y()<100)))//Следующий уровень
        {
            isInCompletedMenu=false;
            cur_level++;
            QFile *file=new QFile(QString("levels/level")+QString::number(cur_level)+QString(".txt"));
            StartGame(scene, this, file);
        }
        if((pt.x()>0)&&((pt.x()<300))&&(pt.y()>150)&&((pt.y()<200)))//Рестарт
        {
            isInCompletedMenu=false;
            QFile *file=new QFile(QString("levels/level")+QString::number(cur_level)+QString(".txt"));
            StartGame(scene, this, file);
        }
        if((pt.x()>0)&&((pt.x()<300))&&(pt.y()>250)&&((pt.y()<300)))//Главное меню
        {
            isInCompletedMenu=false;
            isInMainMenu=true;
            MainMenu(scene, this);
        }
    }
    else if(isTimeLaunched)
    {
        if((pt.x()>560)&&(pt.x()<655)&&(pt.y()>50)&&(pt.y()<80))
        {
            RestartGame(scene, this);
        }
    }
    else if(isInRecordsTable)
    {
        if((pt.x()>-100)&&(pt.x()<100)&&(pt.y()>500)&&(pt.y()<550))//назад
        {
            isInRecordsTable=false;
            isInMainMenu=true;
            MainMenu(scene, this);
        }
        for(int i=0; i<inset_number; i++)//номера вкладок
        {
            if((pt.x()>105+i*50)&&(pt.x()<105+i*50+70)&&(pt.y()>505)&&(pt.y()<535))
            {
                scene->addRect(0, 0, 500, 490, QPen(Qt::white), QBrush(Qt::white));
                scene->addRect(105+(cur_inset-1)*50, 505, 65, 30, QPen(Qt::white), QBrush(Qt::white));
                QGraphicsTextItem *InsetName=new QGraphicsTextItem();
                InsetName->setPlainText(QString::number(cur_inset*10-9)+"-"+QString::number(cur_inset*10));
                InsetName->setFont(QFont("helvetica", 10));
                InsetName->setPos(105+(cur_inset-1)*50, 505);
                InsetName->setTextWidth(70);
                InsetName->document()->setPageSize(QSizeF(70, 30));
                InsetName->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
                scene->addItem(InsetName);
                cur_inset=i+1;
                scene->addRect(105+i*50, 505, 65, 30, QPen(Qt::white), QBrush(Qt::white));
                QGraphicsTextItem *CurInsetName=new QGraphicsTextItem();
                CurInsetName->setPlainText(QString::number(cur_inset*10-9)+"-"+QString::number(cur_inset*10));
                CurInsetName->setFont(QFont("helvetica", 11, QFont::Bold));
                CurInsetName->setPos(105+(cur_inset-1)*50, 505);
                CurInsetName->setTextWidth(70);
                CurInsetName->document()->setPageSize(QSizeF(70, 30));
                CurInsetName->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
                scene->addItem(CurInsetName);
                scene->addRect(0, 0, 250, 30, QPen(Qt::black), QBrush(Qt::white));
                QGraphicsTextItem *LevelName=new QGraphicsTextItem("level");
                LevelName->setFont(QFont("helvetica", 13, QFont::Bold));
                LevelName->setPos(0, 0);
                LevelName->setTextWidth(250);
                LevelName->document()->setPageSize(QSizeF(250, 50));
                LevelName->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
                scene->addItem(LevelName);
                scene->addRect(250, 0, 250, 30, QPen(Qt::black), QBrush(Qt::white));
                QGraphicsTextItem *Record=new QGraphicsTextItem("record");
                Record->setFont(QFont("helvetica", 13, QFont::Bold));
                Record->setPos(250, 0);
                Record->setTextWidth(250);
                Record->document()->setPageSize(QSizeF(250, 50));
                Record->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
                scene->addItem(Record);
                int start=10*i;
                do
                {
                    int length=level_names.length();
                    if(length-10*i<=10)
                        levels_number_on_inset=length-10*i;
                    else
                        levels_number_on_inset=10;
                    for(int j=start; j<length; j++)
                    {
                        if(j>=10+10*i)//на вкладке 10 уровней
                            break;
                        scene->addRect(0, 30+30*(j-10*i), 250, 30, QPen(Qt::black), QBrush(Qt::white));
                        QGraphicsTextItem *LevelName=new QGraphicsTextItem(level_names.at(j));
                        LevelName->setFont(QFont("helvetica", 13));
                        LevelName->setPos(0, 30+30*(j-10*i));
                        LevelName->setTextWidth(250);
                        LevelName->document()->setPageSize(QSizeF(250, 50));
                        LevelName->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
                        scene->addItem(LevelName);
                        scene->addRect(250, 30+30*(j-10*i), 250, 30, QPen(Qt::black), QBrush(Qt::white));
                        QGraphicsTextItem *Record=new QGraphicsTextItem(level_records.at(j));
                        Record->setFont(QFont("helvetica", 13));
                        Record->setPos(250, 30+30*(j-10*i));
                        Record->setTextWidth(250);
                        Record->document()->setPageSize(QSizeF(250, 50));
                        Record->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
                        scene->addItem(Record);
                    }
                    start=length;
                }while((isInChooseLevelMenu)&&(!isLevelListLoaded));
            }
        }
    }
    else if(isInChooseLevelMenu)
    {
        for(int i=0; i<levels_number_on_inset; i++)//уровни
        {
            if((i%2==0)&&(pt.x()>0)&&(pt.x()<200)&&(pt.y()>50*i)&&(pt.y()<50*i+50))
            {
                isInChooseLevelMenu=false;
                cur_level=i+1;
                QFile *file=new QFile(QString("levels/level")+QString::number((cur_inset-1)*10+i+1)+QString(".txt"));
                StartGame(scene, this, file);
            }
            if((i%2==1)&&(pt.x()>300)&&(pt.x()<500)&&(pt.y()>50*(i-1))&&(pt.y()<50*(i-1)+50))
            {
                isInChooseLevelMenu=false;
                cur_level=i+1;
                QFile *file=new QFile(QString("levels/level")+QString::number((cur_inset-1)*10+i+1)+QString(".txt"));
                StartGame(scene, this, file);
            }
        }
        if((pt.x()>-100)&&(pt.x()<100)&&(pt.y()>500)&&(pt.y()<550))//назад
        {
            isInChooseLevelMenu=false;
            isInMainMenu=true;
            MainMenu(scene, this);
        }
        for(int i=0; i<inset_number; i++)//номера вкладок
        {
            if((pt.x()>105+i*50)&&(pt.x()<105+i*50+70)&&(pt.y()>505)&&(pt.y()<535))
            {
                scene->addRect(0, 0, 500, 490, QPen(Qt::white), QBrush(Qt::white));
                scene->addRect(105+(cur_inset-1)*50, 505, 65, 30, QPen(Qt::white), QBrush(Qt::white));
                QGraphicsTextItem *InsetName=new QGraphicsTextItem();
                InsetName->setPlainText(QString::number(cur_inset*10-9)+"-"+QString::number(cur_inset*10));
                InsetName->setFont(QFont("helvetica", 10));
                InsetName->setPos(105+(cur_inset-1)*50, 505);
                InsetName->setTextWidth(70);
                InsetName->document()->setPageSize(QSizeF(70, 30));
                InsetName->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
                scene->addItem(InsetName);
                cur_inset=i+1;
                scene->addRect(105+i*50, 505, 65, 30, QPen(Qt::white), QBrush(Qt::white));
                QGraphicsTextItem *CurInsetName=new QGraphicsTextItem();
                CurInsetName->setPlainText(QString::number(cur_inset*10-9)+"-"+QString::number(cur_inset*10));
                CurInsetName->setFont(QFont("helvetica", 11, QFont::Bold));
                CurInsetName->setPos(105+(cur_inset-1)*50, 505);
                CurInsetName->setTextWidth(70);
                CurInsetName->document()->setPageSize(QSizeF(70, 30));
                CurInsetName->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
                scene->addItem(CurInsetName);
                int start=10*i;
                do
                {
                    int length=level_names.length();
                    if(length-10*i<=10)
                        levels_number_on_inset=length-10*i;
                    else
                        levels_number_on_inset=10;
                    for(int j=start; j<length; j++)
                    {
                        if(j>=10+10*i)//на вкладке 10 уровней
                            break;
                        if(j%2==0)//первый столбик
                            DrawButton(scene, 0, 50*(j-10*i), 200, 50, level_names.at(j));
                        else//второй столбик
                            DrawButton(scene, 300, 50*(j-1-10*i), 200, 50, level_names.at(j));
                    }
                    start=length;
                }while((isInChooseLevelMenu)&&(!isLevelListLoaded));
            }
        }
    }
    else//уровень, таймер запущен
    {
        if((pt.x()>560)&&(pt.x()<655)&&(pt.y()>50)&&(pt.y()<80))
        {
            LaunchTime(scene);
            return;
        }
        else if((pt.x()>560)&&(pt.x()<590)&&(pt.y()>90)&&(pt.y()<120))
        {
            SelectAddWallTool(scene);
            return;
        }
        else if((pt.x()>600)&&(pt.x()<630)&&(pt.y()>90)&&(pt.y()<120))
        {
            SelectRemoveWallTool(scene);
            return;
        }
        else if((pt.x()>560)&&(pt.x()<655)&&(pt.y()>140)&&(pt.y()<170))//Удалить все стены
        {
            score=1000;
            isZeroScore=false;
            UpdateScore();
            for(int i=0; i<userObjects.length();)
            {
                QGraphicsItem* item=reinterpret_cast<QGraphicsItem*>(userObjects.at(i)->GetUserData());
                world->DestroyBody(userObjects.at(i));
                userObjects.removeAt(i);
                scene->removeItem(item);
            }
            scene->removeItem(tmpLine);
            return;
        }
        if(!simulation)//во время симуляции редактирование запрещено
        {
            if (AddMode&&(pt.x()>leftOffset  && pt.y()>topOffset) && (pt.x()<winWidth-rightOffset  && pt.y()<winHeight-bottomOffset))
            {
                if(!isZeroScore)
                {
                    QGraphicsItem *ball=reinterpret_cast<QGraphicsItem*>(ballPointer->GetUserData());
                    QGraphicsEllipseItem *checkCircle=new QGraphicsEllipseItem(pt.x(), pt.y(), 1, 1);
                    if(!ball->collidesWithItem(checkCircle))//если пользователь нажал не на шар
                    {
                        coordsStack.push(pt);
                        if (coordsStack.size()==2)
                        {
                            QPointF temp1 = coordsStack.top();
                            coordsStack.pop();
                            QPointF temp2 = coordsStack.top();
                            QGraphicsLineItem *checkLine=new QGraphicsLineItem(temp1.x(), temp1.y(), temp2.x(), temp2.y());
                            qreal x0=checkLine->boundingRect().x();
                            qreal y0=checkLine->boundingRect().y();
                            qreal x1=checkLine->boundingRect().x()+checkLine->boundingRect().width();
                            qreal y1=checkLine->boundingRect().y()+checkLine->boundingRect().height();
                            qreal length=qSqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0));
                            QGraphicsLineItem *line;
                            if(length<score)
                            {
                                line=createStaticLine(temp1.x(), temp1.y(), temp2.x(), temp2.y(), "user");
                                x0=line->boundingRect().x();
                                y0=line->boundingRect().y();
                                x1=line->boundingRect().x()+line->boundingRect().width();
                                y1=line->boundingRect().y()+line->boundingRect().height();
                                score-=qSqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0));
                                UpdateScore();
                            }
                            else
                            {
                                scene->removeItem(tmpLine);
                                QMessageBox::information(0,"","You can not add line whith such length while score is such small",QMessageBox::Ok|QMessageBox::NoButton);
                            }
                            if (!coordsStack.empty())
                                while (!coordsStack.empty())
                                    coordsStack.pop();
                        }
                    }
                    delete checkCircle;
                }
                else
                    QMessageBox::information(0,"","You can not add lines while score is zero",QMessageBox::Ok|QMessageBox::NoButton);
            }
            if (!AddMode&&(pt.x()>leftOffset  && pt.y()>topOffset) && (pt.x()<winWidth-rightOffset  && pt.y()<winHeight-bottomOffset))
            {
                QGraphicsEllipseItem *checkCircle=new QGraphicsEllipseItem(pt.x(), pt.y(), 3, 3);
                for(int i=0; i<userObjects.length(); i++)
                {
                    QGraphicsLineItem* item=reinterpret_cast<QGraphicsLineItem*>(userObjects.at(i)->GetUserData());
                    if(item->collidesWithItem(checkCircle))
                    {
                        qreal x0=item->boundingRect().x();
                        qreal y0=item->boundingRect().y();
                        qreal x1=item->boundingRect().x()+item->boundingRect().width();
                        qreal y1=item->boundingRect().y()+item->boundingRect().height();
                        score+=qSqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0));
                        UpdateScore();
                        world->DestroyBody(userObjects.at(i));
                        userObjects.removeAt(i);
                        scene->removeItem(item);
                        scene->removeItem(tmpLine);
                    }
                }
                delete checkCircle;
            }
        }
    }
}
void MyQGraphicsView::mouseReleaseEvent(QMouseEvent *e)
{
    LeftButtonDown=false;
}
void MyQGraphicsView::mouseMoveEvent(QMouseEvent * e)
{
    QPointF pt = mapToScene(e->pos());
    if(isInMainMenu)
    {

    }
    else if(isInPauseMenu)
    {

    }
    else if(isInCompletedMenu)
    {

    }
    else if(isTimeLaunched)
    {

    }
    else if(isInChooseLevelMenu)
    {

    }
    else//уровень
    {
        if(!simulation)
        {
            if ((pt.x()>leftOffset  && pt.y()>topOffset) && (pt.x()<winWidth-rightOffset  && pt.y()<winHeight-bottomOffset))
            {
                if (coordsStack.size()==1)
                {
                    QGraphicsItem *ball=reinterpret_cast<QGraphicsItem*>(ballPointer->GetUserData());
                    QPointF temp1 = coordsStack.top();
                    QGraphicsLineItem *checkLine=new QGraphicsLineItem(temp1.x(), temp1.y(), pt.x(), pt.y());
                    scene->addItem(tmpLine);
                    if(!ball->collidesWithItem(checkLine))//чтобы стена не проходила сквозь шар
                        tmpLine->setLine(temp1.x(), temp1.y(), pt.x(), pt.y());
                    delete checkLine;
                }
            }
        }
    }
}
