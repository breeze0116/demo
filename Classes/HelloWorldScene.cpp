#include "HelloWorldScene.h"
#include "TcpUtil.h"
#include "MsgQueue.h"
#include "message.pb.h"

using namespace cocos2d;

CCScene* HelloWorld::scene()
{
    CCScene * scene = NULL;
    do 
    {
        // 'scene' is an autorelease object
        scene = CCScene::create();
        CC_BREAK_IF(! scene);

        // 'layer' is an autorelease object
        HelloWorld *layer = HelloWorld::create();
        CC_BREAK_IF(! layer);

        // add layer as a child to scene
        scene->addChild(layer);
    } while (0);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    bool bRet = false;
    do 
    {
        CC_BREAK_IF(! CCLayer::init());
        CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
            "CloseNormal.png",
            "CloseSelected.png",
            this,
            menu_selector(HelloWorld::menuCloseCallback));
        CC_BREAK_IF(! pCloseItem);

        // Place the menu item bottom-right conner.
        pCloseItem->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width - 20, 20));

        // Create a menu with the "close" menu item, it's an auto release object.
        CCMenu* pMenu = CCMenu::create(pCloseItem, NULL);
        pMenu->setPosition(CCPointZero);
        CC_BREAK_IF(! pMenu);


		CCMenuItemImage *pBeginItem = CCMenuItemImage::create(
			"CloseNormal.png",
			"CloseSelected.png",
			this,
			menu_selector(HelloWorld::menuBeginCallback));
		CC_BREAK_IF(! pBeginItem);

		// Place the menu item bottom-right conner.
		pBeginItem->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width/4, CCDirector::sharedDirector()->getWinSize().height/2));

		// Create a menu with the "close" menu item, it's an auto release object.
		CCMenu* pMenu2 = CCMenu::create(pBeginItem, NULL);
		pMenu2->setPosition(CCPointZero);
		CC_BREAK_IF(! pMenu2);

		CCMenuItemImage *pSendItem = CCMenuItemImage::create(
			"CloseNormal.png",
			"CloseSelected.png",
			this,
			menu_selector(HelloWorld::menuSendCallback));
		CC_BREAK_IF(! pSendItem);

		// Place the menu item bottom-right conner.
		pSendItem->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width-100, CCDirector::sharedDirector()->getWinSize().height/2));

		// Create a menu with the "close" menu item, it's an auto release object.
		CCMenu* pMenu3 = CCMenu::create(pSendItem, NULL);
		pMenu3->setPosition(CCPointZero);
		CC_BREAK_IF(! pMenu3);


        // Add the menu to HelloWorld layer as a child layer.
        this->addChild(pMenu, 1);
		this->addChild(pMenu2, 100);
		this->addChild(pMenu3, 100);

		 txtLabel = CCLabelTTF::create("", "", 20);
		 txtLabel->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width/2, CCDirector::sharedDirector()->getWinSize().height/3));
		 this->addChild(txtLabel,101);

		this->schedule(schedule_selector(HelloWorld::msgLogic),0.1f);

        bRet = true;
    } while (0);

    return bRet;
}

void HelloWorld::menuCloseCallback(CCObject* pSender)
{
    // "close" menu item clicked
    CCDirector::sharedDirector()->end();
}

void HelloWorld::menuBeginCallback(CCObject* pSender)
{
	TcpUtil* tcpUtil = TcpUtil::shareTcpUtil();
	if (tcpUtil->isRuning())
	{
		tcpUtil->tcp_stop();
	}
	else
	{
		tcpUtil->tcp_start();
	}
}

void HelloWorld::menuSendCallback(CCObject* pSender)
{
	TcpUtil* tcpUtil = TcpUtil::shareTcpUtil();
	std::string msg = "{\"username\":\"test106\",\"password\":\"111111\"}";
	tcpUtil->pushSendQueue(msg,105);
}

void HelloWorld::msgLogic(float dt)
{
	MsgQueue* msgQueue = TcpUtil::shareTcpUtil()->getRecvQueue();
	GameMSG msg = msgQueue->getAFirstGameMessage();
	if (msg.tag)
	{
		bullfight::LoginMsg loginMsg;
		loginMsg.ParseFromString(msg.msg);
		char txt[1024] = {0};
		sprintf(txt,"userid:%d,username:%s",loginMsg.userinfo().userid(),loginMsg.userinfo().nickname().c_str());
		txtLabel->setString(txt);
		CCLog("size:%d, type:%d name: %s",msgQueue->getSize(),msg.msgID,loginMsg.userinfo().nickname().c_str());
	}
}