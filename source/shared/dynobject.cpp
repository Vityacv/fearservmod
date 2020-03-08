#include "pch.h"
#include "shared/dynobject.h"

void DynObject::addObject(DynObject *parent) {  // add object to list
    m_nodes.insert(parent);
}

void DynObject::removeObject(DynObject *parent)  // remove object from list
{
    const auto& it = m_nodes.find(parent);
    if(m_nodes.end() != it)
        m_nodes.erase(it);
    delete parent;
}

DynObject::DynObject(DynObject *parent) {
    if (parent) parent->addObject(this);
}

DynObject::~DynObject() {
    for(auto it = m_nodes.begin(); it != m_nodes.end();) {
        auto key = *it;
        it = m_nodes.erase(it);
        delete key;
    }
//    while (m_nodes.count()) {
//        DynObjectNode *node = m_nodes.begin();
//        m_nodes.free(node);
//        delete node->m_item;
//    }
}
