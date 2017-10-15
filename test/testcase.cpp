#include <QCoreApplication>
#include <iostream>
#include <QtTest/QtTest>
#include <QTest>
#include <QStringList>
#include "def.h"
#include "abstractcommand.h"
#include "frontcontroller.h"
#include "view.h"
#include "cmdevt.h"
#include "model.h"

/// model
class TestModel: public Model<TestModel>
{
    ADD_CLASS_FIELD(QString, strDataInModel, getDataInModel, setDataInModel)

protected:
    TestModel() { m_strDataInModel = "Hello MVC";}
    friend class Model<TestModel>;
};

/// view
class TestView : public View
{
public:
    TestView()
    {
        listenToParams("fir_param", MAKE_CALLBACK(TestView::firParamListener));
        listenToParams("sec_param", MAKE_CALLBACK(TestView::secParamListener));
        listenToParams(QStringList()<<"fir_param"<<"sec_param",
                       MAKE_CALLBACK(TestView::multiParamListener));
    }


    /// it receives the result from command
    void firParamListener(UpdateUIEvt& rcEvt)
    {
        m_strFirString = rcEvt.getParameter("fir_param").toString();
    }
    ADD_CLASS_FIELD_NOSETTER(QString, strFirString, getFirString)

    void secParamListener(UpdateUIEvt& rcEvt)
    {
        m_strSecString = rcEvt.getParameter("sec_param").toString();
    }
    ADD_CLASS_FIELD_NOSETTER(QString, strSecString, getSecString)


    void multiParamListener(UpdateUIEvt& rcEvt)
    {
        m_strMultiString = rcEvt.getParameter("fir_param").toString() + " " +
                rcEvt.getParameter("sec_param").toString();
    }
    ADD_CLASS_FIELD_NOSETTER(QString, strMultiString, getMultiString)
};

/// controller
/// the default one is used (FrontController)

/// command, it manipulates the model and writes the result to output parameter. The output parameter will
/// be pass to view automatically.
class FirParamCommand : public AbstractCommand
{
    Q_OBJECT
public:
    /// Q_INVOKABLE is necessary for constructor
    Q_INVOKABLE explicit FirParamCommand(QObject *parent = 0):AbstractCommand(parent) {setInWorkerThread(false);}
    bool execute(CommandParameter &rcInputArg, CommandParameter &rcOutputArg)
    {
        Q_UNUSED(rcInputArg)
        rcOutputArg.setParameter("fir_param", TestModel::getInstance()->getDataInModel());
        return true;
    }

};

class SecParamCommand : public AbstractCommand
{
    Q_OBJECT
public:
    /// Q_INVOKABLE is necessary for constructor
    Q_INVOKABLE explicit SecParamCommand(QObject *parent = 0):AbstractCommand(parent) {setInWorkerThread(true);}
    bool execute(CommandParameter &rcInputArg, CommandParameter &rcOutputArg)
    {
        Q_UNUSED(rcInputArg)
        rcOutputArg.setParameter("sec_param", "this is the second param");
        return true;
    }

};

class NestedCommand : public AbstractCommand
{
    Q_OBJECT
public:
    /// Q_INVOKABLE is necessary for constructor
    Q_INVOKABLE explicit NestedCommand(QObject *parent = 0):AbstractCommand(parent) {setInWorkerThread(false);}
    bool execute(CommandParameter &rcInputArg, CommandParameter &rcOutputArg)
    {
        Q_UNUSED(rcInputArg)
        Q_UNUSED(rcOutputArg)

        CmdEvt cFirstNestedEvt("fir_param_command");
        cFirstNestedEvt.dispatch();

        CmdEvt cSecondNestedEvt("sec_param_command");
        cSecondNestedEvt.dispatch();
        return true;
    }

};

class MultiParamCommand : public AbstractCommand
{
    Q_OBJECT
public:
    /// Q_INVOKABLE is necessary for constructor
    Q_INVOKABLE explicit MultiParamCommand(QObject *parent = 0):AbstractCommand(parent) {setInWorkerThread(false);}
    bool execute(CommandParameter &rcInputArg, CommandParameter &rcOutputArg)
    {
        Q_UNUSED(rcInputArg)
        rcOutputArg.setParameter("fir_param", TestModel::getInstance()->getDataInModel());
        rcOutputArg.setParameter("sec_param", "this is the second param");
        return true;
    }

};


/// test case
class TestCase : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        /// controller
        FrontController* pcFC = FrontController::getInstance();
        pcFC->registerCommand("fir_param_command",   &FirParamCommand::staticMetaObject);
        pcFC->registerCommand("sec_param_command",   &SecParamCommand::staticMetaObject);
        pcFC->registerCommand("multi_param_command", &MultiParamCommand::staticMetaObject);
        pcFC->registerCommand("nested_command", &NestedCommand::staticMetaObject);

    }


    void firParamTest()
    {
        /// view
        TestView cView;
        /// event (in real case, this event should be dispatch from user interface, i.e. the views)
        CmdEvt cRequestEvt("fir_param_command");
        cRequestEvt.dispatch();
        /// verify
        QVERIFY(cView.getFirString()=="Hello MVC");
        QVERIFY(cView.getSecString().isEmpty());
    }

    void secParamTest()
    {
        /// view
        TestView cView;
        /// event (in real case, this event should be dispatch from user interface, i.e. the views)
        CmdEvt cRequestEvt("sec_param_command");
        cRequestEvt.dispatch();
        /// verify
        QVERIFY(cView.getFirString().isEmpty());
        QVERIFY(cView.getSecString()=="this is the second param");
    }



    ///TODO check multi parameter listening case
    void multiParamsListening()
    {
        /// view
        TestView cView;

        /// event (in real case, this event should be dispatch from user interface, i.e. the views)
        CmdEvt cRequestEvt("multi_param_command");
        cRequestEvt.dispatch();

        /// verify
        QVERIFY(cView.getFirString()=="Hello MVC");
        QVERIFY(cView.getSecString()=="this is the second param");
        QVERIFY(cView.getMultiString()=="Hello MVC this is the second param");

    }

    void nestedCommandTest()
    {
        /// view
        TestView cView;

        /// event (in real case, this event should be dispatch from user interface, i.e. the views)
        CmdEvt cRequestEvt("nested_command");
        cRequestEvt.dispatch();

        /// verify
        QVERIFY(cView.getFirString()=="Hello MVC");
        QVERIFY(cView.getSecString()=="this is the second param");
    }

    void example()
    {
        Module cModule;

        /// subscribe to an event
        cModule.subscribeToEvtByName("I am a test event",
            [](Event& rcEvt)->bool
        {
            Q_UNUSED(rcEvt)
                qDebug() << "Hello EvtBus!";
            return true;
        }
        );

        Event cEvent("I am a test event");              ///< create an event
        cEvent.dispatch();                                  ///< dispatch
                                                            /// output: "Hello EvtBus!"*/
    }

};

/// test main
QTEST_MAIN(TestCase)
#include "testcase.moc"
