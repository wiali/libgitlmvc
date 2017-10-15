#include <QCoreApplication>
#include <iostream>
#include <QtTest/QtTest>
#include <QTest>
#include <QStringList>
#include "def.h"
#include "module.h"
#include "eventbus.h"

/// test event bus
class TestModule : public Module
{
public:
    TestModule(EventBus* pcEventBus = NULL) :
        Module(pcEventBus)
    {
        this->m_bNotified = false;
    }

    void subscribeInsideClass()
    {
        subscribeToEvtByName("TEST_EVENT_1", MAKE_CALLBACK(TestModule::callback));
    }

    bool callback(Event& rcEvt)
    {
        Q_UNUSED(rcEvt)
            this->m_bNotified = true;
        return true;
    }
    ADD_CLASS_FIELD(bool, bNotified, getNotified, setNotified)
};

/// custom event
class CustomEvent : public Event
{
    CLONABLE(CustomEvent)
public:
    CustomEvent(const QString& strEvtName) : Event(strEvtName) { m_strCustomVar = "Custom String"; }
    ADD_CLASS_FIELD(QString, strCustomVar, getCustomVar, setCustomVar)
};

class CustomEventListener : public Module
{
public:
    CustomEventListener()
    {
        this->m_bNotified = false;
    }

    bool callback(Event& rcEvt)
    {
        CustomEvent& pcCusEvt = static_cast<CustomEvent&>(rcEvt);
        this->m_bNotified = true;
        this->m_strCustomVar = pcCusEvt.getCustomVar();
        return true;
    }

    ADD_CLASS_FIELD(bool, bNotified, getNotified, setNotified)
    ADD_CLASS_FIELD(QString, strCustomVar, getCustomVar, setCustomVar)
};

/// test case
class TestCase2 : public QObject
{
    Q_OBJECT

    private slots:
    void lamdaListening()
    {
        TestModule cModule;
        cModule.subscribeToEvtByName("TEST_EVENT_1",
            [&](Event& e)->bool
        {
            Q_UNUSED(e)
                cModule.setNotified(true);
            return true;
        });
        QVERIFY(!cModule.getNotified());
        Event cEvt("TEST_EVENT_1");
        cModule.dispatchEvt(cEvt);
        QVERIFY(cModule.getNotified());
    }

    void listenInsideClass()
    {
        TestModule cModule;
        cModule.subscribeInsideClass();
        QVERIFY(!cModule.getNotified());
        Event cEvt("TEST_EVENT_1");
        cEvt.dispatch();
        QVERIFY(cModule.getNotified());
    }

    void listenOutsideClass()
    {
        TestModule cModule;
        cModule.subscribeToEvtByName("TEST_EVENT_1", MAKE_CALLBACK_OBJ(cModule, TestModule::callback));
        QVERIFY(!cModule.getNotified());
        Event cEvt("TEST_EVENT_1");
        cModule.dispatchEvt(cEvt);
        QVERIFY(cModule.getNotified());
    }

    void unsubscribe()
    {
        TestModule cModule;
        cModule.subscribeToEvtByName("TEST_EVENT_1", MAKE_CALLBACK_OBJ(cModule, TestModule::callback));
        cModule.unsubscribeToEvtByName("TEST_EVENT_1");
        QVERIFY(!cModule.getNotified());
        Event cEvt("TEST_EVENT_1");
        cModule.dispatchEvt(cEvt);
        QVERIFY(!cModule.getNotified());
    }

    void oneToMany()
    {
        TestModule cSender;
        TestModule cModule1;
        TestModule cModule2;
        TestModule cModule3;
        cModule1.subscribeToEvtByName("TEST_EVENT_1", MAKE_CALLBACK_OBJ(cModule1, TestModule::callback));
        cModule2.subscribeToEvtByName("TEST_EVENT_1", MAKE_CALLBACK_OBJ(cModule2, TestModule::callback));
        cModule3.subscribeToEvtByName("TEST_EVENT_2", MAKE_CALLBACK_OBJ(cModule3, TestModule::callback));

        Event cEvt1("TEST_EVENT_1");
        cSender.dispatchEvt(cEvt1);
        QVERIFY(cModule1.getNotified());
        QVERIFY(cModule2.getNotified());
        QVERIFY(!cModule3.getNotified());

        Event cEvt2("TEST_EVENT_2");
        cSender.dispatchEvt(cEvt2);
        QVERIFY(cModule3.getNotified());
    }

    void customEventTest()
    {
        CustomEventListener cModule;
        cModule.subscribeToEvtByName("TEST_EVENT_1", MAKE_CALLBACK_OBJ(cModule, CustomEventListener::callback));
        CustomEvent cEvt("TEST_EVENT_1");
        cEvt.dispatch();
        QVERIFY(cModule.getNotified());
        QVERIFY(cModule.getCustomVar() == QString("Custom String"));
    }

    void multiplyEventBus()
    {
        EventBus* pcBus1 = EventBus::create(); TestModule cModule1(pcBus1); TestModule cModule2(pcBus1);
        EventBus* pcBus2 = EventBus::create(); TestModule cModule3(pcBus2); TestModule cModule4(pcBus2);

        /// all module are listening to the same events, but on different event buses.
        cModule1.subscribeToEvtByName("TEST_EVENT_1", MAKE_CALLBACK_OBJ(cModule1, TestModule::callback));
        cModule2.subscribeToEvtByName("TEST_EVENT_1", MAKE_CALLBACK_OBJ(cModule2, TestModule::callback));
        cModule3.subscribeToEvtByName("TEST_EVENT_1", MAKE_CALLBACK_OBJ(cModule3, TestModule::callback));
        cModule4.subscribeToEvtByName("TEST_EVENT_1", MAKE_CALLBACK_OBJ(cModule4, TestModule::callback));

        /// event
        CustomEvent cEvt("TEST_EVENT_1");

        /// no one get notified because no module is attached to the default event bus
        cEvt.dispatch();
        QVERIFY(!cModule1.getNotified());
        QVERIFY(!cModule2.getNotified());
        QVERIFY(!cModule3.getNotified());
        QVERIFY(!cModule4.getNotified());

        /// this will only notify module 1 & 2
        cEvt.dispatch(pcBus1);
        QVERIFY(cModule1.getNotified());
        QVERIFY(cModule2.getNotified());
        QVERIFY(!cModule3.getNotified());
        QVERIFY(!cModule4.getNotified());

        /// this will notify module 3 & 4
        cEvt.dispatch(cModule3.getEventBus());
        QVERIFY(cModule3.getNotified());
        QVERIFY(cModule4.getNotified());

        /// make sure everyone is attached to the correct event bus
        QVERIFY(cModule1.getEventBus() == pcBus1);
        QVERIFY(cModule2.getEventBus() == pcBus1);
        QVERIFY(cModule3.getEventBus() == pcBus2);
        QVERIFY(cModule4.getEventBus() == pcBus2);

        ///
        TestModule cModule5(pcBus1);
        cModule5.subscribeToEvtByName("TEST_EVENT_1", MAKE_CALLBACK_OBJ(cModule5, TestModule::callback));

        cEvt.dispatch(pcBus2);
        QVERIFY(!cModule5.getNotified());

        cModule5.attach(pcBus2);
        cEvt.dispatch(pcBus2);
        QVERIFY(cModule5.getNotified());
    }
};

/// test main
QTEST_MAIN(TestCase2)
#include "testcase2.moc"

