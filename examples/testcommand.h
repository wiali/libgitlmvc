#ifndef TESTCOMMAND_H
#define TESTCOMMAND_H
#include "abstractcommand.h"
#include "testmodel.h"

/// command, it manipulates the model and writes the result to output parameter. The output parameter will
/// be pass to view automatically.
class FirParamCommand : public AbstractCommand
{
    Q_OBJECT
public:
    /// Q_INVOKABLE is necessary for constructor
    Q_INVOKABLE explicit FirParamCommand(QObject *parent = 0):AbstractCommand(parent) {}
    bool execute(CommandParameter &rcInputArg, CommandParameter &rcOutputArg)
    {
        QString strDataToCommand = rcInputArg.getParameter("data_to_command").toString();
        TestModel::getInstance()->setDataInModel(strDataToCommand);
        rcOutputArg.setParameter("data_to_view", strDataToCommand);
        return true;
    }

};

#endif // TESTCOMMAND_H
