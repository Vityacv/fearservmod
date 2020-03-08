#ifndef _DYNOBJECT_H
#define _DYNOBJECT_H

class DynObject;
#include <set>

class DynObject {
    std::set<DynObject *> m_nodes;

   public:
    void addObject(DynObject *parent);
    void setParent(DynObject *parent);
    void removeObject(DynObject *parent);
    DynObject(DynObject *parent = nullptr);
    virtual ~DynObject();
};
#endif
