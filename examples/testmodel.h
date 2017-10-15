#ifndef TESTMODEL_H
#define TESTMODEL_H
#include "def.h"
#include "model.h"
/// model
class TestModel: public Model<TestModel>
{
    ADD_CLASS_FIELD(QString, strDataInModel, getDataInModel, setDataInModel)

protected:
    TestModel() {}
    friend class Model<TestModel>;
};

#endif // TESTMODEL_H
